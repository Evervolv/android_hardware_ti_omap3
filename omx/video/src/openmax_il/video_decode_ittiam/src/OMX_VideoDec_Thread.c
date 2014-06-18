/*****************************************************************************/
/*                                                                           */
/*					  Ittiam Video Decoder OMX-IL Component  	             */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2010                             */
/*                                                                           */
/*  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and */
/*  is protected under Indian  Copyright Law as an unpublished work. Its use */
/*  and  disclosure  is  limited by  the terms  and  conditions of a license */
/*  agreement. It may not be copied or otherwise  reproduced or disclosed to */
/*  persons outside the licensee's organization except in accordance with the*/
/*  terms  and  conditions   of  such  an  agreement.  All  copies  and      */
/*  reproductions shall be the property of Ittiam Systems Private Limited and*/
/*  must bear this notice in its entirety.                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : OMX_VideoDecThread.c                                 */
/*                                                                           */
/*  Description       : This file contains Video Decoder Thread 		     */
/*																			 */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          17 11 2009   ITTIAM          Draft                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/
/* User include files */
#include <OMX_Core.h>
#include <OMX_TI_Debug.h>
#include "OMX_TI_Common.h"
//#include "overlay_common.h"
#include "LCML_DspCodec.h"
#include "LCML_Types.h"
#include "LCML_CodecInterface.h"
#include "OMX_IttiamVideoDec_Utils.h"
#include "h264_dsp_api.h"
#include "mpeg4_dsp_api.h"
#include "wmv9_dsp_api.h"
#include "OMX_IttiamVideoDec_DSP.h"
#include "OMX_IttiamVideoDec_Thread.h"
#include "OMX_VideoDec_Parse.h"
#include "OMX_IttiamVideoDec_Logger.h"
#include "split_dsp_dec.h"
#include "xdas.h"
#include "xdm.h"
#include "ivideo.h"
#include "ividdec2.h"
#include "split/ividdec2.h"
#include "ittiam_datatypes.h"
#include "xdm_extension_dec2.h"
#include "split_dec.h"

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_OMX_VidDec_Thread"
#include <utils/Log.h>

/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/
/* Default timeout used to come out of blocking calls */
#define VIDD_TIMEOUT (1000) /* milliseconds */

/*****************************************************************************/
/*  Function Name : OMX_VidDec_Thread                                        */
/*  Description   : This is the open max thread. This method is in charge of */
/*				    listening to the buffers coming from DSP, application or */
/*					commands through the pipes                               */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
void* OMX_VidDec_Thread (void *pThreadData)
{
    int                      status;
    int                      fdmax;
    fd_set                   rfds;
    OMX_ERRORTYPE            eError = OMX_ErrorNone;
    OMX_COMMANDTYPE          eCmd;
    OMX_U32                  nParam1;
    OMX_PTR    				 pCmdData;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate;
    sigset_t                 set;
    struct timespec          tv;

    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pThreadData;

#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->pPERFcomp = PERF_Create(PERF_FOURS("VD T"),
                          PERF_ModuleComponent | PERF_ModuleVideoDecode);
#endif

    /* Looking for highest no of file descriptor for pipes in order to put */
    /* in select loop                                                      */
    fdmax = pComponentPrivate->cmdPipe[VIDDEC_PIPE_READ];

    if (pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ] > fdmax)
    {
        fdmax = pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ];
    }

    if (pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ] > fdmax)
    {
        fdmax = pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ];
    }

    if (pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ] > fdmax)
    {
        fdmax = pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ];
    }

    while (1)
    {
#ifdef VIDDEC_FLAGGED_EOS
        if (!pComponentPrivate->bUseFlaggedEos)
        {
#endif
            LOG_BUF("PlayCompleted = %d , BufFound = %d FlushOut = %d \n",
            pComponentPrivate->bPlayCompleted,
            pComponentPrivate->bBuffFound,
            pComponentPrivate->bFlushOut);

            if(pComponentPrivate->bPlayCompleted == OMX_TRUE &&
				pComponentPrivate->bBuffFound == OMX_FALSE &&
				pComponentPrivate->bFlushOut == OMX_FALSE)
            {
                OMX_VidDec_HandlePipes(pComponentPrivate);
                OMX_VidDec_HandlePipes(pComponentPrivate);
                pComponentPrivate->bPipeCleaned = 0;
                VIDDEC_LCML_Flush(pComponentPrivate, VIDDEC_OUTPUT_PORT);
                pComponentPrivate->bFlushOut = OMX_TRUE;
            }
 #ifdef VIDDEC_FLAGGED_EOS
        }
 #endif

        LOG_BUF("Going to fd_set\n");
        FD_ZERO (&rfds);
        FD_SET(pComponentPrivate->cmdPipe[VIDDEC_PIPE_READ], &rfds);
        FD_SET(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ], &rfds);
        FD_SET(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ], &rfds);
        FD_SET(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ], &rfds);

        LOG_BUF("Out of fd_set going to pselect\n");

        /* If the component is flushing, reduce the timeout interval */
        {
            OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;

            if((OMX_TRUE == pSplit->m_isFlushing) ||
			   (pSplit->bStop == OMX_TRUE))
            {
                tv.tv_sec = 0;
                tv.tv_nsec = 30 * 1000 * 1000;
            }
            else
            {
                tv.tv_sec = 1;
                tv.tv_nsec = 0;
            }
        }
        
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &rfds, NULL, NULL, &tv, &set);
        sigdelset (&set, SIGALRM);

        LOG_BUF("Out of pselect\n");
        if ((0 == status) && (pComponentPrivate->eState == OMX_StateExecuting))
        {
            /* If state transition is not issued for pause, then any pause    */
            /* for greater than 10 seconds might wrongly be considered as a   */
            /* hang on DSP. To avoid this condition, check and ensure at      */
            /* least one input and one output buffer is currently held in DSP */
            /* Only if both input and output buffers are held and not returned*/
            /* for 10 seconds it can be concluded that DSP is hung. Note in   */
            /* player is paused, DSP will not have input and output buffers   */
            /* with it. Currently nothing is done when pause is issued to     */
            /* Socket Node. 												  */
            OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
            int 					  i;
            int 					  inp_buf_queued = 0, out_buf_queued = 0;

            /* Check if any input buffer is queued to DSP and not returned yet*/
            for(i = 0; i < MAX_DSP_INP_BUFS; i++)
            {
                if((NULL != pSplit->dsp_inp_buf_status[i].buf) &&
                  (BUF_QUEUED == pSplit->dsp_inp_buf_status[i].buf_status))
                {
                    inp_buf_queued = 1;
                    break;
                }
            }

            /* Check if any output buffer is queued to DSP and not returned */
            for(i = 0; i < MAX_DSP_OUT_BUFS; i++)
            {
                if((NULL != pSplit->dsp_out_buf_status[i].buf) &&
                  (BUF_QUEUED == pSplit->dsp_out_buf_status[i].buf_status))
                {
                    out_buf_queued = 1;
                    break;
                }
            }

            if((pSplit->arm_process_running == 1) ||
			   (inp_buf_queued && out_buf_queued))
            {
                /* If atleast one output buffer and one input buffer is held */
                /* by DSP and not returned for 10 seconds then it means DSP  */
                /* is hung. In such case declare a time out and return       */

                pSplit->timeout_cnt++;

                if(pSplit->timeout_cnt > 10  &&
				   pComponentPrivate->iEndofInput == 0 &&
				   pSplit->timeout_reported == 0)
                {
                    LOG_BUF("TIMEOUT occured\n");
                    pSplit->timeout_reported = 1;
                    sleep(1);
                    VIDDEC_CloseCodec(pComponentPrivate);
                }
            }
            else
            {
                pSplit->timeout_cnt = 0;
                pSplit->timeout_reported = 0;
            }

            LOG_BUF("No output\n");
        }
        else if (-1 == status)
        {
            LOG_BUF("Error!!!!!!!! \n");
            LOG_STATUS_OMX_CMDS("Error in Select EventHandler OMX_ErrorInsufficientResources \n");
            pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorInsufficientResources,
						OMX_TI_ErrorSevere,
						"Error from Component Thread in select");

        }
        else
        {
            if (FD_ISSET(pComponentPrivate->cmdPipe[VIDDEC_PIPE_READ], &rfds))
            {
				/* Read the command pipe */
				read(pComponentPrivate->cmdPipe[VIDDEC_PIPE_READ],
													&eCmd, sizeof(eCmd));

				LOG_BUF("Reading cmd pipe %d \n",
													(int)eCmd);

				/* read the command data pipe */
				read(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_READ],
											&nParam1, sizeof(nParam1));

				LOG_BUF("Reading cmdDataPipe %d \n", (int)nParam1);

#ifdef __PERF_INSTRUMENTATION__
			PERF_ReceivedCommand(pComponentPrivate->pPERFcomp,
								 eCmd, nParam1, PERF_ModuleLLMM);
#endif
				if (eCmd == OMX_CommandStateSet)
				{
					LOG_BUF("Command for state set to %lu\n", nParam1);

					if ((OMX_S32)nParam1 < -2)
					{
						LOG_BUF("Incorrect variable value used\n");
					}
					if ((OMX_S32)nParam1 != -1 && (OMX_S32)nParam1 != -2)
					{
						LOG_BUF(
												"Param is %lu\n", nParam1);

						/* This function will handle all the state */
						/* transitions 							   */
						eError = VIDDEC_HandleCommandStateSet(
											pComponentPrivate,  nParam1);
						if (eError != OMX_ErrorNone)
						{
							/* Do Nothing as error msg has already been */
							/* sent                                     */
							LOG_BUF("VIDDEC_HandleCommandStateSet returned failed\n");
						}
					}
					else if ((OMX_S32)nParam1 == -1)
					{
						/* This happens during de-init */
						LOG_BUF("Params is -1, break\n");

						break;
					}
					else if ((OMX_S32)nParam1 == -2)
					{
						LOG_BUF("Params is -2, two returns and break\n");

						OMX_VidDec_HandlePipes(pComponentPrivate);
						VIDDEC_Handle_InvalidState(pComponentPrivate);
						break;
					}
				}
				else if (eCmd == OMX_CommandPortDisable)
				{
					LOG_BUF("Go to port disable\n");
					eError = VIDDEC_DisablePort(pComponentPrivate, nParam1);
					if (eError != OMX_ErrorNone)
					{
						LOG_STATUS_OMX_CMDS("EventHandler OMX_ErrorInsufficientResources \n");
						pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							eError,
							OMX_TI_ErrorSevere,
							"Error in DisablePort function");
					}
				}
				else if (eCmd == OMX_CommandPortEnable)
				{
					LOG_BUF("Got ot port enable\n");

					/* Sumukh !!!!! why disable when u have to enable */
					eError = VIDDEC_DisablePort(pComponentPrivate, nParam1);

					eError = VIDDEC_EnablePort(pComponentPrivate, nParam1);
					if (eError != OMX_ErrorNone)
					{
						LOG_STATUS_OMX_CMDS("OMX_CommandPortEnable EventHandler OMX_EventError \n");
						pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							eError,
							OMX_TI_ErrorSevere,
							"Error in EnablePort function");
					}
				}
				else if (eCmd == OMX_CommandFlush)
				{
					LOG_BUF("CommandFlush\n");

					/* Command issued by IL client before seek */
					eError = VIDDEC_HandleCommandFlush (pComponentPrivate,
														nParam1, OMX_TRUE);
					if (eError != OMX_ErrorNone)
					{
						LOG_STATUS_OMX_CMDS("VIDDEC_HandleCommandFlush EventHandler OMX_EventError \n");
						pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							eError,
							OMX_TI_ErrorSevere,
							"Error in Handling Flush function");
					}
				}
				else if (eCmd == OMX_CommandMarkBuffer)
				{
					OMX_U8 tmpCount = pComponentPrivate->nInCmdMarkBufIndex;
					LOG_BUF("Mark buffer!\n");

					//Sumukh!!!!
					read(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_READ],
								&pCmdData, sizeof(pCmdData));

					pComponentPrivate->
						arrCmdMarkBufIndex[tmpCount].hMarkTargetComponent =
							((OMX_MARKTYPE*)(pCmdData))->hMarkTargetComponent;

					pComponentPrivate->
								arrCmdMarkBufIndex[tmpCount].pMarkData =
									((OMX_MARKTYPE*)(pCmdData))->pMarkData;

					pComponentPrivate->nInCmdMarkBufIndex++;
					pComponentPrivate->nInCmdMarkBufIndex %=
												VIDDEC_MAX_QUEUE_SIZE;
				}
			}
            if(pComponentPrivate->bPipeCleaned)
            {
                pComponentPrivate->bPipeCleaned =0;
            }
            else
            {
                LOG_BUF("In else, pComponentPrivate->bDynamicConfigurationInProgress is %d\n",
					pComponentPrivate->bDynamicConfigurationInProgress);

				/* If output is filled and ready to be returned back to APP */
                if (FD_ISSET(pComponentPrivate->
									filled_outBuf_Q[VIDDEC_PIPE_READ], &rfds))
                {
                    if(pComponentPrivate->bDynamicConfigurationInProgress)
                    {
                        VIDDEC_WAIT_CODE();
                        continue;
                    }

                    eError = VIDDEC_HandleDataBuf_FromDsp(pComponentPrivate);
                    if (eError != OMX_ErrorNone)
                    {
                        LOG_BUF("Error while handling filled DSP output buffer\n");

                        VIDDEC_FatalErrorRecover(pComponentPrivate);
                    }
                }

                if (FD_ISSET(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ], &rfds))
                {
                    if(pComponentPrivate->bDynamicConfigurationInProgress)
                    {
						LOG_BUF("pComponentPrivate->bDynamicConfigurationInProgress is %d\n",
						pComponentPrivate->bDynamicConfigurationInProgress);
                        VIDDEC_WAIT_CODE();
                        continue;
                    }
                    eError = VIDDEC_HandleFreeDataBuf(pComponentPrivate);
                    if (eError != OMX_ErrorNone)
                    {
                        LOG_BUF("Error while processing free input buffers\n");

                        VIDDEC_FatalErrorRecover(pComponentPrivate);

                    }
                }
                if (FD_ISSET(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ], &rfds))
                {
					if(pComponentPrivate->bDynamicConfigurationInProgress)
					{
						LOG_BUF("pComponentPrivate->bDynamicConfigurationInProgress is %d\n",
						pComponentPrivate->bDynamicConfigurationInProgress);
						VIDDEC_WAIT_CODE();
						continue;
					}

                    eError = VIDDEC_HandleFreeOutputBufferFromApp(pComponentPrivate);
                    if (eError != OMX_ErrorNone)
                    {
                        LOG_BUF("Error while processing free output buffer\n");

                        VIDDEC_FatalErrorRecover(pComponentPrivate);

                    }
                }
            }
        }
    }

#ifdef __PERF_INSTRUMENTATION__
    PERF_Done(pComponentPrivate->pPERFcomp);
#endif

    LOG_BUF("Component Thread exits");
    return (void*)OMX_ErrorNone;
}


/*****************************************************************************/
/*  Function Name : OMX_VidDec_Return                                        */
/*  Description   : Processed input buffers, Free and filled O/P buffers     */
/*                  are handled here                                         */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
void OMX_VidDec_HandlePipes(void *pThreadData)
{
    OMX_S32                  status = 0;
    sigset_t 			     set;
    struct 				     timespec tv;
    int 					 fdmax = 0;
    OMX_U32 			     iLock = 0;
    fd_set 		             rfds;
    OMX_ERRORTYPE			 eError = OMX_ErrorNone;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pThreadData;

    /* Looking for highest number of file descriptor for pipes in order to */
    /* put in select loop 												   */
    fdmax = pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ];

    if (pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ] > fdmax)
    {
        fdmax = pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ];
    }

    if (pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ] > fdmax)
    {
        fdmax = pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ];
    }

    while ((pComponentPrivate->bInputBCountApp  > 0) ||
		   (pComponentPrivate->bOutputBCountApp > 0) ||
           (pComponentPrivate->bInputBCountDsp  > 0) ||
           (pComponentPrivate->bOutputBCountDsp > 0))
    {
        FD_ZERO (&rfds);
        FD_SET(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ], &rfds);
        FD_SET(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ], &rfds);
        FD_SET(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ], &rfds);

        tv.tv_sec = 0;
        tv.tv_nsec = 0;

        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &rfds, NULL, NULL, &tv, &set);
        sigdelset (&set, SIGALRM);

        if (0 == status)
        {
            iLock++;
            if (iLock > 2)
            {
                pComponentPrivate->bPipeCleaned = 1;
                break;
            }
        }
        else if (-1 == status)
        {
            LOG_STATUS_OMX_CMDS("Error in Select EventHandler OMX_ErrorInsufficientResources \n");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventError,
						   OMX_ErrorInsufficientResources,
						   OMX_TI_ErrorSevere,
						   "Error from Component Thread in select");
        }
        else
        {
            if (FD_ISSET(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ], &rfds))
            {
                eError = VIDDEC_HandleDataBuf_FromDsp(pComponentPrivate);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("Error while handling filled DSP output buffer\n");
                    VIDDEC_FatalErrorRecover(pComponentPrivate);
                }

            }

            if (FD_ISSET(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ], &rfds))
            {
                eError = VIDDEC_HandleFreeDataBuf(pComponentPrivate);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("Error while processing free input buffers\n");

                    VIDDEC_FatalErrorRecover(pComponentPrivate);
                }
            }
            if (FD_ISSET(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ], &rfds))
            {
				eError = VIDDEC_HandleFreeOutputBufferFromApp(pComponentPrivate);
				if (eError != OMX_ErrorNone)
				{
					LOG_BUF("Error while processing free output buffer\n");
					LOG_BUF("\n Error while processing free output buffers 2");
					VIDDEC_FatalErrorRecover(pComponentPrivate);
				}
            }
        }
    }

    pComponentPrivate->bPipeCleaned = OMX_TRUE;

	LOG_BUF_COUNT("After VIDDEC_RETURN: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
			pComponentPrivate->bInputBCountApp,
			pComponentPrivate->bOutputBCountApp,
			pComponentPrivate->bInputBCountDsp,
			pComponentPrivate->bOutputBCountDsp);

    return;
}


/*****************************************************************************/
/*  Function Name : OMX_VidDec_Return_except_Freeoutbuf                      */
/*  Description   : Processed input buffers and filled O/P buffers           */
/*                  are handled here                                         */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
void OMX_VidDec_Return_except_Freeoutbuf (void* pThreadData)
{
    OMX_S32                  status = 0;
    sigset_t 			     set;
    struct 				     timespec tv;
    int 					 fdmax = 0;
    OMX_U32 			     iLock = 0;
    fd_set 		             rfds;
    OMX_ERRORTYPE			 eError = OMX_ErrorNone;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pThreadData;

    /* Looking for highest number of file descriptor for pipes in order to */
    /* put in select loop 												   */
    fdmax = pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ];

    if (pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ] > fdmax)
    {
        fdmax = pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ];
    }

    while ((pComponentPrivate->bInputBCountApp  > 0) ||
		   (pComponentPrivate->bOutputBCountApp > 0) ||
           (pComponentPrivate->bInputBCountDsp  > 0) ||
           (pComponentPrivate->bOutputBCountDsp > 0))
    {
        FD_ZERO (&rfds);
        FD_SET(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ], &rfds);
        FD_SET(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ], &rfds);
     
        tv.tv_sec = 0;
        tv.tv_nsec = 0;

        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &rfds, NULL, NULL, &tv, &set);
        sigdelset (&set, SIGALRM);

        if (0 == status)
        {
            iLock++;
            if (iLock > 2)
            {
                pComponentPrivate->bPipeCleaned = 1;
                break;
            }
        }
        else if (-1 == status)
        {
            LOG_STATUS_OMX_CMDS("Error in Select EventHandler OMX_ErrorInsufficientResources \n");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventError,
						   OMX_ErrorInsufficientResources,
						   OMX_TI_ErrorSevere,
						   "Error from Component Thread in select");
        }
        else
        {
            if (FD_ISSET(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ], &rfds))
            {
                eError = VIDDEC_HandleDataBuf_FromDsp(pComponentPrivate);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("Error while handling filled DSP output buffer\n");
                    VIDDEC_FatalErrorRecover(pComponentPrivate);
                }

            }

            if (FD_ISSET(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ], &rfds))
            {
                eError = VIDDEC_HandleFreeDataBuf(pComponentPrivate);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("Error while processing free input buffers\n");

                    VIDDEC_FatalErrorRecover(pComponentPrivate);
                }
            }
        }
    }

    pComponentPrivate->bPipeCleaned = OMX_TRUE;

	LOG_BUF_COUNT("After VIDDEC_RETURN: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
			pComponentPrivate->bInputBCountApp,
			pComponentPrivate->bOutputBCountApp,
			pComponentPrivate->bInputBCountDsp,
			pComponentPrivate->bOutputBCountDsp);

    return;
}
