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
/*  File Name         : OMX_VideoDec_Utils.c                                 */
/*                                                                           */
/*  Description       : contains OMX Video Decoder utility implementation.   */
/*                                                                           */
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
/*                       File Includes                                       */
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
#define LOG_TAG "720p_Video_decoder_utils"
#include <utils/Log.h>


/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_GetRMFrecuency									 */
/*                                                                           */
/*  Description   : Return the value for frecuecny to use RM.                */
/*                                                                           */
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

OMX_U32 VIDDEC_GetRMFrecuency(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
   OMX_U32 nReturnValue          = VIDDEC_MPU;
   OMX_U32 tempFrameWidth        = pComponentPrivate->pInPortDef->
								   format.video.nFrameWidth;
   OMX_U32 tempFrameHeight       = pComponentPrivate->pInPortDef->
   						           format.video.nFrameHeight;
   OMX_U32 tempCompressionFormat = pComponentPrivate->pInPortDef->
   							       format.video.eCompressionFormat;
   LOG_STATUS_UTILS("+++ENTERING\n");

#ifdef RESOURCE_MANAGER_ENABLED
    /* resolution for greater than CIF */
	if ((OMX_U16)(tempFrameWidth > VIDDEC_CIF_WIDTH) ||
		(OMX_U16)(tempFrameHeight > VIDDEC_CIF_HEIGHT))
	{
		if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
			if ((OMX_U16)tempFrameHeight > VIDDEC_D1MAX_HEIGHT ||
				(OMX_U16)tempFrameWidth > VIDDEC_D1MAX_WIDTH)
			{
				LOG_STATUS_UTILS("RM Proxy 720p Freq request for AVC\n");
				if(OMX_VIDEO_AVCProfileBaseline == pComponentPrivate->
													pH264->eProfile)
				{
					LOG_STATUS_UTILS("H264 BP RM request");
					nReturnValue = VIDDEC_RM_FREC_H264_BP_720P;
				}
				else if(OMX_VIDEO_AVCProfileMain == pComponentPrivate->
													pH264->eProfile)
				{
					LOG_STATUS_UTILS("H264 MP RM request");
					nReturnValue = VIDDEC_RM_FREC_H264_MP_720P;
				}
				else if(OMX_VIDEO_AVCProfileHigh == pComponentPrivate->
													pH264->eProfile)
				{
					LOG_STATUS_UTILS("H264 HP RM request");
					nReturnValue = VIDDEC_RM_FREC_H264_HP_720P;
				}
				else
				{
					LOG_STATUS_UTILS("H264 Max RM request");
					nReturnValue = VIDDEC_RM_FREC_H264_HP_720P;
				}
			}
			else
			{
				nReturnValue = VIDDEC_RM_FREC_H264_VGA;
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
				if ((OMX_U16)tempFrameHeight > VIDDEC_D1MAX_HEIGHT ||
					(OMX_U16)tempFrameWidth > VIDDEC_D1MAX_WIDTH)
				{
					LOG_STATUS_UTILS("RM Proxy 720p Freq request for WMV\n");
				nReturnValue = VIDDEC_RM_FREC_WMV_720P;
			}
			else
			{
				nReturnValue = VIDDEC_RM_FREC_WMV_VGA;
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
			if ((OMX_U16)tempFrameHeight > VIDDEC_D1MAX_HEIGHT ||
				(OMX_U16)tempFrameWidth > VIDDEC_D1MAX_WIDTH)
			{
					LOG_STATUS_UTILS("RM Proxy 720p Freq request for MPEG4\n");
				nReturnValue = VIDDEC_RM_FREC_MPEG4_720P;
			}
			else
			{
				nReturnValue = VIDDEC_RM_FREC_MPEG4_VGA;
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
			nReturnValue = VIDDEC_RM_FREC_H263_VGA;
		}
		else
		{
			nReturnValue = VIDDEC_MPU;
		}
	}
	/* resolution from QCIF up to CIF */
	else if (((OMX_U16)(tempFrameWidth <= VIDDEC_CIF_WIDTH) &&
		      (OMX_U16)(tempFrameWidth > VIDDEC_QCIF_WIDTH)) ||
		      ((OMX_U16)(tempFrameHeight <= VIDDEC_CIF_HEIGHT) &&
		      (OMX_U16)(tempFrameHeight > VIDDEC_QCIF_HEIGHT)))
	{
		if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
                nReturnValue = VIDDEC_RM_FREC_H264_CIF;
        }
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
                nReturnValue = VIDDEC_RM_FREC_WMV_CIF;
        }
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
                nReturnValue = VIDDEC_RM_FREC_MPEG4_CIF;
        }
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
                nReturnValue = VIDDEC_RM_FREC_H263_CIF;
        }
		else
		{
                nReturnValue = VIDDEC_MPU;
        }
    }
    /* resolution up to QCIF */
	else if (((OMX_U16)(tempFrameWidth <= VIDDEC_QCIF_WIDTH) &&
			(OMX_U16)(tempFrameWidth >= VIDDEC_MIN_WIDTH)) ||
			((OMX_U16)(tempFrameHeight <= VIDDEC_QCIF_HEIGHT) &&
			(OMX_U16)(tempFrameHeight >= VIDDEC_MIN_HEIGHT)))
	{
		if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
                nReturnValue = VIDDEC_RM_FREC_H264_QCIF;
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
                nReturnValue = VIDDEC_RM_FREC_WMV_QCIF;
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
                nReturnValue = VIDDEC_RM_FREC_MPEG4_QCIF;
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
                nReturnValue = VIDDEC_RM_FREC_H263_QCIF;
		}
		else
		{
                nReturnValue = VIDDEC_MPU;
		}
	}
	else
	{
            nReturnValue = VIDDEC_MPU;
    }
    LOG_STATUS_UTILS(
    			"Used RM Frec value = %d\n",(int)nReturnValue);
#else /* RESOURCE_MANAGER_ENABLED */
    LOG_STATUS_UTILS("Used RM Frec defaulted value = %d\n",(int)nReturnValue);
#endif /* RESOURCE_MANAGER_ENABLED */

    LOG_STATUS_UTILS("RM Frequency set : %lu", nReturnValue);
    LOG_STATUS_UTILS("---EXITING\n");

    return nReturnValue;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_RM_ExecutingToIdle   	    					 */
/*                                                                           */
/*  Description   : Free RM resources                                        */
/*                                                                           */
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
OMX_ERRORTYPE VIDDEC_RM_ExecutingToIdle(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
	OMX_U32       tempCompressionFormat = pComponentPrivate->pInPortDef->
										format.video.eCompressionFormat;
	OMX_ERRORTYPE eError			    = OMX_ErrorNone;
	LOG_STATUS_UTILS("+++ENTERING\n");
#ifdef RESOURCE_MANAGER_ENABLED
	if(pComponentPrivate->eRMProxyState != VidDec_RMPROXY_State_Unload)
	{
		OMX_PRMGR2("memory usage 2 %d : %d bytes\n",
			(unsigned int)pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0],
			(unsigned int)VIDDEC_MEMUSAGE);

		if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					 RMProxy_StateSet, OMX_720P_Decode_COMPONENT, OMX_StateIdle,
					 VIDDEC_MEMUSAGE, NULL);
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					 RMProxy_StateSet, OMX_720P_Decode_COMPONENT, OMX_StateIdle,
					 VIDDEC_MEMUSAGE, NULL);
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					 RMProxy_StateSet, OMX_H263_Decode_COMPONENT, OMX_StateIdle,
					 VIDDEC_MEMUSAGE, NULL);
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					 RMProxy_StateSet, OMX_WMV_Decode_COMPONENT, OMX_StateIdle,
					 VIDDEC_MEMUSAGE, NULL);
		}
	}
#endif
	LOG_STATUS_UTILS("---EXITING\n");
	return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_RM_LoadedToIdle   								 */
/*                                                                           */
/*  Description   : Free RM resources                                        */
/*                                                                           */
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
OMX_ERRORTYPE VIDDEC_RM_LoadedToIdle(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
	OMX_U32       tempCompressionFormat = pComponentPrivate->pInPortDef->
											format.video.eCompressionFormat;
    OMX_ERRORTYPE eError			    = OMX_ErrorNone;
	LOG_STATUS_UTILS("+++ENTERING\n");

#ifdef RESOURCE_MANAGER_ENABLED
	pComponentPrivate->rmproxyCallback.RMPROXY_Callback =
									(void *)VIDDEC_ResourceManagerCallback;

	if(pComponentPrivate->eRMProxyState != VidDec_RMPROXY_State_Unload)
	{
		LOG_STATUS_UTILS("RMProxy request resource\n");
		OMX_PRMGR2("memory usage 1 %u : %u bytes\n",
		(unsigned int)pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0],
		(unsigned int)VIDDEC_MEMUSAGE);

		if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
				RMProxy_RequestResource,
				OMX_720P_Decode_COMPONENT,
				VIDDEC_GetRMFrecuency(pComponentPrivate),
				VIDDEC_MEMUSAGE, &(pComponentPrivate->rmproxyCallback));
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
			LOG_STATUS_UTILS(
					"RMProxy request resource for MPEG4 decoder\n");

			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
				RMProxy_RequestResource,
				OMX_720P_Decode_COMPONENT,
				VIDDEC_GetRMFrecuency(pComponentPrivate),
				VIDDEC_MEMUSAGE, &(pComponentPrivate->rmproxyCallback));

			LOG_STATUS_UTILS(
				"RMProxy request resource for MPEG4 decoder succeeded\n");
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
						RMProxy_RequestResource,
						OMX_H263_Decode_COMPONENT,
						VIDDEC_GetRMFrecuency(pComponentPrivate),
						VIDDEC_MEMUSAGE,
						&(pComponentPrivate->rmproxyCallback));
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
				RMProxy_RequestResource,
				OMX_WMV_Decode_COMPONENT,
				VIDDEC_GetRMFrecuency(pComponentPrivate),
				VIDDEC_MEMUSAGE,
				&(pComponentPrivate->rmproxyCallback));
		}
		else
		{
			LOG_STATUS_UTILS(
					"RMProxy request resource unsupported setting\n");

			eError = OMX_ErrorUnsupportedSetting;
			pComponentPrivate->cbInfo.EventHandler(
				pComponentPrivate->pHandle,
				pComponentPrivate->pHandle->pApplicationPrivate,
				OMX_EventError,
				OMX_ErrorUnsupportedSetting,
				OMX_TI_ErrorMajor,
				"RM SendCommand OMX_ErrorUnsupportedSetting Error");

			LOG_STATUS_UTILS(
				"OMX_ErrorUnsupportedSetting 0x%x\n",eError);
			goto EXIT;
		}

		pComponentPrivate->eRMProxyState = VidDec_RMPROXY_State_Registered;
		if (eError != OMX_ErrorNone)
		{
			LOG_STATUS_UTILS(
			"RMProxy request resource failed 0x%x\n", eError);
			pComponentPrivate->eState = OMX_StateLoaded;

			pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorInsufficientResources,
						OMX_TI_ErrorMajor,
						"RM SendCommand Error");

			LOG_STATUS_UTILS("OMX_ErrorUnsupportedSetting 0x%x\n",eError);
			goto EXIT;
		}

		LOG_STATUS_UTILS("RMProxy state set to registered\n");
	}
#endif

EXIT:
	LOG_STATUS_UTILS("---EXITING\n");
	return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_RM_StateSetToExecute								 */
/*                                                                           */
/*  Description   : Free RM resources                                        */
/*                                                                           */
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
OMX_ERRORTYPE VIDDEC_RM_StateSetToExecute(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
	OMX_U32       tempCompressionFormat = pComponentPrivate->pInPortDef->
											format.video.eCompressionFormat;
    OMX_ERRORTYPE eError			    = OMX_ErrorNone;
	LOG_STATUS_UTILS("+++ENTERING\n");

#ifdef RESOURCE_MANAGER_ENABLED
    if(pComponentPrivate->eRMProxyState != VidDec_RMPROXY_State_Unload)
    {
        OMX_PRMGR2("memory usage 3 %d : %d bytes\n",
			(unsigned int)pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0],
			(unsigned int)VIDDEC_MEMUSAGE);

        if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
        {
            eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
							RMProxy_StateSet, OMX_720P_Decode_COMPONENT,
							OMX_StateExecuting, VIDDEC_MEMUSAGE, NULL);
        }
        else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
        {
            eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					 RMProxy_StateSet, OMX_720P_Decode_COMPONENT,
					 OMX_StateExecuting, VIDDEC_MEMUSAGE, NULL);
        }
        else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
        {
            eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
						RMProxy_StateSet, OMX_H263_Decode_COMPONENT,
						OMX_StateExecuting, VIDDEC_MEMUSAGE, NULL);
        }
        else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
        {
            eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
						RMProxy_StateSet, OMX_WMV_Decode_COMPONENT,
						OMX_StateExecuting, VIDDEC_MEMUSAGE, NULL);
        }

        else
        {
            eError = OMX_ErrorUnsupportedSetting;
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorUnsupportedSetting,
						OMX_TI_ErrorMinor,
						"RM SendCommand OMX_ErrorUnsupportedSetting Error");

            LOG_STATUS_UTILS("OMX_ErrorUnsupportedSetting 0x%x\n",eError);
            goto EXIT;
        }
        if (eError != OMX_ErrorNone)
        {
            pComponentPrivate->eState = OMX_StateLoaded;
             pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							OMX_ErrorInsufficientResources,
							OMX_TI_ErrorMajor,
							"RM SendCommand Error");
             LOG_STATUS_UTILS(
							"OMX_ErrorUnsupportedSetting 0x%x\n",eError);
             goto EXIT;
        }
    }
#endif

EXIT:
	LOG_STATUS_UTILS("---EXITING\n");
	return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_RM_free_resources								 */
/*                                                                           */
/*  Description   : Free RM resources                                        */
/*                                                                           */
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
OMX_ERRORTYPE VIDDEC_RM_free_resources(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
	OMX_U32       tempCompressionFormat = pComponentPrivate->pInPortDef->
											format.video.eCompressionFormat;
    OMX_ERRORTYPE eError			    = OMX_ErrorNone;
	LOG_STATUS_UTILS("+++ENTERING\n");

#ifdef RESOURCE_MANAGER_ENABLED
	if(pComponentPrivate->eRMProxyState == VidDec_RMPROXY_State_Registered)
	{
		OMX_PRMGR2("memory usage 4 %d : %d bytes\n",
			(unsigned int)pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0],
			(unsigned int)VIDDEC_MEMUSAGE);

		if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					RMProxy_FreeResource, OMX_720P_Decode_COMPONENT,
					 0, VIDDEC_MEMUSAGE, NULL);
			if (eError != OMX_ErrorNone)
			{
				 LOG_STATUS_UTILS(
				 "Error returned from destroy ResourceManagerProxy thread\n");
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					RMProxy_FreeResource, OMX_WMV_Decode_COMPONENT,
					0, VIDDEC_MEMUSAGE, NULL);
			if (eError != OMX_ErrorNone)
			{
				LOG_STATUS_UTILS(
				"Error returned from destroy ResourceManagerProxy thread\n");
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		 {
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
						RMProxy_FreeResource, OMX_720P_Decode_COMPONENT,
						0, VIDDEC_MEMUSAGE, NULL);
			if (eError != OMX_ErrorNone)
			{
				LOG_STATUS_UTILS(
				"Error returned from destroy ResourceManagerProxy thread\n");
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
			eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
					 RMProxy_FreeResource, OMX_H263_Decode_COMPONENT,
					 0, VIDDEC_MEMUSAGE, NULL);
			if (eError != OMX_ErrorNone)
			{
				 LOG_STATUS_UTILS(
				 "Error returned from destroy ResourceManagerProxy thread\n");
			}
		}
		else
		{
			eError = OMX_ErrorUnsupportedSetting;
			goto EXIT;
		}
		pComponentPrivate->eRMProxyState = VidDec_RMPROXY_State_Load;
	}
	if(pComponentPrivate->eRMProxyState != VidDec_RMPROXY_State_Unload)
    {
        LOG_STATUS_UTILS("RMProxy_DeinitalizeEx \n");
        eError = RMProxy_DeinitalizeEx(OMX_COMPONENTTYPE_VIDEO);
        if (eError != OMX_ErrorNone)
        {
            LOG_STATUS_UTILS(
            	"Error returned from destroy ResourceManagerProxy thread\n");
            LOG_STATUS_UTILS(
            						"RMProxy_DeinitalizeEx 0x%x\n", eError);
        }
        pComponentPrivate->eRMProxyState = VidDec_RMPROXY_State_Unload;
    }
#endif

EXIT:
	LOG_STATUS_UTILS("---EXITING\n");
	return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_Init									     */
/*                                                                           */
/*  Description   : Initialize Circular Buffer  							 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_CircBuf_Init(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
								  VIDDEC_CBUFFER_TYPE       nTypeIndex,
								  VIDDEC_PORT_INDEX         nPortIndex)
{
    OMX_ERRORTYPE           eError       = OMX_ErrorNone;
    VIDDEC_CIRCULAR_BUFFER  *pTempCBuffer = NULL;
    OMX_U32                 nCount       = 0;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        return eError;
    }
#ifdef VIDDEC_CBUFFER_LOCK
    if(pTempCBuffer->m_lock != NULL)
    {
        free(pTempCBuffer->m_lock);
        pTempCBuffer->m_lock = NULL;
    }
    OMX_MALLOC_STRUCT(pTempCBuffer->m_lock, pthread_mutex_t,pComponentPrivate->
    					nMemUsage[VIDDDEC_Enum_MemLevel3]);
    pthread_mutex_init(pTempCBuffer->m_lock, NULL);
#endif
    for(nCount = 0; nCount >= CBUFFER_SIZE; nCount++)
    {
        pTempCBuffer->pElement[nCount] = NULL;
    }
EXIT:
    pTempCBuffer->nCount = 0;
    pTempCBuffer->nHead = 0;
    pTempCBuffer->nTail = 0;
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_Flush   								     */
/*                                                                           */
/*  Description   : Flush Circular Buffer 		 							 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_CircBuf_Flush(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
								   VIDDEC_CBUFFER_TYPE       nTypeIndex,
								   VIDDEC_PORT_INDEX         nPortIndex)
{
    OMX_ERRORTYPE           eError       = OMX_ErrorNone;
    VIDDEC_CIRCULAR_BUFFER *pTempCBuffer = NULL;
    OMX_U32                 nCount       = 0;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        return eError;
    }
#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_lock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif
    for (nCount = pTempCBuffer->nTail; nCount <= pTempCBuffer->nHead; nCount++)
    {
        pTempCBuffer->pElement[nCount] = NULL;
    }
    pTempCBuffer->nCount = 0;
    pTempCBuffer->nHead = 0;
    pTempCBuffer->nTail = 0;

#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_unlock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_DeInit									 */
/*                                                                           */
/*  Description   : Initialize Circular Buffer  							 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_CircBuf_DeInit(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
									VIDDEC_CBUFFER_TYPE       nTypeIndex,
									VIDDEC_PORT_INDEX         nPortIndex)
{
    OMX_ERRORTYPE           eError       = OMX_ErrorNone;
    VIDDEC_CIRCULAR_BUFFER *pTempCBuffer = NULL;
    OMX_U32                 nCount       = 0;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        return eError;
    }
#ifdef VIDDEC_CBUFFER_LOCK
    if(pTempCBuffer->m_lock != NULL)
    {
        free(pTempCBuffer->m_lock);
        pTempCBuffer->m_lock = NULL;
    }
#endif
    for(nCount = 0; nCount >= CBUFFER_SIZE; nCount++)
    {
        pTempCBuffer->pElement[nCount] = NULL;
    }
    LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_GetHead  							     */
/*                                                                           */
/*  Description   : Get the head of the Circular Buffer						 */
/*                                                                           */
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
OMX_U8 VIDDEC_CircBuf_GetHead(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							  VIDDEC_CBUFFER_TYPE 	    nTypeIndex,
							  VIDDEC_PORT_INDEX		    nPortIndex)
{
    VIDDEC_CIRCULAR_BUFFER *pTempCBuffer = NULL;
    OMX_U8 				   ucHead = 0;

    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        ucHead = 0;
        return 0;
    }

#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_lock(pTempCBuffer->m_lock) != 0)
    {
        return 0;
    }
#endif /* VIDDEC_CBUFFER_LOCK */

    ucHead = pTempCBuffer->nHead;

#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_unlock(pTempCBuffer->m_lock) != 0)
    {
        return 0;
    }
#endif /* VIDDEC_CBUFFER_LOCK */

    return ucHead;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_Add									     */
/*                                                                           */
/*  Description   : Initialize Circular Buffer  							 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_CircBuf_Add(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
								 VIDDEC_CBUFFER_TYPE       nTypeIndex,
								 VIDDEC_PORT_INDEX         nPortIndex,
								 OMX_PTR                   pElement)
{
    OMX_ERRORTYPE              eError = OMX_ErrorNone;
    VIDDEC_CIRCULAR_BUFFER     *pTempCBuffer = NULL;
    VIDDEC_CBUFFER_BUFFERFLAGS *tmp1 = (VIDDEC_CBUFFER_BUFFERFLAGS *)pElement;
    VIDDEC_CBUFFER_BUFFERFLAGS *tmp2 = NULL;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        return eError;
    }

#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_lock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif /*                         VIDDEC_CBUFFER_LOCK                        */

	/* Assign tmp2 to the largest element present in the queue. The head will */
	/* always have the largest timestamp as the queue is sorted               */
	if(pTempCBuffer->nHead == 0)
	{
		tmp2 = (VIDDEC_CBUFFER_BUFFERFLAGS *)pTempCBuffer->
											pElement[CBUFFER_SIZE - 1];
	}
	else
	{
		tmp2 = (VIDDEC_CBUFFER_BUFFERFLAGS *)pTempCBuffer->
											 pElement[pTempCBuffer->nHead - 1];
	}

	/* If the count is not zero but the head is NULL it means that something */
	/* has gone wrong                                                        */
	if((0 != pTempCBuffer->nCount) && (NULL == tmp2))
	{
		eError = OMX_ErrorHardware;
        return eError;
	}

	/* If the current timestamp is the first timestamp i.e the queue is empty */
	/* or the current timestamp is greater than the last timestamp stored     */
	/* in the queue, just add the current timestamp to the queue              */
	if((0 == pTempCBuffer->nCount) || (tmp1->nTimeStamp >= tmp2->nTimeStamp))
	{
		pTempCBuffer->pElement[pTempCBuffer->nHead++] = pElement;
		pTempCBuffer->nCount++;

		if(pTempCBuffer->nHead >= CBUFFER_SIZE)
		{
			pTempCBuffer->nHead = 0;
		}
	}
	/* If the current timestamp is lesser than the last timestamp stored    */
	/* in the queue, it has to be inserted in the queue at its proper place */
	else
	{
		OMX_U32 head  = pTempCBuffer->nHead;
		OMX_U32 count = pTempCBuffer->nCount;
		OMX_S32 i = 0;

		/* traverse through the queue till we find a timestamp that is lesser */
		/* than the current timestamp                                         */
		for(i = 0; i <= count; i++)
		{
			if(head == 0)
				head = CBUFFER_SIZE;

			tmp2 = (VIDDEC_CBUFFER_BUFFERFLAGS *)pTempCBuffer->
														pElement[head - 1];

			/* tmp2 will be NULL when we have reached the end of the queue */
			/* and find that the current timestamp is the least timestamp  */
			/* that is  present in the queue                               */
			if((tmp2 == NULL) || (tmp1->nTimeStamp >= tmp2->nTimeStamp))
			{
				OMX_S32 j = 0;
				OMX_S32 k = 0;

				/* When we find the place where the ts has to be inserted, */
				/* all the timestamps that are ahead of this place are     */
				/* shifted												   */
				for(j = 0; j < i; j++)
				{
					k = pTempCBuffer->nHead - j;
					if(0 == k)
					{
						pTempCBuffer->pElement[k] =
							          pTempCBuffer->pElement[CBUFFER_SIZE - 1];
					}
					else if(k < 0)
					{
						pTempCBuffer->pElement[CBUFFER_SIZE + k] =
							 pTempCBuffer->pElement[CBUFFER_SIZE + k - 1];
					}
					else
					{
						pTempCBuffer->pElement[k] =
									  pTempCBuffer->pElement[k - 1];
					}
				}

				/* Assign the current ts to the hole that was created in the */
				/* list                                                      */
				if(0 == k)
				{
					pTempCBuffer->pElement[CBUFFER_SIZE - 1] = pElement;
				}
				else if(k < 0)
				{
					pTempCBuffer->pElement[CBUFFER_SIZE + k - 1] = pElement;
				}
				else
				{
					pTempCBuffer->pElement[k - 1] = pElement;
				}

				/* Increment the head and the count */
				pTempCBuffer->nCount++;
				pTempCBuffer->nHead++;
				if(pTempCBuffer->nHead >= CBUFFER_SIZE)
				{
					pTempCBuffer->nHead = 0;
				}

				/* Timestamp has been added in the proper place so break out */
				/* of the loop												 */
				break;
			}
			else
			{
				/* Decrement the head to compare the current ts to the prev */
				/* one in the list                                          */
				head--;
			}
		}/* End of For loop comparing the current ts to the stored ones */
	}

#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_unlock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif /*                         VIDDEC_CBUFFER_LOCK                        */
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_Remove									 */
/*                                                                           */
/*  Description   : Initialize Circular Buffer  							 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_CircBuf_Remove(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
									VIDDEC_CBUFFER_TYPE       nTypeIndex,
									VIDDEC_PORT_INDEX         nPortIndex,
									OMX_PTR*                  pElement)
{
    OMX_ERRORTYPE           eError       = OMX_ErrorNone;
    VIDDEC_CIRCULAR_BUFFER *pTempCBuffer = NULL;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        return eError;
    }
#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_lock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif /*                        VIDDEC_CBUFFER_LOCK                         */
    if(pTempCBuffer->nCount)
    {
        *pElement = pTempCBuffer->pElement[pTempCBuffer->nTail];
        pTempCBuffer->pElement[pTempCBuffer->nTail++] = NULL;
        pTempCBuffer->nCount--;
        if(pTempCBuffer->nTail >= CBUFFER_SIZE)
        {
            pTempCBuffer->nTail = 0;
        }
    }
    else
    {
        *pElement = NULL;
    }
#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_unlock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif /*                        VIDDEC_CBUFFER_LOCK                         */
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CircBuf_Count									 */
/*                                                                           */
/*  Description   : Initialize Circular Buffer  							 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_CircBuf_Count(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
								   VIDDEC_CBUFFER_TYPE       nTypeIndex,
								   VIDDEC_PORT_INDEX         nPortIndex,
								   OMX_U8*                   pCount)
{
    OMX_ERRORTYPE           eError       = OMX_ErrorNone;
    VIDDEC_CIRCULAR_BUFFER *pTempCBuffer = NULL;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(nTypeIndex == VIDDEC_CBUFFER_TIMESTAMP)
    {
        pTempCBuffer = &pComponentPrivate->pCompPort[nPortIndex]->eTimeStamp;
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        pCount = 0;
        return eError;
    }
#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_lock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif /*                           VIDDEC_CBUFFER_LOCK                      */

    *pCount = pTempCBuffer->nCount;

#ifdef VIDDEC_CBUFFER_LOCK
    if(pthread_mutex_unlock(pTempCBuffer->m_lock) != 0)
    {
        eError = OMX_ErrorHardware;
        return eError;
    }
#endif /*                          VIDDEC_CBUFFER_LOCK                       */
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_Start_ComponentThread							 */
/*                                                                           */
/*  Description   : starts the component thread and all the pipes to achieve */
/*                  communication between dsp and application for commands 	 */
/*                  and buffer interchanging  		 						 */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_Start_ComponentThread(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE             eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE		  *pComp = (OMX_COMPONENTTYPE*)hComponent;
    VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate =
						(VIDDEC_COMPONENT_PRIVATE*)pComp->pComponentPrivate;

    LOG_STATUS_UTILS("+++ENTERING\n");

    /* create the pipe used to maintain free input buffers */
    eError = pipe(pComponentPrivate->free_inpBuf_Q);
    if (eError)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* create the pipe used to maintain free output buffers */
    eError = pipe(pComponentPrivate->free_outBuf_Q);
    if (eError)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* create the pipe used to maintain dsp output/encoded buffers */
    eError = pipe(pComponentPrivate->filled_outBuf_Q);
    if (eError)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /*  create the pipe used to send commands to the OMX-IL component */
    eError = pipe(pComponentPrivate->cmdPipe);
    if (eError)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /*  create the pipe used to send commands to the OMX-IL component */
    eError = pipe(pComponentPrivate->cmdDataPipe);
    if (eError)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    eError = create_split_pipes(pComponentPrivate);
    if(eError) goto EXIT;

    /* Create the Component Thread */
    eError = pthread_create(&(pComponentPrivate->ComponentThread),
                            NULL,
                            OMX_VidDec_Thread,
                            pComponentPrivate);

    LOG_STATUS_UTILS("pthread_create 0x%lx\n",
    			(OMX_U32) pComponentPrivate->ComponentThread);

    if (eError || !pComponentPrivate->ComponentThread)
    {
        LOG_STATUS_UTILS("pthread_create  0x%x\n",eError);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

#ifdef __PERF_INSTRUMENTATION__
    PERF_ThreadCreated(pComponentPrivate->pPERF,
                       pComponentPrivate->ComponentThread,
                       PERF_FOURS("VD T"));
#endif /* __PERF_INSTRUMENTATION__ */

EXIT:
    LOG_STATUS_UTILS("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_Stop_ComponentThread							     */
/*                                                                           */
/*  Description   : This function is called by the component during de-init  */
/*                  to close component thread, Command pipe & data pipes.	 */
/*                                                                           */
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
OMX_ERRORTYPE VIDDEC_Stop_ComponentThread(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*        pHandle           = (OMX_COMPONENTTYPE*)pComponent;
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate =
				(VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
    OMX_ERRORTYPE             threadError       = OMX_ErrorNone;
    OMX_ERRORTYPE             err               = OMX_ErrorNone;
    OMX_PTR                   pApp_ptr          = pComponentPrivate->pHandle->
    														pApplicationPrivate;
    int                       pthreadError      = 0;

    /* Join the component thread */
    LOG_STATUS_UTILS("+++ENTERING\n");
    LOG_STATUS_UTILS("pthread_join 0x%lx\n",
    			(OMX_U32) pComponentPrivate->ComponentThread);

    if(pComponentPrivate->bLCMLOut == OMX_TRUE)
    {
        if (0 != pthreadError)
        {
            eError = OMX_ErrorHardware;
        }
    }
    else
    {
        pthreadError = pthread_join(pComponentPrivate->ComponentThread,
        												(void*)&threadError);
        if (0 != pthreadError)
        {
            eError = OMX_ErrorHardware;
        }
    }

    /* Check for the errors */
    if (OMX_ErrorNone != eError)
    {
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorSevere,
                                               "Error while closing Component Thread\n");
    }

    /* close the data pipe handles */
    err = close(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_READ]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing data pipe\n");
    }

    err = close(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_READ]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing data pipe\n");
    }

    err = close(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_READ]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing data pipe\n");
    }

    err = close(pComponentPrivate->free_inpBuf_Q[VIDDEC_PIPE_WRITE]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing data pipe\n");
    }

    err = close(pComponentPrivate->free_outBuf_Q[VIDDEC_PIPE_WRITE]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing data pipe\n");
    }

    err = close(pComponentPrivate->filled_outBuf_Q[VIDDEC_PIPE_WRITE]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing data pipe\n");
    }

    /* Close the command pipe handles */
    err = close(pComponentPrivate->cmdPipe[VIDDEC_PIPE_READ]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing cmd pipe\n");
    }

    err = close(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing cmd pipe\n");
    }

    /* Close the command data pipe handles */
    err = close (pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_READ]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing cmd pipe\n");
    }

    err = close (pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE]);
    if (0 != err)
    {
        eError = OMX_ErrorHardware;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pApp_ptr,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               "Error while closing cmd pipe\n");
    }


    eError = close_split_pipes(pComponentPrivate);

    LOG_STATUS_UTILS("---EXITING(0x%x)\n",eError);

    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_ResourceManagerCallback					         */
/*                                                                           */
/*  Description   : Returns Resource manager callback                        */
/*                              	                                         */
/*                                                                           */
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
#ifdef RESOURCE_MANAGER_ENABLED
void VIDDEC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData)
{

    VIDDEC_COMPONENT_PRIVATE *pCompPrivate = NULL;
    OMX_COMPONENTTYPE 		 *pHandle = (OMX_COMPONENTTYPE *)cbData.hComponent;
	LOG_STATUS_UTILS("+++ENTERING\n");

    pCompPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    if (*(cbData.RM_Error) == OMX_RmProxyCallback_FatalError)
    {
        LOG_STATUS_UTILS("\nOMX_RmProxyCallback_FatalError");
        VIDDEC_FatalErrorRecover(pCompPrivate);
    }
    LOG_STATUS_UTILS("---EXITING\n");
}
#endif

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_CloseCodec					         			 */
/*                                                                           */
/*  Description   : Destroys the codec when error has occured. LCML resources*/
/*                  and RM resources are freed.                              */
/*                              	                                         */
/*                                                                           */
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
OMX_ERRORTYPE VIDDEC_CloseCodec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE       eError                = OMX_ErrorUndefined;
    LCML_DSP_INTERFACE *pLcmlHandle           = NULL;
    OMX_U32             tempCompressionFormat = pComponentPrivate->pInPortDef->
												format.video.eCompressionFormat;
    LOG_STATUS_UTILS("+++ENTERING\n");

    if(pComponentPrivate->pModLCML == NULL)
    {
        goto EXIT;
    }
    if (!(pComponentPrivate->eState == OMX_StateLoaded) &&
        !(pComponentPrivate->eState == OMX_StateWaitForResources))
    {
        pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->pLCML;
        if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
           pComponentPrivate->pLCML != NULL)
        {
            eError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
            							pCodecinterfacehandle,
                                      EMMCodecControlDestroy, NULL);
            /* intentionally skip error checking on codec destroy since an */
            /* error is actually expected                                  */
            LOG_STATUS_UTILS(
                       "LCML_ControlCodec called EMMCodecControlDestroy 0x%p\n",
                       pLcmlHandle);
            pComponentPrivate->eLCMLState = VidDec_LCML_State_Destroy;
            LOG_STATUS_UTILS(
                       "LCML_ControlCodec called EMMCodecControlDestroy 0x%p\n",
                        pLcmlHandle);

            {
                sleep(1);
                pComponentPrivate->bBuffFound = OMX_TRUE;
                pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                                       pComponentPrivate->pHandle->pApplicationPrivate,
                                                       OMX_EventBufferFlag,
                                                       VIDDEC_OUTPUT_PORT,
                                                       OMX_BUFFERFLAG_EOS,
                                                        NULL);

                pComponentPrivate->EOSHasBeenSentToAPP = OMX_TRUE;

                pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                                       pComponentPrivate->pHandle->pApplicationPrivate,
                                                       OMX_EventError,
                                                       OMX_ErrorHardware,
                                                       OMX_TI_ErrorSevere,
                                                       "Timeout occured on DSP input and output buffers\n");
                sleep(5);
            }

            if(pComponentPrivate->pModLCML != NULL)
            {
                dlclose(pComponentPrivate->pModLCML);
                pComponentPrivate->pModLCML = NULL;
                pComponentPrivate->pLCML = NULL;
            }

            pComponentPrivate->eLCMLState = VidDec_LCML_State_Unload;

            pComponentPrivate->bLCMLHalted = OMX_TRUE;
        }
    }

#ifdef RESOURCE_MANAGER_ENABLED
    if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
        LOG_STATUS_UTILS("\n VIDDEC_CloseCodec : RM frees resources");
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
    }

    if (eError != OMX_ErrorNone)
    {
        LOG_STATUS_UTILS(
        			"::RMProxy_FreeResource failed in FatalErrorRecover\n");
    }
    eError = RMProxy_Deinitalize();
    LOG_STATUS_UTILS("\n VIDDEC_CloseCodec : RMProxy_Deinitalize");
    if (eError != OMX_ErrorNone)
    {
        LOG_STATUS_UTILS("::From RMProxy_Deinitalize\n");
    }
#endif
    /* regardless of success from above,            */
    /* still send the Invalid State error to client */

EXIT:
    LOG_STATUS_UTILS("---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_FatalErrorRecover					             */
/*                                                                           */
/*  Description   : Destroys the codec when error has occured.LCML resources */
/*                  and RM resources are freed.                              */
/*                              	                                         */
/*                                                                           */
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

OMX_ERRORTYPE VIDDEC_FatalErrorRecover(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE                       eError = OMX_ErrorUndefined;
    LCML_DSP_INTERFACE                  *pLcmlHandle = NULL;
    OMX_U32 tempCompressionFormat      =pComponentPrivate->pInPortDef->
   							         format.video.eCompressionFormat;
    LOG_STATUS_UTILS("+++ENTERING\n");
    if(pComponentPrivate->pModLCML == NULL)
    {
        goto EXIT;
    }
    if (!(pComponentPrivate->eState == OMX_StateLoaded) &&
        !(pComponentPrivate->eState == OMX_StateWaitForResources))
    {
        pLcmlHandle = (LCML_DSP_INTERFACE*)pComponentPrivate->pLCML;
        if(pComponentPrivate->eLCMLState != VidDec_LCML_State_Unload &&
            pComponentPrivate->pLCML != NULL)
        {
            LOG_STATUS_UTILS("\n VIDDEC_FatalErrorRecover : LCML ControlCodec is called");
            LOG_STATUS_UTILS("LCML_ControlCodec called EMMCodecControlDestroy 0x%p\n",
							  pLcmlHandle);
            eError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
            							pCodecinterfacehandle,
                                      EMMCodecControlDestroy, NULL);

            /* intentionally skip error checking on codec destroy since an   */
            /* error is actually expected                                    */
            pComponentPrivate->eLCMLState = VidDec_LCML_State_Destroy;

            if(pComponentPrivate->pModLCML != NULL)
            {
                dlclose(pComponentPrivate->pModLCML);
                pComponentPrivate->pModLCML = NULL;
                pComponentPrivate->pLCML = NULL;
                LOG_STATUS_UTILS("\n VIDDEC_FatalErrorRecover: LCML is closed");
            }

            pComponentPrivate->eLCMLState = VidDec_LCML_State_Unload;

            pComponentPrivate->bLCMLHalted = OMX_TRUE;
        }
    }

#ifdef RESOURCE_MANAGER_ENABLED
    if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
        LOG_STATUS_UTILS("\n VIDDEC_FatalErrorRecover : RM frees resources");
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
        								RMProxy_FreeResource,
        								OMX_720P_Decode_COMPONENT,
        								VIDDEC_GetRMFrecuency(pComponentPrivate),
        								VIDDEC_MEMUSAGE,
        								&(pComponentPrivate->rmproxyCallback));
    }

    if (eError != OMX_ErrorNone)
    {
        LOG_STATUS_UTILS("::RMProxy_FreeResource failed in FatalErrorRecover\n");
    }

    LOG_STATUS_UTILS("\n VIDDEC_FatalErrorRecover : RMProxy_Deinitalize");
    eError = RMProxy_Deinitalize();

    if (eError != OMX_ErrorNone)
    {
        LOG_STATUS_UTILS("::From RMProxy_Deinitalize\n");
    }
#endif
    /*  regardless of success from above,            */
    /*  still send the Invalid State error to client */
    LOG_STATUS_UTILS("\n VIDDEC_FatalErrorRecover : VIDDEC_Handle_InvalidState");
    eError = VIDDEC_Handle_InvalidState(pComponentPrivate);

EXIT:
    LOG_STATUS_UTILS("---EXITING(0x%x)\n",eError);
   return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMXIndex2StringArray					                 */
/*                                                                           */
/*  Description   : Returns Index accordingly                                */
/*                              	                                         */
/*                                                                           */
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
char *OMXIndex2StringArray(OMX_U32 index)
{
    if(index == OMX_IndexComponentStartUnused)                           return "OMX_IndexComponentStartUnused = 0x01000000";
    if(index == OMX_IndexParamPriorityMgmt)                              return "OMX_IndexParamPriorityMgmt";
    if(index == OMX_IndexParamAudioInit)                                 return "OMX_IndexParamAudioInit";
    if(index == OMX_IndexParamImageInit)                                 return "OMX_IndexParamImageInit";
    if(index == OMX_IndexParamVideoInit)                                 return "OMX_IndexParamVideoInit";
    if(index == OMX_IndexParamOtherInit)                                 return "OMX_IndexParamOtherInit";
    if(index == OMX_IndexParamNumAvailableStreams)                       return "OMX_IndexParamNumAvailableStreams";
    if(index == OMX_IndexParamActiveStream)                              return "OMX_IndexParamActiveStream";
    if(index == OMX_IndexParamSuspensionPolicy)                          return "OMX_IndexParamSuspensionPolicy";
    if(index == OMX_IndexParamComponentSuspended)                        return "OMX_IndexParamComponentSuspended";
    if(index == OMX_IndexConfigCapturing)                                return "OMX_IndexConfigCapturing";
    if(index == OMX_IndexConfigCaptureMode)                              return "OMX_IndexConfigCaptureMode";
    if(index == OMX_IndexAutoPauseAfterCapture)                          return "OMX_IndexAutoPauseAfterCapture";
    if(index == OMX_IndexParamContentURI)                                return "OMX_IndexParamContentURI";
    if(index == OMX_IndexParamCustomContentPipe)                         return "OMX_IndexParamCustomContentPipe";
    if(index == OMX_IndexParamDisableResourceConcealment)                return "OMX_IndexParamDisableResourceConcealment";
    if(index == OMX_IndexConfigMetadataItemCount)                        return "OMX_IndexConfigMetadataItemCount";
    if(index == OMX_IndexConfigContainerNodeCount)                       return "OMX_IndexConfigContainerNodeCount";
    if(index == OMX_IndexConfigMetadataItem)                             return "OMX_IndexConfigMetadataItem";
    if(index == OMX_IndexConfigCounterNodeID)                            return "OMX_IndexConfigCounterNodeID";
    if(index == OMX_IndexParamMetadataFilterType)                        return "OMX_IndexParamMetadataFilterType";
    if(index == OMX_IndexParamMetadataKeyFilter)                         return "OMX_IndexParamMetadataKeyFilter";
    if(index == OMX_IndexConfigPriorityMgmt)                             return "OMX_IndexConfigPriorityMgmt";
    if(index == OMX_IndexParamStandardComponentRole)                     return "OMX_IndexParamStandardComponentRole";
    if(index == OMX_IndexPortStartUnused)                                return "OMX_IndexPortStartUnused = 0x02000000";
    if(index == OMX_IndexParamPortDefinition)                            return "OMX_IndexParamPortDefinition";
    if(index == OMX_IndexParamCompBufferSupplier)                        return "OMX_IndexParamCompBufferSupplier";
    if(index == OMX_IndexReservedStartUnused)                            return "OMX_IndexReservedStartUnused = 0x03000000";
    if(index == OMX_IndexAudioStartUnused)                               return "OMX_IndexAudioStartUnused = 0x04000000";
    if(index == OMX_IndexParamAudioPortFormat)                           return "OMX_IndexParamAudioPortFormat";
    if(index == OMX_IndexParamAudioPcm)                                  return "OMX_IndexParamAudioPcm";
    if(index == OMX_IndexParamAudioAac)                                  return "OMX_IndexParamAudioAac";
    if(index == OMX_IndexParamAudioRa)                                   return "OMX_IndexParamAudioRa";
    if(index == OMX_IndexParamAudioMp3)                                  return "OMX_IndexParamAudioMp3";
    if(index == OMX_IndexParamAudioAdpcm)                                return "OMX_IndexParamAudioAdpcm";
    if(index == OMX_IndexParamAudioG723)                                 return "OMX_IndexParamAudioG723";
    if(index == OMX_IndexParamAudioG729)                                 return "OMX_IndexParamAudioG729";
    if(index == OMX_IndexParamAudioAmr)                                  return "OMX_IndexParamAudioAmr";
    if(index == OMX_IndexParamAudioWma)                                  return "OMX_IndexParamAudioWma";
    if(index == OMX_IndexParamAudioSbc)                                  return "OMX_IndexParamAudioSbc";
    if(index == OMX_IndexParamAudioMidi)                                 return "OMX_IndexParamAudioMidi";
    if(index == OMX_IndexParamAudioGsm_FR)                               return "OMX_IndexParamAudioGsm_FR";
    if(index == OMX_IndexParamAudioMidiLoadUserSound)                    return "OMX_IndexParamAudioMidiLoadUserSound";
    if(index == OMX_IndexParamAudioG726)                                 return "OMX_IndexParamAudioG726";
    if(index == OMX_IndexParamAudioGsm_EFR)                              return "OMX_IndexParamAudioGsm_EFR";
    if(index == OMX_IndexParamAudioGsm_HR)                               return "OMX_IndexParamAudioGsm_HR";
    if(index == OMX_IndexParamAudioPdc_FR)                               return "OMX_IndexParamAudioPdc_FR";
    if(index == OMX_IndexParamAudioPdc_EFR)                              return "OMX_IndexParamAudioPdc_EFR";
    if(index == OMX_IndexParamAudioPdc_HR)                               return "OMX_IndexParamAudioPdc_HR";
    if(index == OMX_IndexParamAudioTdma_FR)                              return "OMX_IndexParamAudioTdma_FR";
    if(index == OMX_IndexParamAudioTdma_EFR)                             return "OMX_IndexParamAudioTdma_EFR";
    if(index == OMX_IndexParamAudioQcelp8)                               return "OMX_IndexParamAudioQcelp8";
    if(index == OMX_IndexParamAudioQcelp13)                              return "OMX_IndexParamAudioQcelp13";
    if(index == OMX_IndexParamAudioEvrc)                                 return "OMX_IndexParamAudioEvrc";
    if(index == OMX_IndexParamAudioSmv)                                  return "OMX_IndexParamAudioSmv";
    if(index == OMX_IndexParamAudioVorbis)                               return "OMX_IndexParamAudioVorbis";
    if(index == OMX_IndexConfigAudioMidiImmediateEvent)                  return "OMX_IndexConfigAudioMidiImmediateEvent";
    if(index == OMX_IndexConfigAudioMidiControl)                         return "OMX_IndexConfigAudioMidiControl";
    if(index == OMX_IndexConfigAudioMidiSoundBankProgram)                return "OMX_IndexConfigAudioMidiSoundBankProgram";
    if(index == OMX_IndexConfigAudioMidiStatus)                          return "OMX_IndexConfigAudioMidiStatus";
    if(index == OMX_IndexConfigAudioMidiMetaEvent)                       return "OMX_IndexConfigAudioMidiMetaEvent";
    if(index == OMX_IndexConfigAudioMidiMetaEventData)                   return "OMX_IndexConfigAudioMidiMetaEventData";
    if(index == OMX_IndexConfigAudioVolume)                              return "OMX_IndexConfigAudioVolume";
    if(index == OMX_IndexConfigAudioBalance)                             return "OMX_IndexConfigAudioBalance";
    if(index == OMX_IndexConfigAudioChannelMute)                         return "OMX_IndexConfigAudioChannelMute";
    if(index == OMX_IndexConfigAudioMute)                                return "OMX_IndexConfigAudioMute";
    if(index == OMX_IndexConfigAudioLoudness)                            return "OMX_IndexConfigAudioLoudness";
    if(index == OMX_IndexConfigAudioEchoCancelation)                     return "OMX_IndexConfigAudioEchoCancelation";
    if(index == OMX_IndexConfigAudioNoiseReduction)                      return "OMX_IndexConfigAudioNoiseReduction";
    if(index == OMX_IndexConfigAudioBass)                                return "OMX_IndexConfigAudioBass";
    if(index == OMX_IndexConfigAudioTreble)                              return "OMX_IndexConfigAudioTreble";
    if(index == OMX_IndexConfigAudioStereoWidening)                      return "OMX_IndexConfigAudioStereoWidening";
    if(index == OMX_IndexConfigAudioChorus)                              return "OMX_IndexConfigAudioChorus";
    if(index == OMX_IndexConfigAudioEqualizer)                           return "OMX_IndexConfigAudioEqualizer";
    if(index == OMX_IndexConfigAudioReverberation)                       return "OMX_IndexConfigAudioReverberation";
    if(index == OMX_IndexConfigAudioChannelVolume)                       return "OMX_IndexConfigAudioChannelVolume";
    if(index == OMX_IndexImageStartUnused)                               return "OMX_IndexImageStartUnused = 0x05000000";
    if(index == OMX_IndexParamImagePortFormat)                           return "OMX_IndexParamImagePortFormat";
    if(index == OMX_IndexParamFlashControl)                              return "OMX_IndexParamFlashControl";
    if(index == OMX_IndexConfigFocusControl)                             return "OMX_IndexConfigFocusControl";
    if(index == OMX_IndexParamQFactor)                                   return "OMX_IndexParamQFactor";
    if(index == OMX_IndexParamQuantizationTable)                         return "OMX_IndexParamQuantizationTable";
    if(index == OMX_IndexParamHuffmanTable)                              return "OMX_IndexParamHuffmanTable";
    if(index == OMX_IndexConfigFlashControl)                             return "OMX_IndexConfigFlashControl";
    if(index == OMX_IndexVideoStartUnused)                               return "OMX_IndexVideoStartUnused = 0x06000000";
    if(index == OMX_IndexParamVideoPortFormat)                           return "OMX_IndexParamVideoPortFormat";
    if(index == OMX_IndexParamVideoQuantization)                         return "OMX_IndexParamVideoQuantization";
    if(index == OMX_IndexParamVideoFastUpdate)                           return "OMX_IndexParamVideoFastUpdate";
    if(index == OMX_IndexParamVideoBitrate)                              return "OMX_IndexParamVideoBitrate";
    if(index == OMX_IndexParamVideoMotionVector)                         return "OMX_IndexParamVideoMotionVector";
    if(index == OMX_IndexParamVideoIntraRefresh)                         return "OMX_IndexParamVideoIntraRefresh";
    if(index == OMX_IndexParamVideoErrorCorrection)                      return "OMX_IndexParamVideoErrorCorrection";
    if(index == OMX_IndexParamVideoVBSMC)                                return "OMX_IndexParamVideoVBSMC";
    if(index == OMX_IndexParamVideoMpeg2)                                return "OMX_IndexParamVideoMpeg2";
    if(index == OMX_IndexParamVideoMpeg4)                                return "OMX_IndexParamVideoMpeg4";
    if(index == OMX_IndexParamVideoWmv)                                  return "OMX_IndexParamVideoWmv";
    if(index == OMX_IndexParamVideoRv)                                   return "OMX_IndexParamVideoRv";
    if(index == OMX_IndexParamVideoAvc)                                  return "OMX_IndexParamVideoAvc";
    if(index == OMX_IndexParamVideoH263)                                 return "OMX_IndexParamVideoH263";
    if(index == OMX_IndexParamVideoProfileLevelQuerySupported)           return "OMX_IndexParamVideoProfileLevelQuerySupported";
    if(index == OMX_IndexParamVideoProfileLevelCurrent)                  return "OMX_IndexParamVideoProfileLevelCurrent";
    if(index == OMX_IndexConfigVideoBitrate)                             return "OMX_IndexConfigVideoBitrate";
    if(index == OMX_IndexConfigVideoFramerate)                           return "OMX_IndexConfigVideoFramerate";
    if(index == OMX_IndexConfigVideoIntraVOPRefresh)                     return "OMX_IndexConfigVideoIntraVOPRefresh";
    if(index == OMX_IndexConfigVideoIntraMBRefresh)                      return "OMX_IndexConfigVideoIntraMBRefresh";
    if(index == OMX_IndexConfigVideoMBErrorReporting)                    return "OMX_IndexConfigVideoMBErrorReporting";
    if(index == OMX_IndexParamVideoMacroblocksPerFrame)                  return "OMX_IndexParamVideoMacroblocksPerFrame";
    if(index == OMX_IndexConfigVideoMacroBlockErrorMap)                  return "OMX_IndexConfigVideoMacroBlockErrorMap";
    if(index == OMX_IndexParamVideoSliceFMO)                             return "OMX_IndexParamVideoSliceFMO";
    if(index == OMX_IndexConfigVideoAVCIntraPeriod)                      return "OMX_IndexConfigVideoAVCIntraPeriod";
    if(index == OMX_IndexConfigVideoNalSize)                             return "OMX_IndexConfigVideoNalSize";
    if(index == OMX_IndexCommonStartUnused)                              return "OMX_IndexCommonStartUnused = 0x07000000";
    if(index == OMX_IndexParamCommonDeblocking)                          return "OMX_IndexParamCommonDeblocking";
    if(index == OMX_IndexParamCommonSensorMode)                          return "OMX_IndexParamCommonSensorMode";
    if(index == OMX_IndexParamCommonInterleave)                          return "OMX_IndexParamCommonInterleave";
    if(index == OMX_IndexConfigCommonColorFormatConversion)              return "OMX_IndexConfigCommonColorFormatConversion";
    if(index == OMX_IndexConfigCommonScale)                              return "OMX_IndexConfigCommonScale";
    if(index == OMX_IndexConfigCommonImageFilter)                        return "OMX_IndexConfigCommonImageFilter";
    if(index == OMX_IndexConfigCommonColorEnhancement)                   return "OMX_IndexConfigCommonColorEnhancement";
    if(index == OMX_IndexConfigCommonColorKey)                           return "OMX_IndexConfigCommonColorKey";
    if(index == OMX_IndexConfigCommonColorBlend)                         return "OMX_IndexConfigCommonColorBlend";
    if(index == OMX_IndexConfigCommonFrameStabilisation)                 return "OMX_IndexConfigCommonFrameStabilisation";
    if(index == OMX_IndexConfigCommonRotate)                             return "OMX_IndexConfigCommonRotate";
    if(index == OMX_IndexConfigCommonMirror)                             return "OMX_IndexConfigCommonMirror";
    if(index == OMX_IndexConfigCommonOutputPosition)                     return "OMX_IndexConfigCommonOutputPosition";
    if(index == OMX_IndexConfigCommonInputCrop)                          return "OMX_IndexConfigCommonInputCrop";
    if(index == OMX_IndexConfigCommonOutputCrop)                         return "OMX_IndexConfigCommonOutputCrop";
    if(index == OMX_IndexConfigCommonDigitalZoom)                        return "OMX_IndexConfigCommonDigitalZoom";
    if(index == OMX_IndexConfigCommonOpticalZoom)                        return "OMX_IndexConfigCommonOpticalZoom";
    if(index == OMX_IndexConfigCommonWhiteBalance)                       return "OMX_IndexConfigCommonWhiteBalance";
    if(index == OMX_IndexConfigCommonExposure)                           return "OMX_IndexConfigCommonExposure";
    if(index == OMX_IndexConfigCommonContrast)                           return "OMX_IndexConfigCommonContrast";
    if(index == OMX_IndexConfigCommonBrightness)                         return "OMX_IndexConfigCommonBrightness";
    if(index == OMX_IndexConfigCommonBacklight)                          return "OMX_IndexConfigCommonBacklight";
    if(index == OMX_IndexConfigCommonGamma)                              return "OMX_IndexConfigCommonGamma";
    if(index == OMX_IndexConfigCommonSaturation)                         return "OMX_IndexConfigCommonSaturation";
    if(index == OMX_IndexConfigCommonLightness)                          return "OMX_IndexConfigCommonLightness";
    if(index == OMX_IndexConfigCommonExclusionRect)                      return "OMX_IndexConfigCommonExclusionRect";
    if(index == OMX_IndexConfigCommonDithering)                          return "OMX_IndexConfigCommonDithering";
    if(index == OMX_IndexConfigCommonPlaneBlend)                         return "OMX_IndexConfigCommonPlaneBlend";
    if(index == OMX_IndexConfigCommonExposureValue)                      return "OMX_IndexConfigCommonExposureValue";
    if(index == OMX_IndexConfigCommonOutputSize)                         return "OMX_IndexConfigCommonOutputSize";
    if(index == OMX_IndexParamCommonExtraQuantData)                      return "OMX_IndexParamCommonExtraQuantData";
    if(index == OMX_IndexConfigCommonFocusRegion)                        return "OMX_IndexConfigCommonFocusRegion";
    if(index == OMX_IndexConfigCommonFocusStatus)                        return "OMX_IndexConfigCommonFocusStatus";
    if(index == OMX_IndexConfigCommonTransitionEffect)                   return "OMX_IndexConfigCommonTransitionEffect";
    if(index == OMX_IndexOtherStartUnused)                               return "OMX_IndexOtherStartUnused = 0x08000000";
    if(index == OMX_IndexParamOtherPortFormat)                           return "OMX_IndexParamOtherPortFormat";
    if(index == OMX_IndexConfigOtherPower)                               return "OMX_IndexConfigOtherPower";
    if(index == OMX_IndexConfigOtherStats)                               return "OMX_IndexConfigOtherStats";
    if(index == OMX_IndexTimeStartUnused)                                return "OMX_IndexTimeStartUnused = 0x09000000";
    if(index == OMX_IndexConfigTimeScale)                                return "OMX_IndexConfigTimeScale";
    if(index == OMX_IndexConfigTimeClockState)                           return "OMX_IndexConfigTimeClockState";
    if(index == OMX_IndexConfigTimeActiveRefClock)                       return "OMX_IndexConfigTimeActiveRefClock";
    if(index == OMX_IndexConfigTimeCurrentMediaTime)                     return "OMX_IndexConfigTimeCurrentMediaTime";
    if(index == OMX_IndexConfigTimeCurrentWallTime)                      return "OMX_IndexConfigTimeCurrentWallTime";
    if(index == OMX_IndexConfigTimeCurrentAudioReference)                return "OMX_IndexConfigTimeCurrentAudioReference";
    if(index == OMX_IndexConfigTimeCurrentVideoReference)                return "OMX_IndexConfigTimeCurrentVideoReference";
    if(index == OMX_IndexConfigTimeMediaTimeRequest)                     return "OMX_IndexConfigTimeMediaTimeRequest";
    if(index == OMX_IndexConfigTimeClientStartTime)                      return "OMX_IndexConfigTimeClientStartTime";
    if(index == OMX_IndexConfigTimePosition)                             return "OMX_IndexConfigTimePosition";
    if(index == OMX_IndexConfigTimeSeekMode)                             return "OMX_IndexConfigTimeSeekMode";
    if(index == OMX_IndexKhronosExtensions)                              return "OMX_IndexKhronosExtensions = 0x6F000000";
    if(index == OMX_IndexVendorStartUnused)                              return "OMX_IndexVendorStartUnused = 0x7F000000";
    if(index == OMX_IndexMax)                                            return "OMX_IndexMax = 0x7FFFFFFF";
    if(index == VideoDecodeCustomParambUseFlaggedEos)                    return "VideoDecodeCustomParambUseFlaggedEos";

    return "Not found";
}


