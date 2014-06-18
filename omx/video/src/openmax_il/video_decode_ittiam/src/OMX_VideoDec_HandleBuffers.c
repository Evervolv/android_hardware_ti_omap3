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
/*  File Name         : OMX_VideoDec_HandleBuffers.c                         */
/*                                                                           */
/*  Description       : This file contains the functions that handle	     */
/*						buffer movement between different modules    		 */
/*                                                                           */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          17 11 2009   A.D.Almeida     Draft                               */
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
#include "ittiam_logo.h"

//sasken start
#define ENABLE_GRALLOC_BUFFERS
#define USE_ION
#ifdef USE_ION
#include <sys/mman.h>
#include <sys/eventfd.h>
#include <ion.h>
#endif

#define LOG_NDEBUG 0
#include <utils/Log.h>
#include <stdlib.h>
#include "hal_public.h" 
//sasken stop

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_VideoDec_HandleBuffers"
#include <utils/Log.h>

/*****************************************************************************/
/* Variables used for profiling                                              */
/*****************************************************************************/
#if DUMP_DSP_PROCESS_TIME
unsigned long long dumpDspProcessTime[MAX_PROFILE];
int dumpDspProcessTimeIndex = 1;
FILE *gDumpDspProcessTime = NULL;
#endif /* DUMP_DSP_PROCESS_TIME */

#if INP_DATA_BUF_PROFILE
unsigned long long inpDataBufFromApp[MAX_PROFILE];
int inpDataBufFromAppIndex = 0;
FILE *gInpDataBufAppFP = NULL;
#endif /* INP_DATA_BUF_PROFILE */

#if INP_DATA_BUF_DONE_PROFILE
unsigned long long inpDataBufDoneBefore[MAX_PROFILE];
int inpDataBufDoneBeforeIndex = 0;
unsigned long long inpDataBufDoneAfter[MAX_PROFILE];
int inpDataBufDoneAfterIndex = 0;
FILE *gInpDataBufDoneFP = NULL;
#endif /* INP_DATA_BUF_DONE_PROFILE */


#if OUT_DATA_BUF_DONE_PROFILE
unsigned long long outDataBufDoneBefore[MAX_PROFILE];
int outDataBufDoneBeforeIndex = 0;
unsigned long long outDataBufDoneAfter[MAX_PROFILE];
int outDataBufDoneAfterIndex = 0;
FILE *gOutDataBufDoneFP = NULL;
#endif /* OUT_DATA_BUF_DONE_PROFILE */

#if LCML_OUT_BUF_ISSUE_PROFILE
unsigned long long lcmlOutBufIssue[MAX_PROFILE];
int lcmlOutBufIssueIndex = 0;
FILE *gLcmlOutBufIssueProfileFP = NULL;
#endif /* LCML_OUT_BUF_ISSUE_PROFILE */

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleFreeOutputBufferFromApp                     */
/*  Description   : Receives a free output buffer and calls a function to    */
/*					queue it to the DSP                                      */
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
OMX_ERRORTYPE VIDDEC_HandleFreeOutputBufferFromApp(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE 		   eError 			= OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE   *pBuffHead;
    OMX_U32 			   size_out_buf;
    OMX_S32                ret;
    VIDDEC_BUFFER_PRIVATE  *pBufferPrivate 	= NULL;

    LOG_BUF("+++ENTERING\n");
    size_out_buf = (OMX_U32)pComponentPrivate->pOutPortDef->nBufferSize;

	/* Reads the output buffer that was written into this pipe in FTB */
    ret = read(pComponentPrivate->free_outBuf_Q[0],
										&pBuffHead, sizeof(pBuffHead));
    if (ret == -1)
    {
        LOG_BUF("Error while reading from the pipe\n");
        eError = OMX_ErrorHardware;
        goto EXIT;
    }
    LOG_BUF("pBuffHead 0x%p eExecuteToIdle 0x%x\n",
								pBuffHead, pComponentPrivate->eExecuteToIdle);

    if(pBuffHead->pOutputPortPrivate != NULL)
    {
        pBufferPrivate = (VIDDEC_BUFFER_PRIVATE *)pBuffHead->pOutputPortPrivate;
#ifdef VIDDEC_FLAGGED_EOS
        if (!pComponentPrivate->bUseFlaggedEos)
        {
#endif
            if(((pComponentPrivate->bPlayCompleted == 0) &&
			    (pComponentPrivate->bBuffFound == OMX_FALSE)) ||
               ((pComponentPrivate->bPlayCompleted == 1) &&
                (pComponentPrivate->bBuffFound == OMX_TRUE)))
            {
				/* This function does LCML_Queuebuffer of the output buffer */
 //sasken start
		if(pComponentPrivate->pCompPort[1]->VIDDECBufferType == GrallocPointers) {
           
		eError = VIDDEC_LCML_QueueOutputBuffer(pComponentPrivate,pBuffHead,pBuffHead->pPlatformPrivate);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("LCML_QueueBuffer 0x%x\n"
											, eError);
                    eError = OMX_ErrorHardware;
                    goto EXIT;
               }
	}else 
		{
           
		eError = VIDDEC_LCML_QueueOutputBuffer(pComponentPrivate,pBuffHead,pBuffHead->pBuffer);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("LCML_QueueBuffer 0x%x\n", eError);
                    eError = OMX_ErrorHardware;
                    goto EXIT;
                }
            }
			// sasken stop
			}
            else
            {
                pComponentPrivate->bBuffFound = OMX_TRUE;

                /* This case is for tunneled components. Not used presently */
                if (pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
                {
					pBuffHead->nFlags           |= OMX_BUFFERFLAG_EOS;
					pBuffHead->nFilledLen        = 0;
					pBufferPrivate->eBufferOwner =
											VIDDEC_BUFFER_WITH_TUNNELEDCOMP;

					LOG_BUF("tunnel eBufferOwner 0x%x\n",
									pBufferPrivate->eBufferOwner);
#ifdef __PERF_INSTRUMENTATION__
					PERF_SendingFrame(pComponentPrivate->pPERFcomp,
									  pBuffHead->pBuffer,
									  pBuffHead->nFilledLen,
									  PERF_ModuleLLMM);
#endif
					LOG_BUF("nFlags %x nFilledLen %x\n",
						(int)pBuffHead->nFlags,(int)pBuffHead->nFilledLen);

					VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);
					eError = OMX_EmptyThisBuffer(
							pComponentPrivate->pCompPort[1]->hTunnelComponent,
							pBuffHead);
                }
                else
                {
                    pBuffHead->nFlags           |= OMX_BUFFERFLAG_EOS;
                    pBuffHead->nFilledLen        = 0;
					pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;

                    LOG_BUF("Output Buffer marked as eos and sent back nFlags %x nFilledLen %x\n\n",
						(int)pBuffHead->nFlags,(int)pBuffHead->nFilledLen);

                    VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);

                    pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventBufferFlag,
							VIDDEC_OUTPUT_PORT,
							OMX_BUFFERFLAG_EOS,
							NULL);

                    pComponentPrivate->cbInfo.FillBufferDone(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							pBuffHead);

					/* Decreasing the Output buffer from app count after FBD */
					pthread_mutex_lock(&pComponentPrivate->
												OutputBCountAppMutex);
					pComponentPrivate->bOutputBCountApp--;
					pthread_mutex_unlock(&pComponentPrivate->
												OutputBCountAppMutex);
                }
            }
#ifdef VIDDEC_FLAGGED_EOS
        }
        else
        {
            if(pComponentPrivate->bPlayCompleted == 0 ||
              (pComponentPrivate->bBuffFound == OMX_TRUE &&
               pComponentPrivate->bPlayCompleted == 1))
            {
#if LCML_OUT_BUF_ISSUE_PROFILE
    lcml_out_buff_issue_profile();
#endif
//sasken start
		if(pComponentPrivate->pCompPort[1]->VIDDECBufferType == GrallocPointers) {
           
		eError = VIDDEC_LCML_QueueOutputBuffer(pComponentPrivate,pBuffHead,pBuffHead->pPlatformPrivate);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("LCML_QueueBuffer 0x%x\n"
											, eError);
                    eError = OMX_ErrorHardware;
                    goto EXIT;
                }
	}else 
		{
           
		eError = VIDDEC_LCML_QueueOutputBuffer(pComponentPrivate,pBuffHead,pBuffHead->pBuffer);
                if (eError != OMX_ErrorNone)
                {
                    LOG_BUF("LCML_QueueBuffer 0x%x\n"
											, eError);
                    eError = OMX_ErrorHardware;
                    goto EXIT;
                }
            }
			// sasken stop
			}
            else
            {
				/* if the last output buffer wans't marked then return the  */
				/* next output buffer                                       */
                pComponentPrivate->bBuffFound = OMX_TRUE;
                if (pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
                {
					pBuffHead->nFlags           |= OMX_BUFFERFLAG_EOS;
					pBuffHead->nFilledLen 		 = 0;
					pBufferPrivate->eBufferOwner =
									VIDDEC_BUFFER_WITH_TUNNELEDCOMP;

					LOG_BUF("tunnel eBufferOwner 0x%x\n",
										pBufferPrivate->eBufferOwner);
					LOG_BUF("nFlags %x nFilledLen %x\n",
						(int)pBuffHead->nFlags,(int)pBuffHead->nFilledLen);

					VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);

					eError = OMX_EmptyThisBuffer(
						pComponentPrivate->pCompPort[1]->hTunnelComponent,
						pBuffHead);
                }
                else
                {
                    pBuffHead->nFlags           |= OMX_BUFFERFLAG_EOS;
                    pBuffHead->nFilledLen        = 0;
                    pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;

                    LOG_BUF("nFlags %x nFilledLen %x\n\n",
						(int)pBuffHead->nFlags,(int)pBuffHead->nFilledLen);

                    LOG_BUF("Returning buffer STANDALONE buffer eBufferOwner 0x%x\n",
											pBufferPrivate->eBufferOwner);

                    VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);

                    pComponentPrivate->cbInfo.EventHandler(
								pComponentPrivate->pHandle,
								pComponentPrivate->pHandle->pApplicationPrivate,
								OMX_EventBufferFlag,
								VIDDEC_OUTPUT_PORT,
								OMX_BUFFERFLAG_EOS,
								NULL);

                    pComponentPrivate->cbInfo.FillBufferDone(
								pComponentPrivate->pHandle,
								pComponentPrivate->pHandle->pApplicationPrivate,
								pBuffHead);

					/* Decreasing the Output buffer from app count after FBD */
					pthread_mutex_lock(&pComponentPrivate->
														OutputBCountAppMutex);
					pComponentPrivate->bOutputBCountApp--;
					pthread_mutex_unlock(&pComponentPrivate->
														OutputBCountAppMutex);
                }
            }
        }
#endif
    }
    else
    {
        LOG_BUF("null element ****\n");
    }

EXIT:
    LOG_BUF("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleDataBuf_FromApp                             */
/*  Description   : Called from ASC thread for header parsing and queueing   */
/*					of input timestamps                                      */
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
OMX_ERRORTYPE VIDDEC_HandleDataBuf_FromApp(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							OMX_BUFFERHEADERTYPE     *pBuffHead,
							OMX_U32                  *bdoNotProcess)
{
    OMX_ERRORTYPE 		  eError = OMX_ErrorNone;
    VIDDEC_BUFFER_PRIVATE *pBufferPrivate = NULL;
    OMX_U32 			  inpBufSize;
	OMX_U32 			  tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;
    static int count = 0;

    LOG_BUF("+++ENTERING\n");
    LOG_BUF("pComponentPrivate 0x%p iEndofInputSent 0x%x\n",
						pComponentPrivate, pComponentPrivate->iEndofInputSent);

    inpBufSize = pComponentPrivate->pInPortDef->nBufferSize;

    if(pComponentPrivate->pInPortDef->format.video.nFrameWidth != MULTIPLE_32(pComponentPrivate->pInPortDef->format.video.nFrameWidth))
    {
    if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
	if(0 == count)
        {
			pComponentPrivate->controlCallNeeded = OMX_TRUE;
			// Make callback for cropping
		pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
			                               pComponentPrivate->pHandle->pApplicationPrivate,
			                               OMX_EventPortSettingsChanged,
			                               VIDDEC_OUTPUT_PORT,
			                               OMX_IndexConfigCommonOutputCrop,
			                               NULL);		
	}
    }
    }
    ++count;

#ifdef PARSE_HEADER
    if((((tempCompressionFormat == OMX_VIDEO_CodingAVC)   ||
         (tempCompressionFormat== OMX_VIDEO_CodingMPEG4)  ||
         (tempCompressionFormat == OMX_VIDEO_CodingH263)) ||
        ((tempCompressionFormat == OMX_VIDEO_CodingWMV)   &&
             pComponentPrivate->ProcessMode == 1)) &&
            pComponentPrivate->bParserEnabled &&
            (pComponentPrivate->bFirstHeader == OMX_FALSE ||
             (pComponentPrivate->bFirstHeader == OMX_TRUE &&
             pBuffHead->nFlags & OMX_BUFFERFLAG_CODECCONFIG)))
    		/* Condition add it to catch VTC duplicating the config buffers */
    {
        pComponentPrivate->bFirstHeader = OMX_TRUE;
        /* If VIDDEC_ParseHeader() does not return OMX_ErrorNone, then        */
        /* reconfiguration is required.                                       */
        /* eError is set to OMX_ErrorNone after saving the buffer, which will */
        /* be used later by the reconfiguration logic.                        */
        eError = VIDDEC_ParseHeader(pComponentPrivate, pBuffHead);

        /* Check first if resolution is supported */
        if(eError == OMX_ErrorUnsupportedSetting)
        {
            /* Here we start the process of error handling since    */
            /*  component has start but resolution is NOT supported */
            LOG_BUF("Handling eError: %x", eError);

            pComponentPrivate->cbInfo.EmptyBufferDone(
							pComponentPrivate->pHandle,
                            pComponentPrivate->pHandle->pApplicationPrivate,
                            pBuffHead);

			/* Decrease the input buffer from APP count after doing EBD */
			pthread_mutex_lock(&pComponentPrivate->InputBCountAppMutex);
			pComponentPrivate->bInputBCountApp--;
			pthread_mutex_unlock(&pComponentPrivate->InputBCountAppMutex);

            LOG_BUF("nLastErrorSeverity = OMX_TI_ErrorCritical");
            LOG_BUF("Sending: OMX_EventError->OMX_ErrorUnsupportedSetting");

            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							eError,
							OMX_ErrorUnsupportedSetting,
			"eError: OMX_ErrorUnsupportedSetting, set it as OMX_TI_ErrorCritical");

            eError = OMX_ErrorNone;
            *bdoNotProcess = 1;
            goto EXIT;
        }
        /* The MPEG4 algorithm expects both the configuration buffer          */
        /* and the first data buffer to be in the same frame - this logic only*/
        /* applies when in frame mode and when the framework sends the config */
        /* data separately. Similar situation is handled elsewhere for H.264  */
        /* & WMV decoding.													  */
        else if(eError != OMX_ErrorNone ||
			   ((tempCompressionFormat == OMX_VIDEO_CodingMPEG4) &&
                 pComponentPrivate->ProcessMode == 0))
        {
            eError = OMX_ErrorNone;
        }
        else
        {
            /* We have received only one part of the Config Buffer, we need */
            /* to wait for more buffers. ONLY FOR AVC                       */
            if(tempCompressionFormat == OMX_VIDEO_CodingAVC &&
                pComponentPrivate->bConfigBufferCompleteAVC == FALSE)
            {
                /* Set bFirstHeader flag to false so next buffer enters to */
                /* ParseHeade again                                        */
                pComponentPrivate->bFirstHeader = OMX_FALSE;
                LOG_BUF("AVC: bConfigBufferCompleateAVC == FALSE!");
                *bdoNotProcess = 1;
                goto EXIT;
            }
            eError = OMX_ErrorNone;
        }
    }
#endif

    LOG_BUF("iEndofInputSent %d, pBuffHead: %p , nFilledLen %lu, EOS %lu\n",
		pComponentPrivate->iEndofInputSent, pBuffHead, pBuffHead->nFilledLen,
		pBuffHead->nFlags & OMX_BUFFERFLAG_EOS);

    pBufferPrivate = (VIDDEC_BUFFER_PRIVATE* )pBuffHead->pInputPortPrivate;
    pBufferPrivate = pBuffHead->pInputPortPrivate;
    pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;

    if(pComponentPrivate->nInCmdMarkBufIndex !=
										pComponentPrivate->nOutCmdMarkBufIndex)
    {
		OMX_U8 tmpInIdx  = pComponentPrivate->nInMarkBufIndex;
		OMX_U8 tmpOutIdx = pComponentPrivate->nOutCmdMarkBufIndex;

        pComponentPrivate->arrMarkBufIndex[tmpInIdx].hMarkTargetComponent =
		pComponentPrivate->arrCmdMarkBufIndex[tmpOutIdx].hMarkTargetComponent;

        pComponentPrivate->arrMarkBufIndex[tmpInIdx].pMarkData =
				pComponentPrivate->arrCmdMarkBufIndex[tmpOutIdx].pMarkData;

        pComponentPrivate->nOutCmdMarkBufIndex++;
        pComponentPrivate->nOutCmdMarkBufIndex %= VIDDEC_MAX_QUEUE_SIZE;
        pComponentPrivate->nInMarkBufIndex++;
        pComponentPrivate->nInMarkBufIndex %= VIDDEC_MAX_QUEUE_SIZE;
    }
    else
    {
		OMX_U8 tmpInIdx  = pComponentPrivate->nInMarkBufIndex;

        pComponentPrivate->arrMarkBufIndex[tmpInIdx].hMarkTargetComponent =
											pBuffHead->hMarkTargetComponent;
        pComponentPrivate->arrMarkBufIndex[tmpInIdx].pMarkData =
											pBuffHead->pMarkData;

        pComponentPrivate->nInMarkBufIndex++;
        pComponentPrivate->nInMarkBufIndex %= VIDDEC_MAX_QUEUE_SIZE;
    }

    pBufferPrivate = (VIDDEC_BUFFER_PRIVATE* )pBuffHead->pInputPortPrivate;

    /* Insert codec data to the 1st frame in WMV */
    if(tempCompressionFormat == OMX_VIDEO_CodingWMV &&
	   pComponentPrivate->ProcessMode == 0 &&
	   pBuffHead->nFilledLen != 0)
    {
#if 0
//	#ifndef PV_OMX_VC1_DEC_TEST
        if(pComponentPrivate->nWMVFileType == VIDDEC_WMV_ELEMSTREAM &&
          (pBuffHead->nFilledLen + 4) <= pBuffHead->nAllocLen &&
          !(pBuffHead->nFlags & OMX_BUFFERFLAG_CODECCONFIG))
        {
            OMX_WMV_INSERT_CODEC_DATA(pBuffHead);
        }
#endif
    }
    if(pBuffHead->nFlags & OMX_BUFFERFLAG_EOS)
    {
        pComponentPrivate->frameCount          = 0;
        pComponentPrivate->EOSHasBeenSentToAPP = OMX_FALSE;
        pComponentPrivate->iEndofInput         = 0x1;
    }

    {
		/* If the input buffer contains codec config data or it is an EOS */
		/* buffer (zero size) do not queue its timestamp                  */
        if(pComponentPrivate->ProcessMode == 0 &&
		  !(pBuffHead->nFlags & OMX_BUFFERFLAG_CODECCONFIG) &&
		  !(pBuffHead->nFlags & OMX_BUFFERFLAG_EOS))
        {
            OMX_PTR pBufferFlags = NULL;
			OMX_S32 ucIndex 	 = 0;

#if DUMP_TIMESTAMPS
            {
				dump_inptimestamp(pBuffHead);
            }
#endif

            ucIndex = VIDDEC_CircBuf_GetHead(pComponentPrivate,
                                             VIDDEC_CBUFFER_TIMESTAMP,
                                             VIDDEC_INPUT_PORT);

            pComponentPrivate->aBufferFlags[ucIndex].nTimeStamp =
													pBuffHead->nTimeStamp;
            pComponentPrivate->aBufferFlags[ucIndex].nTickCount =
													pBuffHead->nTickCount;
            pBuffHead->nFlags &= ~OMX_BUFFERFLAG_EOS;
            pComponentPrivate->aBufferFlags[ucIndex].nFlags = pBuffHead->nFlags;
            pBufferFlags = &pComponentPrivate->aBufferFlags[ucIndex];

			VIDDEC_CircBuf_Add(pComponentPrivate,
							   VIDDEC_CBUFFER_TIMESTAMP,
							   VIDDEC_INPUT_PORT,
							   pBufferFlags);

        }

        LOG_BUF("Input pBuffHead->nTimeStamp %lld\n",	pBuffHead->nTimeStamp);

        pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;
    }
    if((pComponentPrivate->iEndofInput == 0x1) &&
	   (pComponentPrivate->iEndofInputSent == 0))
    {
        pComponentPrivate->iEndofInputSent = 1;
    }

#if INP_DATA_BUF_PROFILE
	inp_data_buff_profile();
#endif

EXIT:
    LOG_BUF("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleDataBuf_FromDsp                             */
/*  Description   : This function receives filled output buffers from DSP    */
/*                  and returns it back to the application                   */
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
OMX_ERRORTYPE VIDDEC_HandleDataBuf_FromDsp(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE         eError = OMX_ErrorNone;
    OMX_ERRORTYPE         eExtendedError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE  *pBuffHead;
    VIDDEC_BUFFER_PRIVATE *pBufferPrivate = NULL;
	OMX_S32 			  ret;
	OMX_U32 			  tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;
	
//sasken start
  	IMG_native_handle_t*  grallocHandle;
//sasken stop

#if DUMP_DSP_OUT_PARAMS
    FILE *fp;
    static frameCount = 0;
#endif

	/* Read the filled output buffer pipe which has been written in */
	/* LCML_Callback after receiving it from the DSP                */
    ret = read(pComponentPrivate->filled_outBuf_Q[0],
			   &pBuffHead, sizeof(pBuffHead));
    if (ret == -1)
    {
        LOG_BUF("Error while reading from dsp out pipe\n");
        eError = OMX_ErrorHardware;
        goto EXIT;
    }
    if (pComponentPrivate->eState == OMX_StateLoaded ||
		pComponentPrivate->eState == OMX_StateIdle)
	{
        eError = OMX_ErrorNone;
        goto EXIT;
    }

    LOG_BUF_META("FBD: pBuffHead %p Index %lu nAllocLen %lu nFilledLen %lu nFlags= %lu",
			pBuffHead, pBuffHead->nInputPortIndex, pBuffHead->nAllocLen,
			pBuffHead->nFilledLen, pBuffHead->nFlags);

    if(pBuffHead != NULL)
    {
        OMX_S32 				nErrorCode = 0;
        OMX_S32 			    nInternalErrorCode = 0;
        OMX_U32 	      	    ulDisplayID = 0;
        ITTVDEC_UALGOutputParam *pUalgOutParams = NULL;

        pBufferPrivate = (VIDDEC_BUFFER_PRIVATE *)pBuffHead->pOutputPortPrivate;
        pBuffHead->nFlags &= ~(OMX_BUFFERFLAG_SYNCFRAME);
        pBuffHead->nFlags &= ~(VIDDEC_BUFFERFLAG_FRAMETYPE_MASK);
        pBuffHead->nFlags &= ~(VIDDEC_BUFFERFLAG_EXTENDERROR_MASK);

		pUalgOutParams = (ITTVDEC_UALGOutputParam *)pBufferPrivate->pUalgParam;
		nErrorCode     = (pUalgOutParams->iErrorCode);
		ulDisplayID    = pUalgOutParams->ulDisplayID;

#if DUMP_DSP_PROCESS_TIME
		if(dumpDspProcessTimeIndex < MAX_PROFILE)
		{
			dumpDspProcessTime[dumpDspProcessTimeIndex++] =
										pUalgOutParams->dspProcessTime;
		}
#endif
#if DUMP_DSP_OUT_PARAMS
		frameCount++;
		LOG_BUF("DSP_OUT_PARAM Frame count %d\n", frameCount);
		LOG_BUF("DSP_OUT_PARAM ulDisplayID %d\n", pUalgOutParams->ulDisplayID);
		LOG_BUF("DSP_OUT_PARAM uBytesConsumed %d\n", pUalgOutParams->uBytesConsumed);
		LOG_BUF("DSP_OUT_PARAM iErrorCode %d\n", pUalgOutParams->iErrorCode);
		LOG_BUF("DSP_OUT_PARAM ulDecodedFrameType %d\n", pUalgOutParams->ulDecodedFrameType);
#endif

		if(pUalgOutParams->ulLastFrame == 1)
		{
			LOG_BUF("$$$$$$$$$$ Setting to eos $$$$$$$$ \n");
			pBuffHead->nFlags |= OMX_BUFFERFLAG_EOS;
		}

		//sUMUKH!!!  Need to check this .... WMV gives error for everything
		/*if(0 != pUalgOutParams->dspExtendedError)
		{
			pBuffHead->nFlags    |= OMX_BUFFERFLAG_DECODEONLY;
			pBuffHead->nFilledLen = 0;
		}*/

	   if(pUalgOutParams->ulDecodedFrameType == VIDDEC_I_FRAME)
		{
			pBuffHead->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
			pBuffHead->nFlags |= VIDDEC_BUFFERFLAG_FRAMETYPE_I_FRAME;
		}
		else if(pUalgOutParams->ulDecodedFrameType == VIDDEC_P_FRAME)
		{
			pBuffHead->nFlags |= VIDDEC_BUFFERFLAG_FRAMETYPE_P_FRAME;
		}
		else if(pUalgOutParams->ulDecodedFrameType == VIDDEC_B_FRAME)
		{
			pBuffHead->nFlags |= VIDDEC_BUFFERFLAG_FRAMETYPE_B_FRAME;
		}
		else
		{
			pBuffHead->nFlags |= VIDDEC_BUFFERFLAG_FRAMETYPE_IDR_FRAME;
		}

		pBuffHead->nFlags |= (nErrorCode<<12);

		if((nErrorCode & 0xff) != 0)
		{
			nInternalErrorCode =
				((nErrorCode & VIDDEC_BUFFERFLAG_EXTENDERROR_DIRTY)>>12);

			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_FATALERROR))
			{
				eExtendedError        = OMX_ErrorStreamCorrupt;
				pBuffHead->nFlags    |= OMX_BUFFERFLAG_DATACORRUPT;
				pBuffHead->nFilledLen = 0;
				LOG_BUF("Not Recoverable Error Detected in buffer %p %lu(int# %lx/%lu) OMX_ErrorStreamCorrupt\n",
					pBuffHead, ulDisplayID,
					(nErrorCode & VIDDEC_BUFFERFLAG_EXTENDERROR_DIRTY),
					pBuffHead->nFilledLen);
				ALOGE("Not Recoverable Error Detected in buffer");
			}
			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_APPLIEDCONCEALMENT))
			{
					pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
					LOG_BUF("Applied Concealment in buffer %p %lu(int# %lx/%lu)\n",
						pBuffHead, ulDisplayID, nInternalErrorCode,
						pBuffHead->nFilledLen);
			}
			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_INSUFFICIENTDATA))
			{
					pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
					pBuffHead->nFilledLen = 0;
					LOG_BUF("Insufficient Data in buffer %p %lu(int# %lx/%lu)\n",
						pBuffHead, ulDisplayID,
						nInternalErrorCode, pBuffHead->nFilledLen);
			}
			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_CORRUPTEDDATA))
			{
					pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
					pBuffHead->nFilledLen = 0;
					LOG_BUF("Corrupted Data in buffer %p %lu(int# %lx/%lu)\n",
						pBuffHead, ulDisplayID,
						nInternalErrorCode, pBuffHead->nFilledLen);
			}
			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_CORRUPTEDHEADER))
			{
					pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
					pBuffHead->nFilledLen = 0;
					LOG_BUF("Corrupted Header in buffer %p %lu(int# %lx/%lu)\n",
						pBuffHead, ulDisplayID,
						nInternalErrorCode, pBuffHead->nFilledLen);
			}
			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_UNSUPPORTEDINPUT))
			{
					pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
					pBuffHead->nFilledLen = 0;
					LOG_BUF("Unsupported Input in buffer %p %lu(int# %lx/%lu)\n",
								pBuffHead, ulDisplayID,
								nInternalErrorCode, pBuffHead->nFilledLen);
			}
			if(VIDDEC_ISFLAGSET(nErrorCode,VIDDEC_XDM_UNSUPPORTEDPARAM))
			{
					pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
					pBuffHead->nFilledLen = 0;
					LOG_BUF("Unsupported Parameter in buffer %p %lu(int# %lx/%lu)\n",
						pBuffHead, ulDisplayID, nInternalErrorCode,
						pBuffHead->nFilledLen);
			}
		}

		VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);
		pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;

		if((pBuffHead->nFlags & OMX_BUFFERFLAG_EOS))
		{
			pComponentPrivate->bBuffFound = OMX_TRUE;
			pComponentPrivate->cbInfo.EventHandler(
				pComponentPrivate->pHandle,
				pComponentPrivate->pHandle->pApplicationPrivate,
				OMX_EventBufferFlag,
				VIDDEC_OUTPUT_PORT,
				OMX_BUFFERFLAG_EOS,
				NULL);

			pComponentPrivate->EOSHasBeenSentToAPP = OMX_TRUE;


		}

#if DUMP_YUV
		dump_yuv(pBuffHead);
#endif

#if OUT_DATA_BUF_DONE_PROFILE
		out_data_buf_done_profile(1);
#endif

#if DUMP_TIMESTAMPS
		dump_outtimestamp(pBuffHead);
#endif

#ifdef ENABLE_ITTIAM_LOGO
		if(pBuffHead->nFilledLen > 0)
		{
			insert_logo((char *)pBuffHead->pBuffer,
					 (char *)g_ittiam_logo_array,
					 pComponentPrivate->pOutPortDef->format.video.nFrameHeight,
					 pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
					 40,
					 90);
		}
#endif
		/* WMV decoder gives zero byte output buffers which is causing audio */
		/* to seek back to the beginning in SF(fix for Lock/Unlock and flush */
		/*  cases). Also, in PV zero buffers are returned after EOS          */
		if((OMX_VIDEO_CodingWMV == tempCompressionFormat) &&
			(0 == pBuffHead->nFilledLen) &&
			(OMX_TRUE == pComponentPrivate->isOpBufFlushComplete) &&
			!(OMX_TRUE == pComponentPrivate->EOSHasBeenSentToAPP))
		{
			ret = write (pComponentPrivate->free_outBuf_Q[1],
								&(pBuffHead), sizeof (pBuffHead));
		}
		else
		{
			//sasken start
			if(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->VIDDECBufferType == GrallocPointers) {
	    			grallocHandle = (IMG_native_handle_t*)pBuffHead->pBuffer;

				pComponentPrivate->grallocModule->unlock((gralloc_module_t const *) pComponentPrivate->grallocModule,
						                         (buffer_handle_t)grallocHandle);
			}
			//sasken stop

			pComponentPrivate->cbInfo.FillBufferDone(
					pComponentPrivate->pHandle,
					pComponentPrivate->pHandle->pApplicationPrivate,
					pBuffHead);

			/* Decreasing the Output buffer from app count after FBD */
			pthread_mutex_lock(&pComponentPrivate->OutputBCountAppMutex);
			pComponentPrivate->bOutputBCountApp--;
			pthread_mutex_unlock(&pComponentPrivate->OutputBCountAppMutex);

			/* Decreasing o/p buffer from DSP count also */
			pthread_mutex_lock(&pComponentPrivate->OutputBCountDspMutex);
			pComponentPrivate->bOutputBCountDsp--;
			pthread_mutex_unlock(&pComponentPrivate->OutputBCountDspMutex);
		}

		if(0 == pComponentPrivate->bOutputBCountDsp)
		    pComponentPrivate->isOpBufFlushComplete = OMX_TRUE;

		LOG_BUF_COUNT("FBD: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
			pComponentPrivate->bInputBCountApp,
			pComponentPrivate->bOutputBCountApp,
			pComponentPrivate->bInputBCountDsp,
			pComponentPrivate->bOutputBCountDsp);

#if OUT_DATA_BUF_DONE_PROFILE
		out_data_buf_done_profile(0);
#endif

    }
    else
    {
       LOG_BUF("FBD: ISSUE: Got a null buffer \n");
    }

EXIT:
    if(eExtendedError != OMX_ErrorNone)
    {
        eError = eExtendedError;
    }
    LOG_BUF("---EXITING(0x%x)\n",eError);

    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleFreeDataBuf                                 */
/*  Description   : Read From input buffer written by ASC pipe               */
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
OMX_ERRORTYPE VIDDEC_HandleFreeDataBuf(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate )
{
    OMX_ERRORTYPE         eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE  *pBuffHead;
    VIDDEC_BUFFER_PRIVATE *pBufferPrivate = NULL;
    OMX_S32 			  ret;
    OMX_S32 			  inputbufsize =
							(OMX_S32)pComponentPrivate->pInPortDef->nBufferSize;

	/* Read the input buffer that has been written into this pipe by the ASC */
	/* thread after processing                                               */
    ret = read(pComponentPrivate->free_inpBuf_Q[0],
				&pBuffHead, sizeof(pBuffHead));

    if ((pComponentPrivate->eState == OMX_StateLoaded) ||
		(pComponentPrivate->eState == OMX_StateIdle))
	{
        eError = OMX_ErrorNone;
        LOG_BUF("ISSUE Read after state change!\n");
    }

    if (pBuffHead != NULL)
    {
        pBufferPrivate =
					(VIDDEC_BUFFER_PRIVATE *)pBuffHead->pInputPortPrivate;
        pBuffHead->nAllocLen 		 = inputbufsize;
        pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;

        LOG_BUF("EBD: pBuffHead 0x%p eExecuteToIdle 0x%x EOS %lu\n",
				pBuffHead,pComponentPrivate->eExecuteToIdle,
				pBuffHead->nFlags & OMX_BUFFERFLAG_EOS);

#if INP_DATA_BUF_DONE_PROFILE
	if(pBuffHead->nFlags & OMX_BUFFERFLAG_EOS)
		inp_data_buff_done_profile(1, 1);
	else
		inp_data_buff_done_profile(1, 0);
#endif

        pComponentPrivate->cbInfo.EmptyBufferDone(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							pBuffHead);

		/* Decrease the input buffer from APP count after doing EBD */
		pthread_mutex_lock(&pComponentPrivate->InputBCountAppMutex);
		pComponentPrivate->bInputBCountApp--;
		pthread_mutex_unlock(&pComponentPrivate->InputBCountAppMutex);

		LOG_BUF_COUNT("EBD: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
			pComponentPrivate->bInputBCountApp,
			pComponentPrivate->bOutputBCountApp,
			pComponentPrivate->bInputBCountDsp,
			pComponentPrivate->bOutputBCountDsp);

#if INP_DATA_BUF_DONE_PROFILE
		if(pBuffHead->nFlags & OMX_BUFFERFLAG_EOS)
			inp_data_buff_done_profile(0, 1);
		else
			inp_data_buff_done_profile(0, 0);
#endif
    }
    else
    {
       LOG_BUF("ISSUE\n");
    }
    LOG_BUF("---EXITING(0x%x) \n",eError);

    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ReturnBuffers                                     */
/*  Description   : Returning all Input and Output Buffers                   */
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
OMX_ERRORTYPE VIDDEC_ReturnBuffers (VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									OMX_U32  				  nParam1,
									OMX_BOOL 				  bRetDSP)
{
    OMX_U8 				 i = 0;
    OMX_ERRORTYPE        eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pBuffHead;

    LOG_BUF("+++ENTERING\n");
    LOG_BUF("pComponentPrivate 0x%p nParam1 0x%lx bRetDSP 0x%x\n",
									pComponentPrivate,nParam1,bRetDSP);

	/* Return all input buffers */
    if((nParam1 == pComponentPrivate->pInPortFormat->nPortIndex) ||
	   (nParam1 == OMX_ALL))
    {
        for (i = 0; i < pComponentPrivate->pInPortDef->nBufferCountActual; i++)
        {
			VIDDEC_BUFFER_PRIVATE *tmpBufPrivate = pComponentPrivate->
					pCompPort[VIDDEC_INPUT_PORT]->pBufferPrivate[i];

            if((tmpBufPrivate->eBufferOwner == VIDDEC_BUFFER_WITH_DSP) &&
				bRetDSP)
            {
                LOG_BUF("inBuffer 0x%p eBufferOwner 0x%x\n",
							tmpBufPrivate->pBufferHdr,
							tmpBufPrivate->eBufferOwner);

                tmpBufPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;
                tmpBufPrivate->pBufferHdr->nFilledLen = 0;

#ifdef __PERF_INSTRUMENTATION__
                PERF_SendingFrame(pComponentPrivate->pPERFcomp,
						tmpBufPrivate->pBufferHdr->pBuffer,
						tmpBufPrivate->pBufferHdr->nFilledLen,
						PERF_ModuleHLMM);
#endif
				pBuffHead = (OMX_BUFFERHEADERTYPE *)tmpBufPrivate->pBufferHdr;

                eError = pComponentPrivate->cbInfo.EmptyBufferDone(
						(OMX_HANDLETYPE *)pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						pBuffHead);

				/* Decrease the input buffer from APP count after doing EBD */
				pthread_mutex_lock(&pComponentPrivate->InputBCountAppMutex);
				pComponentPrivate->bInputBCountApp--;
				pthread_mutex_unlock(&pComponentPrivate->InputBCountAppMutex);
            }
        }
    }
    /* Return all output buffers */
    if ((nParam1 == pComponentPrivate->pOutPortFormat->nPortIndex) ||
		(nParam1 == OMX_ALL))
    {
		/* The below code is applicable to tunneled components */
        if (pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
												hTunnelComponent != NULL)
        {
            for (i = 0; i < pComponentPrivate->pOutPortDef->nBufferCountActual; i++)
            {
				VIDDEC_BUFFER_PRIVATE *tmpBufPrivate = pComponentPrivate->
							pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i];

				LOG_BUF("tunnelVideoDecBuffer[%x]=%x-%lx\n",
					   i, tmpBufPrivate->eBufferOwner,
					   pComponentPrivate->pOutPortDef->nBufferCountActual);

				LOG_BUF("enter return %lx\n",
					pComponentPrivate->pOutPortDef->nBufferCountActual);

                if((tmpBufPrivate->eBufferOwner == VIDDEC_BUFFER_WITH_DSP) &&
				    bRetDSP)
                {
                    tmpBufPrivate->eBufferOwner =
											VIDDEC_BUFFER_WITH_TUNNELEDCOMP;

					LOG_BUF("Buffer 0x%x eBufferOwner 0x%x\n",
								(int)tmpBufPrivate->pBufferHdr,
								tmpBufPrivate->eBufferOwner);

					tmpBufPrivate->pBufferHdr->nFilledLen = 0;

#ifdef __PERF_INSTRUMENTATION__
                    PERF_SendingFrame(pComponentPrivate->pPERFcomp,
								tmpBufPrivate->pBufferHdr->pBuffer,
								tmpBufPrivate->pBufferHdr->nFilledLen,
								PERF_ModuleLLMM);
#endif

                    LOG_BUF("VideDec->PostProc EmptyThisBuffer\n");
                    LOG_BUF("wait to return buffer\n");

                    pBuffHead = (OMX_BUFFERHEADERTYPE *)tmpBufPrivate->pBufferHdr;

                    VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);

                    eError = OMX_EmptyThisBuffer(pComponentPrivate->
							pCompPort[VIDDEC_OUTPUT_PORT]->hTunnelComponent,
							pBuffHead);
                }
            }
        }
        else
        {
            for (i = 0; i < pComponentPrivate->pOutPortDef->nBufferCountActual;
				 i++)
            {
				VIDDEC_BUFFER_PRIVATE *tmpBufPrivate = pComponentPrivate->
							pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i];

                if(((pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i]->eBufferOwner == VIDDEC_BUFFER_WITH_DSP) ||
                   (pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i]->eBufferOwner == VIDDEC_BUFFER_WITH_COMPONENT))
                   && bRetDSP)
                {
                    LOG_BUF("xBuffer 0x%p eBufferOwner 0x%x\n",
											tmpBufPrivate->pBufferHdr,
											tmpBufPrivate->eBufferOwner);

					tmpBufPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;
					
					tmpBufPrivate->pBufferHdr->nFilledLen = 0;

#ifdef __PERF_INSTRUMENTATION__
                    PERF_SendingFrame(pComponentPrivate->pPERFcomp,
								tmpBufPrivate->pBufferHdr->pBuffer,
								tmpBufPrivate->pBufferHdr->nFilledLen,
								PERF_ModuleHLMM);
#endif

                    pBuffHead = (OMX_BUFFERHEADERTYPE *)tmpBufPrivate->
																pBufferHdr;

                    VIDDEC_Propagate_Mark(pComponentPrivate, pBuffHead);

                    eError = pComponentPrivate->cbInfo.FillBufferDone(
							(OMX_HANDLETYPE *)pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							pBuffHead);
					/* Empty the free output buffer pipe by one element */
                    if(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i]->eBufferOwner == VIDDEC_BUFFER_WITH_COMPONENT)
                    {
                        sigset_t set;
                        struct timespec tv;
                        fd_set rfds;
                        int fdmax = 0;
                        int status;

                        fdmax = pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ];

                        FD_ZERO (&rfds);
                        FD_SET(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ], &rfds);

                        tv.tv_sec = 0;
                        tv.tv_nsec = 1000 * 1000;

                        sigemptyset (&set);
                        sigaddset (&set, SIGALRM);
                        status = pselect (fdmax+1, &rfds, NULL, NULL, &tv, &set);
                        sigdelset (&set, SIGALRM);

                        if (FD_ISSET(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ], &rfds)) {
                            OMX_ERRORTYPE eError = OMX_ErrorNone;
                            OMX_BUFFERHEADERTYPE* pBuffHead;
                            int ret;

                            OMX_PRSTATE2(pComponentPrivate->dbg, "eExecuteToIdle 0x%x\n",pComponentPrivate->eExecuteToIdle);

                            ret = read(pComponentPrivate->free_outBuf_Q[0], &pBuffHead, sizeof(pBuffHead));
                            if (ret == -1)
                            {
                                OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from the pipe\n");
                                eError = OMX_ErrorHardware;
                                pthread_mutex_lock(&pComponentPrivate->OutputBCountAppMutex);
                                pComponentPrivate->bOutputBCountApp--;
                                pthread_mutex_unlock(&pComponentPrivate->OutputBCountAppMutex);
                                goto EXIT;
                            }

                            ALOGD(" Read from pipe %s LINE: %d :: pBuffHead %p",
                                __FUNCTION__, __LINE__, pBuffHead);
                        }
                    }
					/* Decreasing the Output buffer from app count after FBD */
					pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i]->eBufferOwner = VIDDEC_BUFFER_WITH_CLIENT;
					pthread_mutex_lock(&pComponentPrivate->
														OutputBCountAppMutex);
					pComponentPrivate->bOutputBCountApp--;
					pthread_mutex_unlock(&pComponentPrivate->
														OutputBCountAppMutex);
				}
            }
        }
    }

	LOG_BUF_COUNT("after return buffers: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,\n",
				pComponentPrivate->bInputBCountApp,
				pComponentPrivate->bOutputBCountApp,
				pComponentPrivate->bInputBCountDsp,
				pComponentPrivate->bOutputBCountDsp);

EXIT:
    LOG_BUF("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_Propagate_Mark						             */
/*  Description   : Function called for propagate mark from input buffers to */
/*                  output buffers	                                         */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/

OMX_ERRORTYPE VIDDEC_Propagate_Mark(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									OMX_BUFFERHEADERTYPE     *pBuffHead)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    if (pBuffHead->nFilledLen != 0)
    {
        pBuffHead->hMarkTargetComponent = pComponentPrivate->
        								arrMarkBufIndex[pComponentPrivate->
        								nInMarkBufIndex].hMarkTargetComponent;

        pBuffHead->pMarkData = pComponentPrivate->
        						arrMarkBufIndex[pComponentPrivate->
        						nInMarkBufIndex].pMarkData;

        pComponentPrivate->nOutMarkBufIndex++;

        pComponentPrivate->nOutMarkBufIndex %= VIDDEC_MAX_QUEUE_SIZE;
        eError = OMX_ErrorNone;
    }
    if(pBuffHead->hMarkTargetComponent == pComponentPrivate->pHandle)
    {
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventMark");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                        pComponentPrivate->pHandle->pApplicationPrivate,
                        OMX_EventMark,
                        0,
                        0,
                        pBuffHead->pMarkData);
    }

    return eError;
}
/*****************************************************************************/
/*  Function Name : dump_dsp_profile_info					                 */
/*  Description   : Dumps DSP Profile into a file                            */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if DUMP_DSP_PROCESS_TIME
void dump_dsp_profile_info()
{
    int index = 0;
    unsigned long long dspProcessingTimeInUS;
    unsigned long long dspPeak4WindowTimeInUS = 0;
    unsigned long long dspPeakTimeInUS = 0;
    unsigned long long dspAvgTimeInUS = 0;
    ALOGD("Dumping DSP profile info to /tmp/dsp_process_time.txt assuming 800MHz dsp clock\n");
    //gDumpDspProcessTime = fopen("/tmp/dsp_process_time.txt", "w");

    for(index = 0; index < dumpDspProcessTimeIndex; index++)
    {
        dspProcessingTimeInUS = dumpDspProcessTime[index];
        //fprintf(gDumpDspProcessTime, "DSPProcessTime %llu\n", dumpDspProcessTime[index]);

        dspAvgTimeInUS += dspProcessingTimeInUS;

        if(dspProcessingTimeInUS > dspPeakTimeInUS)
        {
            dspPeakTimeInUS = dspProcessingTimeInUS;
        }

        if(index < dumpDspProcessTimeIndex - 4)
        {
            unsigned long long tmp;
            tmp = dumpDspProcessTime[index] + dumpDspProcessTime[index + 1] + dumpDspProcessTime[index + 2] + dumpDspProcessTime[index + 3] ;
            tmp /= 4;
            if(tmp > dspPeak4WindowTimeInUS)
            {
                dspPeak4WindowTimeInUS = tmp;
            }
        }
    }
    dspAvgTimeInUS /= dumpDspProcessTimeIndex;
    /*fprintf(gDumpDspProcessTime, "\n dspAvgTimeInUS         = %llu", dspAvgTimeInUS);
    fprintf(gDumpDspProcessTime, "\n dspPeak4WindowTimeInUS = %llu", dspPeak4WindowTimeInUS);
    fprintf(gDumpDspProcessTime, "\n dspPeakTimeInUS        = %llu", dspPeakTimeInUS);
    fclose(gDumpDspProcessTime);*/
    ALOGD("dspAvgTimeInUS = %llu", dspAvgTimeInUS);
    ALOGD("dspPeak4WindowTimeInUS = %llu", dspPeak4WindowTimeInUS);
    ALOGD("dspPeakTimeInUS = %llu", dspPeakTimeInUS);
}
#endif

/*****************************************************************************/
/*  Function Name : lcml_out_buff_issue_profile				                 */
/*  Description   : Dumps DSP Profile into a file                            */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if LCML_OUT_BUF_ISSUE_PROFILE
void lcml_out_buff_issue_profile()
{
	struct timeval   startTime;
	gettimeofday(&startTime, NULL);
	startTime.tv_usec += startTime.tv_sec * 1000 * 1000;
	if(lcmlOutBufIssueIndex < MAX_PROFILE)
	{
		lcmlOutBufIssue[lcmlOutBufIssueIndex++] = startTime.tv_usec;
	}
	else if(MAX_PROFILE == lcmlOutBufIssueIndex)
	{
		char *prefixStr = "Issuing Output YUV buffer to DSP ";
		int index = 0;
		gLcmlOutBufIssueProfileFP = fopen("/tmp/lcml_out_buf_issue.txt", "w");

		for(index = 0; index < lcmlOutBufIssueIndex; index++)
				fprintf(gLcmlOutBufIssueProfileFP, "\n %s count %u is %llu",
						prefixStr, index, lcmlOutBufIssue[index]);

		fclose(gLcmlOutBufIssueProfileFP);
		lcmlOutBufIssueIndex++;
	}
}
#endif

/*****************************************************************************/
/*  Function Name : out_data_buf_done_profile				                 */
/*  Description   : Dumps DSP Profile into a file                            */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if OUT_DATA_BUF_DONE_PROFILE
void out_data_buf_done_profile(int x)
{
	struct timeval   startTime;
	gettimeofday(&startTime, NULL);
	startTime.tv_usec += startTime.tv_sec * 1000 * 1000;
	char *prefixStr;
	if(outDataBufDoneBeforeIndex < MAX_PROFILE)
	{
		outDataBufDoneBefore[outDataBufDoneBeforeIndex++] =
													startTime.tv_usec;
	}
	else if(outDataBufDoneBeforeIndex == MAX_PROFILE)
	{
		int index = 0;
		if (x)
		{
			char *prefixStr = "FillBufferDone issued to app at ";
					gOutDataBufDoneFP = fopen("/tmp/fill_buf_done_start.txt", "w");
		}
		else
		{
			char *prefixStr = "FillBufferDone returns from app at ";
					gOutDataBufDoneFP = fopen("/tmp/fill_buf_done_end.txt", "w");
		}

		for(index = 0; index < outDataBufDoneBeforeIndex; index++)
			fprintf(gOutDataBufDoneFP, "\n %s is %llu", prefixStr,
							outDataBufDoneBefore[index]);
		fclose(gOutDataBufDoneFP);
	}
}
#endif

/*****************************************************************************/
/*  Function Name : inp_data_buff_done_profile				                 */
/*  Description   : Dumps EBD Profile into a file                            */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
#if INP_DATA_BUF_DONE_PROFILE
void inp_data_buff_done_profile(int isIssued ,int isEOS)
{
	struct timeval startTime;
	char           *prefixStr;

	gettimeofday(&startTime, NULL);
	startTime.tv_usec += startTime.tv_sec * 1000 * 1000;

	if(inpDataBufDoneBeforeIndex < MAX_PROFILE)
	{
		inpDataBufDoneBefore[inpDataBufDoneBeforeIndex++] =	startTime.tv_usec;
	}
	if(isEOS)
	{
		int index = 0;
		if (isIssued)
		{
			char *prefixStr = "INP Data buf done callback issued at ";
			gInpDataBufDoneFP = fopen("/tmp/empty_buf_done_start.txt", "w");
		}

		else
		{
			char *prefixStr = "INP Data buf done callback completed at ";
			gInpDataBufDoneFP = fopen("/tmp/empty_buf_done_end.txt", "w");
		}

		for(index = 0; index < inpDataBufDoneBeforeIndex; index++)
		{
			fprintf(gInpDataBufDoneFP, "\n %s is %llu", prefixStr,
											inpDataBufDoneBefore[index]);
		}
		fclose(gInpDataBufDoneFP);
	}
}
#endif

/*****************************************************************************/
/*  Function Name : inp_data_buff_done_profile				                 */
/*  Description   : Dumps ETB Profile into a file                            */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if INP_DATA_BUF_PROFILE
void inp_data_buff_profile()
{
	struct timeval startTime;

    gettimeofday(&startTime, NULL);
    startTime.tv_usec += startTime.tv_sec * 1000 * 1000;

    if(inpDataBufFromAppIndex < MAX_PROFILE)
    {
        inpDataBufFromApp[inpDataBufFromAppIndex++] = startTime.tv_usec;
    }
    else if(MAX_PROFILE == inpDataBufFromAppIndex)
    {
        char *prefixStr = "INP Data buf from app at ";
        int  index      = 0;

        gInpDataBufAppFP = fopen("/tmp/inp_data_buf_from_app.txt", "w");

        for(index = 0; index < inpDataBufFromAppIndex; index++)
            fprintf(gInpDataBufAppFP, "\n %s is %llu", prefixStr,
					inpDataBufFromApp[index]);

        fclose(gInpDataBufAppFP);
    }
}
#endif

/*****************************************************************************/
/*  Function Name : dump_yuv                 				                 */
/*  Description   : dumps the YUV output to a file                           */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if DUMP_YUV
void dump_yuv(OMX_BUFFERHEADERTYPE  *pBuffHead)
{
	static int count = 0;
	static FILE *fp = NULL;

	if(count >= 0 && count < 10)
	{
		fp = fopen("/tmp/dump.yuv","ab+");
		fwrite(pBuffHead->pBuffer, pBuffHead->nFilledLen, 1, fp);
		fclose(fp);

		ALOGD( "**********************************\n");
		ALOGD( "Dumping the file!!!!\n");
		ALOGD( "**********************************\n");
		count++;
	}
	else
	{
		count++;
	}
}
#endif

/*****************************************************************************/
/*  Function Name : dump_inptimestamp          				                 */
/*  Description   : dumps the i/p timestamp to a file                        */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if DUMP_TIMESTAMPS
void dump_inptimestamp(OMX_BUFFERHEADERTYPE *pBuffHead)
{
		FILE *inptime_stamp;
		inptime_stamp = fopen("/tmp/inpTS.txt", "a");
		fprintf(inptime_stamp, "Input time stamp is %lld\n", pBuffHead->nTimeStamp);
		fclose(inptime_stamp);
}
#endif

/*****************************************************************************/
/*  Function Name : dump_outtimestamp          				                 */
/*  Description   : dumps the o/p timestamp to a file                        */
/*  Inputs        : None                                                     */
/*  Globals       : None                                                     */
/*  Processing    : None                                                     */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         24 10 2008   Ittiam          Draft                                */
/*****************************************************************************/
#if DUMP_TIMESTAMPS
void dump_outtimestamp(OMX_BUFFERHEADERTYPE *pBuffHead)
{
		FILE *outtime_stamp;
		outtime_stamp = fopen("/tmp/outTS.txt", "a");
		fprintf(outtime_stamp, "Output time stamp is %lld\n", pBuffHead->nTimeStamp);
		fclose(outtime_stamp);
}
#endif

