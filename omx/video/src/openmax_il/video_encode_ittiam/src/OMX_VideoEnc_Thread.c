/*****************************************************************************/
/*                                                                           */
/*                             Standard Files                                */
/*                  ITTIAM SYSTEMS PVT LTD, BANGALORE                        */
/*                          COPYRIGHT(C) 2006                                */
/*                                                                           */
/*  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and */
/*  is protected under Indian  Copyright Law as an unpublished work. Its use */
/*  and  disclosure  is  limited by  the terms  and  conditions of a license */
/*  agreement. It may not be copied or otherwise  reproduced or disclosed to */
/*  persons outside the licensee's organization except in accordance with the*/
/*  the  terms  and  conditions   of  such  an  agreement.  All  copies  and */
/*  reproductions shall be the property of Ittiam Systems Private Limited and*/
/*  must bear this notice in its entirety.                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : ittiam_datatypes.h                                   */
/*                                                                           */
/*  Description       : This file has the definitions of the data types used */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         06 04 2006   Malavika        Draft                                */
/*                                                                           */
/*****************************************************************************/


/* ------compilation control switches ----------------------------------------*/
/******************************************************************************
*  INCLUDE FILES
*******************************************************************************/
/* ----- system and platform files -------------------------------------------*/
#define _XOPEN_SOURCE 600
#include <wchar.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>

#include <dbapi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


#include "OMX_ividenc1_utils.h"
#include "OMX_ividenc1_api_structs.h"
/*------- Program Header Files -----------------------------------------------*/
#include "OMX_VideoEnc_Utils.h"
#include "OMX_VideoEncoder.h"
#include "iv_enc_export.h"

/*----------------------------------------------------------------------------*/
/**
  * handleCommand()
  *
  * Called by OMX_VIDENC_Thread function.
  * This function will handle all the OMX commands sent. It also handles the
  * the exit command sent to this thread from the OMX_ComponentDeinit function
  *
  * @param pComponentPrivate
  *        threadExit
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE handleCommand(VIDENC_COMPONENT_PRIVATE *pComponentPrivate, OMX_BOOL *pthreadExit)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMMANDTYPE eCmd = -1;
    OMX_U32 nParam1;
    int nRet = -1;
    OMX_PTR pCmdData = NULL;

    nRet = read(pComponentPrivate->nCmdPipe[0],
                &eCmd,
                sizeof(eCmd));
    if (nRet == -1)
    {
        OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from cmdPipe\n");
        OMX_VIDENC_SET_ERROR_BAIL(eError,
                                  OMX_ErrorHardware,
                                  pComponentPrivate);
    }

#ifdef __PERF_INSTRUMENTATION__
            PERF_ReceivedCommand(pComponentPrivate->pPERFcomp,
                                 eCmd, 0, PERF_ModuleLLMM);
#endif
    if ((OMX_S32)eCmd == -1)
    {
        OMX_PRCOMM2(pComponentPrivate->dbg, "Received OMX encoder shut down command.\n");

        /* Set flag to exit DSP wait thread */
        *pthreadExit = OMX_TRUE;

        /* Return with sucess */
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorNone);
    }

    if (eCmd == OMX_CommandMarkBuffer)
    {
        nRet = read(pComponentPrivate->nCmdDataPipe[0],
                    &pCmdData,
                    sizeof(pCmdData));
        if (nRet == -1)
        {
            OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from cmdDataPipe\n");
            OMX_VIDENC_SET_ERROR_BAIL(eError,
                                      OMX_ErrorHardware,
                                      pComponentPrivate);
        }
    }
    else
    {
        nRet = read(pComponentPrivate->nCmdDataPipe[0],
                    &nParam1,
                    sizeof(nParam1));
        if (nRet == -1)
        {
            OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from cmdDataPipe\n");
            OMX_VIDENC_SET_ERROR_BAIL(eError,
                                      OMX_ErrorHardware,
                                      pComponentPrivate);
        }
    }

#ifdef __PERF_INSTRUMENTATION__
        PERF_ReceivedCommand(pComponentPrivate->pPERFcomp,
                             eCmd,
                             (eCmd == OMX_CommandMarkBuffer) ? ((OMX_U32) pCmdData) : nParam1,
                             PERF_ModuleLLMM);
#endif

    switch (eCmd)
    {
        case OMX_CommandStateSet :
        OMX_PRSTATE2(pComponentPrivate->dbg, "Enters OMX_VIDENC_HandleCommandStateSet\n");
            eError = OMX_VIDENC_HandleCommandStateSet(pComponentPrivate,
                                                      nParam1);
            if(OMX_ErrorInvalidState == eError)
            {
                /* Do not exit from Main Thread if OMX_ErrorInvalidState has occurred */
                OMX_PRSTATE4(pComponentPrivate->dbg, "Not exiting, but got OMX_ErrorInvalidState%s:%d Also resetting eError", __FUNCTION__, __LINE__);
                eError = OMX_ErrorNone;
            }
            else
            {
                OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
                OMX_PRSTATE4(pComponentPrivate->dbg, "Exits OMX_VIDENC_HandleCommandStateSet %s:%d", __FUNCTION__, __LINE__);
            }
            break;
        case OMX_CommandFlush :
        OMX_PRSTATE2(pComponentPrivate->dbg, "Enters OMX_VIDENC_HandleCommandFlush\n");
            eError = OMX_VIDENC_HandleCommandFlush(pComponentPrivate,
                                                   nParam1,
                                                   OMX_FALSE);
            OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
        OMX_PRSTATE2(pComponentPrivate->dbg, "Exits OMX_VIDENC_HandleCommandFlush\n");
            break;
        case OMX_CommandPortDisable :
        OMX_PRSTATE2(pComponentPrivate->dbg, "Enters OMX_VIDENC_HandleCommandDisablePort\n");
            eError = OMX_VIDENC_HandleCommandDisablePort(pComponentPrivate,
                                                         nParam1);
            OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
        OMX_PRSTATE2(pComponentPrivate->dbg, "Exits OMX_VIDENC_HandleCommandDisablePort\n");
            break;
        case OMX_CommandPortEnable :
        OMX_PRSTATE2(pComponentPrivate->dbg, "Enters OMX_VIDENC_HandleCommandDisablePort\n");
            eError = OMX_VIDENC_HandleCommandEnablePort(pComponentPrivate,
                                                        nParam1);
            OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
        OMX_PRSTATE2(pComponentPrivate->dbg, "Exits OMX_VIDENC_HandleCommandDisablePort\n");
            break;
        case OMX_CommandMarkBuffer :
            if (!pComponentPrivate->pMarkBuf)
            {
                pComponentPrivate->pMarkBuf = (OMX_MARKTYPE*)(pCmdData);
            }
            break;
        default:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorUndefined,
                                     0,
                                     NULL);
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * waitForACKFromDSPWaitThread()
  *
  * Called by OMX_VIDENC_Thread function.
  *
  * @param
  *
  * @retval
  *
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE waitForACKFromDSPWaitThread(VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
    int             fdmax = -1;
    int             nRet = -1;
    fd_set          rfds;
    sigset_t        set;
    OMX_COMMANDTYPE eCmd;
    OMX_ERRORTYPE   eError = OMX_ErrorNone;
    int             status = -1;

    /* Send a message to the DSP wait thread requesting for a ACK once a     */
    /* has been completed.                                                   */
    eCmd = SEND_DSP_ACK;
    ALOGD("Sending SEND_DSP_ACK to nDsp_wait_thread_event_pipe");
    nRet = write(pComponentPrivate->nVidenc_DSP_wait_comm_thread[1], &eCmd, sizeof(eCmd));
    if (nRet == -1)
    {
        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                       pComponentPrivate->dbg, OMX_PRCOMM4,
                       "Failed to write to nCmdPipe.\n");
    }

    /* Wait for the ACK from the DSP thread so that further dsp triggers can */
    /* be issued. This wait has no timeout value, it is instead infinite wait*/
    /* This is necessary because no further DSP triggers can happen without  */
    /* the previous frame encoding has been completed.                       */
    fdmax = pComponentPrivate->nDSP_wait_videnc_comm_thread[0];

    FD_ZERO (&rfds);
    FD_SET (pComponentPrivate->nDSP_wait_videnc_comm_thread[0], &rfds);

    sigemptyset(&set);
    sigaddset(&set,SIGALRM);
    status = pselect(fdmax+1, &rfds, NULL, NULL, NULL, &set);

    if (0 == status)
    {
        ALOGD("The timeout value is specified as NULL should never come here");
        sched_yield();
    }
    else if (-1 == status)
    {
        ALOGE("pselect returned an error");
        OMX_VIDENC_SET_ERROR_BAIL(eError, OMX_ErrorHardware, pComponentPrivate);
    }
    else
    {
        if (FD_ISSET(pComponentPrivate->nDSP_wait_videnc_comm_thread[0], &rfds))
        {
            nRet = read(pComponentPrivate->nDSP_wait_videnc_comm_thread[0], &eCmd, sizeof(eCmd));
            if (nRet == -1)
            {
                OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from nDSP_wait_videnc_comm_thread\n");
                OMX_VIDENC_SET_ERROR_BAIL(eError,
                                          OMX_ErrorHardware,
                                          pComponentPrivate);
            }

            if(DSP_ACK != eCmd)
            {
                ALOGE("Unknown command received from the DSP wait thread");
            }
        }
    }

OMX_CONF_CMD_BAIL:
    return eError;

}

/*----------------------------------------------------------------------------*/
/**
  * readBuffersAndIssueProcess()
  *
  * Called by OMX_VIDENC_Thread function.
  *
  * @param pComponentPrivate
  *        threadExit
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE readBuffersAndIssueProcess(VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
    int                     nRet                 = -1;
    OMX_ERRORTYPE           eError               = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE*   pInputBufHead        = NULL;
    OMX_BUFFERHEADERTYPE*   pOutputBufHead       = NULL;
    VIDENC_BUFFER_PRIVATE*  pInputBufferPrivate  = NULL;
    VIDENC_BUFFER_PRIVATE*  pOutputBufferPrivate = NULL;

    /* Read an input buffer from the pipe */
    nRet = read(pComponentPrivate->nFilled_iPipe[0], &(pInputBufHead), sizeof(pInputBufHead));
    if (nRet == -1)
    {
        OMX_TRACE4(pComponentPrivate->dbg, "Error while reading from the input pipe\n");
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
    }

    if (pInputBufHead != NULL)
    {
        pInputBufferPrivate = (VIDENC_BUFFER_PRIVATE*)pInputBufHead->pInputPortPrivate;
    }
    else
    {
        ALOGE("Got an NULL input buffer!!");
    }

    OMX_DBG_CHECK_CMD(pComponentPrivate->dbg, pInputBufHead, pInputBufferPrivate, 1);
    pInputBufferPrivate->bReadFromPipe = OMX_TRUE;

    /* Read an output buffer from the pipe */
    nRet = read(pComponentPrivate->nFree_oPipe[0], &pOutputBufHead, sizeof(pOutputBufHead));
    if (nRet == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "Error while reading from the pipe\n");
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
    }

    if (pOutputBufHead != NULL)
    {
        pOutputBufferPrivate = (VIDENC_BUFFER_PRIVATE*)pOutputBufHead->pOutputPortPrivate;
    }
    else
    {
        ALOGE("Got an NULL output buffer!!");
    }

    OMX_DBG_CHECK_CMD(pComponentPrivate->dbg, pOutputBufHead, pOutputBufferPrivate, 1);
    pOutputBufferPrivate->bReadFromPipe = OMX_TRUE;

    if (pOutputBufferPrivate->eBufferOwner == VIDENC_BUFFER_WITH_DSP ||
        pOutputBufferPrivate->eBufferOwner == VIDENC_BUFFER_WITH_CLIENT)
    {
        ALOGE("Invalid use case the output buffer ownership is with either the DSP or client");
        goto OMX_CONF_CMD_BAIL;
    }

    if(!pOutputBufferPrivate || !pComponentPrivate->pCodecHandle)
    {
        ALOGE("Invalid pointers either outputbuffer or codecHandle ");
        goto OMX_CONF_CMD_BAIL;
    }


    /* We have one input and one output buffer with us. One frame encoding   */
    /* can be issued now                                                     */
    eError = OMX_VIDENC_process(pComponentPrivate, pInputBufHead, pOutputBufHead);
    if(OMX_ErrorNone != eError)
    {
        OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_VIDENC_process");
        goto OMX_CONF_CMD_BAIL;
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_VideoEnc_Thread()
  *
  * Called by VIDENC_Start_ComponentThread function.
  *
  * This is the main thread of the OMXIL component which will handle all the
  * OMX commands from IL client and also check for buffer availability.
  * Other than commands, this thread first checks if an input buffer is
  * available and then checks if an output buffer is available. If both buffers
  * are available, it triggers a DSP encoding and signals DSP wait thread to
  * issue corresponding ARM encoding and DSP wait.
  *
  * @param pThreadData
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
void* OMX_VIDENC_Thread (void* pThreadData)
{
    int status = -1;
    int fdmax = -1;
    int fdmaxOutputPipe = -1;
    fd_set rfds;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BOOL threadExit = OMX_FALSE;
    int nRet = -1;
    VIDENC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    sigset_t set;
    OMX_COMMANDTYPE eCmd = -1;

    if (!pThreadData)
    {
        eError = OMX_ErrorBadParameter;
        goto OMX_CONF_CMD_BAIL;
    }

    pComponentPrivate = (VIDENC_COMPONENT_PRIVATE*)pThreadData;

#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->pPERFcomp = PERF_Create(PERF_FOURCC('V', 'E', ' ', 'T'),
                                               PERF_ModuleComponent |
                                               PERF_ModuleVideoEncode);
#endif

    /* First we wait on only the input and command pipes. This wait is an     */
    /* infinite wait. If a command is received then that command will be      */
    /* processed and the control will come back and wait for further inputs   */
    /* or commands to come. If an input buffer is received then it waits for  */
    /* an output buffer to issue one frame encoding process.                  */

    /** Looking for highest number of file descriptor
        for pipes inorder to put in select loop */

    fdmax = pComponentPrivate->nCmdPipe[0];

    if (pComponentPrivate->nFilled_iPipe[0] > fdmax)
    {
        fdmax = pComponentPrivate->nFilled_iPipe[0];
    }

    while (1)
    {
        /* Wait for commands or input buffers to come into the OMX component   */
        FD_ZERO (&rfds);
        FD_SET (pComponentPrivate->nCmdPipe[0], &rfds);
        FD_SET (pComponentPrivate->nFilled_iPipe[0], &rfds);

        sigemptyset(&set);
        sigaddset(&set,SIGALRM);

        status = pselect(fdmax+1, &rfds, NULL, NULL, NULL, &set);

        if (0 == status)
        {
            ALOGD("The timeout value is specified as NULL should never come here");
            sched_yield();
        }
        else if (-1 == status)
        {
            if (pComponentPrivate->eState != OMX_StateLoaded)
            {
                OMX_TRACE3(pComponentPrivate->dbg, "select() error.\n");
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorHardware, 0, NULL);
            }
            OMX_VIDENC_SET_ERROR_BAIL(eError, OMX_ErrorHardware, pComponentPrivate);
        }
        else
        {
            if (FD_ISSET(pComponentPrivate->nCmdPipe[0], &rfds))
            {
                /* Received command from IL client. Check the command and take appropriate action */
                eError = handleCommand(pComponentPrivate, &threadExit);
                if(OMX_ErrorNone != eError)
                {
                   ALOGE("Failure to handle the command, bailing out");
                   goto OMX_CONF_CMD_BAIL;
                }

                /* Received shut down command from IL client. Flag DSP wait thread to exit */
                if(OMX_TRUE == threadExit)
                {
                    eCmd = -1;

                    ALOGD("Sending EXIT to nDsp_wait_thread_event_pipe");
                    nRet = write(pComponentPrivate->nDsp_wait_thread_event_pipe[1], &eCmd, sizeof(eCmd));
                    if (nRet == -1)
                    {
                        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                       pComponentPrivate->dbg, OMX_PRCOMM4,
                                       "Failed to write to nCmdPipe.\n");
                    }

                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorNone);
                }
                /* Once the event has been handled go back and wait for more   */
                /* events or input buffers to come in                          */
                continue;
            }

            if((FD_ISSET(pComponentPrivate->nFilled_iPipe[0], &rfds)) &&
                (pComponentPrivate->eState != OMX_StatePause &&
                pComponentPrivate->eState != OMX_StateIdle))
            {
                /* One input buffer is aviable. Now check if an output        */
                /* buffer also available                                      */

                FD_ZERO (&rfds);
                FD_SET (pComponentPrivate->nFree_oPipe[0], &rfds);
				FD_SET (pComponentPrivate->nCmdPipe[0], &rfds);

                fdmaxOutputPipe = pComponentPrivate->nFree_oPipe[0];

				if(pComponentPrivate->nCmdPipe[0] > fdmaxOutputPipe)
					fdmaxOutputPipe = pComponentPrivate->nCmdPipe[0];

                sigemptyset(&set);
                sigaddset(&set,SIGALRM);

                /* TODO - Do we need to handle events here in this pselect?  */
                /* Is there any situation where you will get a command but   */
                /* you won't get any further output buffers? If yes, then it */
                /* will hang here indefinitely.                              */

                /* Review comment: Yes, check for even here as well */

                status = pselect(fdmaxOutputPipe+1, &rfds, NULL, NULL, NULL, &set);

                if (0 == status)
                {
                    ALOGD("The timeout value is specified as NULL should never come here");
                    sched_yield();
                }
                else if (-1 == status)
                {
                    if (pComponentPrivate->eState != OMX_StateLoaded)
                    {
                        OMX_TRACE3(pComponentPrivate->dbg, "select() error.\n");
                        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorHardware, 0, NULL);
                    }
                    OMX_VIDENC_SET_ERROR_BAIL(eError, OMX_ErrorHardware, pComponentPrivate);
                }
                else if (FD_ISSET(pComponentPrivate->nCmdPipe[0], &rfds))
				{
					/* Received command from IL client. Check the command and take appropriate action */
					eError = handleCommand(pComponentPrivate, &threadExit);
					if(OMX_ErrorNone != eError)
					{
					   ALOGE("Failure to handle the command, bailing out");
					   goto OMX_CONF_CMD_BAIL;
					}

					/* Received shut down command from IL client. Flag DSP wait thread to exit */
					if(OMX_TRUE == threadExit)
					{
						eCmd = -1;

						ALOGD("Sending EXIT to nDsp_wait_thread_event_pipe");
						nRet = write(pComponentPrivate->nDsp_wait_thread_event_pipe[1], &eCmd, sizeof(eCmd));
						if (nRet == -1)
						{
							OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
										   pComponentPrivate->dbg, OMX_PRCOMM4,
										   "Failed to write to nCmdPipe.\n");
						}

						OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorNone);
					}
					/* Once the event has been handled go back and wait for more   */
					/* events or input buffers to come in                          */
					continue;
				}			
                else if(FD_ISSET(pComponentPrivate->nFree_oPipe[0], &rfds) &&
                                (pComponentPrivate->eState != OMX_StatePause &&
                                 pComponentPrivate->eState != OMX_StateIdle))
                {
                    /* Check if you have triggered more than                 */
                    /* MAX_BUFFERS_TO_DSP (=2 for one frame stag             */
                    /* ger encoder ) buffers to DSP, if yes wait for at      */
                    /* least one buffer to be completed                      */

                    if(pComponentPrivate->buffersTriggered >= MAX_BUFFERS_TO_DSP)
                    {
                        //ALOGD("Going into waitForACKFromDSPWaitThread");
                        //ALOGD("%lu >= %d", pComponentPrivate->buffersTriggered, MAX_BUFFERS_TO_DSP);
                        eError = waitForACKFromDSPWaitThread(pComponentPrivate);
                        if(OMX_ErrorNone != eError)
                        {
                           ALOGE("error from waitForACKFromDSPWaitThread, bailing out");
                           goto OMX_CONF_CMD_BAIL;
                        }
                        //ALOGD("Out of waitForACKFromDSPWaitThread");
                    }

                    eError = readBuffersAndIssueProcess(pComponentPrivate);
                    if(OMX_ErrorNone != eError)
                    {
                       ALOGE("error from readBuffersAndIssueProcess, bailing out");
                       goto OMX_CONF_CMD_BAIL;
                    }
                }
            }
        }
    }/* while(1) */

OMX_CONF_CMD_BAIL:


#ifdef __PERF_INSTRUMENTATION__
    PERF_Done(pComponentPrivate->pPERFcomp);
#endif

    ALOGD("Bailing out of the videnc thread");
    OMX_PRINT4(pComponentPrivate->dbg, "OMX_VIDENC_Thread Exits\n");
    return (void*)eError;
}
