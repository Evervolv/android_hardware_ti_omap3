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
/*  File Name         : split_dsp_dec.c                                      */
/*                                                                           */
/*  Description       : This file contains the function for interacting	     */
/*						with LCML Framework									 */
/*                                                                           */
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
#define LOG_TAG "720p_split_dsp_dec"
#include <utils/Log.h>

/*****************************************************************************/
/* Variables Used for profiling                                              */
/*****************************************************************************/
#if OUT_DATA_BUF_PROFILE
unsigned long long outFreeBufFromApp[MAX_PROFILE];
int outFreeBufFromAppIndex = 0;
FILE *gOutDataBufAppFP = NULL;
#endif

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Start                                        */
/*  Description   : This Function initializes LCML.                          */
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
OMX_ERRORTYPE VIDDEC_LCML_Start(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE      eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;

    pLcmlHandle = (LCML_DSP_INTERFACE*)pComponentPrivate->pLCML;
    if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
        pComponentPrivate->eLCMLState != VidDec_LCML_State_Destroy &&
        pComponentPrivate->pLCML != NULL &&
        pComponentPrivate->bLCMLHalted != OMX_TRUE)
    {
        LOG_STATUS_LCML_API("LCML_ControlCodec called EMMCodecControlStart 0x%p\n",pLcmlHandle);

        eError = LCML_ControlCodec(
					((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
						EMMCodecControlStart,
						NULL);
        if (eError != OMX_ErrorNone)
        {
            eError = OMX_ErrorHardware;
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorHardware,
						OMX_TI_ErrorSevere,
						"LCML_ControlCodec Start");
            LOG_STATUS_LCML_API("Error Occurred in Codec Start... 0x%x\n",eError);
        }
    }
    pComponentPrivate->eLCMLState = VidDec_LCML_State_Start;

    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Destroy                                      */
/*  Description   : This Function Deinitializes LCML.                        */
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
OMX_ERRORTYPE VIDDEC_LCML_Destroy(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE      eError       = OMX_ErrorNone;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;

    if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
            pComponentPrivate->pLCML != NULL &&
			pComponentPrivate->bIsDSPInitialised != 0)
    {
        pLcmlHandle = (LCML_DSP_INTERFACE *) pComponentPrivate->pLCML;
        LOG_STATUS_LCML_API(
			"$$$$$ LCML_ControlCodec called MMCodecControlDestroy \n");
        eError = LCML_ControlCodec(
					((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
					 EMMCodecControlDestroy,
					 NULL);

        if (eError != OMX_ErrorNone)
        {
            LOG_STATUS_LCML_API(
						"Error Occurred in Codec Destroy...%d \n",eError);

            eError = OMX_ErrorHardware;
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventError,
							   eError,
							   OMX_TI_ErrorCritical,
							   NULL);

            LOG_STATUS_LCML_API(
						"Incorrect State Transition 0x%x\n", eError);
            goto EXIT;
        }
    }
    pComponentPrivate->eLCMLState = VidDec_LCML_State_Destroy;
    LOG_STATUS_LCML_API(
	"$$$$$ LCML_ControlCodec called EMMCodecControlDestroy 0x%p\n",pLcmlHandle);

    if(pComponentPrivate->pModLCML != NULL)
    {
        dlclose(pComponentPrivate->pModLCML);
        pComponentPrivate->pModLCML = NULL;
        pComponentPrivate->pLCML = NULL;
    }

    pComponentPrivate->eLCMLState = VidDec_LCML_State_Unload;
    LOG_STATUS_LCML_API(
					"Closed LCML lib 0x%p\n",pComponentPrivate->pModLCML);
EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Pause                                        */
/*  Description   : This Function Pauses LCML.                               */
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
OMX_ERRORTYPE VIDDEC_LCML_Pause(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE      eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;

    if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
       pComponentPrivate->eLCMLState != VidDec_LCML_State_Destroy &&
       pComponentPrivate->pLCML != NULL &&
       pComponentPrivate->bLCMLHalted != OMX_TRUE)
    {
        pLcmlHandle = (LCML_DSP_INTERFACE*)pComponentPrivate->pLCML;
        LOG_STATUS_LCML_API("$$$$$ LCML_ControlCodec called EMMCodecControlPause 0x%p\n",pLcmlHandle);

        eError = LCML_ControlCodec(
			((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
			EMMCodecControlPause, NULL);
        if (eError != OMX_ErrorNone)
        {
            eError = OMX_ErrorHardware;
            LOG_STATUS_LCML_API("Error during EMMCodecControlPause...\n");
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventError,
							   eError,
							   OMX_TI_ErrorSevere,
							   NULL);
            VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
        }

        eError = OMX_ErrorNone;
        VIDDEC_PTHREAD_MUTEX_WAIT(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
    }

    pComponentPrivate->eLCMLState = VidDec_LCML_State_Pause;
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Stop                                         */
/*  Description   : This Function Stops LCML.                                */
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
OMX_ERRORTYPE VIDDEC_LCML_Stop(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;

    if(pComponentPrivate->eLCMLState  != VidDec_LCML_State_Unload &&
       pComponentPrivate->eLCMLState  != VidDec_LCML_State_Destroy &&
       pComponentPrivate->pLCML       != NULL &&
       pComponentPrivate->bLCMLHalted != OMX_TRUE)
    {
        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
        pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->pLCML;
        LOG_STATUS_LCML_API("Sending stop command to dsp \n");
        eError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                    MMCodecControlStop,
                                    NULL);
        if (eError != OMX_ErrorNone)
        {
            eError = OMX_ErrorHardware;
            LOG_STATUS_LCML_API("Error Occurred in Codec Stop...\n");
            VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
        }

        pComponentPrivate->eLCMLState = VidDec_LCML_State_Stop;
        VIDDEC_PTHREAD_MUTEX_WAIT(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
    }

    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}
/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Flush                                        */
/*  Description   : handles LCML flush command.                              */
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

OMX_ERRORTYPE VIDDEC_LCML_Flush(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate, OMX_U32 portID)
{
    OMX_U32 			aParam[4];
    OMX_ERRORTYPE 		eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;

    if(pComponentPrivate->eLCMLState  != VidDec_LCML_State_Unload &&
       pComponentPrivate->eLCMLState  != VidDec_LCML_State_Destroy &&
       pComponentPrivate->pLCML       != NULL &&
       pComponentPrivate->bLCMLHalted != OMX_TRUE)
    {
        aParam[0] = USN_STRMCMD_FLUSH;
        aParam[1] = portID;
        aParam[2] = 0;
        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
        pLcmlHandle = (LCML_DSP_INTERFACE*)pComponentPrivate->pLCML;
        LOG_STATUS_LCML_API("Calling DSP side flush\n");

        eError = LCML_ControlCodec(
			((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
			EMMCodecControlStrmCtrl,
			(void*)aParam);
        if (eError != OMX_ErrorNone)
        {
            eError = OMX_ErrorHardware;
            LOG_STATUS_LCML_API("Error in control call\n");
            VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
            goto EXIT;
        }
        LOG_STATUS_LCML_API("Wait on mutex\n");
        VIDDEC_PTHREAD_MUTEX_WAIT(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
    }

EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);

    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LoadLCMLLibrary                                   */
/*  Description   : This Functions LOAD LCML Library.                        */
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
OMX_ERRORTYPE VIDDEC_LoadLCMLLibrary(OMX_HANDLETYPE phandle)
{
    VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate = NULL;
    OMX_ERRORTYPE 			  eError = OMX_ErrorNone;
    void					  *pMyLCML;
    VIDDEC_fpo 				  fpGetHandle;
    char				      *error;
    OMX_HANDLETYPE 			  hLCML = NULL;

	pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)phandle;
    pMyLCML = dlopen("libLCML.so", RTLD_LAZY);
    if (!pMyLCML)
    {
        LOG_STATUS_LCML_API("OMX_ErrorBadParameter\n");
        fputs(dlerror(), stderr);
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    fpGetHandle = dlsym(pMyLCML, "GetHandle");
    if ((error = dlerror()) != NULL)
    {
        LOG_STATUS_LCML_API("OMX_ErrorBadParameter\n");
        fputs(error, stderr);
        dlclose(pMyLCML);
        pMyLCML = NULL;
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }
    eError = (*fpGetHandle)(&hLCML);
    if (eError != OMX_ErrorNone)
    {
        LOG_STATUS_LCML_API("OMX_ErrorBadParameter\n");
        dlclose(pMyLCML);
        pMyLCML = NULL;
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pComponentPrivate->pModLCML = pMyLCML;

    pComponentPrivate->eLCMLState = VidDec_LCML_State_Load;
    LOG_STATUS_LCML_API("LCML Handler 0x%p\n",hLCML);

    pComponentPrivate->pLCML = (LCML_DSP_INTERFACE *)hLCML;
    pComponentPrivate->pLCML->pComponentPrivate = pComponentPrivate;

    pComponentPrivate->bLCMLOut = OMX_TRUE;
    eError = OMX_ErrorNone;
EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);

    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_QueueOutputBuffer                            */
/*  Description   : This Function Queue's Output buffer.                     */
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
OMX_ERRORTYPE VIDDEC_LCML_QueueOutputBuffer(
									VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                    OMX_BUFFERHEADERTYPE *pBuffHead,
				    OMX_PTR *pBuffer)
{
    OMX_ERRORTYPE         eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE    *pLcmlHandle;
    VIDDEC_BUFFER_PRIVATE *pBufferPrivate = NULL;
    TMMCodecBufferType    bufType;
    OMX_U32 			  nAllocLen = pBuffHead->nAllocLen;
    OMX_U32 			  nFilledLen = pBuffHead->nFilledLen;
    OMX_U32 			  tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;

    pLcmlHandle = (LCML_DSP_INTERFACE *)(pComponentPrivate->pLCML);
    pBufferPrivate = (VIDDEC_BUFFER_PRIVATE *)pBuffHead->pOutputPortPrivate;

    if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
        pComponentPrivate->eLCMLState != VidDec_LCML_State_Destroy &&
        pComponentPrivate->pLCML != NULL)
    {
#ifdef KHRONOS_1_1
        if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
        {
            WMV9VDEC_UALGOutputParam* pUalgOutParams = NULL;
            pUalgOutParams = (WMV9VDEC_UALGOutputParam *)pBufferPrivate->pUalgParam;
        }
#endif
        pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_DSP;
        LOG_STATUS_LCML_API("2; Found output buffer %x: Sending to DSP eBufferOwner 0x%x\n", (int)pBuffHead, pBufferPrivate->eBufferOwner);

#if OUT_DATA_BUF_PROFILE
{
	out_data_buf_profile();
}
#endif

#ifndef DISABLE_MAP_REUSE
        bufType = EMMCodecOutputBufferMapReuse;
#else
        bufType = EMMCodecOutputBufferMapBufLen;
#endif

        {
            /* Keep track of output buffers queued to DSP. This will be used */
            /* later to detect a hang on DSP. 								 */
            int i;
            OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
            for(i = 0; i < MAX_DSP_OUT_BUFS; i++)
            {
                if(pBuffHead->pBuffer == pSplit->dsp_out_buf_status[i].buf)
                {
                    pSplit->dsp_out_buf_status[i].buf_status = BUF_QUEUED;
                    break;
                }
                else if(NULL == pSplit->dsp_out_buf_status[i].buf)
                {
                    pSplit->dsp_out_buf_status[i].buf = pBuffHead->pBuffer;
                    pSplit->dsp_out_buf_status[i].buf_status = BUF_QUEUED;
                    break;
                }
            }
        }

        eError = LCML_QueueBuffer(
				((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
				bufType,
				pBuffer,
				nAllocLen,
				nFilledLen,
				(OMX_U8*)(pBufferPrivate->pUalgParam),
				(OMX_S32)pBufferPrivate->nUalgParamSize,
				(OMX_U8*)pBuffHead);

        if (eError != OMX_ErrorNone)
        {
            LOG_STATUS_LCML_API("LCML_QueueBuffer 0x%x\n", eError);
            eError = OMX_ErrorHardware;
            goto EXIT;
        }
    }
    else
    {
        eError = OMX_ErrorHardware;
        goto EXIT;
    }
EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : print_all_create_args                                    */
/*  Description   : This functions Print ll arguements.                      */
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
void print_all_create_args(ITTVDEC_SNCreatePhArg *pCreatePhaseArgs)
{
    LOG_STATUS_LCML_API(" unNumOfStreams:           %d \n", pCreatePhaseArgs->unNumOfStreams);
    LOG_STATUS_LCML_API(" unInputStreamID:          %d \n", pCreatePhaseArgs->unInputStreamID);
    LOG_STATUS_LCML_API(" unInputBufferType:        %d \n", pCreatePhaseArgs->unInputBufferType);
    LOG_STATUS_LCML_API(" unlInputNumBufsPerStream: %d \n", pCreatePhaseArgs->unlInputNumBufsPerStream);
    LOG_STATUS_LCML_API(" unOutputStreamID:         %d \n", pCreatePhaseArgs->unOutputStreamID);
    LOG_STATUS_LCML_API(" unOutputBufferType:       %d \n", pCreatePhaseArgs->unOutputBufferType);
    LOG_STATUS_LCML_API(" unOutputNumBufsPerStream: %d \n", pCreatePhaseArgs->unOutputNumBufsPerStream);
    LOG_STATUS_LCML_API(" unReserved:               %d \n", pCreatePhaseArgs->unReserved);
    LOG_STATUS_LCML_API(" ulMaxWidth:               %lu \n", pCreatePhaseArgs->ulMaxWidth);
    LOG_STATUS_LCML_API(" ulMaxHeight:              %lu \n", pCreatePhaseArgs->ulMaxHeight);
    LOG_STATUS_LCML_API(" ulYUVFormat:              %lu \n", pCreatePhaseArgs->ulYUVFormat);
    LOG_STATUS_LCML_API(" ulMaxFrameRate:           %lu \n", pCreatePhaseArgs->ulMaxFrameRate);
    LOG_STATUS_LCML_API(" ulMaxBitRate:             %lu \n", pCreatePhaseArgs->ulMaxBitRate);
    LOG_STATUS_LCML_API(" ulDataEndianness:         %lu \n", pCreatePhaseArgs->ulDataEndianness);
    LOG_STATUS_LCML_API(" ulProfile:                %lu \n", pCreatePhaseArgs->ulProfile);
    LOG_STATUS_LCML_API(" ulMaxLevel:               %lu \n", pCreatePhaseArgs->ulMaxLevel);
    LOG_STATUS_LCML_API(" ulProcessMode:            %lu \n", pCreatePhaseArgs->ulProcessMode);
    LOG_STATUS_LCML_API(" ulDeblock:       %lu \n", pCreatePhaseArgs->ulDeblock);
    LOG_STATUS_LCML_API(" ulDisplayWidth:           %lu \n", pCreatePhaseArgs->ulDisplayWidth);
}

#define FRMPIC_ALLOC_TIME          1
#define PAD_LEN_Y_H                   24
#define PAD_LEN_Y_V                   20
#define PAD_LEN_UV_H                  8
#define PAD_LEN_UV_V                  8
#define PAD_MV_BANK_ROW               64
#define SIZEOF_MVPRED                 12
static OMX_U32 get_max_ref_frms(OMX_U32 wd, OMX_U32 ht)
{
    OMX_U32 i4_size;
    OMX_U32 wd_mbs = wd / 16;
    OMX_U32 ht_mbs = ht / 16;
    OMX_U32 num_frms;

    /* Allocation is done assuming a level  of 3.1 */
    i4_size = 6912000;

    num_frms = i4_size / (wd_mbs * (ht_mbs << (1 - FRMPIC_ALLOC_TIME)));
    num_frms = num_frms / 384;
    if(num_frms > 16)
        num_frms = 16;


    return(num_frms);

}

OMX_U32 compute_dpb_sizes (OMX_U32 wd, OMX_U32 ht, OMX_U32 num_alloc_frames,
                           OMX_S32 i4_frame_only)
{

    OMX_U32 wd_y, ht_y, wd_uv, ht_uv, size_y, size_uv, num_frms;
    OMX_U32 dpb_size, mvbuf_size, total_size;

    wd_y = wd + (PAD_LEN_Y_H << 1);
    ht_y = ht + (PAD_LEN_Y_V << (2 - i4_frame_only));
    wd_uv = (wd >> 1) + (PAD_LEN_UV_H << 1);
    ht_uv = (ht >> 1) + (PAD_LEN_UV_V << (2 - i4_frame_only));

    size_y = wd_y * ht_y;
    size_y = (((size_y + 127) >> 7) << 7);
    size_uv = (wd_uv * ht_uv) * 2;
    size_uv = (((size_uv + 127) >> 7) << 7);
    dpb_size = num_alloc_frames * (size_y + size_uv);

    total_size = dpb_size;
    total_size = (((total_size + 127) >> 7) << 7);
    return (total_size);
}

OMX_U32 compute_mvbuf_sizes (OMX_U32 wd, OMX_U32 ht, OMX_U32 max_ref_bufs,
                             OMX_S32 i4_frame_only)
{

    OMX_U32 wd_y, ht_y, num_frms;
    OMX_U32 dpb_size, mvbuf_size, total_size;

    wd_y = wd + (PAD_LEN_Y_H << 1);
    ht_y = ht + (PAD_LEN_Y_V << (2 - i4_frame_only));

    mvbuf_size = ((wd * (ht + PAD_MV_BANK_ROW)) >> 4) * SIZEOF_MVPRED;
    mvbuf_size = (((mvbuf_size + 127) >> 7) << 7);
    mvbuf_size *= (max_ref_bufs + 1);

    total_size = mvbuf_size;
    total_size = (((total_size + 127) >> 7) << 7);
    return (total_size);
}

#define MIN(x, y) (((x) > (y))? (y) : (x))
OMX_U32 get_ddr_alg_heap_profile_id(OMX_U32 wd, OMX_U32 ht, OMX_S32 num_ref_frames,
                                  OMX_S32 num_reorder_frames)
{
    OMX_U32 total_size;

    OMX_S32 i4_num_ref_frames_alloc, i4_num_alloc_frames;
    OMX_S32 max_DPB_size = get_max_ref_frms(wd, ht);

    OMX_S32 i4_num_ref_frames = max_DPB_size;
    OMX_S32 i4_display_delay = max_DPB_size + 1;

    if(num_ref_frames > 16)
       num_ref_frames = 16;

    if(num_reorder_frames > 16)
       num_reorder_frames = 16;
    {

        {
            i4_num_ref_frames = (num_ref_frames > 0) ? num_ref_frames: max_DPB_size;

            i4_display_delay = (num_reorder_frames > 0) ? (num_reorder_frames + 1) : (max_DPB_size + 1);

            i4_display_delay = MIN(i4_display_delay, (max_DPB_size + 1));
        }

        i4_num_alloc_frames = i4_num_ref_frames + i4_display_delay;


        /* clip num_alloc frames */
        if(i4_num_alloc_frames > ((max_DPB_size * 2) + 1))
            i4_num_alloc_frames = (max_DPB_size * 2) + 1;

        /* clip num_ref_frame */
        if(i4_num_ref_frames > i4_num_alloc_frames)
            i4_num_ref_frames = i4_num_alloc_frames;

        i4_num_ref_frames_alloc = MIN(i4_num_ref_frames,
                                      max_DPB_size);

    }
    /* Account for about 1.6MB of other DSP memtabs and 1MB for rest of the data in DDR etc */
    total_size = 1600 * 1024 + 1024 * 1024;

    total_size += compute_dpb_sizes(wd, ht, i4_num_alloc_frames, 1) ;
    total_size += compute_mvbuf_sizes(wd, ht, i4_num_ref_frames_alloc, 1) ;
    ALOGD("Set total_size: %d\n", total_size);
    total_size = total_size >> 21;
    ALOGD("Set after total_size: %d\n", total_size);
    return total_size;
}
/*****************************************************************************/
/*  Function Name : VIDDEC_InitDSPDec                                        */
/*  Description   : Function to fill DSP structures via LCML				 */
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
OMX_ERRORTYPE VIDDEC_InitDSPDec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE         eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE    *pLcmlHandle = NULL;
    LCML_DSP              *lcml_dsp = NULL;
    OMX_U32 		      nInpBuff = MAX_PRIVATE_IN_BUFFERS;
    OMX_U32 			  nInpBuffSize = 0;
    OMX_U32 			  nOutBuff = MAX_PRIVATE_OUT_BUFFERS;
    OMX_U32 			  nOutBuffSize = 0;
    ITTVDEC_SNCreatePhArg *pCreatePhaseArgs = NULL;
    LCML_CALLBACKTYPE     cb;
    OMX_U32 			  nFrameWidth = pComponentPrivate->pOutPortDef->
										format.video.nFrameWidth;
    OMX_U32 			  nFrameHeight = pComponentPrivate->pOutPortDef->
										format.video.nFrameHeight;
    OMX_U32 			  tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;

    LOG_STATUS_LCML_API("+++ENTERING\n");
    /* Get number of input and output buffers */
    nInpBuff = pComponentPrivate->pInPortDef->nBufferCountActual;
    nOutBuff = pComponentPrivate->pOutPortDef->nBufferCountActual;




    /* Back it up for further use in this function */
    nInpBuffSize = pComponentPrivate->pInPortDef->nBufferSize;
    nOutBuffSize = pComponentPrivate->pOutPortDef->nBufferSize;

    pLcmlHandle = (LCML_DSP_INTERFACE*)pComponentPrivate->pLCML;
    lcml_dsp = (((LCML_DSP_INTERFACE*)pLcmlHandle)->dspCodec);

    lcml_dsp->DeviceInfo.TypeofDevice = 0;
    lcml_dsp->DeviceInfo.DspStream    = NULL;

    lcml_dsp->In_BufInfo.nBuffers      = nInpBuff;
    lcml_dsp->In_BufInfo.nSize         = nInpBuffSize;
    lcml_dsp->In_BufInfo.DataTrMethod  = DMM_METHOD;

    lcml_dsp->Out_BufInfo.nBuffers     = nOutBuff;
    lcml_dsp->Out_BufInfo.nSize        = nOutBuffSize;
    lcml_dsp->Out_BufInfo.DataTrMethod = DMM_METHOD;

    lcml_dsp->NodeInfo.nNumOfDLLs       = OMX_MP4DEC_NUM_DLLS;

    if(tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
    {
            lcml_dsp->NodeInfo.AllUUIDs[0].uuid =
										(struct DSP_UUID *)&MP4DSOCKET_TI_UUID;
            strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[0].DllName,
					(char*)MP4_DEC_NODE_DLL);
            lcml_dsp->NodeInfo.AllUUIDs[0].eDllType = DLL_NODEOBJECT;

            lcml_dsp->NodeInfo.AllUUIDs[1].uuid =
										(struct DSP_UUID *)&MP4DSOCKET_TI_UUID;
            strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[1].DllName,
					(char*)MP4_DEC_NODE_DLL);
            lcml_dsp->NodeInfo.AllUUIDs[1].eDllType = DLL_DEPENDENT;
    }
    else if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
            lcml_dsp->NodeInfo.AllUUIDs[0].uuid =
								(struct DSP_UUID *)&H264VDSOCKET_TI_UUID;
            strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[0].DllName,
					(char*)H264_DEC_NODE_DLL);
            lcml_dsp->NodeInfo.AllUUIDs[0].eDllType = DLL_NODEOBJECT;

            lcml_dsp->NodeInfo.AllUUIDs[1].uuid =
								(struct DSP_UUID *)&H264VDSOCKET_TI_UUID;
            strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[1].DllName,
					(char*)H264_DEC_NODE_DLL);
            lcml_dsp->NodeInfo.AllUUIDs[1].eDllType = DLL_DEPENDENT;
    }
    else if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
            lcml_dsp->NodeInfo.AllUUIDs[0].uuid =
							(struct DSP_UUID *)&WMVDSOCKET_TI_UUID;
            strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[0].DllName,
					(char*)WMV_DEC_NODE_DLL);
            lcml_dsp->NodeInfo.AllUUIDs[0].eDllType = DLL_NODEOBJECT;

            lcml_dsp->NodeInfo.AllUUIDs[1].uuid =
							(struct DSP_UUID *)&WMVDSOCKET_TI_UUID;
            strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[1].DllName,
								(char*)WMV_DEC_NODE_DLL);
            lcml_dsp->NodeInfo.AllUUIDs[1].eDllType = DLL_DEPENDENT;
    }

	pComponentPrivate->eMBErrorReport.bEnabled = FALSE;

    lcml_dsp->NodeInfo.AllUUIDs[2].uuid = (struct DSP_UUID *)&RINGIO_TI_UUID;
    strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[2].DllName,
			(char*)RINGIO_NODE_DLL);
    lcml_dsp->NodeInfo.AllUUIDs[2].eDllType = DLL_DEPENDENT;

    lcml_dsp->NodeInfo.AllUUIDs[3].uuid = (struct DSP_UUID *)&USN_UUID;
    strcpy ((char*)lcml_dsp->NodeInfo.AllUUIDs[3].DllName,(char*)USN_DLL);
    lcml_dsp->NodeInfo.AllUUIDs[3].eDllType = DLL_DEPENDENT;

    lcml_dsp->SegID     = 0;
    lcml_dsp->Timeout   = -1;
    lcml_dsp->Alignment = 0;
    lcml_dsp->Priority  = 5;

    if (nFrameWidth * nFrameHeight > 640 * 480)
    {
        lcml_dsp->ProfileID = 4;
    }
    else if (nFrameWidth * nFrameHeight > 352 * 288)
    {
        lcml_dsp->ProfileID = 3;
    }
    else if (nFrameWidth * nFrameHeight > 176 * 144)
    {
        lcml_dsp->ProfileID = 2;
    }
    else if (nFrameWidth * nFrameHeight >= 16 * 16)
    {
        lcml_dsp->ProfileID = 1;
    }
    else
    {
        eError = OMX_ErrorUnsupportedSetting;
        goto EXIT;
    }

    OMX_MALLOC_STRUCT(pCreatePhaseArgs, ITTVDEC_SNCreatePhArg,
					  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel4]);
    if (pCreatePhaseArgs == NULL)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    pCreatePhaseArgs->unNumOfStreams            = 2;
    pCreatePhaseArgs->unInputStreamID           = 0;
    pCreatePhaseArgs->unInputBufferType         = 0;
    pCreatePhaseArgs->unlInputNumBufsPerStream  = (OMX_U16)(pComponentPrivate->
										pInPortDef->nBufferCountActual);
    pCreatePhaseArgs->unOutputStreamID          = 1;
    pCreatePhaseArgs->unOutputBufferType        = 0;
    pCreatePhaseArgs->unOutputNumBufsPerStream  = (OMX_U16)(pComponentPrivate->
										pOutPortDef->nBufferCountActual);
    pCreatePhaseArgs->ulMaxWidth                = (OMX_U16) nFrameWidth;
    pCreatePhaseArgs->ulMaxHeight               = (OMX_U16) nFrameHeight;

    if (pComponentPrivate->pOutPortDef->format.video.eColorFormat ==
														VIDDEC_COLORFORMAT422)
    {
        pCreatePhaseArgs->ulYUVFormat = MP4VIDDEC_YUVFORMAT_INTERLEAVED422;
    }
    else if (pComponentPrivate->pOutPortDef->format.video.eColorFormat ==
														VIDDEC_COLORFORMAT420)
    {
        pCreatePhaseArgs->ulYUVFormat = MP4VIDDEC_YUVFORMAT_PLANAR420;
    }
    else
    {
        LOG_STATUS_LCML_API("Incorrect Color format %x\n",
				pComponentPrivate->pOutPortDef->format.video.eColorFormat);
        LOG_STATUS_LCML_API("lcml_dsp->ProfileID = %lu\n",
				lcml_dsp->ProfileID);
        eError = OMX_ErrorUnsupportedSetting;
        goto EXIT;
    }
    
    pCreatePhaseArgs->ulMaxFrameRate   = 30000;
    pCreatePhaseArgs->ulMaxBitRate     = 20000000;
    pCreatePhaseArgs->ulDataEndianness = 1;
    if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
    {
        pCreatePhaseArgs->ulProfile = 0;
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        pCreatePhaseArgs->ulProfile = 1;
    }
    else
    {
        pCreatePhaseArgs->ulProfile = 8;
    }
#define ENABLE_REDUCED_MEM_REQ 1
#if ENABLE_REDUCED_MEM_REQ
    if (pComponentPrivate->pInPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        OMX_S32 extended_params = 0;

        /* i4_num_ref_frames is signalled using ulMaxFrameRate */
        pCreatePhaseArgs->ulMaxFrameRate            = -1;
        pCreatePhaseArgs->ulMaxBitRate              = -1;

        //ALOGD("Setting pCreatePhaseArgs->ulMaxFrameRate %d and pCreatePhaseArgs->ulMaxBitRate %d Wd %d Ht %d\n",
        //pCreatePhaseArgs->ulMaxFrameRate,pCreatePhaseArgs->ulMaxBitRate,
        //pCreatePhaseArgs->ulMaxWidth, pCreatePhaseArgs->ulMaxHeight);
        /*lcml_dsp->ProfileID = get_ddr_alg_heap_profile_id(pCreatePhaseArgs->ulMaxWidth,
                                                          pCreatePhaseArgs->ulMaxHeight,
                                                          pCreatePhaseArgs->ulMaxFrameRate,
                                                          pCreatePhaseArgs->ulMaxBitRate);*/
        //ALOGD("Setting lcml_dsp->ProfileID: %d\n", lcml_dsp->ProfileID);
        
        lcml_dsp->ProfileID  = 0;
    }
#else
	if(pComponentPrivate->pInPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
	{
		lcml_dsp->ProfileID  = 0;
	}
#endif
	
    pCreatePhaseArgs->ulMaxLevel    = -1;
    pCreatePhaseArgs->ulProcessMode = pComponentPrivate->ProcessMode;

	/* Omkiran: If greater than D1 deblock is off, else deblock is on. */
    if( pCreatePhaseArgs->ulMaxWidth > 720 )
        pCreatePhaseArgs->ulDeblock = 0;
    else
        pCreatePhaseArgs->ulDeblock = 0; /* 0 :Dblk On, 1:Dblk Off */

	if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
	{
		pCreatePhaseArgs->ulDeblock = 0;
	}
    pCreatePhaseArgs->ulDisplayWidth            = 0;
    pCreatePhaseArgs->endArgs                   = END_OF_CR_PHASE_ARGS;
/* enable buffer locking */
    pCreatePhaseArgs->unReserved                = 0;

    memcpy(pComponentPrivate->arr, pCreatePhaseArgs,
			sizeof(ITTVDEC_SNCreatePhArg));
    lcml_dsp->pCrPhArgs = pComponentPrivate->arr;
    cb.LCML_Callback    = (void*) VIDDEC_LCML_Callback;


    LOG_STATUS_LCML_API("Calling print args\n");
    print_all_create_args(pCreatePhaseArgs);
    free(pCreatePhaseArgs);
    pCreatePhaseArgs = NULL;

    if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
        pComponentPrivate->pLCML != NULL)
    {
        pComponentPrivate->eLCMLState = VidDec_LCML_State_Init;

        eError = LCML_InitMMCodec(
					((LCML_DSP_INTERFACE *)pLcmlHandle)->pCodecinterfacehandle,
					  NULL,
					  &pLcmlHandle,
					  NULL,
					  &cb);
        if (eError != OMX_ErrorNone)
        {
            LOG_STATUS_LCML_API("LCML_InitMMCodec Failed!...%x\n",eError);
            eError = OMX_ErrorInvalidState;
            goto EXIT;
        }

		/* Set the flag confirming DSP initialisation */
		pComponentPrivate->bIsDSPInitialised = 1;
    }
    else
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LoadCodec                                         */
/*  Description   : function will get LCML resource and start the codec.     */
/*  Inputs        : pComponentPrivate            Component private structure */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       : OMX_ErrorNone              Success, ready to roll        */
/*                  OMX_ErrorUndefined 										 */
/*                  OMX_ErrorInsufficientResources   Not enough memory       */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_LoadCodec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE      eError = OMX_ErrorUndefined;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;
	OMX_U32 		   tempCompressionFormat = pComponentPrivate->
							pInPortDef->format.video.eCompressionFormat;

    VIDDEC_LoadLCMLLibrary(pComponentPrivate);

#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->lcml_nCntOpReceived = 0;
#endif
    eError = OMX_ErrorNone;

    if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
		tempCompressionFormat == OMX_VIDEO_CodingAVC ||
		tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        eError = create_split_arm_side_component(pComponentPrivate);
        if(eError == OMX_ErrorNone)
        {
            LOG_STATUS_LCML_API("Call was successfull\n");
            eError = VIDDEC_InitDSPDec(pComponentPrivate);
            if(eError != OMX_ErrorNone)
            {
				LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
                pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							eError,
							OMX_TI_ErrorSevere,
							"DSP Initialization");
                goto EXIT;
            }
        }
        else
        {
            ALOGE("Failed to create ARM Component");
            LOG_STATUS_LCML_API("Call was a failure\n");
        }
    }
    else
    {
        LOG_STATUS_LCML_API("OMX_ErrorUnsupportedSetting\n");
        eError = OMX_ErrorUnsupportedSetting;
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
        pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorUnsupportedSetting,
						OMX_TI_ErrorMinor,
						"DSP Initialization");
        goto EXIT;
    }

    pComponentPrivate->bLCMLOut    = OMX_FALSE;
    pComponentPrivate->bLCMLHalted = OMX_FALSE;

    pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->pLCML;
    pComponentPrivate->eLCMLState = VidDec_LCML_State_Init;

    ALOGD("OUTPUT width=%lu height=%lu\n",
		pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
		pComponentPrivate->pOutPortDef->format.video.nFrameHeight);
    ALOGD("INPUT width=%lu height=%lu\n",
		pComponentPrivate->pInPortDef->format.video.nFrameWidth,
		pComponentPrivate->pInPortDef->format.video.nFrameHeight);

#ifdef VIDDEC_FLAGGED_EOS
    if (pComponentPrivate->bUseFlaggedEos)
    {
        if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
            pComponentPrivate->eLCMLState != VidDec_LCML_State_Destroy &&
            pComponentPrivate->pLCML != NULL &&
            pComponentPrivate->bLCMLHalted != OMX_TRUE)
        {
            LOG_STATUS_LCML_API("LCML_ControlCodec called EMMCodecControlUsnEos 0x%p\n",
				pLcmlHandle);

            eError = LCML_ControlCodec(
					((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
					EMMCodecControlUsnEos,
					NULL);
            if (eError != OMX_ErrorNone)
            {
                LOG_STATUS_LCML_API("error in EMMCodecControlUsnEos %x\n",eError);

                eError = OMX_ErrorHardware;

                LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
                pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorHardware,
						OMX_TI_ErrorSevere,
						"LCML_ControlCodec EMMCodecControlUsnEos function");

                LOG_STATUS_LCML_API(
								"OMX_ErrorHardware 0x%x\n",eError);
                goto EXIT;
            }
        }
    }
#endif


    if((tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
       tempCompressionFormat == OMX_VIDEO_CodingAVC ||
       tempCompressionFormat == OMX_VIDEO_CodingWMV) &&
       pComponentPrivate->eState == OMX_StateIdle)
    {
            VIDDEC_LCML_Start(pComponentPrivate);
            asc_set_params_control_call(pComponentPrivate);
            create_asc_thread(pComponentPrivate);
    }

    eError = OMX_ErrorNone;

EXIT:
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_UnloadCodec                                       */
/*  Description   : function will stop & destroy the codec. LCML resource is */
/*                  also being freed.     									 */
/*  Inputs        : pComponentPrivate            Component private structure */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       : OMX_ErrorNone              Success, ready to roll        */
/*                  OMX_ErrorUndefined 										 */
/*                  OMX_ErrorInsufficientResources   Not enough memory       */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_UnloadCodec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE	   eError = OMX_ErrorUndefined;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;

    LOG_STATUS_LCML_API("+++ENTERING\n");

    if (!(pComponentPrivate->eState == OMX_StateLoaded) &&
        !(pComponentPrivate->eState == OMX_StateWaitForResources))
    {
        pLcmlHandle = (LCML_DSP_INTERFACE*)pComponentPrivate->pLCML;

        if (pComponentPrivate->eState == OMX_StateExecuting)
        {
                VIDDEC_LCML_Stop(pComponentPrivate);
        }
        VIDDEC_LCML_Destroy(pComponentPrivate);

        pComponentPrivate->bLCMLHalted = OMX_TRUE;
    }
    eError = OMX_ErrorNone;
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Callback_OutputBufferProduced                */
/*  Description   :  give callback denoting completion of output buffer.     */
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
OMX_ERRORTYPE VIDDEC_LCML_Callback_OutputBufferProduced(void *argsCb [10])
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_ERRORTYPE 			 eError = OMX_ErrorNone;
    VIDDEC_BUFFER_PRIVATE    *pBufferPrivate = NULL;
    OMX_S32 			     nRetVal = 0;

    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)(
						(LCML_DSP_INTERFACE*)argsCb[6])->pComponentPrivate;

    OMX_BUFFERHEADERTYPE* pBuffHead = NULL;
    VIDDEC_BUFFER_PRIVATE* pBuffPriv = NULL;
    OMX_U8* pBuffer;
    if ((OMX_U32)argsCb [0] != EMMCodecOuputBuffer)
    {
        LOG_STATUS_LCML_API("ISSUE\n");
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    if (pComponentPrivate->eState == OMX_StateLoaded ||
		pComponentPrivate->eState == OMX_StateIdle)
    {
        LOG_STATUS_LCML_API("ISSUE\n");
        eError = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    pBuffHead = (OMX_BUFFERHEADERTYPE *)argsCb[7];
    if(pBuffHead != NULL)
    {
        pBuffPriv = (VIDDEC_BUFFER_PRIVATE*)pBuffHead->pOutputPortPrivate;
        if (pBuffPriv != NULL)
        {
            if(pBuffPriv->eBufferOwner != VIDDEC_BUFFER_WITH_CLIENT)
            {
                pBuffer = (OMX_U8*)argsCb[1];
                pBuffHead->nFilledLen = (OMX_S32)argsCb[8];

                /* Retrieve time stamp information */
                if ((pComponentPrivate->ProcessMode) == 0 &&
					(0 != pBuffHead->nFilledLen))
                {
                    OMX_PTR pBufferFlags;
                    VIDDEC_CircBuf_Remove(pComponentPrivate,
                                    VIDDEC_CBUFFER_TIMESTAMP,
                                    VIDDEC_INPUT_PORT,
                                    &pBufferFlags);
                    if(pBufferFlags != NULL)
                    {
                        pBuffHead->nTimeStamp = (OMX_TICKS)((
                        VIDDEC_CBUFFER_BUFFERFLAGS *)pBufferFlags)->nTimeStamp;

                        pBuffHead->nFlags |= (((OMX_U32)((
						VIDDEC_CBUFFER_BUFFERFLAGS *)pBufferFlags)->nFlags) &
						OMX_BUFFERFLAG_DECODEONLY);

                        pBuffHead->nTickCount = (OMX_U32)((
                        VIDDEC_CBUFFER_BUFFERFLAGS *)pBufferFlags)->nTickCount;
                    }
                    else
                    {
                        pBuffHead->nTimeStamp = 0;
                        pBuffHead->nTickCount = 0;
                    }
                }
                else
                {
                    if (pBuffHead->nFilledLen != 0)
                    {
                        pBuffHead->nTimeStamp = (OMX_TICKS)pComponentPrivate->
								arrBufIndex[pComponentPrivate->nOutBufIndex];
                        pComponentPrivate->nOutBufIndex++;
                        pComponentPrivate->nOutBufIndex %= VIDDEC_MAX_QUEUE_SIZE;
                    }
                    else
                    {
                        LOG_STATUS_LCML_API("\n Zero byte buffer set timestamp to 0");
                        pBuffHead->nTimeStamp = 0;
                    }
                }
                if(pBuffHead != NULL)
                {
                    pBuffHead->nFilledLen = (OMX_S32)argsCb[8];

                    /* Assert filled-length is non-zero. This print should not*/
                    /* be seen during playback except for few frames at start */
                    /* and end                                                */
                    if(pBuffHead->nFilledLen == 0)
                    {
                       //LOG_STATUS_LCML_API("FL0");
                    }

                    pBufferPrivate = (VIDDEC_BUFFER_PRIVATE* )pBuffHead->pOutputPortPrivate;

					pthread_mutex_lock(&pComponentPrivate->OutputBCountDspMutex);
                    pComponentPrivate->bOutputBCountDsp++;
					pthread_mutex_unlock(&pComponentPrivate->OutputBCountDspMutex);
                    /*LOG_BUF_COUNT("LCMLCallback Output: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
							pComponentPrivate->bInputBCountApp,
							pComponentPrivate->bOutputBCountApp,
							pComponentPrivate->bInputBCountDsp,
							pComponentPrivate->bOutputBCountDsp);*/

                    pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;

                    if (pComponentPrivate->bUseFlaggedEos)
                    {
                        if ((pBuffHead->nFlags & OMX_BUFFERFLAG_EOS))
                        {
                            LOG_STATUS_LCML_API("EOS found\n");
                            pComponentPrivate->bPlayCompleted = OMX_TRUE;
                        }
                    }

                    nRetVal = write(pComponentPrivate->filled_outBuf_Q[1],
									&pBuffHead,sizeof(pBuffHead));
                    if(nRetVal == -1)
                    {
                        LOG_STATUS_LCML_API("writing to the input pipe %x (%ld)\n",
							OMX_ErrorInsufficientResources,nRetVal);

						LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
                        pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							OMX_ErrorInsufficientResources,
							OMX_TI_ErrorSevere,
							"Error writing to the output pipe");
                    }
                    {
                        int 					  i;
                        OMX_VIDEO_PARAM_SPLITTYPE *pSplit = NULL;

                        pSplit = pComponentPrivate->pSplit;

                        for(i = 0; i < MAX_DSP_OUT_BUFS; i++)
                        {
                            if(pBuffHead->pBuffer == pSplit->dsp_out_buf_status[i].buf)
                            {
                                pSplit->dsp_out_buf_status[i].buf_status =
																BUF_RELEASED;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                    LOG_STATUS_LCML_API(
						"ISSUE: Buffer dropped: Output buffer processed pBuffHead %p owner %d\n",
							pBuffHead,pBuffPriv->eBufferOwner);
            }
        }
        else
        {
                LOG_STATUS_LCML_API("output port private is null\n");
        }
    }
    else
    {
       LOG_STATUS_LCML_API("ISSUE\n");
    }

EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Callback_InputBufferReleased                 */
/*  Description   : give callback that input buffer is taken for processing. */
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
OMX_ERRORTYPE VIDDEC_LCML_Callback_InputBufferReleased(void *argsCb[10])
{
    VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate = NULL;
    OMX_ERRORTYPE 			  eError = OMX_ErrorNone;
    VIDDEC_BUFFER_PRIVATE     *pBufferPrivate = NULL;
    OMX_S32 nRetVal = 0;

    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)((
							LCML_DSP_INTERFACE*)argsCb[6])->pComponentPrivate;

    OMX_BUFFERHEADERTYPE  *pBuffHead = NULL;
    VIDDEC_BUFFER_PRIVATE *pBuffPriv = NULL;
    OMX_U8				  *pBuffer;
    if ((OMX_U32)argsCb [0] != EMMCodecInputBuffer)
    {
        LOG_STATUS_LCML_API("ISSUE\n");
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    if (pComponentPrivate->eState == OMX_StateLoaded ||
		pComponentPrivate->eState == OMX_StateIdle)
    {
        LOG_STATUS_LCML_API("ISSUE\n");
        eError = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    pBuffHead = (OMX_BUFFERHEADERTYPE*)argsCb[7];
    if(pBuffHead != NULL)
    {
        pBuffPriv = (VIDDEC_BUFFER_PRIVATE*)pBuffHead->pInputPortPrivate;
        if (pBuffPriv == NULL )
        {
            LOG_STATUS_LCML_API("pInputPortPrivate is null \n");
            goto EXIT;
        }
        else
        {
			pBuffer = (OMX_U8*)argsCb[1];
			if(pBuffer != NULL)
			{
				pBufferPrivate = (VIDDEC_BUFFER_PRIVATE* )pBuffHead->pInputPortPrivate;
				pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;
				/* Work-around done to avoid flushing of 0 byte buffers.    */
				/* Actual fix to be fixed by setting nFilledLen to size of  */
			    /* buffer before invoking LCML_QueueBuffer for issuing      */
				/* IM buffer to DSP 										*/

				/* This is because the input buffer to the split dsp codec is */
				/* im buffer. So put it back on to the context pipe. 		  */

				pthread_mutex_lock(&pComponentPrivate->InputBCountDspMutex);
				pComponentPrivate->bInputBCountDsp--;
				pthread_mutex_unlock(&pComponentPrivate->InputBCountDspMutex);

				nRetVal = put_context_into_free_pipe(pComponentPrivate, pBuffHead);

				if(nRetVal == -1)
				{
					LOG_STATUS_LCML_API("writing to the input pipe %x (%lu)\n",
							OMX_ErrorInsufficientResources,nRetVal);

					LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
					pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorInsufficientResources,
						OMX_TI_ErrorSevere,
						"Error writing to the output pipe");
				}

            }
            else
            {

                LOG_STATUS_LCML_API(
                "ISSUE: BufferDropped: in InputBuffer processed pBuffHead %p owner %d\n",
						pBuffHead,pBuffPriv->eBufferOwner);
            }
        }
    }

EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_LCML_Callback					                 */
/*  Description   : Callback() function will be called LCML component to     */
/*					write the msg											 */
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
OMX_ERRORTYPE VIDDEC_LCML_Callback (TUsnCodecEvent event, void *argsCb [10])
{
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    VIDDEC_BUFFER_PRIVATE* pBufferPrivate = NULL;
    OMX_S32 nRetVal = 0;

    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)(
						(LCML_DSP_INTERFACE *)argsCb[6])->pComponentPrivate;

	LOG_STATUS_LCML_API("Entering\n");
    LOG_STATUS_LCML_API("Got Callback from LCML: 0x%x Arg[0] = %d \n",
						event, (int)argsCb[0]);

    if(pComponentPrivate->pCompPort[0] == NULL)
    {
        LOG_STATUS_LCML_API("************ERROR in lcmlcallback***********\n");
        goto EXIT;
    }
    if (event == EMMCodecProcessingPaused)
    {
        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_SIGNAL(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
        LOG_STATUS_LCML_API("EMMCodecProcessingPaused\n");
    }
    if (event == EMMCodecAlgCtrlAck)
    {
        LOG_STATUS_LCML_API("EMMCodecAlgCtrlAck\n");
        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_SIGNAL(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
    }
    if (event == EMMCodecProcessingStoped)
    {
        LOG_STATUS_LCML_API("EMMCodecProcessingStoped\n");
        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_SIGNAL(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
    }
    if (event == EMMCodecProcessingStarted)
    {
        LOG_STATUS_LCML_API("EMMCodecProcessingStarted\n");
        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_SIGNAL(pComponentPrivate->sMutex);
        VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
    }

    if (event == EMMCodecBufferProcessed)
    {
        if ((OMX_U32)argsCb [0] == EMMCodecOuputBuffer)
        {
            LOG_STATUS_LCML_API("EMMCodecBufferProcessed output\n");
            eError = VIDDEC_LCML_Callback_OutputBufferProduced(argsCb);
            if(eError != OMX_ErrorNone)
            {
				//    goto EXIT;
            }
        }
        if ((OMX_U32)argsCb [0] == EMMCodecInputBuffer)
        {
            LOG_STATUS_LCML_API("EMMCodecBufferProcessed input\n");
            eError = VIDDEC_LCML_Callback_InputBufferReleased(argsCb);
            if(eError != OMX_ErrorNone)
            {
				//    goto EXIT;
            }
        }
    }
    /**************************************************************************/
    if (event == EMMCodecBufferNotProcessed)
    {
        LOG_STATUS_LCML_API("ISSUE EMMCodecBufferNotProcessed\n");
        if ((OMX_U32)argsCb [0] == EMMCodecOuputBuffer)
        {
            OMX_BUFFERHEADERTYPE  *pBuffHead = NULL;
            VIDDEC_BUFFER_PRIVATE *pBuffPriv = NULL;
            OMX_U8			      *pBuffer;

            pBuffHead = (OMX_BUFFERHEADERTYPE*)argsCb[7];
            pBuffPriv = (VIDDEC_BUFFER_PRIVATE*)pBuffHead->pOutputPortPrivate;

        if ((pComponentPrivate->eState != OMX_StateLoaded) &&
			(pComponentPrivate->eState != OMX_StateIdle)   &&
			(pBuffHead != NULL) &&
			(pBuffPriv != NULL))
        {

			if(pBuffPriv->eBufferOwner != VIDDEC_BUFFER_WITH_CLIENT)
			{
#ifdef __PERF_INSTRUMENTATION__
				pComponentPrivate->lcml_nCntOpReceived++;
				if (pComponentPrivate->lcml_nCntOpReceived == 4)
				{
					PERF_Boundary(pComponentPrivate->pPERFcomp,
								  PERF_BoundaryStart | PERF_BoundarySteadyState);
				}
#endif
				pBuffer = (OMX_U8*)argsCb[1];
				/* Retrieve time stamp information */
				if (pComponentPrivate->ProcessMode == 0)
				{
					OMX_PTR pBufferFlags;
					VIDDEC_CircBuf_Remove(pComponentPrivate,
									   VIDDEC_CBUFFER_TIMESTAMP,
									   VIDDEC_INPUT_PORT,
									   &pBufferFlags);
					if(pBufferFlags != NULL)
					{
						pBuffHead->nTimeStamp = (OMX_TICKS)(
						(VIDDEC_CBUFFER_BUFFERFLAGS*)pBufferFlags)->nTimeStamp;

						pBuffHead->nFlags |= (((OMX_U32)(
						(VIDDEC_CBUFFER_BUFFERFLAGS *)pBufferFlags)->nFlags) &
						OMX_BUFFERFLAG_DECODEONLY);

						pBuffHead->nTickCount = (OMX_U32)(
						(VIDDEC_CBUFFER_BUFFERFLAGS*)pBufferFlags)->nTickCount;
					}
					else
					{
						pBuffHead->nTimeStamp = 0;
						pBuffHead->nTickCount = 0;
					}
				}
				else
				{
					if (pBuffHead->nFilledLen != 0)
					{
						pBuffHead->nTimeStamp =
						(OMX_TICKS)pComponentPrivate->
							arrBufIndex[pComponentPrivate->nOutBufIndex];

						pComponentPrivate->nOutBufIndex++;
						pComponentPrivate->nOutBufIndex %=
												VIDDEC_MAX_QUEUE_SIZE;
					}
				}
				if(pBuffHead != NULL)
				{
					pBuffHead->nFilledLen = (OMX_S32)argsCb[8];
					pBufferPrivate =
					(VIDDEC_BUFFER_PRIVATE *)pBuffHead->pOutputPortPrivate;

					pthread_mutex_lock(&pComponentPrivate->OutputBCountDspMutex);
					pComponentPrivate->bOutputBCountDsp++;
					pthread_mutex_unlock(&pComponentPrivate->OutputBCountDspMutex);

					LOG_BUF_COUNT("LCMLCallback OutputNotProcessed: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
							pComponentPrivate->bInputBCountApp,
							pComponentPrivate->bOutputBCountApp,
							pComponentPrivate->bInputBCountDsp,
							pComponentPrivate->bOutputBCountDsp);

					pBufferPrivate->eBufferOwner =
										VIDDEC_BUFFER_WITH_COMPONENT;
					LOG_STATUS_LCML_API("eBufferOwner 0x%x\n",
									pBufferPrivate->eBufferOwner);

#ifdef VIDDEC_FLAGGED_EOS
					LOG_STATUS_LCML_API("Looking for eos %u %lu \n",
						pComponentPrivate->bUseFlaggedEos,
						pBuffHead->nFlags & OMX_BUFFERFLAG_EOS );

					if (pComponentPrivate->bUseFlaggedEos)
					{
						if ((pBuffHead->nFlags & OMX_BUFFERFLAG_EOS))
						{
							LOG_STATUS_LCML_API("Found eos\n");
							pComponentPrivate->bPlayCompleted = OMX_TRUE;
						}
					}
#endif
					nRetVal = write(pComponentPrivate->filled_outBuf_Q[1],
									&pBuffHead,sizeof(pBuffHead));
					if(nRetVal == -1)
					{
						LOG_STATUS_LCML_API("writing to the input pipe %x (%lu)\n",
						OMX_ErrorInsufficientResources,nRetVal);

						LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
						pComponentPrivate->cbInfo.EventHandler(
								pComponentPrivate->pHandle,
								pComponentPrivate->pHandle->pApplicationPrivate,
								OMX_EventError,
								OMX_ErrorInsufficientResources,
								OMX_TI_ErrorSevere,
								"Error writing to the output pipe");
					}
				}
			}
			else
			{
				LOG_STATUS_LCML_API("ISSUE: Buffer dropped Output buffer not processed pBuffHead %p owner %d\n",
								 pBuffHead,pBuffPriv->eBufferOwner);
			}
        }
    }
        if ((OMX_U32)argsCb [0] == EMMCodecInputBuffer)
        {
            OMX_BUFFERHEADERTYPE* pBuffHead = NULL;
            VIDDEC_BUFFER_PRIVATE* pBuffPriv = NULL;
            OMX_U8* pBuffer;

            if (pComponentPrivate->eState != OMX_StateLoaded &&
				pComponentPrivate->eState != OMX_StateIdle)
            {
                pBuffHead = (OMX_BUFFERHEADERTYPE *)argsCb[7];

                if(pBuffHead != NULL)
                {
                    pBuffPriv = (VIDDEC_BUFFER_PRIVATE *)pBuffHead->
														pInputPortPrivate;
                    if (pBuffPriv == NULL)
                    {
                        goto EXIT;
                    }
                    else
                    {
                        if(pBuffPriv->eBufferOwner != VIDDEC_BUFFER_WITH_CLIENT)
                        {
                            LOG_STATUS_LCML_API(
									"pBuffHead Input 0x%p pBuffer 0x%p\n",
									pBuffHead, argsCb[1]);

                            pBuffer = (OMX_U8 *)argsCb[1];
                            if(pBuffer != NULL)
                            {
                                pBufferPrivate =
								(VIDDEC_BUFFER_PRIVATE *)pBuffHead->pInputPortPrivate;

                                pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;

                                LOG_STATUS_LCML_API("eBufferOwner 0x%x\n",
											pBufferPrivate->eBufferOwner);
#ifdef __PERF_INSTRUMENTATION__
                                PERF_ReceivedFrame(pComponentPrivate->pPERFcomp,
                                                PREF(pBuffHead,pBuffer),
                                                PREF(pBuffHead,nFilledLen),
                                                PERF_ModuleCommonLayer);
#endif
                                pBuffHead->nFilledLen = 0;

                                pthread_mutex_lock(&pComponentPrivate->InputBCountDspMutex);
                                pComponentPrivate->bInputBCountDsp--;
                                pthread_mutex_unlock(&pComponentPrivate->InputBCountDspMutex);

                                nRetVal = put_context_into_free_pipe(pComponentPrivate, pBuffHead);
                                if(nRetVal == -1)
                                {
                                    LOG_STATUS_LCML_API("writing to the input pipe %x (%lu)\n", OMX_ErrorInsufficientResources,nRetVal);
                                    LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
                                    pComponentPrivate->cbInfo.EventHandler(
									pComponentPrivate->pHandle,
									pComponentPrivate->pHandle->pApplicationPrivate,
									OMX_EventError,
									OMX_ErrorInsufficientResources,
									OMX_TI_ErrorSevere,
									"Error writing to the output pipe");
                                }
                            }
                        }
                        else
                        {
                            LOG_STATUS_LCML_API("ISSUE: Buffer dropped: Input buffer Not processed: pBuffHead %p owner %d\n",
											 pBuffHead,pBuffPriv->eBufferOwner);
                        }
                    }
                }
            }
        }
    }
    /**************************************************************************/
    if (event == EMMCodecDspError)
    {
        LOG_STATUS_LCML_API("EMMCodecDspError\n");

        if((argsCb[4] == (void *)NULL) && (argsCb[5] == (void*)NULL))
        {
            LOG_STATUS_LCML_API(
					"DSP in Error: Either MMU_Fault or SYS_ERROR\n");
            pComponentPrivate->bLCMLHalted = OMX_TRUE;
            pComponentPrivate->pHandle->SendCommand(pComponentPrivate->pHandle,
											OMX_CommandStateSet, -2, 0);
        }
        if((int)argsCb[5] == IUALG_ERR_NOT_SUPPORTED)
        {
           LOG_STATUS_LCML_API("Algorithm error. Parameter not supported\n");
           LOG_STATUS_LCML_API("argsCb5 = %p\n",argsCb[5]);
           LOG_STATUS_LCML_API("LCML_Callback: IUALG_ERR_NOT_SUPPORTED\n");

			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
			pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							OMX_ErrorInsufficientResources,
							OMX_TI_ErrorCritical,
							"Error from the DSP");
                goto EXIT;
        }
        if ((int)argsCb [0] == USN_DSPMSG_EVENT)
        {
            if ((int)argsCb [4] == USN_ERR_WARNING)
            {
                if ((int)argsCb [5] == IUALG_WARN_PLAYCOMPLETED)
                {
                    pComponentPrivate->bPlayCompleted = OMX_TRUE;
                    LOG_STATUS_LCML_API("playcompleted %x\n",
									pComponentPrivate->bPlayCompleted);

                }
                else
                {
					LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
                    pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							OMX_ErrorHardware,
							OMX_TI_ErrorCritical,
							"Error from the DSP");
                }
            }
            else
            {
				LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
                pComponentPrivate->cbInfo.EventHandler(
								pComponentPrivate->pHandle,
								pComponentPrivate->pHandle->pApplicationPrivate,
								OMX_EventError,
								OMX_ErrorHardware,
								OMX_TI_ErrorCritical,
								"Error from the DSP");
                goto EXIT;
            }
        }
        else
        {
            pComponentPrivate->bLCMLHalted = OMX_TRUE;
            pComponentPrivate->pHandle->SendCommand(pComponentPrivate->pHandle,
													OMX_CommandStateSet, -2, 0);

        }
    }
    if (event == EMMCodecInternalError ||
		event == EMMCodecInitError)
    {
        LOG_STATUS_LCML_API("EMMCodecInternalError || EMMCodecInitError\n");
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
				pComponentPrivate->pHandle->pApplicationPrivate,
				OMX_EventError,
				OMX_ErrorHardware,
				OMX_TI_ErrorCritical,
				"Error from the DSP");
    }
    if (event == EMMCodecStrmCtrlAck)
    {
        if ((int)argsCb [0] == USN_ERR_NONE)
        {
            LOG_STATUS_LCML_API("EMMCodecStrmCtrlAck\n");
            VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);
            VIDDEC_PTHREAD_MUTEX_SIGNAL(pComponentPrivate->sMutex);
            VIDDEC_PTHREAD_MUTEX_UNLOCK(pComponentPrivate->sMutex);
        }
        else
        {
           LOG_STATUS_LCML_API("Got usn error %d\n",(int)argsCb[0]);
        }
    }

EXIT:
    LOG_STATUS_LCML_API("---EXITING(0x%x)",eError);
    return eError;
}


/*****************************************************************************/
/*  Function Name : VIDDEC_allocate_SN_InputArgument                         */
/*  Description   : Allocate auxilliary information for the buffer           */
/*					(for extending socket node's communication structure)    */
/*					this is a codec-type dependent allocation                */
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

OMX_ERRORTYPE VIDDEC_allocate_SN_InputArgument(
								   VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								   VIDDEC_BUFFER_PRIVATE    *pBufferPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    void          *pUalgInpParams[1];
    OMX_U8*       pTemp = NULL;
    OMX_U32       tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;

    if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        OMX_MALLOC_STRUCT_DSPALIGN(pUalgInpParams[0], WMV9VDEC_UALGInputParam ,
								   sizeof(WMV9VDEC_UALGInputParam));
    }
    else if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        OMX_MALLOC_STRUCT_DSPALIGN(pUalgInpParams[0], H264VDEC_UALGInputParam ,
								   sizeof(H264VDEC_UALGInputParam));
    }
    else
    {
        OMX_MALLOC_STRUCT_DSPALIGN(pUalgInpParams[0], MPEG4VDEC_UALGInputParam ,
								   sizeof(MPEG4VDEC_UALGInputParam));
    }
    LOG_STATUS_LCML_API("Allocated %x for pUalgInpParams \n",
													(int)pUalgInpParams[0]);

    if ((ITTVDEC_UALGInputParam *)(!pUalgInpParams[0]))
    {
        LOG_STATUS_LCML_API("Error: Malloc failed\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    pTemp = (OMX_U8*)pUalgInpParams[0];
    /*pTemp += VIDDEC_PADDING_HALF;*/
    pUalgInpParams[0] = pTemp;

    if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        pBufferPrivate->pUalgParam =
					(WMV9VDEC_UALGInputParam*)(pUalgInpParams[0]);
        pBufferPrivate->nUalgParamSize = sizeof(WMV9VDEC_UALGInputParam);
    }
    else if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        pBufferPrivate->pUalgParam =
					(H264VDEC_UALGInputParam*)(pUalgInpParams[0]);
        pBufferPrivate->nUalgParamSize = sizeof(H264VDEC_UALGInputParam);
    }
    else
    {
        pBufferPrivate->pUalgParam =
					(MPEG4VDEC_UALGInputParam*)(pUalgInpParams[0]);
        pBufferPrivate->nUalgParamSize = sizeof(MPEG4VDEC_UALGInputParam);
    }

    EXIT:
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_free_SN_InputArgument                             */
/*  Description   : Free auxilliary information structure allocated for      */
/*                  the socket node                                          */
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

OMX_ERRORTYPE VIDDEC_free_SN_InputArgument(
									VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									VIDDEC_BUFFER_PRIVATE    *pBufferPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U8        *pTemp = NULL;
	OMX_U32 	  tempCompressionFormat = pComponentPrivate->
				  pInPortDef->format.video.eCompressionFormat;

    pTemp = (OMX_U8*)(pBufferPrivate->pUalgParam);
    /*pTemp -= VIDDEC_PADDING_HALF;*/
    pBufferPrivate->pUalgParam = (OMX_PTR*)pTemp;

    /* free the auxiliary info structure for the corresponding codec */
    if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        OMX_MEMFREE_STRUCT_DSPALIGN(pBufferPrivate->pUalgParam,
										H264VDEC_UALGInputParam);
    }
    else if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        OMX_MEMFREE_STRUCT_DSPALIGN(pBufferPrivate->pUalgParam,
											WMV9VDEC_UALGInputParam);
    }
    else
    {
        OMX_MEMFREE_STRUCT_DSPALIGN(pBufferPrivate->pUalgParam,
											MPEG4VDEC_UALGInputParam);
    }

    pBufferPrivate->pUalgParam = NULL;
    pBufferPrivate->nUalgParamSize = 0;

    return eError;
}

/*****************************************************************************/
/*  Function Name : OUT_DATA_BUF_PROFILE                                     */
/*  Description   : Free auxilliary information structure allocated for      */
/*                  the socket node                                          */
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

#if OUT_DATA_BUF_PROFILE
void out_data_buf_profile()
{
	struct timeval startTime;
	gettimeofday(&startTime, NULL);
	startTime.tv_usec += startTime.tv_sec * 1000 * 1000;
	if(outFreeBufFromAppIndex < MAX_PROFILE)
	{
		outFreeBufFromApp[outFreeBufFromAppIndex++] = startTime.tv_usec;
	}
	else if(MAX_PROFILE == outFreeBufFromAppIndex)
	{
		char *prefixStr = "out free buf from app at ";
		int index = 0;
		gOutDataBufAppFP = fopen("/tmp/out_free_buf.txt", "w");

		for(index = 0; index < outFreeBufFromAppIndex; index++)
				fprintf(gOutDataBufAppFP, "\n %s is %llu", prefixStr, outFreeBufFromApp[index]);

		fclose(gOutDataBufAppFP);

		outFreeBufFromAppIndex++;
    }
}
#endif
