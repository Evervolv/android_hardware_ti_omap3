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
/*  File Name         : OMX_VideoDec_LoadDefaults.c                          */
/*                                                                           */
/*  Description       : This File load Default values to various             */
/*                      Structures.                                          */
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
#include "OMX_VideoDec_LoadDefaults.h"

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_VideoDec_LoadDefaults"
#include <utils/Log.h>

/*****************************************************************************/
/*  Function Name :  VIDDEC_Load_Defaults()                                  */
/*  Description   :  Called during init to load the default values to the    */
/*					 various structures                                      */
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
OMX_ERRORTYPE VIDDEC_Load_Defaults (VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									OMX_S32                  nPassing)
{
    OMX_U32 iCount = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    switch(nPassing)
    {
	case VIDDEC_INIT_ALL:
	case VIDDEC_INIT_STRUCTS:
		pComponentPrivate->nInBufIndex         = 0;
		pComponentPrivate->nOutBufIndex        = 0;
		pComponentPrivate->nInMarkBufIndex     = 0;
		pComponentPrivate->nOutMarkBufIndex    = 0;
		pComponentPrivate->nInCmdMarkBufIndex  = 0;
		pComponentPrivate->nOutCmdMarkBufIndex = 0;

		pComponentPrivate->nLevelIdc  = 0;
		pComponentPrivate->nNumRefFrames  = 0;

		pComponentPrivate->pCompPort[0]->hTunnelComponent = NULL;
		pComponentPrivate->pCompPort[1]->hTunnelComponent = NULL;

		/* Set component version */
		pComponentPrivate->pComponentVersion.s.nVersionMajor = VERSION_MAJOR;
		pComponentPrivate->pComponentVersion.s.nVersionMinor = VERSION_MINOR;
		pComponentPrivate->pComponentVersion.s.nRevision     = VERSION_REVISION;
		pComponentPrivate->pComponentVersion.s.nStep         = VERSION_STEP;

		/* Set spec version */
		pComponentPrivate->pSpecVersion.s.nVersionMajor = VERSION_MAJOR;
		pComponentPrivate->pSpecVersion.s.nVersionMinor = VERSION_MINOR;
		pComponentPrivate->pSpecVersion.s.nRevision     = VERSION_REVISION;
		pComponentPrivate->pSpecVersion.s.nStep         = VERSION_STEP;

		pComponentPrivate->iEndofInput = 0;

		pComponentPrivate->pHandle->pApplicationPrivate = NULL;
		/* Set pPortParamType defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pPortParamType,
							 OMX_PORT_PARAM_TYPE, pComponentPrivate->dbg);

		pComponentPrivate->pPortParamType->nPorts           = NUM_OF_PORTS;
		pComponentPrivate->pPortParamType->nStartPortNumber = VIDDEC_INPUT_PORT;

#ifdef __STD_COMPONENT__
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pPortParamTypeAudio,
							 OMX_PORT_PARAM_TYPE, pComponentPrivate->dbg);

		pComponentPrivate->pPortParamTypeAudio->nPorts           = VIDDEC_ZERO;
		pComponentPrivate->pPortParamTypeAudio->nStartPortNumber = VIDDEC_ZERO;
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pPortParamTypeImage,
							 OMX_PORT_PARAM_TYPE, pComponentPrivate->dbg);

		pComponentPrivate->pPortParamTypeImage->nPorts           = VIDDEC_ZERO;
		pComponentPrivate->pPortParamTypeImage->nStartPortNumber = VIDDEC_ZERO;
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pPortParamTypeOthers,
							 OMX_PORT_PARAM_TYPE, pComponentPrivate->dbg);

		pComponentPrivate->pPortParamTypeOthers->nPorts           = VIDDEC_ZERO;
		pComponentPrivate->pPortParamTypeOthers->nStartPortNumber = VIDDEC_ZERO;
#endif

		pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->nBufferCnt  = 0;
		pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->nBufferCnt = 0;

		/* Set pInPortDef defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pInPortDef,
						OMX_PARAM_PORTDEFINITIONTYPE, pComponentPrivate->dbg);
		pComponentPrivate->pInPortDef->nPortIndex         = VIDDEC_INPUT_PORT;
		pComponentPrivate->pInPortDef->eDir               = OMX_DirInput;
		pComponentPrivate->pInPortDef->nBufferCountActual =
														MAX_PRIVATE_IN_BUFFERS;
		pComponentPrivate->pInPortDef->nBufferCountMin    =
														VIDDEC_BUFFERMINCOUNT;
		pComponentPrivate->pInPortDef->nBufferSize        =
											VIDDEC_DEFAULT_INPUT_BUFFER_SIZE;
		pComponentPrivate->pInPortDef->bEnabled           = VIDDEC_PORT_ENABLED;
		pComponentPrivate->pInPortDef->bPopulated         =
											VIDDEC_PORT_POPULATED;
		pComponentPrivate->pInPortDef->eDomain            = VIDDEC_PORT_DOMAIN;
#ifdef KHRONOS_1_2
		pComponentPrivate->pInPortDef->bBuffersContiguous = OMX_FALSE;
		pComponentPrivate->pInPortDef->nBufferAlignment   = OMX_FALSE;
#endif
		pComponentPrivate->pInPortDef->format.video.pNativeRender 		  =
												VIDDEC_INPUT_PORT_NATIVERENDER;
		pComponentPrivate->pInPortDef->format.video.nFrameWidth  		  =
												VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pInPortDef->format.video.nFrameHeight 		  =
												VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pInPortDef->format.video.nStride       		  =
												VIDDEC_INPUT_PORT_STRIDE;
		pComponentPrivate->pInPortDef->format.video.nSliceHeight 		  =
												VIDDEC_INPUT_PORT_SLICEHEIGHT;
		pComponentPrivate->pInPortDef->format.video.nBitrate     		  =
												VIDDEC_INPUT_PORT_BITRATE;
		pComponentPrivate->pInPortDef->format.video.xFramerate    		  =
												VIDDEC_INPUT_PORT_FRAMERATE;
		pComponentPrivate->pInPortDef->format.video.cMIMEType     		  =
												VIDDEC_MIMETYPEMPEG4;
		pComponentPrivate->pInPortDef->format.video.bFlagErrorConcealment =
										VIDDEC_INPUT_PORT_FLAGERRORCONCEALMENT;
		pComponentPrivate->pInPortDef->format.video.eCompressionFormat    =
										VIDDEC_INPUT_PORT_COMPRESSIONFORMAT;
		pComponentPrivate->pInPortDef->format.video.eColorFormat          =
										VIDDEC_COLORFORMATUNUSED;
#ifdef KHRONOS_1_1
		pComponentPrivate->pInPortDef->format.video.pNativeWindow         = 0;
#endif

		/* Set pOutPortDef defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pOutPortDef,
						OMX_PARAM_PORTDEFINITIONTYPE, pComponentPrivate->dbg);
		pComponentPrivate->pOutPortDef->nPortIndex         = VIDDEC_OUTPUT_PORT;
		pComponentPrivate->pOutPortDef->eDir               = OMX_DirOutput;
		pComponentPrivate->pOutPortDef->nBufferCountActual =
													MAX_PRIVATE_OUT_BUFFERS;
		pComponentPrivate->pOutPortDef->nBufferCountMin = VIDDEC_BUFFERMINCOUNT;
		pComponentPrivate->pOutPortDef->nBufferSize     =
											VIDDEC_DEFAULT_OUTPUT_BUFFER_SIZE;
		pComponentPrivate->pOutPortDef->bEnabled        = VIDDEC_PORT_ENABLED;
		pComponentPrivate->pOutPortDef->bPopulated      = VIDDEC_PORT_POPULATED;
		pComponentPrivate->pOutPortDef->eDomain         = VIDDEC_PORT_DOMAIN;
#ifdef KHRONOS_1_2
		pComponentPrivate->pInPortDef->bBuffersContiguous = OMX_FALSE;
		pComponentPrivate->pInPortDef->nBufferAlignment   = OMX_FALSE;
#endif
		pComponentPrivate->pOutPortDef->format.video.cMIMEType              =
											VIDDEC_MIMETYPEYUV;
		pComponentPrivate->pOutPortDef->format.video.pNativeRender          =
											VIDDEC_OUTPUT_PORT_NATIVERENDER;
		pComponentPrivate->pOutPortDef->format.video.nFrameWidth            =
											VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pOutPortDef->format.video.nFrameHeight           =
											VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pOutPortDef->format.video.nStride                =
											VIDDEC_OUTPUT_PORT_STRIDE;
		pComponentPrivate->pOutPortDef->format.video.nSliceHeight           =
											VIDDEC_OUTPUT_PORT_SLICEHEIGHT;
		pComponentPrivate->pOutPortDef->format.video.nBitrate               =
											VIDDEC_OUTPUT_PORT_BITRATE;
		pComponentPrivate->pOutPortDef->format.video.xFramerate             =
											VIDDEC_OUTPUT_PORT_FRAMERATE;
		pComponentPrivate->pOutPortDef->format.video.bFlagErrorConcealment  =
									VIDDEC_OUTPUT_PORT_FLAGERRORCONCEALMENT;
		pComponentPrivate->pOutPortDef->format.video.eCompressionFormat     =
									VIDDEC_OUTPUT_PORT_COMPRESSIONFORMAT;
		pComponentPrivate->pOutPortDef->format.video.eColorFormat           =
									VIDDEC_COLORFORMAT420;
#ifdef KHRONOS_1_1
		pComponentPrivate->pOutPortDef->format.video.pNativeWindow = 0;
#endif
		for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
		{
			OMX_MALLOC_STRUCT(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
					pBufferPrivate[iCount], VIDDEC_BUFFER_PRIVATE,
					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

			pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
								pBufferPrivate[iCount]->pBufferHdr = NULL;
		}

		for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
		{
			OMX_MALLOC_STRUCT(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
				pBufferPrivate[iCount], VIDDEC_BUFFER_PRIVATE,
				pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

			pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
								pBufferPrivate[iCount]->pBufferHdr = NULL;
		}

		/* Set pInPortFormat defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pInPortFormat,
					OMX_VIDEO_PARAM_PORTFORMATTYPE, pComponentPrivate->dbg);
		pComponentPrivate->pInPortFormat->nPortIndex         =
												VIDDEC_INPUT_PORT;
		pComponentPrivate->pInPortFormat->nIndex             =
										VIDDEC_DEFAULT_INPUT_INDEX_MPEG4;
		pComponentPrivate->pInPortFormat->eCompressionFormat =
											VIDDEC_INPUT_PORT_COMPRESSIONFORMAT;
		pComponentPrivate->pInPortFormat->eColorFormat       =
											VIDDEC_COLORFORMATUNUSED;
#ifdef KHRONOS_1_1
		pComponentPrivate->pInPortFormat->xFramerate 	     =
													VIDDEC_INPUT_PORT_FRAMERATE;
#endif

		/* Set pOutPortFormat defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pOutPortFormat,
				OMX_VIDEO_PARAM_PORTFORMATTYPE, pComponentPrivate->dbg);

		pComponentPrivate->pOutPortFormat->nPortIndex         =
										VIDDEC_OUTPUT_PORT;
		pComponentPrivate->pOutPortFormat->nIndex             =
										VIDDEC_DEFAULT_OUTPUT_INDEX_PLANAR420;
		pComponentPrivate->pOutPortFormat->eCompressionFormat =
										VIDDEC_OUTPUT_PORT_COMPRESSIONFORMAT;
		pComponentPrivate->pOutPortFormat->eColorFormat       =
										VIDDEC_COLORFORMAT420;
#ifdef KHRONOS_1_1
		pComponentPrivate->pOutPortFormat->xFramerate         =
										VIDDEC_INPUT_PORT_FRAMERATE;
#endif
		/* Set pPriorityMgmt defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pPriorityMgmt,
					OMX_PRIORITYMGMTTYPE, pComponentPrivate->dbg);
		pComponentPrivate->pPriorityMgmt->nGroupPriority = -1;
		pComponentPrivate->pPriorityMgmt->nGroupID       = -1;

		/* Buffer supplier setting */
		pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->eSupplierSetting   =
														OMX_BufferSupplyOutput;

		/* Set pInBufSupplier defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pInBufSupplier,
					OMX_PARAM_BUFFERSUPPLIERTYPE , pComponentPrivate->dbg);
		pComponentPrivate->pInBufSupplier->nPortIndex      = VIDDEC_INPUT_PORT;
		pComponentPrivate->pInBufSupplier->eBufferSupplier =
											VIDDEC_INPUT_PORT_BUFFERSUPPLIER;

		/* Set pOutBufSupplier defaults */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pOutBufSupplier,
					OMX_PARAM_BUFFERSUPPLIERTYPE , pComponentPrivate->dbg);
		pComponentPrivate->pOutBufSupplier->nPortIndex     = VIDDEC_OUTPUT_PORT;
		pComponentPrivate->pOutBufSupplier->eBufferSupplier =
										VIDDEC_OUTPUT_PORT_BUFFERSUPPLIER;

#ifdef KHRONOS_1_1
		/* MBError Reporting code       */
		/* Set eMBErrorReport defaults  */
		OMX_CONF_INIT_STRUCT(&pComponentPrivate->eMBErrorReport,
					OMX_CONFIG_MBERRORREPORTINGTYPE , pComponentPrivate->dbg);
		pComponentPrivate->eMBErrorReport.nPortIndex  = VIDDEC_OUTPUT_PORT;
		pComponentPrivate->eMBErrorReport.bEnabled    = OMX_FALSE;
		/* MBError Reporting code       */
		/* Set eMBErrorMapType defaults */
		for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
		{
			OMX_CONF_INIT_STRUCT(&pComponentPrivate->eMBErrorMapType[iCount],
				OMX_CONFIG_MACROBLOCKERRORMAPTYPE_TI , pComponentPrivate->dbg);
			pComponentPrivate->eMBErrorMapType[iCount].nPortIndex  =
														VIDDEC_OUTPUT_PORT;
			pComponentPrivate->eMBErrorMapType[iCount].nErrMapSize =
						(VIDDEC_DEFAULT_WIDTH * VIDDEC_DEFAULT_HEIGHT) / 256;
		}
		pComponentPrivate->cMBErrorIndexIn = 0;
		pComponentPrivate->cMBErrorIndexOut = 0;

#endif

		pComponentPrivate->nPendingStateChangeRequests = 0;
		if(pthread_mutex_init(&pComponentPrivate->mutexStateChangeRequest, NULL))
		{
			return OMX_ErrorUndefined;
		}
		if(pthread_cond_init (&pComponentPrivate->StateChangeCondition, NULL))
		{
			return OMX_ErrorUndefined;
		}

		/* Set pMpeg4 defaults */
		OMX_CONF_INIT_STRUCT (pComponentPrivate->pMpeg4,
			OMX_VIDEO_PARAM_MPEG4TYPE, pComponentPrivate->dbg);
		pComponentPrivate->pMpeg4->nPortIndex               =
									VIDDEC_DEFAULT_MPEG4_PORTINDEX;
		pComponentPrivate->pMpeg4->nSliceHeaderSpacing      =
									VIDDEC_DEFAULT_MPEG4_SLICEHEADERSPACING;
		pComponentPrivate->pMpeg4->bSVH                     =
									VIDDEC_DEFAULT_MPEG4_SVH;
		pComponentPrivate->pMpeg4->bGov                     =
									VIDDEC_DEFAULT_MPEG4_GOV;
		pComponentPrivate->pMpeg4->nPFrames                 =
									VIDDEC_DEFAULT_MPEG4_PFRAMES;
		pComponentPrivate->pMpeg4->nBFrames                 =
									VIDDEC_DEFAULT_MPEG4_BFRAMES;
		pComponentPrivate->pMpeg4->nIDCVLCThreshold         =
									VIDDEC_DEFAULT_MPEG4_IDCVLCTHRESHOLD;
		pComponentPrivate->pMpeg4->bACPred                  =
									VIDDEC_DEFAULT_MPEG4_ACPRED;
		pComponentPrivate->pMpeg4->nMaxPacketSize           =
									VIDDEC_DEFAULT_MPEG4_MAXPACKETSIZE;
		pComponentPrivate->pMpeg4->nTimeIncRes              =
									VIDDEC_DEFAULT_MPEG4_TIMEINCRES;
		pComponentPrivate->pMpeg4->eProfile                 =
									VIDDEC_DEFAULT_MPEG4_PROFILE;
		pComponentPrivate->pMpeg4->eLevel                   =
									VIDDEC_DEFAULT_MPEG4_LEVEL;
		pComponentPrivate->pMpeg4->nAllowedPictureTypes     =
									VIDDEC_DEFAULT_MPEG4_ALLOWEDPICTURETYPES;
		pComponentPrivate->pMpeg4->nHeaderExtension         =
									VIDDEC_DEFAULT_MPEG4_HEADEREXTENSION;
		pComponentPrivate->pMpeg4->bReversibleVLC           =
									VIDDEC_DEFAULT_MPEG4_REVERSIBLEVLC;

		pComponentPrivate->ProcessMode         = 0;
#ifdef VIDDEC_FLAGGED_EOS
		pComponentPrivate->bUseFlaggedEos      = OMX_TRUE;
#endif
		pComponentPrivate->wmvProfile          = VIDDEC_WMV_PROFILEMAX;

		pComponentPrivate->bIsNALBigEndian     = OMX_FALSE;
		pComponentPrivate->eLCMLState          = VidDec_LCML_State_Unload;
		pComponentPrivate->bLCMLHalted         = OMX_TRUE;
		pComponentPrivate->bLCMLOut            = OMX_FALSE;
		pComponentPrivate->eRMProxyState       = VidDec_RMPROXY_State_Unload;
		pComponentPrivate->nWMVFileType        = VIDDEC_WMV_RCVSTREAM;
		pComponentPrivate->bParserEnabled      = OMX_TRUE;
		pComponentPrivate->H264BitStreamFormat = 0;


                pComponentPrivate->isLevelCheckDone = 0;
                pComponentPrivate->isRefFrameCheckDone = 0;
		pComponentPrivate->AVCProfileType = 0;

		VIDDEC_CircBuf_Init(pComponentPrivate, VIDDEC_CBUFFER_TIMESTAMP,
							VIDDEC_INPUT_PORT);

		VIDDEC_PTHREAD_MUTEX_INIT(pComponentPrivate->sMutex);
		VIDDEC_PTHREAD_SEMAPHORE_INIT(pComponentPrivate->sInSemaphore);
		VIDDEC_PTHREAD_SEMAPHORE_INIT(pComponentPrivate->sOutSemaphore);

		for (iCount = 0; iCount < CBUFFER_SIZE; iCount++)
		{
			pComponentPrivate->aBufferFlags[iCount].nTimeStamp = 0;
			pComponentPrivate->aBufferFlags[iCount].nFlags = 0;
			pComponentPrivate->aBufferFlags[iCount].pMarkData = NULL;
			pComponentPrivate->aBufferFlags[iCount].hMarkTargetComponent = NULL;
		}

#ifdef ANDROID
		/* Set PV (opencore) capability flags */
		pComponentPrivate->pPVCapabilityFlags->iIsOMXComponentMultiThreaded =
																	OMX_TRUE;

		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentSupportsExternalOutputBufferAlloc = OMX_TRUE;
		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentSupportsExternalInputBufferAlloc = OMX_FALSE;
		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentSupportsMovableInputBuffers = OMX_FALSE;
		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentSupportsPartialFrames = OMX_FALSE;
		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;

		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentUsesNALStartCodes = OMX_TRUE;
		pComponentPrivate->pPVCapabilityFlags->
					iOMXComponentUsesFullAVCFrames = OMX_TRUE;
#endif
		/* Set default deblocking value for default format MPEG4 */
		OMX_CONF_INIT_STRUCT(pComponentPrivate->pDeblockingParamType,
						OMX_PARAM_DEBLOCKINGTYPE, pComponentPrivate->dbg);
		pComponentPrivate->pDeblockingParamType->nPortIndex = VIDDEC_OUTPUT_PORT;
		pComponentPrivate->pDeblockingParamType->bDeblocking = OMX_FALSE;


	case VIDDEC_INIT_VARS:
	{
			OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
			memset(pSplit->dsp_inp_buf_status, 0,
							sizeof(itt_buf_status_t) * MAX_DSP_INP_BUFS);
			memset(pSplit->dsp_out_buf_status, 0,
							sizeof(itt_buf_status_t) * MAX_DSP_OUT_BUFS);
			pSplit->arm_process_running = 0;
			pSplit->timeout_cnt         = 0;
			pSplit->timeout_reported    = 0;
	}
		/* Set the process mode to zero, frame = 0, stream = 1 */
		VIDDEC_CircBuf_Flush(pComponentPrivate,
						VIDDEC_CBUFFER_TIMESTAMP, VIDDEC_INPUT_PORT);
		pComponentPrivate->eIdleToLoad                        = OMX_StateInvalid;
		pComponentPrivate->iEndofInputSent                    = 0;
		pComponentPrivate->bInputBCountDsp                    = 0;
		pComponentPrivate->bOutputBCountDsp                   = 0;
		pComponentPrivate->bInputBCountApp                    = 0;
		pComponentPrivate->bOutputBCountApp                   = 0;
		pComponentPrivate->bPlayCompleted                     = OMX_FALSE;
		pComponentPrivate->bBuffFound                         = OMX_FALSE;
		pComponentPrivate->bFlushOut                          = OMX_FALSE;
		pComponentPrivate->bFirstHeader                       = OMX_FALSE;
		pComponentPrivate->nCurrentMPEG4ProfileIndex          = 0;
		pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel1]  = 0;
		pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel2]  = 0;
		pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel3]  = 0;
		pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel4]  = 0;
		pComponentPrivate->EOSHasBeenSentToAPP                = OMX_FALSE;
		pComponentPrivate->isOpBufFlushComplete               = OMX_TRUE;
		pComponentPrivate->frameCount                         = 0;
		pComponentPrivate->bDynamicConfigurationInProgress    = OMX_FALSE;
		pComponentPrivate->nInternalConfigBufferFilledAVC     = 0;
		pComponentPrivate->eMBErrorReport.bEnabled            = OMX_FALSE;

		pthread_mutex_init(&pComponentPrivate->InputBCountDspMutex, NULL);
		pthread_mutex_init(&pComponentPrivate->OutputBCountDspMutex, NULL);
		pthread_mutex_init(&pComponentPrivate->InputBCountAppMutex, NULL);
		pthread_mutex_init(&pComponentPrivate->OutputBCountAppMutex, NULL);
	break;

	case VIDDEC_INIT_IDLEEXECUTING:
		/* Set the process mode to zero, frame = 0, stream = 1 */
		pComponentPrivate->iEndofInputSent                  = 0;
		pComponentPrivate->bInputBCountDsp                  = 0;
		pComponentPrivate->bOutputBCountDsp                 = 0;
		pComponentPrivate->bInputBCountApp                  = 0;
		pComponentPrivate->bOutputBCountApp                 = 0;
		pComponentPrivate->bPlayCompleted                   = OMX_FALSE;
		pComponentPrivate->bBuffFound                       = OMX_FALSE;
		pComponentPrivate->bFlushOut                        = OMX_FALSE;
		pComponentPrivate->bFirstHeader                     = OMX_FALSE;
		pComponentPrivate->nInBufIndex  					= 0;
		pComponentPrivate->nOutBufIndex 					= 0;
		pComponentPrivate->nInMarkBufIndex  				= 0;
		pComponentPrivate->nOutMarkBufIndex 				= 0;
		pComponentPrivate->nInCmdMarkBufIndex  				= 0;
		pComponentPrivate->nOutCmdMarkBufIndex 				= 0;

		pthread_mutex_init(&pComponentPrivate->InputBCountDspMutex, NULL);
		pthread_mutex_init(&pComponentPrivate->OutputBCountDspMutex, NULL);
		pthread_mutex_init(&pComponentPrivate->InputBCountAppMutex, NULL);
		pthread_mutex_init(&pComponentPrivate->OutputBCountAppMutex, NULL);
	break;

	case VIDDEC_INIT_WMV9:
		pComponentPrivate->pWMV->nPortIndex                            =
												VIDDEC_DEFAULT_WMV_PORTINDEX;
		pComponentPrivate->pWMV->eFormat                               =
												VIDDEC_DEFAULT_WMV_FORMAT;
		pComponentPrivate->pInPortFormat->nPortIndex                   =
												VIDDEC_INPUT_PORT;
		pComponentPrivate->pInPortFormat->nIndex                       =
												VIDDEC_DEFAULT_INPUT_INDEX_WMV9;
		pComponentPrivate->pInPortFormat->eCompressionFormat           =
												OMX_VIDEO_CodingWMV;
		pComponentPrivate->pInPortFormat->eColorFormat                 =
												OMX_COLOR_FormatUnused;
#ifdef KHRONOS_1_1
		pComponentPrivate->pInPortFormat->xFramerate                   =
												VIDDEC_INPUT_PORT_FRAMERATE;
#endif
		pComponentPrivate->pInPortDef->format.video.nFrameWidth        =
												VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pInPortDef->format.video.nFrameHeight       =
												VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pInPortDef->format.video.nBitrate           =
												VIDDEC_INPUT_PORT_BITRATE;
		pComponentPrivate->pInPortDef->format.video.xFramerate         =
												VIDDEC_INPUT_PORT_FRAMERATE;
		pComponentPrivate->pInPortDef->format.video.eCompressionFormat =
												OMX_VIDEO_CodingWMV;
		pComponentPrivate->pInPortDef->format.video.eColorFormat       =
												OMX_COLOR_FormatUnused;
		pComponentPrivate->pInPortDef->nBufferSize                     =
					pComponentPrivate->pInPortDef->format.video.nFrameWidth  *
					pComponentPrivate->pInPortDef->format.video.nFrameHeight *
					VIDDEC_FACTORFORMAT420;
		pComponentPrivate->pDeblockingParamType->bDeblocking = OMX_TRUE;
	break;

	case VIDDEC_INIT_H264:
		pComponentPrivate->pH264->nPortIndex                            =
												VIDDEC_DEFAULT_H264_PORTINDEX;
		pComponentPrivate->pH264->nPFrames                              =
												VIDDEC_DEFAULT_H264_PFRAMES;
		pComponentPrivate->pH264->nBFrames                              =
												VIDDEC_DEFAULT_H264_BFRAMES;
		pComponentPrivate->pH264->eProfile                              =
												OMX_VIDEO_AVCProfileBaseline;
		pComponentPrivate->pH264->eLevel                                =
												OMX_VIDEO_AVCLevelMax;

		pComponentPrivate->pInPortFormat->nPortIndex                    =
												VIDDEC_INPUT_PORT;
		pComponentPrivate->pInPortFormat->nIndex                        =
												VIDDEC_DEFAULT_INPUT_INDEX_H264;
		pComponentPrivate->pInPortFormat->eCompressionFormat            =
												OMX_VIDEO_CodingAVC;
		pComponentPrivate->pInPortFormat->eColorFormat                  =
												OMX_COLOR_FormatUnused;
#ifdef KHRONOS_1_1
		pComponentPrivate->pInPortFormat->xFramerate                    =
												VIDDEC_INPUT_PORT_FRAMERATE;
#endif
		pComponentPrivate->pInPortDef->format.video.nFrameWidth         =
												VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pInPortDef->format.video.nFrameHeight        =
												VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pInPortDef->format.video.nBitrate            =
												VIDDEC_INPUT_PORT_BITRATE;
		pComponentPrivate->pInPortDef->format.video.xFramerate          =
												VIDDEC_INPUT_PORT_FRAMERATE;
		pComponentPrivate->pInPortDef->format.video.eCompressionFormat  =
												OMX_VIDEO_CodingAVC;
		pComponentPrivate->pInPortDef->format.video.eColorFormat        =
												OMX_COLOR_FormatUnused;
		pComponentPrivate->pInPortDef->nBufferSize                      =
					pComponentPrivate->pInPortDef->format.video.nFrameWidth  *
				    pComponentPrivate->pInPortDef->format.video.nFrameHeight *
					VIDDEC_FACTORFORMAT422;
		pComponentPrivate->pDeblockingParamType->bDeblocking = OMX_TRUE;
		break;

	case VIDDEC_INIT_MPEG4:
		pComponentPrivate->pMpeg4->nPortIndex                           =
												VIDDEC_DEFAULT_MPEG4_PORTINDEX;
		pComponentPrivate->pMpeg4->nPFrames                             =
												VIDDEC_DEFAULT_MPEG4_PFRAMES;
		pComponentPrivate->pMpeg4->nBFrames                             =
												VIDDEC_DEFAULT_MPEG4_BFRAMES;
		pComponentPrivate->pMpeg4->eProfile                             =
												OMX_VIDEO_MPEG4ProfileSimple;
#ifdef KHRONOS_1_1
		pComponentPrivate->pMpeg4->eLevel                               =
												OMX_VIDEO_MPEG4Level1;
#else
		pComponentPrivate->pMpeg4->eLevel                               =
												OMX_VIDEO_MPEG4Levell;
#endif
		pComponentPrivate->pInPortFormat->nPortIndex                    =
												VIDDEC_INPUT_PORT;
		pComponentPrivate->pInPortFormat->nIndex                        =
												VIDDEC_DEFAULT_INPUT_INDEX_MPEG4;
		pComponentPrivate->pInPortFormat->eCompressionFormat            =
												OMX_VIDEO_CodingMPEG4;
		pComponentPrivate->pInPortFormat->eColorFormat                  =
												OMX_COLOR_FormatUnused;
#ifdef KHRONOS_1_1
		pComponentPrivate->pInPortFormat->xFramerate                    =
												VIDDEC_INPUT_PORT_FRAMERATE;
#endif
		pComponentPrivate->pInPortDef->format.video.nFrameWidth         =
												VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pInPortDef->format.video.nFrameHeight        =
												VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pInPortDef->format.video.nBitrate            =
												VIDDEC_INPUT_PORT_BITRATE;
		pComponentPrivate->pInPortDef->format.video.xFramerate          =
												VIDDEC_INPUT_PORT_FRAMERATE;
		pComponentPrivate->pInPortDef->format.video.eCompressionFormat  =
												OMX_VIDEO_CodingMPEG4;
		pComponentPrivate->pInPortDef->format.video.eColorFormat        =
												OMX_COLOR_FormatUnused;
		pComponentPrivate->pInPortDef->nBufferSize                      =
					pComponentPrivate->pInPortDef->format.video.nFrameWidth *
					pComponentPrivate->pInPortDef->format.video.nFrameHeight *
					VIDDEC_FACTORFORMAT420;
		pComponentPrivate->pDeblockingParamType->bDeblocking = OMX_FALSE;
		break;

	case VIDDEC_INIT_PLANAR420:
		pComponentPrivate->pOutPortFormat->nPortIndex                   =
										VIDDEC_OUTPUT_PORT;
		pComponentPrivate->pOutPortFormat->nIndex                       =
										VIDDEC_DEFAULT_OUTPUT_INDEX_PLANAR420;
		pComponentPrivate->pOutPortFormat->eCompressionFormat           =
										OMX_VIDEO_CodingUnused;
		pComponentPrivate->pOutPortFormat->eColorFormat                 =
										VIDDEC_COLORFORMAT420;
#ifdef KHRONOS_1_1
		pComponentPrivate->pOutPortFormat->xFramerate                   =
										VIDDEC_INPUT_PORT_FRAMERATE;
#endif
		pComponentPrivate->pOutPortDef->format.video.nFrameWidth        =
										VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pOutPortDef->format.video.nFrameHeight       =
										VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pOutPortDef->format.video.nBitrate           =
										VIDDEC_OUTPUT_PORT_BITRATE;
		pComponentPrivate->pOutPortDef->format.video.xFramerate         =
										VIDDEC_OUTPUT_PORT_FRAMERATE;
		pComponentPrivate->pOutPortDef->format.video.eCompressionFormat =
										VIDDEC_OUTPUT_PORT_COMPRESSIONFORMAT;
		pComponentPrivate->pOutPortDef->format.video.eColorFormat       =
										VIDDEC_COLORFORMAT420;
		pComponentPrivate->pOutPortDef->nBufferSize                     =
				pComponentPrivate->pOutPortDef->format.video.nFrameWidth  *
				pComponentPrivate->pOutPortDef->format.video.nFrameHeight *
				VIDDEC_FACTORFORMAT420;
		break;

	case VIDDEC_INIT_INTERLEAVED422:
		pComponentPrivate->pOutPortFormat->nPortIndex                   =
									VIDDEC_OUTPUT_PORT;
		pComponentPrivate->pOutPortFormat->nIndex                       =
									VIDDEC_DEFAULT_OUTPUT_INDEX_INTERLEAVED422;
		pComponentPrivate->pOutPortFormat->eCompressionFormat           =
									OMX_VIDEO_CodingUnused;
		pComponentPrivate->pOutPortFormat->eColorFormat                 =
									VIDDEC_COLORFORMAT422;
#ifdef KHRONOS_1_1
		pComponentPrivate->pOutPortFormat->xFramerate                   =
									VIDDEC_INPUT_PORT_FRAMERATE;
#endif
		pComponentPrivate->pOutPortDef->format.video.nFrameWidth        =
									VIDDEC_DEFAULT_WIDTH;
		pComponentPrivate->pOutPortDef->format.video.nFrameHeight       =
									VIDDEC_DEFAULT_HEIGHT;
		pComponentPrivate->pOutPortDef->format.video.nBitrate           =
									VIDDEC_OUTPUT_PORT_BITRATE;
		pComponentPrivate->pOutPortDef->format.video.xFramerate         =
									VIDDEC_OUTPUT_PORT_FRAMERATE;
		pComponentPrivate->pOutPortDef->format.video.eCompressionFormat =
									VIDDEC_OUTPUT_PORT_COMPRESSIONFORMAT;
		pComponentPrivate->pOutPortDef->format.video.eColorFormat       =
									VIDDEC_COLORFORMAT422;
		pComponentPrivate->pOutPortDef->nBufferSize                     =
					pComponentPrivate->pOutPortDef->format.video.nFrameWidth *
					pComponentPrivate->pOutPortDef->format.video.nFrameHeight *
					VIDDEC_FACTORFORMAT422;
		break;
    }

EXIT:
    return(eError);
}
