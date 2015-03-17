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
/*  File Name         : OMX_VideoDecoder.c                                   */
/*                                                                           */
/*  Description       : This file contains OMX_VideoDecoder API              */
/*						Implementation										 */
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
/*                     File Includes                                         */
/*****************************************************************************/
/* User include files */
#include <OMX_Core.h>
#include <OMX_TI_Debug.h>
#include <OMX_Component.h>
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
#include "OMX_IttiamVidDec_CustomCmd.h"
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

//Sasken start
#define ENABLE_GRALLOC_BUFFERS
#define USE_ION
#ifdef USE_ION
#include <sys/mman.h>
#include <sys/eventfd.h>
#include <linux/ion.h>
#endif
#include "hal_public.h"
#include "OMX_IVCommon.h"
int map_fd;
// End of Sasken Changes

#define HAL_NV12_PADDED_PIXEL_FORMAT (OMX_TI_COLOR_FormatYUV420PackedSemiPlanar - OMX_COLOR_FormatVendorStartUnused)

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_OMX_VideoDecoder"
#include <utils/Log.h>

/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/
#define DUMP_BUFFER_ALLOC_DETAILS 0
#define OMX_COMPONENT_VERSION     "DEC_3630_06_11_2012"

OMX_STRING cVideoDecodeName = "OMX.TI.720P.Decoder";

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/

#ifdef VIDDEC_FLAGGED_EOS
VIDDEC_CUSTOM_PARAM sVideoDecCustomParams[] =
{
	{VIDDEC_CUSTOMPARAM_PROCESSMODE, VideoDecodeCustomParamProcessMode},
	{VIDDEC_CUSTOMPARAM_H264BITSTREAMFORMAT, VideoDecodeCustomParamH264BitStreamFormat},
	{VIDDEC_CUSTOMPARAM_WMVPROFILE, VideoDecodeCustomParamWMVProfile},
	{VIDDEC_CUSTOMPARAM_WMVFILETYPE, VideoDecodeCustomParamWMVFileType},
	{VIDDEC_CUSTOMPARAM_PARSERENABLED, VideoDecodeCustomParamParserEnabled},
	{VIDDEC_CUSTOMCONFIG_DEBUG, VideoDecodeCustomConfigDebug},
	{VIDDEC_CUSTOMPARAM_ISNALBIGENDIAN, VideoDecodeCustomParamIsNALBigEndian},
	{VIDDEC_CUSTOMPARAM_BUSEFLAGGEDEOS, VideoDecodeCustomParambUseFlaggedEos},
        {VIDDEC_CUSTOMCONFIG_CACHEABLEBUFFERS, VideoDecodeCustomConfigCacheableBuffers},
	{VIDDEC_ENABLE_ANDROID_NATIVE_BUFFERS, VideoDecodeEnableAndroidNativeBuffers},
        {VIDDEC_GET_ANDROID_NATIVE_BUFFER_USAGE, VideoDecodeGetAndroidNativeBufferUsage},
        {VIDDEC_ANDROID_USE_ANDROID_NATIVE_BUFFER2, VideoDecodeGetAndroiduseAndroidNativeBuffer2}
};
#else
VIDDEC_CUSTOM_PARAM sVideoDecCustomParams[] =
{
	{VIDDEC_CUSTOMPARAM_PROCESSMODE, VideoDecodeCustomParamProcessMode},
	{VIDDEC_CUSTOMPARAM_H264BITSTREAMFORMAT, VideoDecodeCustomParamH264BitStreamFormat},
	{VIDDEC_CUSTOMPARAM_WMVPROFILE, VideoDecodeCustomParamWMVProfile},
	{VIDDEC_CUSTOMPARAM_WMVFILETYPE, VideoDecodeCustomParamWMVFileType},
	{VIDDEC_CUSTOMPARAM_PARSERENABLED, VideoDecodeCustomParamParserEnabled},
	{VIDDEC_CUSTOMCONFIG_DEBUG, VideoDecodeCustomConfigDebug},
	{VIDDEC_CUSTOMPARAM_ISNALBIGENDIAN, VideoDecodeCustomParamIsNALBigEndian}
};
#endif

/* AVC Supported Levels & profiles */
VIDEO_PROFILE_LEVEL_TYPE SupportedAVCProfileLevels[] ={
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel1},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel1b},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel11},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel12},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel13},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel2},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel21},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel22},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel3},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel31},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel32},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel4},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel41},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel42},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel5},
  {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel51},
  //{OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel52},
  
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel1},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel1b},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel11},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel12},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel13},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel2},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel21},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel22},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel3},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel31},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel32},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel4},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel41},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel42},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel5},
  {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel51},
  //{OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel52},
  
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel1},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel1b},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel11},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel12},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel13},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel2},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel21},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel22},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel3},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel31},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel32},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel4},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel41},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel42},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel5},
  {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel51},
  //{OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel52},
  
  {-1,-1}
};

/******************************************************************************/
/*  Extern function declaraions                                               */
/******************************************************************************/
extern OMX_ERRORTYPE VIDDEC_Start_ComponentThread(OMX_HANDLETYPE pHandle);
extern OMX_ERRORTYPE VIDDEC_Stop_ComponentThread(OMX_HANDLETYPE pComponent);
extern OMX_ERRORTYPE VIDDEC_Load_Defaults (
									VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									OMX_S32                   nPassing);

/*****************************************************************************/
/* Static Function Declarations                                              */
/*****************************************************************************/

static OMX_ERRORTYPE VIDDEC_SetCallbacks (OMX_HANDLETYPE    hComp,
                                          OMX_CALLBACKTYPE* pCallBacks,
                                          OMX_PTR           pAppData);

static OMX_ERRORTYPE VIDDEC_GetComponentVersion (OMX_HANDLETYPE   hComp,
                                                 OMX_STRING pComponentName,
                                                 OMX_VERSIONTYPE* pComponent,
                                                 OMX_VERSIONTYPE* pSpecVersion,
                                                 OMX_UUIDTYPE* pComponentUUID);

static OMX_ERRORTYPE VIDDEC_SendCommand (OMX_HANDLETYPE  hComponent,
                                         OMX_COMMANDTYPE Cmd,
                                         OMX_U32         nParam1,
                                         OMX_PTR         pCmdData);

static OMX_ERRORTYPE VIDDEC_GetParameter (OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE  nParamIndex,
                                          OMX_PTR        ComponentParamStruct);

static OMX_ERRORTYPE VIDDEC_SetParameter (OMX_HANDLETYPE hComp,
                                          OMX_INDEXTYPE  nParamIndex,
                                          OMX_PTR        ComponentParamStruct);

static OMX_ERRORTYPE VIDDEC_GetConfig (OMX_HANDLETYPE hComp,
                                       OMX_INDEXTYPE  nConfigIndex,
                                       OMX_PTR        pComponentConfigStructure);

static OMX_ERRORTYPE VIDDEC_SetConfig (OMX_HANDLETYPE hComp,
                                       OMX_INDEXTYPE nConfigIndex,
                                       OMX_PTR       pComponentConfigStructure);

static OMX_ERRORTYPE VIDDEC_EmptyThisBuffer (OMX_HANDLETYPE        hComp,
                                             OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE VIDDEC_FillThisBuffer (OMX_HANDLETYPE        hComp,
                                            OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE VIDDEC_GetState (OMX_HANDLETYPE hComp, OMX_STATETYPE* pState);

static OMX_ERRORTYPE VIDDEC_ComponentTunnelRequest (
								OMX_IN OMX_HANDLETYPE          hComp,
								OMX_IN OMX_U32                 nPort,
								OMX_IN OMX_HANDLETYPE          hTunneledComp,
								OMX_IN OMX_U32                 nTunneledPort,
								OMX_INOUT OMX_TUNNELSETUPTYPE* pTunnelSetup);

static OMX_ERRORTYPE VIDDEC_UseBuffer (
								OMX_IN OMX_HANDLETYPE            hComponent,
                                OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                OMX_IN OMX_U32                   nPortIndex,
                                OMX_IN OMX_PTR                   pAppPrivate,
                                OMX_IN OMX_U32                   nSizeBytes,
                                OMX_IN OMX_U8*                   pBuffer);

static OMX_ERRORTYPE VIDDEC_AllocateBuffer (
									OMX_IN OMX_HANDLETYPE            hComponent,
									OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
									OMX_IN OMX_U32                   nPortIndex,
									OMX_IN OMX_PTR                   pAppPrivate,
									OMX_IN OMX_U32                   nSizeBytes);

static OMX_ERRORTYPE VIDDEC_FreeBuffer (OMX_IN OMX_HANDLETYPE        hComponent,
                                        OMX_IN OMX_U32               nPortIndex,
                                        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE VIDDEC_ComponentDeInit (OMX_HANDLETYPE hComponent);

static OMX_ERRORTYPE VIDDEC_VerifyTunnelConnection (
									VIDDEC_PORT_TYPE              *pPort,
									OMX_HANDLETYPE                hTunneledComp,
									OMX_PARAM_PORTDEFINITIONTYPE* pPortDef);

static OMX_ERRORTYPE VIDDEC_Allocate_DSPResources (
							OMX_IN VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
                            OMX_IN OMX_U32                   nPortIndex);

static OMX_ERRORTYPE VIDDEC_GetExtensionIndex(OMX_IN OMX_HANDLETYPE hComponent,
                                              OMX_IN OMX_STRING cParameterName,
                                              OMX_OUT OMX_INDEXTYPE* pIndexType);

#ifdef KHRONOS_1_1
static OMX_ERRORTYPE ComponentRoleEnum(
                						OMX_IN OMX_HANDLETYPE hComponent,
                						OMX_OUT OMX_U8        *cRole,
                						OMX_IN OMX_U32 		  nIndex);
#endif
/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ComponentInit									     */
/*                                                                           */
/*  Description   : Set all the function pointers of component				 */
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

OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_U32 				  nMemUsage 		= 0;

//sasken start
    OMX_U32 err;
    hw_module_t const* module;
//sasken stop

    LOG_API("+++Entering\n");
	ALOGD("%d ::: In %s ***", __LINE__, __FUNCTION__);
    ALOGD("720p VIDDEC OMX Component Version : %s", OMX_COMPONENT_VERSION);
    OMX_CONF_CHECK_CMD(hComponent, OMX_TRUE, OMX_TRUE);
    pHandle = (OMX_COMPONENTTYPE *)hComponent;

    OMX_MALLOC_STRUCT(pHandle->pComponentPrivate, VIDDEC_COMPONENT_PRIVATE,
    					nMemUsage);
    if (pHandle->pComponentPrivate == NULL)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
    pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0] += nMemUsage;

#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->pPERF = PERF_Create(PERF_FOURS("VD  "),
                                    PERF_ModuleLLMM | PERF_ModuleVideoDecode);
#endif
    OMX_DBG_INIT(pComponentPrivate->dbg, "OMX_DBG_VIDDEC");
    ((VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate)->pHandle = pHandle;

    pHandle->SetCallbacks           = VIDDEC_SetCallbacks;
    pHandle->GetComponentVersion    = VIDDEC_GetComponentVersion;
    pHandle->SendCommand            = VIDDEC_SendCommand;
    pHandle->GetParameter           = VIDDEC_GetParameter;
    pHandle->SetParameter           = VIDDEC_SetParameter;
    pHandle->GetConfig              = VIDDEC_GetConfig;
    pHandle->SetConfig              = VIDDEC_SetConfig;
    pHandle->GetState               = VIDDEC_GetState;
    pHandle->ComponentTunnelRequest = VIDDEC_ComponentTunnelRequest;
    pHandle->UseBuffer              = VIDDEC_UseBuffer;
    pHandle->AllocateBuffer         = VIDDEC_AllocateBuffer;
    pHandle->FreeBuffer             = VIDDEC_FreeBuffer;
    pHandle->EmptyThisBuffer        = VIDDEC_EmptyThisBuffer;
    pHandle->FillThisBuffer         = VIDDEC_FillThisBuffer;
    pHandle->ComponentDeInit        = VIDDEC_ComponentDeInit;
    pHandle->GetExtensionIndex      = VIDDEC_GetExtensionIndex;
#ifdef KHRONOS_1_1
    pHandle->ComponentRoleEnum      = ComponentRoleEnum;
#endif

    OMX_MALLOC_STRUCT(pComponentPrivate->pPortParamType, OMX_PORT_PARAM_TYPE,
    					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
#ifdef __STD_COMPONENT__
    OMX_MALLOC_STRUCT(pComponentPrivate->pPortParamTypeAudio,
    					OMX_PORT_PARAM_TYPE,
    					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pPortParamTypeImage,
    				  OMX_PORT_PARAM_TYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pPortParamTypeOthers,
    				 OMX_PORT_PARAM_TYPE,
    				 pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
#endif
    OMX_MALLOC_STRUCT(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT],
    				  VIDDEC_PORT_TYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT],
    				  VIDDEC_PORT_TYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pInPortDef,
    				  OMX_PARAM_PORTDEFINITIONTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pOutPortDef,
     				  OMX_PARAM_PORTDEFINITIONTYPE,
     				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pInPortFormat,
    				  OMX_VIDEO_PARAM_PORTFORMATTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pOutPortFormat,
    				  OMX_VIDEO_PARAM_PORTFORMATTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pPriorityMgmt,
    				  OMX_PRIORITYMGMTTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pInBufSupplier,
    			 	  OMX_PARAM_BUFFERSUPPLIERTYPE,
    			 	  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pOutBufSupplier,
    				  OMX_PARAM_BUFFERSUPPLIERTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pMpeg4,
    				  OMX_VIDEO_PARAM_MPEG4TYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pH264,
    				  OMX_VIDEO_PARAM_AVCTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pH263,
    				  OMX_VIDEO_PARAM_H263TYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pWMV,
    				  OMX_VIDEO_PARAM_WMVTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pDeblockingParamType,
    				  OMX_PARAM_DEBLOCKINGTYPE,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    OMX_MALLOC_STRUCT(pComponentPrivate->pPVCapabilityFlags,
    				  PV_OMXComponentCapabilityFlagsType,
    				  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

    OMX_MALLOC_STRUCT_SIZED(pComponentPrivate->cComponentName, char,
    						VIDDEC_MAX_NAMESIZE + 1,
    						pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
    if (pComponentPrivate->cComponentName == NULL)
    {
        LOG_API("Error: Malloc failed\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    strncpy(pComponentPrivate->cComponentName, cVideoDecodeName,
    						VIDDEC_MAX_NAMESIZE);
    OMX_CONF_INIT_STRUCT( &pComponentPrivate->componentRole,
    					OMX_PARAM_COMPONENTROLETYPE, pComponentPrivate->dbg);

	LOG_API("Alllocating Split component\n");
    eError = allocate_split_component(pComponentPrivate);

    /*  allocate all structs..private-buffers of all ports get allocated here*/
	LOG_API("+++Entering VIDDEC_Load_Defaults\n");
    VIDDEC_Load_Defaults( pComponentPrivate, VIDDEC_INIT_ALL);

#ifdef RESOURCE_MANAGER_ENABLED

	LOG_API("RMProxy_NewInitalizeEx\n")
    /* load the ResourceManagerProxy thread */
    eError = RMProxy_NewInitalizeEx(OMX_COMPONENTTYPE_VIDEO);
    if (eError != OMX_ErrorNone)
    {
        LOG_API("Error from RMProxy_NewInitalizeEx 0x%x\n", eError);
        pComponentPrivate->eRMProxyState = VidDec_RMPROXY_State_Unload;
        LOG_API("Error returned from loading ResourceManagerProxy thread\n");
        goto EXIT;
    }
    else
    {
        LOG_API("RMProxy_NewInitalizeEx successful\n");
        pComponentPrivate->eRMProxyState = VidDec_RMPROXY_State_Load;
    }

#endif

    /* This plug-in now supports only MPEG4 */
    if( pComponentPrivate->pOutPortFormat->eColorFormat != VIDDEC_COLORFORMAT422)
    {
		 LOG_API("VIDDEC_Load_Defaults with VIDDEC_INIT_INTERLEAVED422\n");
        eError = VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_INTERLEAVED422);
    }

//sasken start
#ifdef USE_ION
    pComponentPrivate->bUseIon = OMX_TRUE;
    pComponentPrivate->bMapIonBuffers = OMX_TRUE;

    pComponentPrivate->ion_fd = ion_open();
//	if(pComponentPrivate->ion_fd == 0)
	if(pComponentPrivate->ion_fd < 0)
	{
		OMX_ERROR4(pComponentPrivate->dbg, "ion_open failed!!!");
		return OMX_ErrorInsufficientResources;
	}

    pComponentPrivate->count_ion_buff = 0;
#endif

//#ifdef ANDROID_QUIRK_LOCK_BUFFER
	err = hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module);
    if (err == 0)
	{
    	pComponentPrivate->grallocModule = (gralloc_module_t const *)module;
    }
	else
	{
         //DOMX_ERROR("FATAL: gralloc api hw_get_module() returned error: Can't find \
		 %s module err = %x", GRALLOC_HARDWARE_MODULE_ID, err);
		eError = OMX_ErrorInsufficientResources;
		return eError;
	}
//#endif
pComponentPrivate->nCropWidth = 0;
pComponentPrivate->nCropHeight = 0;
//sasken stop

    /* Start the component thread */
    eError = VIDDEC_Start_ComponentThread(pHandle);
    if (eError != OMX_ErrorNone)
    {
        LOG_API("Error returned from the Component\n");
        goto EXIT;
    }

EXIT:
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_SetCallbacks									     */
/*                                                                           */
/*  Description   : Sets application callbacks to the component				 */
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

static OMX_ERRORTYPE VIDDEC_SetCallbacks (OMX_HANDLETYPE    pComponent,
                                          OMX_CALLBACKTYPE* pCallBacks,
                                          OMX_PTR           pAppData)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE         *pHandle          = NULL;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    OMX_CONF_CHECK_CMD(pComponent, pCallBacks, OMX_TRUE);

    pHandle = (OMX_COMPONENTTYPE*)pComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
	LOG_API("+++Entering\n");
    /* Copy the callbacks of the application to the component private  */
    memcpy (&(pComponentPrivate->cbInfo), pCallBacks, sizeof(OMX_CALLBACKTYPE));

    /* copy the application private data to component memory */
    pHandle->pApplicationPrivate = pAppData;

    pComponentPrivate->eState = OMX_StateLoaded;

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_GetComponentVersion  						     */
/*                                                                           */
/*  Description   : Collects the component Version							 */
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


static OMX_ERRORTYPE VIDDEC_GetComponentVersion (OMX_HANDLETYPE   hComp,
                                                 OMX_STRING       pComponentName,
                                                 OMX_VERSIONTYPE* pComponentVersion,
                                                 OMX_VERSIONTYPE* pSpecVersion,
                                                 OMX_UUIDTYPE*    pComponentUUID)
{
    OMX_ERRORTYPE             eError            	= OMX_ErrorNone;
    OMX_COMPONENTTYPE*        pHandle           	= NULL;
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate 	= NULL;
	OMX_U32 				  tempCompressionFormat	= 0;
	LOG_API("+++Entering\n");
    if (!hComp || !pComponentName || !pComponentVersion || !pSpecVersion)
    {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pHandle = (OMX_COMPONENTTYPE*)hComp;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
    tempCompressionFormat = pComponentPrivate->pInPortDef->
   							       format.video.eCompressionFormat;

    strcpy(pComponentName, pComponentPrivate->cComponentName);

    memcpy(pComponentVersion, &(pComponentPrivate->pComponentVersion.s),
    		sizeof(pComponentPrivate->pComponentVersion.s));

    memcpy(pSpecVersion, &(pComponentPrivate->pSpecVersion.s),
    		sizeof(pComponentPrivate->pSpecVersion.s));

    if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        memcpy(pComponentUUID, (OMX_UUIDTYPE *)&STRING_H264VDSOCKET_TI_UUID,
        		STRING_UUID_LENGHT);
    }
    else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			memcpy(pComponentUUID, (OMX_UUIDTYPE *)&STRING_WMVDSOCKET_TI_UUID,
					STRING_UUID_LENGHT);
		}
		else if ((tempCompressionFormat == OMX_VIDEO_CodingMPEG4) ||
				 (tempCompressionFormat == OMX_VIDEO_CodingH263))
		{
			memcpy(pComponentUUID, (OMX_UUIDTYPE *)&STRING_MP4DSOCKET_TI_UUID,
					STRING_UUID_LENGHT);
		}
		else
		{
			memcpy(pComponentUUID, (OMX_UUIDTYPE *)&STRING_MP4DSOCKET_TI_UUID,
					STRING_UUID_LENGHT);
		}
EXIT:
	LOG_API("---Exiting\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_SendCommand									     */
/*                                                                           */
/*  Description   : Writes Commands to command pipe							 */
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

static OMX_ERRORTYPE VIDDEC_SendCommand (OMX_HANDLETYPE  hComponent,
                                         OMX_COMMANDTYPE Cmd,
                                         OMX_U32         nParam1,
                                         OMX_PTR         pCmdData)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_S32                   nRet;
    OMX_COMPONENTTYPE*        pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_CONF_CHECK_CMD(hComponent, OMX_TRUE, OMX_TRUE);

    pHandle 		  = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

	LOG_API("+++Entering\n");
    if (pComponentPrivate->eState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }

#ifdef __PERF_INSTRUMENTATION__
    PERF_SendingCommand(pComponentPrivate->pPERF,Cmd,
                        (Cmd == OMX_CommandMarkBuffer) ?
                        (OMX_U32) pCmdData : nParam1,
                        PERF_ModuleComponent);
#endif

    switch (Cmd)
    {
	case OMX_CommandStateSet:
	{
		pComponentPrivate->eIdleToLoad = nParam1;
		pComponentPrivate->eExecuteToIdle = nParam1;
		LOG_API("StateSet command to %lu received", nParam1);

		if(pthread_mutex_lock(&pComponentPrivate->mutexStateChangeRequest))
		{
		   return OMX_ErrorUndefined;
		}

	  /*                          ASSUMPTION							 */
	  /*  The state transition command issued from the IL client will not*/
	  /*  be multiple commands at a time.i.e., after a state transition  */
	  /*  command is issued to the OMX IL component, the IL client willdo*/
	  /*  GetState toverify the completion of the state transition,      */
	  /*  althoughit may not wait until EventHandler callback from       */
	  /*  the OMX IL component.											 */

		pComponentPrivate->nPendingStateChangeRequests = 1;

		if(pthread_mutex_unlock(&pComponentPrivate->mutexStateChangeRequest))
		{
		   return OMX_ErrorUndefined;
		}

		nRet = write(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE],
						&Cmd, sizeof(Cmd));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
						&nParam1, sizeof(nParam1));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
	}
	break;

	case OMX_CommandPortDisable:
	{
		LOG_API("OMX_CommandPortDisable to %lu", nParam1);

		if (nParam1 == VIDDEC_INPUT_PORT)
		{
			if (pComponentPrivate->pInPortDef->bEnabled == OMX_TRUE)
			{
				pComponentPrivate->pInPortDef->bEnabled = OMX_FALSE;
				LOG_API("Disabling VIDDEC_INPUT_PORT 0x%x\n",
							pComponentPrivate->pInPortDef->bEnabled);
			}
			else
			{
				eError = OMX_ErrorIncorrectStateOperation;
				goto EXIT;
			}
		}
		else if (nParam1 == VIDDEC_OUTPUT_PORT)
		{
			if (pComponentPrivate->pOutPortDef->bEnabled == OMX_TRUE)
			{
			pComponentPrivate->pOutPortDef->bEnabled = OMX_FALSE;
				LOG_API("Disabling VIDDEC_OUTPUT_PORT 0x%x\n",
							pComponentPrivate->pOutPortDef->bEnabled);
			}
			else
			{
				eError = OMX_ErrorIncorrectStateOperation;
				goto EXIT;
			}
		}
		else if (nParam1 == OMX_ALL)
		{
			if (pComponentPrivate->pInPortDef->bEnabled == OMX_TRUE &&
				pComponentPrivate->pOutPortDef->bEnabled == OMX_TRUE)
			{
				pComponentPrivate->pInPortDef->bEnabled = OMX_FALSE;
				pComponentPrivate->pOutPortDef->bEnabled = OMX_FALSE;
				LOG_API("Disabling OMX_ALL IN 0x%x OUT 0x%x\n",
							  pComponentPrivate->pInPortDef->bEnabled,
							  pComponentPrivate->pOutPortDef->bEnabled);
			}
			else
			{
				eError = OMX_ErrorIncorrectStateOperation;
				goto EXIT;
			}
		}
		else
		{
			eError = OMX_ErrorBadParameter;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE],
					 &Cmd, sizeof(Cmd));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
						&nParam1, sizeof(nParam1));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
#ifdef ANDROID
		/*Workaround version to handle pv app */
		/*After ports is been flush           */

		if (nParam1 == VIDDEC_INPUT_PORT &&
				pComponentPrivate->bDynamicConfigurationInProgress == OMX_TRUE &&
				pComponentPrivate->bInPortSettingsChanged == OMX_TRUE)
		{
			VIDDEC_PTHREAD_MUTEX_SIGNAL(pComponentPrivate->sDynConfigMutex);
		}
#endif
	}
	break;

	case OMX_CommandPortEnable:
	{
		LOG_API("OMX_CommandPortEnable to %lu", nParam1);

		if (nParam1 == VIDDEC_INPUT_PORT)
		{
			pComponentPrivate->pInPortDef->bEnabled = OMX_TRUE;
			LOG_API("Enabling VIDDEC_INPUT_PORT 0x%x\n",
						 pComponentPrivate->pInPortDef->bEnabled);
		}
		else if (nParam1 == VIDDEC_OUTPUT_PORT)
		{
			pComponentPrivate->pOutPortDef->bEnabled = OMX_TRUE;
			LOG_API("Enabling VIDDEC_OUTPUT_PORT 0x%x\n",
						  pComponentPrivate->pOutPortDef->bEnabled);
		}
		else if (nParam1 == OMX_ALL)
		{
			pComponentPrivate->pInPortDef->bEnabled = OMX_TRUE;
			pComponentPrivate->pOutPortDef->bEnabled = OMX_TRUE;
			LOG_API("Enabling VIDDEC_INPUT_PORT 0x%x\n",
						  pComponentPrivate->pInPortDef->bEnabled);
		}
		nRet = write(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE],
					 &Cmd, sizeof(Cmd));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
					 &nParam1, sizeof(nParam1));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
	}
	break;

	case OMX_CommandFlush:
	{

		if ( nParam1 > 1 && nParam1 != OMX_ALL )
		{
			eError = OMX_ErrorBadPortIndex;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE],
							&Cmd, sizeof(Cmd));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
					 &nParam1, sizeof(nParam1));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
	}
	break;
	case OMX_CommandMarkBuffer:
	{
		if ( nParam1 > VIDDEC_OUTPUT_PORT )
		{
			eError = OMX_ErrorBadPortIndex;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE],
					 &Cmd, sizeof(Cmd));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
					 &nParam1, sizeof(nParam1));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
		nRet = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
					 &pCmdData, sizeof(pCmdData));
		if (nRet == -1)
		{
			eError = OMX_ErrorUndefined;
			goto EXIT;
		}
	}
	break;

	case OMX_CommandMax:
		break;
	default:
		eError = OMX_ErrorUndefined;
    }

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_GetParameter									     */
/*                                                                           */
/*  Description   : Recieves Parameter type									 */
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
static OMX_ERRORTYPE VIDDEC_GetParameter (
								OMX_IN OMX_HANDLETYPE hComponent,
                                OMX_IN  OMX_INDEXTYPE nParamIndex,
                                OMX_INOUT OMX_PTR ComponentParameterStructure)
{
    OMX_COMPONENTTYPE*        pComp             = NULL;
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;

	OMX_TI_PARAMNATIVEBUFFERUSAGE *pUsage = NULL;
	LOG_API("+++Entering\n");
#ifdef KHRONOS_1_1
    OMX_PARAM_COMPONENTROLETYPE *pRole           = NULL;
#endif
    OMX_CONF_CHECK_CMD(hComponent, ComponentParameterStructure, OMX_TRUE);

    pComp             = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pComp->pComponentPrivate;
    OMX_U32 tempCompressionFormat      = pComponentPrivate->pInPortDef->
   							         		format.video.eCompressionFormat;
    /*            add check if state != loaded or port not disabled          */
    if (pComponentPrivate->eState == OMX_StateInvalid)
    {
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);
    }

    switch (nParamIndex)
    {
//sasken start
	case VideoDecodeGetAndroidNativeBufferUsage:
        {
ALOGD("%s:%d VideoDecodeGetAndroidNativeBufferUsage", __FUNCTION__, __LINE__);
    	   pUsage = (OMX_TI_PARAMNATIVEBUFFERUSAGE*)ComponentParameterStructure;
    	   		if(pComponentPrivate->pCompPort[pUsage->nPortIndex]->VIDDECBufferType == GrallocPointers)
    	   		{
    	   			//PROXY_CHK_VERSION(pParamStruct, OMX_TI_PARAMNATIVEBUFFERUSAGE);
    	   			pUsage->nUsage = GRALLOC_USAGE_HW_RENDER;
    	   			goto EXIT;
    	   		}
    	   	break;
        }
        case VideoDecodeGetAndroiduseAndroidNativeBuffer2 :
ALOGD("%s:%d VideoDecodeGetAndroiduseAndroidNativeBuffer2", __FUNCTION__, __LINE__);
            break;
//sasken stop
	case OMX_IndexConfigVideoMBErrorReporting:
	{
		if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
			tempCompressionFormat == OMX_VIDEO_CodingH263 ||
			tempCompressionFormat == OMX_VIDEO_CodingAVC ||
			tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			OMX_CONFIG_MBERRORREPORTINGTYPE* pMBErrorReportTo =
											ComponentParameterStructure;
			pMBErrorReportTo->bEnabled =
								pComponentPrivate->eMBErrorReport.bEnabled;
		}
		else
		{
			eError = OMX_ErrorUnsupportedIndex;
		}
	break;
	}
	case OMX_IndexParamVideoInit:
		memcpy(ComponentParameterStructure,
				pComponentPrivate->pPortParamType,
					sizeof(OMX_PORT_PARAM_TYPE));
	break;

#ifdef __STD_COMPONENT__
	case OMX_IndexParamAudioInit:
		memcpy(ComponentParameterStructure,
				pComponentPrivate->pPortParamTypeAudio,
					sizeof(OMX_PORT_PARAM_TYPE));
	break;
	case OMX_IndexParamImageInit:
		memcpy(ComponentParameterStructure,
				pComponentPrivate->pPortParamTypeImage,
					sizeof(OMX_PORT_PARAM_TYPE));
	break;
	case OMX_IndexParamOtherInit:
		memcpy(ComponentParameterStructure,
				pComponentPrivate->pPortParamTypeOthers,
					sizeof(OMX_PORT_PARAM_TYPE));
	break;
#ifdef KHRONOS_1_1
	case OMX_IndexParamVideoMacroblocksPerFrame:
	{
		OMX_PARAM_MACROBLOCKSTYPE* pMBBlocksTypeTo =
											ComponentParameterStructure;
		pMBBlocksTypeTo->nMacroblocks =
				pComponentPrivate->pOutPortDef->format.video.nFrameWidth *
										pComponentPrivate->pOutPortDef->
										format.video.nFrameHeight / 256;
	break;
	}
	case OMX_IndexParamVideoProfileLevelQuerySupported:
	{
		VIDEO_PROFILE_LEVEL_TYPE* pProfileLevel = NULL;
		
		OMX_VIDEO_PARAM_PROFILELEVELTYPE *pParamProfileLevel =
		(OMX_VIDEO_PARAM_PROFILELEVELTYPE *)ComponentParameterStructure;
		if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
			pParamProfileLevel->eProfile = OMX_VIDEO_MPEG4ProfileSimple;
			pComponentPrivate->pMpeg4->eProfile = pParamProfileLevel->eProfile;
			pComponentPrivate->pMpeg4->eLevel = pParamProfileLevel->eLevel;
			pComponentPrivate->nCurrentMPEG4ProfileIndex =
									pParamProfileLevel->nProfileIndex;
			eError = OMX_ErrorNone;
			switch(pParamProfileLevel->nProfileIndex)
			{
			case 0:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level0;
			break;
			case 1:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level0b;
			break;
			case 2:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level1;
			break;
			case 3:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level2;
			break;
			case 4:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level3;
			break;
			case 5:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level4;
			break;
			case 6:
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4Level4a;
			break;
			case 7:
					pParamProfileLevel->eProfile = OMX_VIDEO_MPEG4ProfileMax;
					pParamProfileLevel->eLevel = OMX_VIDEO_MPEG4LevelMax;
			break;
			default:
					eError = OMX_ErrorNoMore;
			break;
			}
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingAVC)
		{
			#if 0
			pParamProfileLevel->eProfile = OMX_VIDEO_AVCProfileBaseline;
			pComponentPrivate->pH264->eProfile = pParamProfileLevel->eProfile;
			pComponentPrivate->pH264->eLevel = pParamProfileLevel->eLevel;
			pComponentPrivate->nCurrentMPEG4ProfileIndex = pParamProfileLevel->nProfileIndex;
			eError = OMX_ErrorNone;
			switch(pParamProfileLevel->nProfileIndex)
			{
			case 0:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel1;
			break;
			case 1:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel1b;
			break;
			case 2:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel11;
			break;
			case 3:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel12;
			break;
			case 4:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel13;
			break;
			case 5:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel2;
			break;
			case 6:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel21;
					break;
			case 7:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel1;
			break;
			case 8:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel22;
			break;
			case 9:
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevel3;
			break;
			case 10:
					pParamProfileLevel->eProfile = OMX_VIDEO_AVCProfileMax;
					pParamProfileLevel->eLevel = OMX_VIDEO_AVCLevelMax;
			break;
			default:
					eError = OMX_ErrorNoMore;
			break;
			}
			
			#else
			
			pProfileLevel = SupportedAVCProfileLevels;

            /* Point to table entry based on index */
            pProfileLevel += pParamProfileLevel->nProfileIndex;

            /* -1 indicates end of table */
            if(pProfileLevel->nProfile != -1) 
            {
				pParamProfileLevel->eProfile = pProfileLevel->nProfile;
                pParamProfileLevel->eLevel = pProfileLevel->nLevel;
                eError = OMX_ErrorNone;
			}
            else 
				eError = OMX_ErrorNoMore;

			break;	  
			#endif
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			switch(pParamProfileLevel->nProfileIndex)
			{
			case 0:
					pParamProfileLevel->eProfile =
							OMX_VIDEO_WMVFormat9;
					pComponentPrivate->nCurrentMPEG4ProfileIndex =
							pParamProfileLevel->nProfileIndex;
					eError = OMX_ErrorNone;
			break;
			default:
					eError = OMX_ErrorNoMore;
			break;

			}
		}
		else
		{
			eError = OMX_ErrorNoMore;
		}
		}
	case OMX_IndexParamVideoProfileLevelCurrent:
	{
		OMX_VIDEO_PARAM_PROFILELEVELTYPE *pParamProfileLevel =
		(OMX_VIDEO_PARAM_PROFILELEVELTYPE *)ComponentParameterStructure;
		if (tempCompressionFormat ==
										OMX_VIDEO_CodingAVC)
		{
			pParamProfileLevel->eProfile =
					pComponentPrivate->pH264->eProfile;
			pParamProfileLevel->eLevel =
					pComponentPrivate->pH264->eLevel;
			pParamProfileLevel->nProfileIndex =
					pComponentPrivate->nCurrentMPEG4ProfileIndex;
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			pParamProfileLevel->eProfile = OMX_VIDEO_WMVFormat9;
			pParamProfileLevel->eLevel = 0;
			pParamProfileLevel->nProfileIndex =
					pComponentPrivate->nCurrentMPEG4ProfileIndex;
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
		{
			pParamProfileLevel->eProfile =
					pComponentPrivate->pMpeg4->eProfile;
			pParamProfileLevel->eLevel =
					pComponentPrivate->pMpeg4->eLevel;
			pParamProfileLevel->nProfileIndex =
					pComponentPrivate->nCurrentMPEG4ProfileIndex;
		}
		else if (tempCompressionFormat == OMX_VIDEO_CodingH263)
		{
			pParamProfileLevel->eProfile =
					pComponentPrivate->pH263->eProfile;
			pParamProfileLevel->eLevel =
					pComponentPrivate->pH263->eLevel;
			pParamProfileLevel->nProfileIndex =
					pComponentPrivate->nCurrentMPEG4ProfileIndex;
		}
		else
		{
			eError = OMX_ErrorBadParameter;
		}
	}
	break;
	case OMX_IndexParamStandardComponentRole:
		if (ComponentParameterStructure != NULL)
		{
			pRole = (OMX_PARAM_COMPONENTROLETYPE *)ComponentParameterStructure;
			memcpy( pRole, &pComponentPrivate->componentRole,
						sizeof(OMX_PARAM_COMPONENTROLETYPE));
		}
		else
		{
			eError = OMX_ErrorBadParameter;
		}
	break;
#endif  /*KHRONOS     */
#endif  /*STDCOMPONENT*/
	case OMX_IndexParamPortDefinition:
	{
		if (((OMX_PARAM_PORTDEFINITIONTYPE*)
				(ComponentParameterStructure))->nPortIndex ==
				pComponentPrivate->pInPortDef->nPortIndex)
		{
			OMX_PARAM_PORTDEFINITIONTYPE *pPortDef =
								pComponentPrivate->pInPortDef;
			OMX_PARAM_PORTDEFINITIONTYPE *pPortDefParam =
			(OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure;
			memcpy(pPortDefParam, pPortDef,
						sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
		}
		else if (((OMX_PARAM_PORTDEFINITIONTYPE*)
			(ComponentParameterStructure))->nPortIndex ==
				pComponentPrivate->pOutPortDef->nPortIndex)
		{
			OMX_PARAM_PORTDEFINITIONTYPE *pPortDefParam =
			(OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure;
			OMX_PARAM_PORTDEFINITIONTYPE *pPortDef =
					pComponentPrivate->pOutPortDef;

/*			if(pPortDef->format.video.nFrameWidth % 32 != 0)
				pPortDef->format.video.nFrameWidth = ((int)(pPortDef->format.video.nFrameWidth/32) + 1) * 32;*/
			ALOGD("pPortDef->format.video.nFrameWidth is %d", pPortDef->format.video.nFrameWidth);
			ALOGD("pPortDef->format.video.nFrameHeight is %d", pPortDef->format.video.nFrameHeight);
			ALOGD("%s ::: %d Buffer size = %d", __func__, __LINE__, pPortDef->nBufferSize);

			memcpy(pPortDefParam, pPortDef,
					sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
		        

			OMX_PRINT2(pComponentPrivate->dbg,
								"Get Ht %lu Wt %lu \n",
								pPortDef->format.video.nFrameHeight,
								pPortDef->format.video.nFrameWidth);
		}
		else
		{
			eError = OMX_ErrorBadPortIndex;
		}

		LOG_API(
						"CountActual 0x%x CountMin 0x%x Size %d bEnabled %x bPopulated %x compression %d %d %d\n",
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->nBufferCountActual,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->nBufferCountMin,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->nBufferSize,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->bEnabled,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->bPopulated,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->format.video.nFrameWidth,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->format.video.nFrameHeight,
						(int )((OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure)->format.video.eCompressionFormat);
	}
	break;
	case OMX_IndexParamVideoPortFormat:
	{
		OMX_VIDEO_PARAM_PORTFORMATTYPE* pPortFormat =
		(OMX_VIDEO_PARAM_PORTFORMATTYPE*)ComponentParameterStructure;
		if (pPortFormat->nPortIndex ==
				pComponentPrivate->pInPortFormat->nPortIndex)
		{
			switch (pPortFormat->nIndex)
			{
			case VIDDEC_DEFAULT_INPUT_INDEX_H264:
				LOG_API(
						"eCompressionFormat = OMX_VIDEO_CodingAVC\n");
				pComponentPrivate->pInPortFormat->nIndex =
								VIDDEC_DEFAULT_INPUT_INDEX_H264;
				pComponentPrivate->pInPortFormat->eCompressionFormat=
								OMX_VIDEO_CodingAVC;
				tempCompressionFormat=
								OMX_VIDEO_CodingAVC;
			break;

			case VIDDEC_DEFAULT_INPUT_INDEX_MPEG4:
			case VIDDEC_DEFAULT_INPUT_INDEX_H263:
				pComponentPrivate->pInPortFormat->nIndex =
							VIDDEC_DEFAULT_INPUT_INDEX_MPEG4;
				pComponentPrivate->pInPortFormat->eCompressionFormat=
							OMX_VIDEO_CodingMPEG4;
				tempCompressionFormat  =
							OMX_VIDEO_CodingMPEG4;
			break;

			case VIDDEC_DEFAULT_INPUT_INDEX_WMV9:
				LOG_API(
				 "eCompressionFormat = VIDDEC_DEFAULT_INPUT_INDEX_WMV9\n");
				pComponentPrivate->pInPortFormat->nIndex=
							VIDDEC_DEFAULT_INPUT_INDEX_WMV9;
				pComponentPrivate->pInPortFormat->eCompressionFormat=
							OMX_VIDEO_CodingWMV;
				tempCompressionFormat  =
							OMX_VIDEO_CodingWMV;
			break;

			default:

				eError = OMX_ErrorNoMore;
			break;
			}
			if(eError == OMX_ErrorNone)
			{
				memcpy(ComponentParameterStructure,
						pComponentPrivate->pInPortFormat,
						sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
			}
		}
		else if (pPortFormat->nPortIndex ==
					pComponentPrivate->pOutPortFormat->nPortIndex)
		{
			switch (pPortFormat->nIndex)
			{
			case VIDDEC_DEFAULT_OUTPUT_INDEX_PLANAR420:
				pComponentPrivate->pOutPortFormat->eColorFormat =
						VIDDEC_COLORFORMAT420;
			break;
			case VIDDEC_DEFAULT_OUTPUT_INDEX_INTERLEAVED422:
				pComponentPrivate->pOutPortFormat->eColorFormat =
						VIDDEC_COLORFORMAT422;
			break;
			default:
				eError = OMX_ErrorNoMore;
			break;
			}
			if(eError == OMX_ErrorNone)
			{
				memcpy(ComponentParameterStructure,
						pComponentPrivate->pOutPortFormat,
					 sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
			}
		}
		else
		{
			eError = OMX_ErrorBadPortIndex;
		}
	}
    break;
	case OMX_IndexParamPriorityMgmt:
		memcpy(ComponentParameterStructure,pComponentPrivate->pPriorityMgmt,
				sizeof(OMX_PRIORITYMGMTTYPE));

	break;


	case  OMX_IndexParamVideoMpeg4:
	{
		if (((OMX_VIDEO_PARAM_MPEG4TYPE*)
			(ComponentParameterStructure))->nPortIndex ==
				pComponentPrivate->pMpeg4->nPortIndex)
		{
			memcpy(ComponentParameterStructure,
					pComponentPrivate->pMpeg4,
						sizeof(OMX_VIDEO_PARAM_MPEG4TYPE));
		}
		else
		{
			eError = OMX_ErrorBadPortIndex;
		}
	}
	break;

	case  OMX_IndexParamVideoAvc:
	{
		if(((OMX_VIDEO_PARAM_AVCTYPE*)
			(ComponentParameterStructure))->nPortIndex ==
				pComponentPrivate->pH264->nPortIndex)
		{
			memcpy(ComponentParameterStructure,
					pComponentPrivate->pH264,
					sizeof(OMX_VIDEO_PARAM_AVCTYPE));
		}
		else
		{
			eError = OMX_ErrorBadPortIndex;
		}
	}
	break;

	case OMX_IndexParamCompBufferSupplier:
	{
		OMX_PARAM_BUFFERSUPPLIERTYPE* pBuffSupplierParam =
			(OMX_PARAM_BUFFERSUPPLIERTYPE*)ComponentParameterStructure;

		if (pBuffSupplierParam->nPortIndex == 1)
		{
			pBuffSupplierParam->eBufferSupplier =
				pComponentPrivate->
					pCompPort[pBuffSupplierParam->nPortIndex]->
							eSupplierSetting;
		}
		else if (pBuffSupplierParam->nPortIndex == 0)
		{
			pBuffSupplierParam->eBufferSupplier =
				pComponentPrivate->
					pCompPort[pBuffSupplierParam->nPortIndex]->
							eSupplierSetting;
		}
		else
		{
			eError = OMX_ErrorBadPortIndex;
			break;
		}
	}
	break;
	case VideoDecodeCustomParamProcessMode:
		*((OMX_U32 *)ComponentParameterStructure) =
				pComponentPrivate->ProcessMode;
	break;

	case VideoDecodeCustomParamParserEnabled:
		*((OMX_BOOL *)ComponentParameterStructure) =
				pComponentPrivate->bParserEnabled;
	break;

	case VideoDecodeCustomParamH264BitStreamFormat:
		*((OMX_U32 *)ComponentParameterStructure) =
				pComponentPrivate->H264BitStreamFormat;
	break;

	case VideoDecodeCustomParamWMVProfile:
		{
			*((VIDDEC_WMV_PROFILES *)ComponentParameterStructure) =
					pComponentPrivate->wmvProfile;
		}
	break;
	case VideoDecodeCustomParamWMVFileType:
	{
		OMX_PARAM_WMVFILETYPE* pWMVFileType =
				(OMX_PARAM_WMVFILETYPE*)ComponentParameterStructure;
		pWMVFileType->nWmvFileType = pComponentPrivate->nWMVFileType;
	}
	break;
	case VideoDecodeCustomParamIsNALBigEndian:
		*((OMX_BOOL *)ComponentParameterStructure) =
				pComponentPrivate->bIsNALBigEndian;

	break;
#ifdef VIDDEC_FLAGGED_EOS
	case VideoDecodeCustomParambUseFlaggedEos:
		*((OMX_BOOL *)ComponentParameterStructure) =
				pComponentPrivate->bUseFlaggedEos;

	break;
#endif
	case OMX_IndexParamCommonDeblocking:
	{
		memcpy(ComponentParameterStructure,
				pComponentPrivate->pDeblockingParamType,
					sizeof(OMX_PARAM_DEBLOCKINGTYPE));
		break;
	}
#ifdef ANDROID
	/*                     Opencore specific                             */
	/*        Obtain the capabilities of the OMX component               */
	case (OMX_INDEXTYPE) PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX:
		if ((tempCompressionFormat == OMX_VIDEO_CodingAVC) ||
			(tempCompressionFormat == OMX_VIDEO_CodingMPEG4)||
			(tempCompressionFormat == OMX_VIDEO_CodingWMV))
		{
			memcpy(ComponentParameterStructure,
					pComponentPrivate->pPVCapabilityFlags,
					sizeof(PV_OMXComponentCapabilityFlagsType));
			eError = OMX_ErrorNone;
		}
		else
		{
			eError = OMX_ErrorUnsupportedIndex;
		}
		break;
#endif
	default:
		LOG_API(
					"GETPARAMETER >> ERROR OMX_ErrorUnsupportedIndex %d\n",
						 nParamIndex);
		eError = OMX_ErrorUnsupportedIndex;
		break;
        }

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_SetParameter									     */
/*                                                                           */
/*  Description   : Setting the Parameter	    							 */
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

static OMX_ERRORTYPE VIDDEC_SetParameter (OMX_HANDLETYPE hComp,
                                          OMX_INDEXTYPE  nParamIndex,
                                          OMX_PTR        pCompParam)
{
    OMX_COMPONENTTYPE*        pHandle                    = NULL;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate          = NULL;
    OMX_ERRORTYPE             eError                     = OMX_ErrorNone;

OMX_TI_PARAMUSENATIVEBUFFER *pParamNativeBuffer = NULL;

#ifdef KHRONOS_1_1
    OMX_PARAM_COMPONENTROLETYPE *pRole          = NULL;
#endif
    OMX_CONF_CHECK_CMD(hComp, pCompParam, OMX_TRUE);
    pHandle                                     = (OMX_COMPONENTTYPE*)hComp;
    pComponentPrivate                           = pHandle->pComponentPrivate;
    OMX_U32          tempCompressionFormat      = pComponentPrivate->
    								pInPortDef->format.video.eCompressionFormat;

    LOG_API("+++Entering\n");
    if (pComponentPrivate->eState != OMX_StateLoaded &&
    		pComponentPrivate->eState != OMX_StateWaitForResources)
    {

        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);
    }
    switch (nParamIndex)
    {
	
//#ifdef ENABLE_GRALLOC_BUFFERS
        case VideoDecodeEnableAndroidNativeBuffers:
        {
            ALOGD("%s:%d In VideoDecodeEnableAndroidNativeBuffers in VIDDEC_SetParameter", __FUNCTION__, __LINE__);
            pParamNativeBuffer = (OMX_TI_PARAMUSENATIVEBUFFER* )pCompParam;
			if(pParamNativeBuffer->bEnable == OMX_TRUE)
			{
				ALOGD("%s:%d IN VIDDEC_SetParameter, in if-condition ", __FUNCTION__, __LINE__);
				pComponentPrivate->pCompPort[pParamNativeBuffer->nPortIndex]->VIDDECBufferType = GrallocPointers;
				pComponentPrivate->pCompPort[pParamNativeBuffer->nPortIndex]->IsBuffer2D = OMX_TRUE;
			}
			break;
        }
//#endif

case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE* pPortFormat =
				(OMX_VIDEO_PARAM_PORTFORMATTYPE*)pCompParam;
			if (pPortFormat->nPortIndex ==
						pComponentPrivate->pInPortFormat->nPortIndex)
			{
				if(pPortFormat->eColorFormat == OMX_COLOR_FormatUnused)
				{
					switch (pPortFormat->eCompressionFormat)
					{
						case OMX_VIDEO_CodingH263:
							tempCompressionFormat  = OMX_VIDEO_CodingH263;
						break;
						case OMX_VIDEO_CodingAVC:
							tempCompressionFormat  = OMX_VIDEO_CodingAVC;
							LOG_API("eCompressionFormat = OMX_VIDEO_CodingAVC\n");
						break;
						case OMX_VIDEO_CodingMPEG4:
							tempCompressionFormat  = OMX_VIDEO_CodingMPEG4;
							LOG_API("eCompressionFormat = OMX_VIDEO_CodingMPEG4\n");
						break;
						case OMX_VIDEO_CodingWMV:
							LOG_API("eCompressionFormat = OMX_VIDEO_CodingWMV\n");
							tempCompressionFormat  = OMX_VIDEO_CodingWMV;
							break;

						default:
							eError = OMX_ErrorNoMore;
							break;
					}
				}
				else
				{
					eError = OMX_ErrorBadParameter;
				}
				if(eError == OMX_ErrorNone)
				{
					memcpy(pComponentPrivate->pInPortFormat,
							pPortFormat,
							sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
				}
			}
			else if (pPortFormat->nPortIndex ==
						pComponentPrivate->pOutPortFormat->nPortIndex)
			{
				if(pPortFormat->eCompressionFormat == OMX_VIDEO_CodingUnused)
				{
					switch (pPortFormat->eColorFormat)
					{
						case OMX_COLOR_FormatYUV420Planar:
							pComponentPrivate->pOutPortDef->
								format.video.eColorFormat =
									VIDDEC_COLORFORMAT420;
							break;
						case OMX_COLOR_FormatCbYCrY:
							pComponentPrivate->pOutPortDef->
								format.video.eColorFormat =
									VIDDEC_COLORFORMAT422;
							break;
						default:
							eError = OMX_ErrorNoMore;
							break;
					}
				}
				else
				{
					eError = OMX_ErrorBadParameter;
				}
				if(eError == OMX_ErrorNone)
				{
					memcpy(pComponentPrivate->pOutPortFormat,
							pPortFormat,
								sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
				}
			}
			else
			{
				eError = OMX_ErrorBadPortIndex;
			}
		}
		break;
        case OMX_IndexParamVideoInit:
            memcpy(pComponentPrivate->pPortParamType,
            		(OMX_PORT_PARAM_TYPE*)pCompParam,
            			sizeof(OMX_PORT_PARAM_TYPE));
            break;
#ifdef __STD_COMPONENT__
        case OMX_IndexParamAudioInit:
            memcpy(pComponentPrivate->pPortParamTypeAudio,
            		(OMX_PORT_PARAM_TYPE*)pCompParam,
            			sizeof(OMX_PORT_PARAM_TYPE));
            break;
        case OMX_IndexParamImageInit:
            memcpy(pComponentPrivate->pPortParamTypeImage,
            		(OMX_PORT_PARAM_TYPE*)pCompParam,
            			sizeof(OMX_PORT_PARAM_TYPE));
            break;
        case OMX_IndexParamOtherInit:
            memcpy(pComponentPrivate->pPortParamTypeOthers,
            		(OMX_PORT_PARAM_TYPE*)pCompParam,
            			sizeof(OMX_PORT_PARAM_TYPE));
            break;
#endif
        case OMX_IndexParamPortDefinition:
            {
                OMX_PARAM_PORTDEFINITIONTYPE* pComponentParam =
                			(OMX_PARAM_PORTDEFINITIONTYPE*)pCompParam;
                if (pComponentParam->nPortIndex ==
                		pComponentPrivate->pInPortDef->nPortIndex)
               	{
                    
                    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefParam =
                    		(OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParam;
                    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef =
                    		pComponentPrivate->pInPortDef;
                    
                    memcpy(pPortDef, pPortDefParam,
                    			sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

		   
/* CSR-OMAPS00285402 - Fix for the issue I007_MP4_H.264(MP)_AAC-LC(227.4kb).mp4 doesn't work [START] */		    
		    if(pPortDef->format.video.nFrameWidth < 176)
		    {
			ALOGD( "Unsuported low resolution: width %ld",
						pPortDef->format.video.nFrameWidth);
			eError = OMX_ErrorUnsupportedSetting;
			break;

		    }
/* CSR-OMAPS00285402 - Fix for the issue I007_MP4_H.264(MP)_AAC-LC(227.4kb).mp4 doesn't work [END] */
		    
		    if((pPortDef->format.video.nFrameWidth * pPortDef->format.video.nFrameHeight) > (1280 * 720)) 
                    {
			ALOGD( "Unsuported high resolution: width %ld ; height %ld; width * height = %ld",
						pPortDef->format.video.nFrameWidth,
						pPortDef->format.video.nFrameHeight,
						(pPortDef->format.video.nFrameWidth * pPortDef->format.video.nFrameHeight));
						eError = OMX_ErrorUnsupportedSetting;
						break;
		    }
                }
                else if (pComponentParam->nPortIndex ==
                				pComponentPrivate->pOutPortDef->nPortIndex)
                {
                    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefParam =
                    			(OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParam;
                    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef =
                    			pComponentPrivate->pOutPortDef;
                    memcpy(pPortDef, pPortDefParam,
                    			sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

		    if(pPortDef->format.video.nFrameWidth != MULTIPLE_32(pPortDef->format.video.nFrameWidth))
                    {
			ALOGD("%s %d Out port width %d", __func__, __LINE__, pPortDef->format.video.nFrameWidth);
 		    	pPortDef->format.video.nFrameWidth = MULTIPLE_32(pPortDef->format.video.nFrameWidth);
			ALOGD("%s %d Ceiled out port width to %d", __func__, __LINE__, pPortDef->format.video.nFrameWidth);
                    }

                    pPortDef->nBufferSize = pPortDef->format.video.nFrameWidth *
                    pPortDef->format.video.nFrameHeight *((pComponentPrivate->
                    pOutPortFormat->eColorFormat ==
                    VIDDEC_COLORFORMAT420) ? VIDDEC_FACTORFORMAT420 :
                                            			VIDDEC_FACTORFORMAT422);

    		    ALOGD("pPortDef->format.video.nFrameWidth is %d", pPortDef->format.video.nFrameWidth);			
		    ALOGD("pPortDef->format.video.nFrameHeight is %d", pPortDef->format.video.nFrameHeight);
	     	    ALOGD("%s ::: %d Buffer size = %d", __func__, __LINE__, pPortDef->nBufferSize);

                    OMX_PRINT2(pComponentPrivate->dbg,
                    "SetParams Ht %lu Wt %lu Colorinfo %s\n",
                    pPortDef->format.video.nFrameHeight,
                    pPortDef->format.video.nFrameWidth,
                    (pComponentPrivate->pOutPortFormat->eColorFormat ==
                     VIDDEC_COLORFORMAT420) ? "VIDDEC_FACTORFORMAT420" :
                                            		"VIDDEC_FACTORFORMAT422");
                }
                else
                {
                    eError = OMX_ErrorBadPortIndex;
                }
                LOG_API(
                "CountActual 0x%x CountMin 0x%x Size %d bEnabled %x bPopulated %x\n",
                                (int )pComponentParam->nBufferCountActual,
                                (int )pComponentParam->nBufferCountMin,
                                (int )pComponentParam->nBufferSize,
                                (int )pComponentParam->bEnabled,
                                (int )pComponentParam->bPopulated);
            }
            break;
        case OMX_IndexParamVideoMpeg4:

            {
                OMX_VIDEO_PARAM_MPEG4TYPE* pComponentParam =
                		(OMX_VIDEO_PARAM_MPEG4TYPE*)pCompParam;
                if (pComponentParam->nPortIndex ==
                			pComponentPrivate->pMpeg4->nPortIndex)
                {
                    memcpy(pComponentPrivate->pMpeg4,
                    		pCompParam,
                    			sizeof(OMX_VIDEO_PARAM_MPEG4TYPE));
                }
                else
                {
                    eError = OMX_ErrorBadPortIndex;
                }
            }
            break;

        case OMX_IndexParamVideoAvc:
            {
                OMX_VIDEO_PARAM_AVCTYPE* pComponentParam =
                		(OMX_VIDEO_PARAM_AVCTYPE*)pCompParam;
                if (pComponentParam->nPortIndex ==
                			pComponentPrivate->pH264->nPortIndex)
                {
                    memcpy(pComponentPrivate->pH264,
                    			pCompParam,
                    				sizeof(OMX_VIDEO_PARAM_AVCTYPE));
                }
                else
                {
                    eError = OMX_ErrorBadPortIndex;
                }
            }
            break;

        case OMX_IndexParamPriorityMgmt:
            memcpy(pComponentPrivate->pPriorityMgmt,
            		(OMX_PRIORITYMGMTTYPE*)pCompParam,
            				sizeof(OMX_PRIORITYMGMTTYPE));
        break;
        case OMX_IndexParamCompBufferSupplier:
            {
                OMX_PARAM_BUFFERSUPPLIERTYPE* pBuffSupplierParam =
                			(OMX_PARAM_BUFFERSUPPLIERTYPE*)pCompParam;

                if (pBuffSupplierParam->nPortIndex == 1)
                {
                    pComponentPrivate->
                    	pCompPort[pBuffSupplierParam->nPortIndex]->
                    			eSupplierSetting =
                    					pBuffSupplierParam->eBufferSupplier;
                }
                else if (pBuffSupplierParam->nPortIndex == 0)
                {
                    pComponentPrivate->
                    	pCompPort[pBuffSupplierParam->nPortIndex]->
                    			eSupplierSetting =
                    					pBuffSupplierParam->eBufferSupplier;
                }
                else
                {
                    eError = OMX_ErrorBadPortIndex;
                    break;
                }
            }
            break;
        /* Video decode custom parameters */
        case VideoDecodeCustomParamProcessMode:
            pComponentPrivate->ProcessMode =
            		(OMX_U32)(*((OMX_U32 *)pCompParam));
            break;
        case VideoDecodeCustomParamParserEnabled:
            pComponentPrivate->bParserEnabled =
            		(OMX_BOOL)(*((OMX_BOOL *)pCompParam));
            break;
        case VideoDecodeCustomParamH264BitStreamFormat:
            pComponentPrivate->H264BitStreamFormat =
            		(OMX_U32)(*((OMX_U32 *)pCompParam));
            break;
        case VideoDecodeCustomParamWMVProfile:
            {
                pComponentPrivate->wmvProfile =
                		*((VIDDEC_WMV_PROFILES *)pCompParam);
            }
            break;
#ifdef KHRONOS_1_1
        case OMX_IndexParamStandardComponentRole:
            if (pCompParam != NULL)
            {
                OMX_U8* cTempRole = NULL;
                cTempRole = (OMX_U8*)pComponentPrivate->componentRole.cRole;
                pRole = (OMX_PARAM_COMPONENTROLETYPE *)pCompParam;
                if(strcmp( (char*)pRole->cRole,
                				VIDDEC_COMPONENTROLES_H264) == 0)
                {
                    eError = VIDDEC_Load_Defaults(pComponentPrivate,
                    								VIDDEC_INIT_H264);
                }
                else if(strcmp( (char*)pRole->cRole,
                					VIDDEC_COMPONENTROLES_MPEG4) == 0)
                {
                    eError = VIDDEC_Load_Defaults(pComponentPrivate,
                    								VIDDEC_INIT_MPEG4);
                }
                else if(strcmp( (char*)pRole->cRole,
                					VIDDEC_COMPONENTROLES_WMV9) == 0)
                {
                    eError = VIDDEC_Load_Defaults(pComponentPrivate,
                    								VIDDEC_INIT_WMV9);
                }
                else
                {
                    eError = OMX_ErrorBadParameter;
                }
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
#ifdef ANDROID
                /* Set format according with hw accelerated rendering        */
                if( pComponentPrivate->pOutPortFormat->eColorFormat !=
                			VIDDEC_COLORFORMAT422)
                {
                    eError = VIDDEC_Load_Defaults(pComponentPrivate,
                    								VIDDEC_INIT_INTERLEAVED422);
                }
#else
                if( pComponentPrivate->pOutPortFormat->eColorFormat !=
                							VIDDEC_COLORFORMAT420)
                {
                    eError = VIDDEC_Load_Defaults(pComponentPrivate,
                    								VIDDEC_INIT_PLANAR420);
                }
#endif
                memcpy( (void *)&pComponentPrivate->componentRole,
                			(void *)pRole,
                				sizeof(OMX_PARAM_COMPONENTROLETYPE));
            }
            else
            {
                eError = OMX_ErrorBadParameter;
            }
            break;
#endif
        case VideoDecodeCustomParamWMVFileType:
            {
                OMX_PARAM_WMVFILETYPE * pWMVFileType =
                						(OMX_PARAM_WMVFILETYPE*)pCompParam;
                pComponentPrivate->nWMVFileType =
                			(OMX_U32)(pWMVFileType->nWmvFileType);
            }

            break;
        case VideoDecodeCustomParamIsNALBigEndian:
            pComponentPrivate->bIsNALBigEndian =
            				(OMX_BOOL)(*((OMX_BOOL *)pCompParam));
            break;
#ifdef VIDDEC_FLAGGED_EOS
        case VideoDecodeCustomParambUseFlaggedEos:
            pComponentPrivate->bUseFlaggedEos =
            				(OMX_BOOL)(*((OMX_BOOL *)pCompParam));
            break;
#endif
#ifdef KHRONOS_1_1
        case OMX_IndexConfigVideoMBErrorReporting:
        {
            if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
                tempCompressionFormat == OMX_VIDEO_CodingH263 ||
                tempCompressionFormat == OMX_VIDEO_CodingAVC ||
                tempCompressionFormat == OMX_VIDEO_CodingWMV)
             {
                OMX_CONFIG_MBERRORREPORTINGTYPE* pMBErrorReportFrom =
                					pCompParam;
                pComponentPrivate->eMBErrorReport.bEnabled =
                					pMBErrorReportFrom->bEnabled;
             }
            else
            {
                eError = OMX_ErrorUnsupportedIndex;
            }
            break;
        }
        case OMX_IndexParamCommonDeblocking:
        {
            if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
                tempCompressionFormat == OMX_VIDEO_CodingH263)
            {
                pComponentPrivate->pDeblockingParamType->bDeblocking =
                        ((OMX_PARAM_DEBLOCKINGTYPE*)pCompParam)->bDeblocking;
                break;
             }
            else
            {
                eError = OMX_ErrorUnsupportedIndex;
                break;
            }
        }

        case OMX_IndexParamVideoMacroblocksPerFrame:
        case OMX_IndexParamNumAvailableStreams:
        case OMX_IndexParamActiveStream:
        case OMX_IndexParamSuspensionPolicy:
        case OMX_IndexParamComponentSuspended:
        case OMX_IndexAutoPauseAfterCapture:
        case OMX_IndexParamCustomContentPipe:
        case OMX_IndexParamDisableResourceConcealment:
#ifdef KHRONOS_1_2
		case OMX_IndexConfigMetadataItemCount:
		case OMX_IndexConfigContainerNodeCount:
		case OMX_IndexConfigMetadataItem:
		case OMX_IndexConfigCounterNodeID:
		case OMX_IndexParamMetadataFilterType:
		case OMX_IndexConfigCommonTransitionEffect:
		case OMX_IndexKhronosExtensions:
#else
		case OMX_IndexConfigMetaDataSize:
		case OMX_IndexConfigMetaDataAtIndex:
		case OMX_IndexConfigMetaDataAtKey:
		case OMX_IndexConfigMetaDataNodeCount:
		case OMX_IndexConfigMetaDataNode:
		case OMX_IndexConfigMetaDataItemCount:
#endif
        case OMX_IndexParamMetadataKeyFilter:
        case OMX_IndexConfigPriorityMgmt:
        case OMX_IndexConfigAudioChannelVolume:
        case OMX_IndexConfigFlashControl:
        case OMX_IndexParamVideoProfileLevelQuerySupported:
        case OMX_IndexParamVideoProfileLevelCurrent:
        case OMX_IndexConfigVideoBitrate:
        case OMX_IndexConfigVideoFramerate:
        case OMX_IndexConfigVideoIntraVOPRefresh:
        case OMX_IndexConfigVideoIntraMBRefresh:
        case OMX_IndexConfigVideoMacroBlockErrorMap:
        case OMX_IndexParamVideoSliceFMO:
        case OMX_IndexConfigVideoAVCIntraPeriod:
        case OMX_IndexConfigVideoNalSize:
        case OMX_IndexConfigCommonExposureValue:
        case OMX_IndexConfigCommonOutputSize:
        case OMX_IndexParamCommonExtraQuantData:
        case OMX_IndexConfigCommonFocusRegion:
        case OMX_IndexConfigCommonFocusStatus:
        case OMX_IndexParamContentURI:
        case OMX_IndexConfigCaptureMode:
        case OMX_IndexConfigCapturing:
#endif
        default:
            eError = OMX_ErrorUnsupportedIndex;
            break;
    }
EXIT:
	LOG_API("---Exiting\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */

/*  Function Name : VIDDEC_GetConfig									     */
/*                                                                           */
/*  Description   : This Function Get Configuration Required. 				 */
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

static OMX_ERRORTYPE VIDDEC_GetConfig (OMX_HANDLETYPE hComp,
                                       OMX_INDEXTYPE  nConfigIndex,
                                       OMX_PTR        ComponentConfigStructure)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*        pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate = NULL;


    OMX_CONF_CHECK_CMD(hComp, ComponentConfigStructure, OMX_TRUE);

    pHandle           = (OMX_COMPONENTTYPE*)hComp;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    OMX_U32  tempCompressionFormat      = pComponentPrivate->pInPortDef->
    								format.video.eCompressionFormat;

    LOG_API("+++Entering\n");
    if (pComponentPrivate->eState == OMX_StateInvalid)
    {
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);
    }
    else
    {
        switch ((OMX_S32) nConfigIndex)
        {
		case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE *pPortDefParam =
				(OMX_PARAM_PORTDEFINITIONTYPE *)ComponentConfigStructure;
			if (((OMX_PARAM_PORTDEFINITIONTYPE*)
					(ComponentConfigStructure))->nPortIndex ==
					pComponentPrivate->pInPortDef->nPortIndex)
			{
				OMX_PARAM_PORTDEFINITIONTYPE *pPortDef =
							pComponentPrivate->pInPortDef;
				memcpy(pPortDefParam, pPortDef,
								sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
			}
			else if (((OMX_PARAM_PORTDEFINITIONTYPE*)
						(ComponentConfigStructure))->nPortIndex ==
					pComponentPrivate->pOutPortDef->nPortIndex)
			{
				OMX_PARAM_PORTDEFINITIONTYPE *pPortDef =
										pComponentPrivate->pOutPortDef;
				memcpy(pPortDefParam, pPortDef,
								sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
			}
			else
			{
				eError = OMX_ErrorBadPortIndex;
			}
		}
		break;
		case VideoDecodeCustomConfigDebug:
			OMX_DBG_GETCONFIG(pComponentPrivate->dbg,ComponentConfigStructure);
		break;
#ifdef KHRONOS_1_1
		case OMX_IndexConfigVideoMBErrorReporting:
		{
			if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
				tempCompressionFormat == OMX_VIDEO_CodingH263 ||
				tempCompressionFormat == OMX_VIDEO_CodingAVC ||
				tempCompressionFormat == OMX_VIDEO_CodingWMV)
			 {
				OMX_CONFIG_MBERRORREPORTINGTYPE* pMBErrorReportTo =
											ComponentConfigStructure;
				pMBErrorReportTo->bEnabled = pComponentPrivate->
										eMBErrorReport.bEnabled;
			 }
			else
			{
				eError = OMX_ErrorUnsupportedIndex;
			}
			break;
		}
		case OMX_IndexConfigVideoMacroBlockErrorMap:
		{
			if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
				tempCompressionFormat == OMX_VIDEO_CodingH263 ||
				tempCompressionFormat == OMX_VIDEO_CodingAVC ||
				tempCompressionFormat == OMX_VIDEO_CodingWMV)
			 {
				OMX_CONFIG_MACROBLOCKERRORMAPTYPE_TI* pMBErrorMapTypeFrom =
					&pComponentPrivate->
					eMBErrorMapType[pComponentPrivate->cMBErrorIndexOut];
				OMX_CONFIG_MACROBLOCKERRORMAPTYPE_TI* pMBErrorMapTypeTo =
					ComponentConfigStructure;
				OMX_U8* ErrMapFrom = pMBErrorMapTypeFrom->ErrMap;
				OMX_U8* ErrMapTo = pMBErrorMapTypeTo->ErrMap;
				pMBErrorMapTypeTo->nErrMapSize =
								pMBErrorMapTypeFrom->nErrMapSize;
				memcpy(ErrMapTo, ErrMapFrom,
								pMBErrorMapTypeFrom->nErrMapSize);
				pComponentPrivate->cMBErrorIndexOut++;
				pComponentPrivate->cMBErrorIndexOut %=
						pComponentPrivate->pOutPortDef->nBufferCountActual;
			}
			else
			{
				eError = OMX_ErrorUnsupportedIndex;
			}
			break;
		}
		case OMX_IndexParamVideoMacroblocksPerFrame:
			if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
				tempCompressionFormat == OMX_VIDEO_CodingH263 ||
				tempCompressionFormat == OMX_VIDEO_CodingAVC ||
				tempCompressionFormat == OMX_VIDEO_CodingWMV)
			 {
				OMX_PARAM_MACROBLOCKSTYPE* pMBBlocksTypeTo =
												ComponentConfigStructure;
				pMBBlocksTypeTo->nMacroblocks =
				pComponentPrivate->pOutPortDef->format.video.nFrameWidth *
				pComponentPrivate->pOutPortDef->format.video.nFrameHeight / 256;
			 }
			else
			{
				eError = OMX_ErrorUnsupportedIndex;
			}
		break;
            case OMX_IndexConfigCommonOutputCrop:
            {
               OMX_CONFIG_RECTTYPE *crop2 = (OMX_CONFIG_RECTTYPE*)ComponentConfigStructure;
               if (((OMX_CONFIG_RECTTYPE*)(ComponentConfigStructure))->nPortIndex == 
                            pComponentPrivate->pOutPortDef->nPortIndex) 
		{
                        crop2->nLeft   =  0;
                        crop2->nWidth  =  pComponentPrivate->pInPortDef->format.video.nFrameWidth - pComponentPrivate->nCropWidth;
                        crop2->nTop    =  0;
                        crop2->nHeight =  pComponentPrivate->pInPortDef->format.video.nFrameHeight - pComponentPrivate->nCropHeight;

                        ALOGD("Crop Width = %d, Crop Height = %d", pComponentPrivate->nCropWidth, pComponentPrivate->nCropHeight);
			ALOGD("%s ::: %d Croppped width = %d, Cropped Height = %d", __func__, __LINE__, crop2->nWidth, crop2->nHeight);
		}
	    }
	    break;
		case OMX_IndexParamNumAvailableStreams:
		case OMX_IndexParamActiveStream:
		case OMX_IndexParamSuspensionPolicy:
		case OMX_IndexParamComponentSuspended:
		case OMX_IndexAutoPauseAfterCapture:
		case OMX_IndexParamCustomContentPipe:
		case OMX_IndexParamDisableResourceConcealment:
#ifdef KHRONOS_1_2
		case OMX_IndexConfigMetadataItemCount:
		case OMX_IndexConfigContainerNodeCount:
		case OMX_IndexConfigMetadataItem:
		case OMX_IndexConfigCounterNodeID:
		case OMX_IndexParamMetadataFilterType:
		case OMX_IndexConfigCommonTransitionEffect:
		case OMX_IndexKhronosExtensions:
#else
		case OMX_IndexConfigMetaDataSize:
		case OMX_IndexConfigMetaDataAtIndex:
		case OMX_IndexConfigMetaDataAtKey:
		case OMX_IndexConfigMetaDataNodeCount:
		case OMX_IndexConfigMetaDataNode:
		case OMX_IndexConfigMetaDataItemCount:
#endif
		case OMX_IndexParamMetadataKeyFilter:
		case OMX_IndexConfigPriorityMgmt:
		case OMX_IndexParamStandardComponentRole:
		case OMX_IndexConfigAudioChannelVolume:
		case OMX_IndexConfigFlashControl:
		case OMX_IndexParamVideoProfileLevelQuerySupported:
		case OMX_IndexParamVideoProfileLevelCurrent:
		case OMX_IndexConfigVideoBitrate:
		case OMX_IndexConfigVideoFramerate:
		case OMX_IndexConfigVideoIntraVOPRefresh:
		case OMX_IndexConfigVideoIntraMBRefresh:
		case OMX_IndexParamVideoSliceFMO:
		case OMX_IndexConfigVideoAVCIntraPeriod:
		case OMX_IndexConfigVideoNalSize:
		case OMX_IndexConfigCommonExposureValue:
		case OMX_IndexConfigCommonOutputSize:
		case OMX_IndexParamCommonExtraQuantData:
		case OMX_IndexConfigCommonFocusRegion:
		case OMX_IndexConfigCommonFocusStatus:
		case OMX_IndexParamContentURI:
		case OMX_IndexConfigCaptureMode:
		case OMX_IndexConfigCapturing:
#endif
		case OMX_IndexComponentStartUnused:
		case OMX_IndexParamPriorityMgmt:
		case OMX_IndexParamAudioInit:
		case OMX_IndexParamImageInit:
		case OMX_IndexParamVideoInit:
		case OMX_IndexParamOtherInit:
		case OMX_IndexPortStartUnused:
		case OMX_IndexParamCompBufferSupplier:
		case OMX_IndexReservedStartUnused:

            /* Audio parameters and configurations                           */
		case OMX_IndexAudioStartUnused:
		case OMX_IndexParamAudioPortFormat:
		case OMX_IndexParamAudioPcm:
		case OMX_IndexParamAudioAac:
		case OMX_IndexParamAudioRa:
		case OMX_IndexParamAudioMp3:
		case OMX_IndexParamAudioAdpcm:
		case OMX_IndexParamAudioG723:
		case OMX_IndexParamAudioG729:
		case OMX_IndexParamAudioAmr:
		case OMX_IndexParamAudioWma:
		case OMX_IndexParamAudioSbc:
		case OMX_IndexParamAudioMidi:
		case OMX_IndexParamAudioGsm_FR:
		case OMX_IndexParamAudioMidiLoadUserSound:
		case OMX_IndexParamAudioG726:
		case OMX_IndexParamAudioGsm_EFR:
		case OMX_IndexParamAudioGsm_HR:
		case OMX_IndexParamAudioPdc_FR:
		case OMX_IndexParamAudioPdc_EFR:
		case OMX_IndexParamAudioPdc_HR:
		case OMX_IndexParamAudioTdma_FR:
		case OMX_IndexParamAudioTdma_EFR:
		case OMX_IndexParamAudioQcelp8:
		case OMX_IndexParamAudioQcelp13:
		case OMX_IndexParamAudioEvrc:
		case OMX_IndexParamAudioSmv:
		case OMX_IndexParamAudioVorbis:

		case OMX_IndexConfigAudioMidiImmediateEvent:
		case OMX_IndexConfigAudioMidiControl:
		case OMX_IndexConfigAudioMidiSoundBankProgram:
		case OMX_IndexConfigAudioMidiStatus:
		case OMX_IndexConfigAudioMidiMetaEvent:
		case OMX_IndexConfigAudioMidiMetaEventData:
		case OMX_IndexConfigAudioVolume:
		case OMX_IndexConfigAudioBalance:
		case OMX_IndexConfigAudioChannelMute:
		case OMX_IndexConfigAudioMute:
		case OMX_IndexConfigAudioLoudness:
		case OMX_IndexConfigAudioEchoCancelation:
		case OMX_IndexConfigAudioNoiseReduction:
		case OMX_IndexConfigAudioBass:
		case OMX_IndexConfigAudioTreble:
		case OMX_IndexConfigAudioStereoWidening:
		case OMX_IndexConfigAudioChorus:
		case OMX_IndexConfigAudioEqualizer:
		case OMX_IndexConfigAudioReverberation:

		/* Image specific parameters and configurations */
		case OMX_IndexImageStartUnused:
		case OMX_IndexParamImagePortFormat:
		case OMX_IndexParamFlashControl:
		case OMX_IndexConfigFocusControl:
		case OMX_IndexParamQFactor:
		case OMX_IndexParamQuantizationTable:
		case OMX_IndexParamHuffmanTable:

		/* Video specific parameters and configurations */
		case OMX_IndexVideoStartUnused:
		case OMX_IndexParamVideoPortFormat:
		case OMX_IndexParamVideoQuantization:
		case OMX_IndexParamVideoFastUpdate:
		case OMX_IndexParamVideoBitrate:
		case OMX_IndexParamVideoMotionVector:
		case OMX_IndexParamVideoIntraRefresh:
		case OMX_IndexParamVideoErrorCorrection:
		case OMX_IndexParamVideoVBSMC:
		case OMX_IndexParamVideoMpeg2:
		case OMX_IndexParamVideoMpeg4:
		case OMX_IndexParamVideoWmv:
		case OMX_IndexParamVideoRv:
		case OMX_IndexParamVideoAvc:
		case OMX_IndexParamVideoH263:

		/* Image & Video common Configurations */
		case OMX_IndexCommonStartUnused:
		case OMX_IndexParamCommonDeblocking:
		case OMX_IndexParamCommonSensorMode:
		case OMX_IndexParamCommonInterleave:
		case OMX_IndexConfigCommonColorFormatConversion:
		case OMX_IndexConfigCommonScale:
		case OMX_IndexConfigCommonImageFilter:
		case OMX_IndexConfigCommonColorEnhancement:
		case OMX_IndexConfigCommonColorKey:
		case OMX_IndexConfigCommonColorBlend:
		case OMX_IndexConfigCommonFrameStabilisation:
		case OMX_IndexConfigCommonRotate:
		case OMX_IndexConfigCommonMirror:
		case OMX_IndexConfigCommonOutputPosition:
		case OMX_IndexConfigCommonInputCrop:

		case OMX_IndexConfigCommonDigitalZoom:
		case OMX_IndexConfigCommonOpticalZoom:
		case OMX_IndexConfigCommonWhiteBalance:
		case OMX_IndexConfigCommonExposure:
		case OMX_IndexConfigCommonContrast:
		case OMX_IndexConfigCommonBrightness:
		case OMX_IndexConfigCommonBacklight:
		case OMX_IndexConfigCommonGamma:
		case OMX_IndexConfigCommonSaturation:
		case OMX_IndexConfigCommonLightness:
		case OMX_IndexConfigCommonExclusionRect:
		case OMX_IndexConfigCommonDithering:
		case OMX_IndexConfigCommonPlaneBlend:

		/* Reserved Configuration range */
		case OMX_IndexOtherStartUnused:
		case OMX_IndexParamOtherPortFormat:
		case OMX_IndexConfigOtherPower:
		case OMX_IndexConfigOtherStats:

		/* Reserved Time range */
		case OMX_IndexTimeStartUnused:
		case OMX_IndexConfigTimeScale:
		case OMX_IndexConfigTimeClockState:
		case OMX_IndexConfigTimeActiveRefClock:
		case OMX_IndexConfigTimeCurrentMediaTime:
		case OMX_IndexConfigTimeCurrentWallTime:
		case OMX_IndexConfigTimeCurrentAudioReference:
		case OMX_IndexConfigTimeCurrentVideoReference:
		case OMX_IndexConfigTimeMediaTimeRequest:
		case OMX_IndexConfigTimeClientStartTime:
		case OMX_IndexConfigTimePosition:
		case OMX_IndexConfigTimeSeekMode:

		/* Vendor specific area */
#ifdef KHRONOS_1_2
        case OMX_IndexVendorStartUnused:
#else
        case OMX_IndexIndexVendorStartUnused:
#endif
		/*Vendor specific structures should be in the range of 0xFF000000*/
		/*to 0xFFFFFFFF.  This range is not broken out by vendor, so     */
		/*private indexes are not guaranteed unique and therefore should */
		/*only be sent to the appropriate component.                     */

		case OMX_IndexMax:
			eError = OMX_ErrorUnsupportedIndex;
		break;
        }
    }
EXIT:
    LOG_API("---Exiting\n");
    ALOGD("eError = %d", eError);
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_SetConfig									     */
/*                                                                           */
/*  Description   : This Function Set new configuration.					 */
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
static OMX_ERRORTYPE VIDDEC_SetConfig (OMX_HANDLETYPE hComp,
                                       OMX_INDEXTYPE  nConfigIndex,
                                       OMX_PTR        ComponentConfigStructure)
{
    OMX_ERRORTYPE                 eError           = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE* pComponentConfig = NULL;
    OMX_COMPONENTTYPE*            pHandle          = NULL;
    VIDDEC_COMPONENT_PRIVATE*     pComponentPrivate= NULL;
    OMX_S32                       nConfigIndexTemp = 0;

    OMX_CONF_CHECK_CMD(hComp, ComponentConfigStructure, OMX_TRUE);

    pComponentConfig  = (OMX_PARAM_PORTDEFINITIONTYPE*)ComponentConfigStructure;
    pHandle           = (OMX_COMPONENTTYPE*)hComp;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    LOG_API("+++Entering\n");
    if (pComponentPrivate->eState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        LOG_API("state invalid for SetConfig\n");
        goto EXIT;
    }
    else
    {
        nConfigIndexTemp = nConfigIndex;
        switch (nConfigIndexTemp)
        {
		case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL;
			if (pComponentConfig->nPortIndex == VIDDEC_INPUT_PORT)
			{
				OMX_MALLOC_STRUCT(pPortDef, OMX_PARAM_PORTDEFINITIONTYPE,
					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel4]);
				if (pPortDef == NULL)
				{
					 LOG_API("malloc failed\n");
					eError = OMX_ErrorInsufficientResources;
					goto EXIT;
				}
				((VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate)->
								pPortDef[VIDDEC_INPUT_PORT] = pPortDef;
				memcpy(pPortDef, pComponentConfig,
							sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
				free(pPortDef);
			}
			else if (pComponentConfig->nPortIndex == VIDDEC_OUTPUT_PORT)
			{
				OMX_MALLOC_STRUCT(pPortDef, OMX_PARAM_PORTDEFINITIONTYPE,
					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel4]);
				if (pPortDef == NULL)
				{
					 LOG_API("malloc failed\n");
					eError = OMX_ErrorInsufficientResources;
					goto EXIT;
				}
				((VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate)->
					pPortDef[VIDDEC_OUTPUT_PORT] = pPortDef;
				memcpy(pPortDef, pComponentConfig,
							sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
				free(pPortDef);
			}
			break;
		}
            case VideoDecodeCustomConfigDebug:
                OMX_DBG_SETCONFIG(pComponentPrivate->dbg,
                						ComponentConfigStructure);
                break;
#ifdef KHRONOS_1_1
            case OMX_IndexConfigVideoMBErrorReporting:
            {
                if (pComponentPrivate->pInPortDef->
                		format.video.eCompressionFormat ==
                									OMX_VIDEO_CodingMPEG4 ||
                    pComponentPrivate->pInPortDef->
                    	format.video.eCompressionFormat ==
                    								OMX_VIDEO_CodingH263 ||
                    pComponentPrivate->pInPortDef->
                    format.video.eCompressionFormat == OMX_VIDEO_CodingAVC ||
                    pComponentPrivate->pInPortDef->
                    	format.video.eCompressionFormat == OMX_VIDEO_CodingWMV)
                 {
                    OMX_CONFIG_MBERRORREPORTINGTYPE* pMBErrorReportFrom =
                    								ComponentConfigStructure;
                    pComponentPrivate->eMBErrorReport.bEnabled =
                    							pMBErrorReportFrom->bEnabled;
                 }
                else
                {
                    eError = OMX_ErrorUnsupportedIndex;
                }
                break;
            }
            case OMX_IndexParamVideoMacroblocksPerFrame:
            case OMX_IndexParamNumAvailableStreams:
            case OMX_IndexParamActiveStream:
            case OMX_IndexParamSuspensionPolicy:
            case OMX_IndexParamComponentSuspended:
            case OMX_IndexAutoPauseAfterCapture:
            case OMX_IndexParamCustomContentPipe:
            case OMX_IndexParamDisableResourceConcealment:
#ifdef KHRONOS_1_2
            case OMX_IndexConfigMetadataItemCount:
            case OMX_IndexConfigContainerNodeCount:
            case OMX_IndexConfigMetadataItem:
            case OMX_IndexConfigCounterNodeID:
            case OMX_IndexParamMetadataFilterType:
            case OMX_IndexConfigCommonTransitionEffect:
            case OMX_IndexKhronosExtensions:
#else
            case OMX_IndexConfigMetaDataSize:
            case OMX_IndexConfigMetaDataAtIndex:
            case OMX_IndexConfigMetaDataAtKey:
            case OMX_IndexConfigMetaDataNodeCount:
            case OMX_IndexConfigMetaDataNode:
            case OMX_IndexConfigMetaDataItemCount:
#endif
            case OMX_IndexParamMetadataKeyFilter:
            case OMX_IndexConfigPriorityMgmt:
            case OMX_IndexParamStandardComponentRole:
            case OMX_IndexConfigAudioChannelVolume:
            case OMX_IndexConfigFlashControl:
            case OMX_IndexParamVideoProfileLevelQuerySupported:
            case OMX_IndexParamVideoProfileLevelCurrent:
            case OMX_IndexConfigVideoBitrate:
            case OMX_IndexConfigVideoFramerate:
            case OMX_IndexConfigVideoIntraVOPRefresh:
            case OMX_IndexConfigVideoIntraMBRefresh:
            case OMX_IndexParamVideoSliceFMO:
            case OMX_IndexConfigVideoAVCIntraPeriod:
            case OMX_IndexConfigVideoNalSize:
            case OMX_IndexConfigVideoMacroBlockErrorMap:
            case OMX_IndexConfigCommonExposureValue:
            case OMX_IndexConfigCommonOutputSize:
            case OMX_IndexParamCommonExtraQuantData:
            case OMX_IndexConfigCommonFocusRegion:
            case OMX_IndexConfigCommonFocusStatus:
            case OMX_IndexParamContentURI:
            case OMX_IndexConfigCaptureMode:
            case OMX_IndexConfigCapturing:
#endif
            case OMX_IndexComponentStartUnused:
            case OMX_IndexParamPriorityMgmt:
            case OMX_IndexParamAudioInit:
            case OMX_IndexParamImageInit:
            case OMX_IndexParamVideoInit:
            case OMX_IndexParamOtherInit:

            case OMX_IndexPortStartUnused:
            case OMX_IndexParamCompBufferSupplier:
            case OMX_IndexReservedStartUnused:

            /* Audio parameters and configurations */
            case OMX_IndexAudioStartUnused:
            case OMX_IndexParamAudioPortFormat:
            case OMX_IndexParamAudioPcm:
            case OMX_IndexParamAudioAac:
            case OMX_IndexParamAudioRa:
            case OMX_IndexParamAudioMp3:
            case OMX_IndexParamAudioAdpcm:
            case OMX_IndexParamAudioG723:
            case OMX_IndexParamAudioG729:
            case OMX_IndexParamAudioAmr:
            case OMX_IndexParamAudioWma:
            case OMX_IndexParamAudioSbc:
            case OMX_IndexParamAudioMidi:
            case OMX_IndexParamAudioGsm_FR:
            case OMX_IndexParamAudioMidiLoadUserSound:
            case OMX_IndexParamAudioG726:
            case OMX_IndexParamAudioGsm_EFR:
            case OMX_IndexParamAudioGsm_HR:
            case OMX_IndexParamAudioPdc_FR:
            case OMX_IndexParamAudioPdc_EFR:
            case OMX_IndexParamAudioPdc_HR:
            case OMX_IndexParamAudioTdma_FR:
            case OMX_IndexParamAudioTdma_EFR:
            case OMX_IndexParamAudioQcelp8:
            case OMX_IndexParamAudioQcelp13:
            case OMX_IndexParamAudioEvrc:
            case OMX_IndexParamAudioSmv:
            case OMX_IndexParamAudioVorbis:

            case OMX_IndexConfigAudioMidiImmediateEvent:
            case OMX_IndexConfigAudioMidiControl:
            case OMX_IndexConfigAudioMidiSoundBankProgram:
            case OMX_IndexConfigAudioMidiStatus:
            case OMX_IndexConfigAudioMidiMetaEvent:
            case OMX_IndexConfigAudioMidiMetaEventData:
            case OMX_IndexConfigAudioVolume:
            case OMX_IndexConfigAudioBalance:
            case OMX_IndexConfigAudioChannelMute:
            case OMX_IndexConfigAudioMute:
            case OMX_IndexConfigAudioLoudness:
            case OMX_IndexConfigAudioEchoCancelation:
            case OMX_IndexConfigAudioNoiseReduction:
            case OMX_IndexConfigAudioBass:
            case OMX_IndexConfigAudioTreble:
            case OMX_IndexConfigAudioStereoWidening:
            case OMX_IndexConfigAudioChorus:
            case OMX_IndexConfigAudioEqualizer:
            case OMX_IndexConfigAudioReverberation:

            /* Image specific parameters and configurations */
            case OMX_IndexImageStartUnused:
            case OMX_IndexParamImagePortFormat:
            case OMX_IndexParamFlashControl:
            case OMX_IndexConfigFocusControl:
            case OMX_IndexParamQFactor:
            case OMX_IndexParamQuantizationTable:
            case OMX_IndexParamHuffmanTable:

            /* Video specific parameters and configurations */
            case OMX_IndexVideoStartUnused:
            case OMX_IndexParamVideoPortFormat:
            case OMX_IndexParamVideoQuantization:
            case OMX_IndexParamVideoFastUpdate:
            case OMX_IndexParamVideoBitrate:
            case OMX_IndexParamVideoMotionVector:
            case OMX_IndexParamVideoIntraRefresh:
            case OMX_IndexParamVideoErrorCorrection:
            case OMX_IndexParamVideoVBSMC:
            case OMX_IndexParamVideoMpeg2:
            case OMX_IndexParamVideoMpeg4:
            case OMX_IndexParamVideoWmv:
            case OMX_IndexParamVideoRv:
            case OMX_IndexParamVideoAvc:
            case OMX_IndexParamVideoH263:

            /* Image & Video common Configurations */
            case OMX_IndexCommonStartUnused:
            case OMX_IndexParamCommonDeblocking:
            case OMX_IndexParamCommonSensorMode:
            case OMX_IndexParamCommonInterleave:
            case OMX_IndexConfigCommonColorFormatConversion:
            case OMX_IndexConfigCommonScale:
            case OMX_IndexConfigCommonImageFilter:
            case OMX_IndexConfigCommonColorEnhancement:
            case OMX_IndexConfigCommonColorKey:
            case OMX_IndexConfigCommonColorBlend:
            case OMX_IndexConfigCommonFrameStabilisation:
            case OMX_IndexConfigCommonRotate:
            case OMX_IndexConfigCommonMirror:
            case OMX_IndexConfigCommonOutputPosition:
            case OMX_IndexConfigCommonInputCrop:
            case OMX_IndexConfigCommonOutputCrop:
            case OMX_IndexConfigCommonDigitalZoom:
            case OMX_IndexConfigCommonOpticalZoom:
            case OMX_IndexConfigCommonWhiteBalance:
            case OMX_IndexConfigCommonExposure:
            case OMX_IndexConfigCommonContrast:
            case OMX_IndexConfigCommonBrightness:
            case OMX_IndexConfigCommonBacklight:
            case OMX_IndexConfigCommonGamma:
            case OMX_IndexConfigCommonSaturation:
            case OMX_IndexConfigCommonLightness:
            case OMX_IndexConfigCommonExclusionRect:
            case OMX_IndexConfigCommonDithering:
            case OMX_IndexConfigCommonPlaneBlend:

            /* Reserved Configuration range */
            case OMX_IndexOtherStartUnused:
            case OMX_IndexParamOtherPortFormat:
            case OMX_IndexConfigOtherPower:
            case OMX_IndexConfigOtherStats:

            /* Reserved Time range */
            case OMX_IndexTimeStartUnused:
            case OMX_IndexConfigTimeScale:
            case OMX_IndexConfigTimeClockState:
            case OMX_IndexConfigTimeActiveRefClock:
            case OMX_IndexConfigTimeCurrentMediaTime:
            case OMX_IndexConfigTimeCurrentWallTime:
            case OMX_IndexConfigTimeCurrentAudioReference:
            case OMX_IndexConfigTimeCurrentVideoReference:
            case OMX_IndexConfigTimeMediaTimeRequest:
            case OMX_IndexConfigTimeClientStartTime:
            case OMX_IndexConfigTimePosition:
            case OMX_IndexConfigTimeSeekMode:

            /* Vendor specific area */
#ifdef KHRONOS_1_2
            case OMX_IndexVendorStartUnused:
#else
            case OMX_IndexIndexVendorStartUnused:
#endif
            /*Vendor specific structures should be in the range of 0xFF000000*/
            /*to 0xFFFFFFFF.  This range is not broken out by vendor, so     */
            /*private indexes are not guaranteed unique and therefore should */
            /*only be sent to the appropriate component.                     */
            case OMX_IndexMax:
                eError = OMX_ErrorUnsupportedIndex;
            break;
        }
    }

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_GetState								     	     */
/*                                                                           */
/*  Description   : This Function is used to confirm state change request	 */
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

static OMX_ERRORTYPE VIDDEC_GetState (OMX_HANDLETYPE pComponent,
                                      OMX_STATETYPE* pState)
{
    OMX_ERRORTYPE             eError                        = OMX_ErrorUndefined;
    OMX_U32                   nPendingStateChangeRequests   = 0;
    OMX_COMPONENTTYPE*        pHandle                       = NULL;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate             = 0;
    struct                    timespec abs_time             = {0,0};
    int                       ret                           = 0;
    /*           Set to sufficiently high value                              */
    int mutex_timeout                           = 3;

    LOG_API("+++Entering\n");
    OMX_CONF_CHECK_CMD(pComponent, OMX_TRUE, OMX_TRUE);

    pHandle = (OMX_COMPONENTTYPE*)pComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    if (pState == NULL)
    {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    /*Synchronous implementation of getState                                 */
    /*Check for any pending state transition requests                        */
    if(pthread_mutex_lock(&pComponentPrivate->mutexStateChangeRequest))
    {
        return OMX_ErrorUndefined;
    }
    nPendingStateChangeRequests = pComponentPrivate->
    					nPendingStateChangeRequests;
    if(!nPendingStateChangeRequests)
    {
        if(pthread_mutex_unlock(&pComponentPrivate->mutexStateChangeRequest))
        {
            return OMX_ErrorUndefined;
        }
        /* No pending state transitions                                      */
    *pState    = pComponentPrivate->eState;
        eError = OMX_ErrorNone;
    }
    else {
        /*       Wait for component to complete state transition             */
        clock_gettime(CLOCK_REALTIME, &abs_time);
        abs_time.tv_sec += mutex_timeout;
        abs_time.tv_nsec = 0;

        ret = pthread_cond_timedwait(&(pComponentPrivate->StateChangeCondition),
        								&(pComponentPrivate->
        								mutexStateChangeRequest), &abs_time);
        if (!ret)
        {
            /*           Component has completed state transitions           */
            *pState = pComponentPrivate->eState;
            if(pthread_mutex_unlock(&pComponentPrivate->mutexStateChangeRequest))
            {
                 return OMX_ErrorUndefined;
            }
            eError = OMX_ErrorNone;
        }
        else if(ETIMEDOUT == ret)
        {
             /*            Unlock mutex in case of timeout                   */
             pthread_mutex_unlock(&pComponentPrivate->mutexStateChangeRequest);
             return OMX_ErrorTimeout;
        }
     }

    pHandle = NULL;
EXIT:
	LOG_API("---Exiting\n");
    return eError;
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_EmptyThisBuffer								     */
/*                                                                           */
/*  Description   : This Function Empties Input Buffer.						 */
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
static OMX_ERRORTYPE VIDDEC_EmptyThisBuffer (OMX_HANDLETYPE        pComponent,
                                             OMX_BUFFERHEADERTYPE* pBuffHead)
{
    OMX_ERRORTYPE             eError             = OMX_ErrorNone;
    OMX_COMPONENTTYPE         *pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate = NULL;
    VIDDEC_BUFFER_PRIVATE     *pBufferPrivate    = NULL;
    OMX_S32                   ret                = 0;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit            = NULL;

    OMX_CONF_CHECK_CMD(pComponent, OMX_TRUE, OMX_TRUE);

    pHandle           = (OMX_COMPONENTTYPE *)pComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    pSplit            = pComponentPrivate->pSplit;

	LOG_API("+++Entering\n");
	if(pBuffHead->nFlags & OMX_BUFFERFLAG_EOS)
	{
		pSplit->bEosReceivedAtInput = OMX_TRUE;
	}

    if(!pComponentPrivate->pInPortDef->bEnabled)
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);

    if(pBuffHead->nInputPortIndex != VIDDEC_INPUT_PORT)
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorBadPortIndex);

    if(pComponentPrivate->eState != OMX_StateExecuting &&
        pComponentPrivate->eState != OMX_StatePause &&
        pComponentPrivate->eState != OMX_StateIdle)
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);

    OMX_CONF_CHK_VERSION(pBuffHead, OMX_BUFFERHEADERTYPE,
    							 eError, pComponentPrivate->dbg);

    if ((pComponentPrivate->bParserEnabled == OMX_FALSE) &&
        (pComponentPrivate->bFirstHeader == OMX_FALSE) &&
        (pBuffHead->nFilledLen > pBuffHead->nAllocLen))
    {
        pBuffHead->nFilledLen = pBuffHead->nAllocLen;
    }

    pBufferPrivate = (VIDDEC_BUFFER_PRIVATE* )pBuffHead->pInputPortPrivate;
    ret = pBufferPrivate->eBufferOwner;
    pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;

    pthread_mutex_lock(&pComponentPrivate->InputBCountAppMutex);
    pComponentPrivate->bInputBCountApp++;
    pthread_mutex_unlock(&pComponentPrivate->InputBCountAppMutex);

    LOG_API("pBuffer %p Index %lu nAllocLen %lu nFilledLen %lu eOldBufOwner %lu eNewBufOwner %u EOS = %lu",
    			pBuffHead, pBuffHead->nInputPortIndex, pBuffHead->nAllocLen,
   			    pBuffHead->nFilledLen, ret, pBufferPrivate->eBufferOwner,
    			pBuffHead->nFlags & OMX_BUFFERFLAG_EOS );

    LOG_API("ETB: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
                pComponentPrivate->bInputBCountApp,
                pComponentPrivate->bOutputBCountApp,
                pComponentPrivate->bInputBCountDsp,
                pComponentPrivate->bOutputBCountDsp);

    eError = write_to_asc_pipe(pComponentPrivate->pSplit, pBuffHead);

    if (eError != OMX_ErrorNone)
    {
        LOG_API( "Error in Writing to the Data pipe\n");
        eError = OMX_ErrorHardware;
        goto EXIT;
    }

EXIT:
    LOG_API( "---Exiting 0x%x\n",eError);
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_FillThisBuffer   							     */
/*                                                                           */
/*  Description   : This Function Fills the Output Buffer.					 */
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
static OMX_ERRORTYPE VIDDEC_FillThisBuffer (OMX_HANDLETYPE        pComponent,
                                            OMX_BUFFERHEADERTYPE* pBuffHead)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    VIDDEC_BUFFER_PRIVATE*    pBufferPrivate    = NULL;
    int                       ret               = 0;
    OMX_CONF_CHECK_CMD(pComponent, pBuffHead, OMX_TRUE);

//sasken start
	IMG_native_handle_t*  grallocHandle;
	OMX_PARAM_PORTDEFINITIONTYPE sPortDef;
//sasken stop

    pHandle           = (OMX_COMPONENTTYPE *)pComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

	LOG_API("+++Entering\n");
#ifdef __PERF_INSTRUMENTATION__
    PERF_ReceivedFrame(pComponentPrivate->pPERF,
                       pBuffHead->pBuffer,
                       0,
                       PERF_ModuleHLMM);
#endif


    if(!pComponentPrivate->pOutPortDef->bEnabled)
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);

    if(pBuffHead->nOutputPortIndex != 0x1)
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorBadPortIndex);

    if(pComponentPrivate->eState != OMX_StateExecuting &&
        pComponentPrivate->eState != OMX_StatePause &&
        pComponentPrivate->eState != OMX_StateIdle)
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorIncorrectStateOperation);

    OMX_CONF_CHK_VERSION(pBuffHead, OMX_BUFFERHEADERTYPE, eError,
    						 pComponentPrivate->dbg);

    if ((pComponentPrivate->bParserEnabled == OMX_FALSE) &&
        (pComponentPrivate->bFirstHeader == OMX_FALSE) &&
        (pBuffHead->nFilledLen > pBuffHead->nAllocLen))
    {
        LOG_API("bFirstHeader: nFilledLen= %lu <- nAllocLen= %lu\n",
       					 pBuffHead->nFilledLen, pBuffHead->nAllocLen);
        pBuffHead->nFilledLen = pBuffHead->nAllocLen;
    }

//sasken start
if(pComponentPrivate->pCompPort[OMX_VIDEODECODER_OUTPUT_PORT]->VIDDECBufferType
			== GrallocPointers)
	{
		/* Lock the Gralloc buffer till it gets rendered completely */
		/* Extract the Gralloc handle from the Header and then call lock on that */
		/* Note# There is no error check for the pBufferHdr here*/
		grallocHandle = (IMG_native_handle_t*)pBuffHead->pBuffer;
		/*Initializing Structure */
		sPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		sPortDef.nVersion.s.nVersionMajor = OMX_VER_MAJOR;
		sPortDef.nVersion.s.nVersionMinor = OMX_VER_MINOR;
		sPortDef.nVersion.s.nRevision = 0x0;
		sPortDef.nVersion.s.nStep = 0x0;
		sPortDef.nPortIndex = OMX_VIDEODECODER_INPUT_PORT;
		eError = VIDDEC_GetParameter(pHandle,OMX_IndexParamPortDefinition,
				&sPortDef);

		pComponentPrivate->grallocModule->lock((gralloc_module_t const *) pComponentPrivate->grallocModule,
				(buffer_handle_t)grallocHandle, GRALLOC_USAGE_HW_RENDER,
				0,0,sPortDef.format.video.nFrameWidth, sPortDef.format.video.nFrameHeight,NULL);
	}
//sasken stop

		pBufferPrivate = (VIDDEC_BUFFER_PRIVATE* )pBuffHead->pOutputPortPrivate;
    ret = pBufferPrivate->eBufferOwner;
    pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_COMPONENT;

    pthread_mutex_lock(&pComponentPrivate->OutputBCountAppMutex);
    pComponentPrivate->bOutputBCountApp++;
    pthread_mutex_unlock(&pComponentPrivate->OutputBCountAppMutex);

    pBuffHead->nFilledLen = 0;

    LOG_BUF_META("ETB: pBuffHead %p Index %lu nAllocLen %lu nFilledLen %lu nFlags= %lu",
    			pBuffHead, pBuffHead->nInputPortIndex, pBuffHead->nAllocLen,
			pBuffHead->nFilledLen, pBuffHead->nFlags);

    LOG_BUF_COUNT("FTB: InAppCnt = %d, outAppCnt = %d, InDspCnt = %d, outDspCnt = %d,  \n",
       pComponentPrivate->bInputBCountApp, pComponentPrivate->bOutputBCountApp,
       pComponentPrivate->bInputBCountDsp, pComponentPrivate->bOutputBCountDsp);

    ret = write (pComponentPrivate->free_outBuf_Q[1],
    								&(pBuffHead), sizeof (pBuffHead));
    if (ret == -1)
    {
        LOG_API("Error in Writing to the Data pipe\n");
        eError = OMX_ErrorHardware;
        goto EXIT;
    }

EXIT:
    LOG_API( "---Exiting 0x%x\n",eError);
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_ComponentDeinit   							     */
/*                                                                           */
/*  Description   : This Function Deinitialize the Component				 */
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

static OMX_ERRORTYPE VIDDEC_ComponentDeInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE             eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*        pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_COMMANDTYPE           Cmd               = OMX_CommandStateSet;
    OMX_U32                   nParam1           = -1;
    OMX_U32                   buffcount         = 0;
    OMX_U32                   i                 = 0;
    OMX_U32                   iCount            = 0;

    OMX_CONF_CHECK_CMD(hComponent, OMX_TRUE, OMX_TRUE);

    pHandle           = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	LOG_API("+++Entering\n");
	ALOGD("%d ::: In %s ***", __LINE__, __FUNCTION__);
#ifdef __PERF_INSTRUMENTATION__
    PERF_Boundary(pComponentPrivate->pPERF,
                  PERF_BoundaryStart | PERF_BoundaryCleanup);
    PERF_SendingCommand(pComponentPrivate->pPERF,
                        Cmd, nParam1, PERF_ModuleComponent);
#endif
        VIDDEC_LCML_Destroy(pComponentPrivate);

    eError = write(pComponentPrivate->cmdPipe[VIDDEC_PIPE_WRITE],
    						&Cmd, sizeof(Cmd));
    if (eError == -1)
    {
        eError = OMX_ErrorUndefined;
        goto EXIT;
    }
    eError = write(pComponentPrivate->cmdDataPipe[VIDDEC_PIPE_WRITE],
    					&nParam1, sizeof(nParam1));
    if (eError == -1)
    {
       eError = OMX_ErrorUndefined;
       goto EXIT;
    }

    eError = VIDDEC_Stop_ComponentThread(pHandle);
    if (eError != OMX_ErrorNone)
    {
        LOG_API("Error returned from the Component\n");
    }

    for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
    {
        if(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
        			pBufferPrivate[iCount]->bAllocByComponent == OMX_TRUE)
        {
            if(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
            		pBufferPrivate[iCount]->pBufferHdr != NULL)
            {
                OMX_BUFFERHEADERTYPE *pBuffHead = NULL;
                OMX_U8               *pTemp     = NULL;
                pBuffHead = pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
                			pBufferPrivate[iCount]->pBufferHdr;
                if(pBuffHead->pBuffer != NULL)
                {
                    pTemp = (OMX_U8*)(pBuffHead->pBuffer);
                    pBuffHead->pBuffer = (OMX_U8*)pTemp;
                    #if !defined(DSPALIGN_128BYTE)
                    /*The aligned free macro needs the structure type.       */
                    /*This is deprecated after inc 2.5 onwards               */
                    /*TODO:delete this after migration to inc 2.5 is complete*/
                    OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer,
                    									OMX_BUFFERHEADERTYPE);
                    #else
                    OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer, NULL);
                    #endif /*defined(DSPALIGN_128BYTE) */
                    pBuffHead->pBuffer = NULL;
                    pTemp = NULL;
                }
            }
        }
    }

    for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
    {
        if(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
        		pBufferPrivate[iCount]->bAllocByComponent == OMX_TRUE)
        {
            if(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
            			pBufferPrivate[iCount]->pBufferHdr != NULL)
             {
                OMX_BUFFERHEADERTYPE* pBuffHead = NULL;
                OMX_U8*               pTemp     = NULL;
                pBuffHead = pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
                			pBufferPrivate[iCount]->pBufferHdr;
                if(pBuffHead->pBuffer != NULL)
                {
                    pTemp = (OMX_U8*)(pBuffHead->pBuffer);
                    pBuffHead->pBuffer = (OMX_U8*)pTemp;
                    #if !defined(DSPALIGN_128BYTE)
                    /*The aligned free macro needs the structure type.       */
                    /*This is deprecated after inc 2.5 onwards               */
                    /*TODO:delete this after migration to inc 2.5 is complete*/
                    OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer,
                    						OMX_BUFFERHEADERTYPE);
                    #else
                    OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer, NULL);
                    #endif //defined(DSPALIGN_128BYTE)
                    pBuffHead->pBuffer = NULL;
                    pTemp = NULL;
                }
            }
        }
    }

/* During DeInit, check if ION mapped output buffers are freed */
{
    OMX_U32 i;
    OMX_U32 buffcount;
    VIDDEC_PORT_TYPE* pCompPort = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL;
	OMX_BUFFERHEADERTYPE* pBuffHead = NULL;

    pCompPort = pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT];
    pPortDef = pComponentPrivate->pOutPortDef;

    buffcount = pPortDef->nBufferCountActual;
    ALOGD("%s %d Output buffcount = %d", __func__, __LINE__, buffcount);

    for(i = 0; i < buffcount; i++)
    {
		if(0 != pCompPort->pBufferPrivate[i]->pBufferHdr)
		{
			pBuffHead = (OMX_BUFFERHEADERTYPE*)pCompPort->pBufferPrivate[i]->pBufferHdr;
#ifdef USE_ION
	    if(pComponentPrivate->pCompPort[OMX_VIDEODECODER_OUTPUT_PORT]->VIDDECBufferType == GrallocPointers)
	    {
	    	if(pComponentPrivate->bUseIon == OMX_TRUE)
			{
	    		if(pComponentPrivate->bMapIonBuffers == OMX_TRUE)
				{
					ALOGD("%s %d pBuffHead->pPlatformPrivate = %p, pBuffHead->nAllocLen = %d updated to i",
					    __func__, __LINE__, pBuffHead->pPlatformPrivate, pBuffHead->nAllocLen);
					munmap(pBuffHead->pPlatformPrivate, pBuffHead->nAllocLen);
	               	close(pComponentPrivate->mmap_fd[i]);
	    		}
	    	}
	    }
#endif
		}
	}
}

//sasken start
   if(pComponentPrivate->ion_fd >= 0)
	{
           ion_close(pComponentPrivate->ion_fd);
	}
//sasken stop

#ifdef RESOURCE_MANAGER_ENABLED
	eError = VIDDEC_RM_free_resources(pComponentPrivate);
#endif

    VIDDEC_CircBuf_DeInit(pComponentPrivate, VIDDEC_CBUFFER_TIMESTAMP,
    													VIDDEC_INPUT_PORT);
    /*                    Free Resources                                    */
    if(pComponentPrivate->pPortParamType)
    {
        free(pComponentPrivate->pPortParamType);
        pComponentPrivate->pPortParamType = NULL;
    }
#ifdef __STD_COMPONENT__
    if(pComponentPrivate->pPortParamTypeAudio)
    {
        free(pComponentPrivate->pPortParamTypeAudio);
        pComponentPrivate->pPortParamTypeAudio = NULL;
    }
    if(pComponentPrivate->pPortParamTypeImage)
    {
        free(pComponentPrivate->pPortParamTypeImage);
        pComponentPrivate->pPortParamTypeImage = NULL;
    }
    if(pComponentPrivate->pPortParamTypeOthers)
    {
        free(pComponentPrivate->pPortParamTypeOthers);
        pComponentPrivate->pPortParamTypeOthers = NULL;
    }
#endif

    buffcount = MAX_PRIVATE_BUFFERS;
    for (i = 0; i < buffcount; i++)
    {
        if(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->pBufferPrivate[i])
        {
            LOG_API(
            							"BufferPrivate cleared 0x%p\n",
                    pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
                    			pBufferPrivate[i]);
            free(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
            											pBufferPrivate[i]);
            pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]->
            									pBufferPrivate[i] = NULL;
        }
    }

    buffcount = MAX_PRIVATE_BUFFERS;
    for (i = 0; i < buffcount; i++)
    {
        if(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[i])
        {
            LOG_API(
            							"BufferPrivate cleared 0x%p\n",
                    pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
                    	pBufferPrivate[i]);
            free(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
            				pBufferPrivate[i]);
            pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
            									pBufferPrivate[i] = NULL;
        }
    }
    if(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT])
    {
        free(pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]);
        pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT] = NULL;
    }

    if(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT])
    {
        free(pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT]);
        pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT] = NULL;
    }

    if(pComponentPrivate->pInPortDef)
    {
        free(pComponentPrivate->pInPortDef);
        pComponentPrivate->pInPortDef     = NULL;
    }
    if(pComponentPrivate->pOutPortDef)
    {
        free(pComponentPrivate->pOutPortDef);
        pComponentPrivate->pOutPortDef    = NULL;
    }
    if(pComponentPrivate->pInPortFormat)
    {
        free(pComponentPrivate->pInPortFormat);
        pComponentPrivate->pInPortFormat  = NULL;
    }
    if(pComponentPrivate->pOutPortFormat)
    {
        free(pComponentPrivate->pOutPortFormat);
        pComponentPrivate->pOutPortFormat  = NULL;
    }
    if(pComponentPrivate->pPriorityMgmt)
    {
        free(pComponentPrivate->pPriorityMgmt);
        pComponentPrivate->pPriorityMgmt    = NULL;
    }
    if(pComponentPrivate->pInBufSupplier)
    {
        free(pComponentPrivate->pInBufSupplier);
        pComponentPrivate->pInBufSupplier    = NULL;
    }
    if(pComponentPrivate->pOutBufSupplier)
    {
        free(pComponentPrivate->pOutBufSupplier);
        pComponentPrivate->pOutBufSupplier   = NULL;
    }
    if(pComponentPrivate->pMpeg4 != NULL)
    {
        free(pComponentPrivate->pMpeg4);
        pComponentPrivate->pMpeg4            = NULL;
    }
    if(pComponentPrivate->pH264 != NULL)
    {
        free(pComponentPrivate->pH264);
        pComponentPrivate->pH264             = NULL;
    }
    if(pComponentPrivate->pH263 != NULL)
    {
        free(pComponentPrivate->pH263);
        pComponentPrivate->pH263             = NULL;
    }
    if(pComponentPrivate->pWMV != NULL)
    {
        free(pComponentPrivate->pWMV);
        pComponentPrivate->pWMV              = NULL;
    }
    if(pComponentPrivate->pDeblockingParamType != NULL)
    {
        free(pComponentPrivate->pDeblockingParamType);
        pComponentPrivate->pDeblockingParamType = NULL;
    }
#ifdef ANDROID
    if(pComponentPrivate->pPVCapabilityFlags != NULL)
    {
        free(pComponentPrivate->pPVCapabilityFlags);
        pComponentPrivate->pPVCapabilityFlags   = NULL;
    }
#endif
    if(pComponentPrivate->cComponentName != NULL)
    {
        free(pComponentPrivate->cComponentName);
        pComponentPrivate->cComponentName       = NULL;
    }


    if(pComponentPrivate->pSplit != NULL)
    {
		OMX_TRACE1(pComponentPrivate->dbg,
								"Split: Freeing up pSplit structure\n");
		free(pComponentPrivate->pSplit);
		OMX_TRACE1(pComponentPrivate->dbg, "Split: Freeing up done\n");
		pComponentPrivate->pSplit = NULL;
    }

    pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]  = 0;
    pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel1]  = 0;
    pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel2]  = 0;
    pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel3]  = 0;
    pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel4]  = 0;

#ifdef __PERF_INSTRUMENTATION__
    PERF_Boundary(pComponentPrivate->pPERF,
                  PERF_BoundaryComplete | PERF_BoundaryCleanup);
    PERF_Done(pComponentPrivate->pPERF);
#endif


    OMX_DBG_CLOSE(pComponentPrivate->dbg);

    VIDDEC_PTHREAD_MUTEX_DESTROY(pComponentPrivate->sMutex);
    VIDDEC_PTHREAD_SEMAPHORE_DESTROY(pComponentPrivate->sInSemaphore);
    VIDDEC_PTHREAD_SEMAPHORE_DESTROY(pComponentPrivate->sOutSemaphore);

    if(pHandle->pComponentPrivate != NULL)
    {
        free(pHandle->pComponentPrivate);
        pHandle->pComponentPrivate = NULL;
        pComponentPrivate = NULL;
    }
EXIT:
    LOG_API("---Exiting\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_UseBuffer        							     */
/*                                                                           */
/*  Description   : This Function Issues Buffer to the component.			 */
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
static OMX_ERRORTYPE VIDDEC_UseBuffer(OMX_IN OMX_HANDLETYPE     hComponent,
                               OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                               OMX_IN OMX_U32                   nPortIndex,
                               OMX_IN OMX_PTR                   pAppPrivate,
                               OMX_IN OMX_U32                   nSizeBytes,
                               OMX_IN OMX_U8*                   pBuffer)
{
    OMX_ERRORTYPE                 eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*            pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE*     pComponentPrivate = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef          = NULL;
    VIDDEC_PORT_TYPE*             pCompPort         = NULL;
    OMX_U8                        pBufferCnt        = 0;
    OMX_CONF_CHECK_CMD(hComponent, pBuffer, OMX_TRUE);


    pHandle           = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

//sasken start
#ifdef  ENABLE_GRALLOC_BUFFERS
	OMX_PTR buff_t;
	struct ion_map_gralloc_to_ionhandle_data data;
#endif
//sasken stop

	LOG_API("+++Entering\n");
#ifdef __PERF_INSTRUMENTATION__
    PERF_ReceivedFrame(pComponentPrivate->pPERF,
                       pBuffer,
                       nSizeBytes,
                       PERF_ModuleHLMM);
#endif

    if (nPortIndex == pComponentPrivate->pInPortFormat->nPortIndex)
    {
        pCompPort = pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT];
        pBufferCnt = pCompPort->nBufferCnt;
        pPortDef = pComponentPrivate->pInPortDef;
        LOG_API(
        					"OMX_UseBuffer >> INPUT Buf 0x%x nSizeBytes %lu\n",
            					(unsigned int)pBuffer, nSizeBytes);
    }
    else if (nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
    {
        pCompPort = pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT];
        pBufferCnt = pCompPort->nBufferCnt;
        pPortDef = pComponentPrivate->pOutPortDef;
        LOG_API(
        				"OMX_UseBuffer >> OUTPUT Buf 0x%x nSizeBytes %lu\n",
           				 (unsigned int)pBuffer, nSizeBytes);
    }
    else
    {
        eError = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    if(!pPortDef->bEnabled)
    {
        LOG_API( "Error: port disabled\n");
        eError = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    LOG_API(
    					"pPortDef->nBufferSize %d nSizeBytes %d %d\n",
    						(int )pPortDef->nBufferSize,
        (		int )nSizeBytes,(int )(pPortDef->nBufferSize > nSizeBytes));
   							 pPortDef->nBufferSize = nSizeBytes;
    if(nSizeBytes != pPortDef->nBufferSize || pPortDef->bPopulated)
    {
        OMX_PRBUFFER4(pComponentPrivate->dbg, "Error: badparameter\n");
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    OMX_MALLOC_STRUCT(pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr,
    						OMX_BUFFERHEADERTYPE,pComponentPrivate->
    							nMemUsage[VIDDDEC_Enum_MemLevel1]);
    if (!pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr)
    {
        LOG_API("Error: Malloc failed\n");
        eError = OMX_ErrorInsufficientResources;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                            pComponentPrivate->pHandle->pApplicationPrivate,
                            OMX_EventError,eError,OMX_TI_ErrorMajor,
                             NULL);
        goto EXIT;
    }

    *ppBufferHdr = pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr;
    memset(*ppBufferHdr, 0, sizeof(OMX_BUFFERHEADERTYPE));
    OMX_CONF_INIT_STRUCT(pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr,
    						OMX_BUFFERHEADERTYPE, pComponentPrivate->dbg);

    (*ppBufferHdr)->pBuffer     = pBuffer;
    (*ppBufferHdr)->nAllocLen   = nSizeBytes;
    (*ppBufferHdr)->pAppPrivate = pAppPrivate;
    (*ppBufferHdr)->pMarkData   = NULL;

//sasken start
#ifdef  ENABLE_GRALLOC_BUFFERS

    if (nPortIndex == VIDDEC_OUTPUT_PORT) {

		if(pComponentPrivate->pCompPort[1]->VIDDECBufferType == GrallocPointers) {
			data.gralloc_handle = ((IMG_native_handle_t*)(*ppBufferHdr)->pBuffer)->fd[0];

                   
        if (ion_ioctl(pComponentPrivate->ion_fd, ION_IOC_MAP_GRALLOC, &data)) {
				ALOGV("ion_ioctl fail");
			}

	if (ion_map(pComponentPrivate->ion_fd, data.handleY, (*ppBufferHdr)->nAllocLen, PROT_READ | PROT_WRITE,
									MAP_SHARED, 0, &buff_t, &pComponentPrivate->mmap_fd[pBufferCnt]) < 0) {
						ALOGV("ION map failed");
		}

		(*ppBufferHdr)->pPlatformPrivate = buff_t;
		ALOGD("%s %d ION map done at 0x%x", __func__, __LINE__, (*ppBufferHdr)->pPlatformPrivate);
		}
    }
#endif
//sasken stop

	if (pCompPort->hTunnelComponent != NULL)
    {
        if (pPortDef->eDir == OMX_DirInput)
        {
            (*ppBufferHdr)->nInputPortIndex  = nPortIndex;
            (*ppBufferHdr)->nOutputPortIndex =
            			pComponentPrivate->pCompPort[nPortIndex]->nTunnelPort;
        }
        else
        {
            (*ppBufferHdr)->nInputPortIndex  =
            			pComponentPrivate->pCompPort[nPortIndex]->nTunnelPort;
            (*ppBufferHdr)->nOutputPortIndex = nPortIndex;
        }
    }
    else
    {
        if (nPortIndex == VIDDEC_INPUT_PORT)
        {
            (*ppBufferHdr)->nInputPortIndex  = VIDDEC_INPUT_PORT;
            (*ppBufferHdr)->nOutputPortIndex = VIDDEC_NOPORT;
        }
        else
        {
            (*ppBufferHdr)->nInputPortIndex  = VIDDEC_NOPORT;
            (*ppBufferHdr)->nOutputPortIndex = VIDDEC_OUTPUT_PORT;
        }
    }

    if (nPortIndex == VIDDEC_INPUT_PORT)
    {
        (*ppBufferHdr)->pInputPortPrivate =
        				pComponentPrivate->pCompPort[nPortIndex]->
        											pBufferPrivate[pBufferCnt];
    }
    else
    {
        (*ppBufferHdr)->pOutputPortPrivate =
        			pComponentPrivate->pCompPort[nPortIndex]->
        										pBufferPrivate[pBufferCnt];
    }
    pComponentPrivate->pCompPort[nPortIndex]->pBufferPrivate[pBufferCnt]->
    											bAllocByComponent = OMX_FALSE;

    if (pCompPort->hTunnelComponent != NULL)
    {
        pComponentPrivate->pCompPort[nPortIndex]->
        					pBufferPrivate[pBufferCnt]->eBufferOwner =
        									VIDDEC_BUFFER_WITH_TUNNELEDCOMP;
    }
    else
    {
        pComponentPrivate->pCompPort[nPortIndex]->
        				pBufferPrivate[pBufferCnt]->eBufferOwner =
        											VIDDEC_BUFFER_WITH_CLIENT;
    }

    LOG_API(
    			"ppBufferHdr 0x%p pBuffer 0x%p nAllocLen %lu eBufferOwner %d\n",
       			 *ppBufferHdr,
        		pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr->pBuffer,
        		(*ppBufferHdr)->nAllocLen,
        		pCompPort->pBufferPrivate[pBufferCnt]->eBufferOwner);

    eError = VIDDEC_Allocate_DSPResources(pComponentPrivate, nPortIndex);
    if (eError != OMX_ErrorNone)
    {
        eError = OMX_ErrorInsufficientResources;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,

                           pComponentPrivate->pHandle->pApplicationPrivate,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               NULL);
        goto EXIT;
    }

    pCompPort->nBufferCnt++;

    if (pCompPort->nBufferCnt == pPortDef->nBufferCountActual)
    {
        pPortDef->bPopulated = OMX_TRUE;

        if (nPortIndex == VIDDEC_INPUT_PORT)
        {
            VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sInSemaphore);
        }
        else
        {
            VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sOutSemaphore);
        }
    }
EXIT:
    LOG_API( "---Exiting eError 0x%x\n", eError);
    LOG_API("---Exiting\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_FreeBuffer        							     */
/*                                                                           */
/*  Description   : This Function Frees the buffer after its use.			 */
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
static OMX_ERRORTYPE VIDDEC_FreeBuffer (OMX_IN OMX_HANDLETYPE        hComponent,
                                        OMX_IN OMX_U32              nPortIndex,
                                        OMX_IN OMX_BUFFERHEADERTYPE* pBuffHead)
{
    OMX_ERRORTYPE                 eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*            pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE*     pComponentPrivate = NULL;
    VIDDEC_BUFFER_PRIVATE*        pBufferPrivate    = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn        = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut       = NULL;
    VIDDEC_PORT_TYPE*             pCompPort         = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef          = NULL;
    OMX_U8*                       pTemp             = NULL;
    OMX_U32                       i                 = 0;
    VIDDEC_PORT_TYPE*             pInCompPort       = NULL;
    OMX_U8                        pInBufferCnt      = 0;
    VIDDEC_PORT_TYPE*             pOutCompPort      = NULL;
    OMX_U8                        pOutBufferCnt     = 0;
    OMX_U32                       buffcount         = 0;
    OMX_STATETYPE                 TunnelState       = OMX_StateInvalid;
    OMX_BOOL                      bTransIdle        = OMX_FALSE;

    OMX_CONF_CHECK_CMD(hComponent, pBuffHead, OMX_TRUE);

    pHandle = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

    LOG_API("+++Entering\n");
    ALOGD(
    "+++Entering pHandle 0x%p pBuffHead 0x%p nPortIndex %lu nFilledLen %lx nAllocLen %lx\n",
        hComponent, pBuffHead, nPortIndex,
        				pBuffHead->nFilledLen,pBuffHead->nAllocLen);

    pPortDefIn = pComponentPrivate->pInPortDef;
    pPortDefOut = pComponentPrivate->pOutPortDef;

    pInCompPort = pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT];
    pInBufferCnt = pInCompPort->nBufferCnt;
    pOutCompPort = pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT];
    pOutBufferCnt = pOutCompPort->nBufferCnt;

    if (nPortIndex == pComponentPrivate->pInPortFormat->nPortIndex)
    {
        pPortDef = pComponentPrivate->pInPortDef;
        pCompPort = pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT];
        pBufferPrivate = pBuffHead->pInputPortPrivate;
    }
    else if (nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
    {
        pPortDef = pComponentPrivate->pOutPortDef;
        pCompPort = pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT];
        pBufferPrivate = pBuffHead->pOutputPortPrivate;
    }
    else
    {
        eError = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

// was here



    if (pPortDefIn->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
        pPortDefIn->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC ||
        pPortDefIn->format.video.eCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        if (nPortIndex == pComponentPrivate->pInPortFormat->nPortIndex)
        {
            VIDDEC_free_SN_InputArgument(pComponentPrivate, pBufferPrivate);
        }
        else if (nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
        {
            pTemp = (OMX_U8*)(pBufferPrivate->pUalgParam);
            pBufferPrivate->pUalgParam = (OMX_PTR*)pTemp;
            #if !defined(DSPALIGN_128BYTE)
            /*The aligned free macro needs the structure type.               */
            /*This is deprecated after inc 2.5 onwards                       */
            /*TODO:delete this after migration to inc 2.5 is complete        */
            OMX_MEMFREE_STRUCT_DSPALIGN(pBufferPrivate->pUalgParam,
            								ITTVDEC_UALGOutputParam_t);
            #else
            OMX_MEMFREE_STRUCT_DSPALIGN(pBufferPrivate->pUalgParam, NULL);
            #endif //defined(DSPALIGN_128BYTE)
            pBufferPrivate->pUalgParam = NULL;
            pBufferPrivate->nUalgParamSize = 0;
        }
        else
        {
            eError = OMX_ErrorBadPortIndex;
            goto EXIT;
        }
    }
    else
    {
        LOG_API(
        					"Error: Invalid Compression Type\n");
        goto EXIT;
    }
    LOG_API(
    					"bAllocByComponent 0x%x pBuffer 0x%p\n",
    							(int )pBufferPrivate->bAllocByComponent,
        							pBuffHead->pBuffer);
    if (pBufferPrivate->bAllocByComponent == OMX_TRUE)
    {
        if(pBuffHead->pBuffer != NULL)
        {
#ifdef __PERF_INSTRUMENTATION__
           PERF_SendingFrame(pComponentPrivate->pPERFcomp,
                             pBuffHead->pBuffer,
                             pBuffHead->nAllocLen,
                             PERF_ModuleMemory);
#endif

           pTemp = (OMX_U8*)(pBuffHead->pBuffer);
#ifdef VIDDEC_WMVPOINTERFIXED
            if (pComponentPrivate->pInPortDef->
            				format.video.eCompressionFormat ==
            						OMX_VIDEO_CodingWMV &&
                					nPortIndex == VIDDEC_INPUT_PORT &&
                						pComponentPrivate->ProcessMode == 0)
            {
                pTemp -= VIDDEC_WMV_BUFFER_OFFSET;
                LOG_API(
                				"Removing extra wmv padding %d pBuffer 0x%p\n",
                				VIDDEC_WMV_BUFFER_OFFSET,
                   					 pTemp);
            }
#endif
           pBuffHead->pBuffer = (OMX_U8*)pTemp;
           #if !defined(DSPALIGN_128BYTE)
           /*The aligned free macro needs the structure type.                */
           /*This is deprecated after inc 2.5 onwards                        */


           /*TODO:delete this after migration to inc 2.5 is complete         */
           OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer, OMX_BUFFERHEADERTYPE);
           #else
           OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer, NULL);
           #endif //defined(DSPALIGN_128BYTE)

           pBuffHead->pBuffer = NULL;
        }
    }

    
// now here
//sasken start
   //if(pComponentPrivate->pCompPort[OMX_VIDEODECODER_OUTPUT_PORT]->VIDDECBufferType == GrallocPointers)
    //{   //work around
    	//munmap(pBuffHead->pPlatformPrivate, pBuffHead->nAllocLen);
   // }
	#ifdef USE_ION
	    if(pComponentPrivate->pCompPort[OMX_VIDEODECODER_OUTPUT_PORT]->VIDDECBufferType == GrallocPointers
	    		&& nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
	    {
	    	if(pComponentPrivate->bUseIon == OMX_TRUE)
		{
	    		if(pComponentPrivate->bMapIonBuffers == OMX_TRUE)
			{
				ALOGD("%s %d pBuffHead->pPlatformPrivate = %p, pBuffHead->nAllocLen = %d updated to (pOutBufferCnt - 1)",
				    __func__, __LINE__, pBuffHead->pPlatformPrivate, pBuffHead->nAllocLen);
				munmap(pBuffHead->pPlatformPrivate, pBuffHead->nAllocLen);
	                	close(pComponentPrivate->mmap_fd[pOutBufferCnt - 1]);
	    }
	    	}
	    }
	#endif	
   
   //sasken stop

    buffcount = pPortDef->nBufferCountActual;
    for(i = 0; i < buffcount; i++){

        /* Checking for NULL pointer to avoid NULL pointer freeing. Not       */
        /*  sure why OpenCore should make a call with NULL pointer for freeing*/
        if ((pCompPort->pBufferPrivate[i]->pBufferHdr == pBuffHead) &&
        				(0 != pBuffHead))
        {
            LOG_API(
            					"buffcount %lu eBufferOwner 0x%x\n",
            						i,
            						pCompPort->pBufferPrivate[i]->eBufferOwner);
            free(pCompPort->pBufferPrivate[i]->pBufferHdr);
            pCompPort->pBufferPrivate[i]->pBufferHdr = NULL;
            pBuffHead = NULL;
        }
    }
    if(pComponentPrivate->
    		pCompPort[VIDDEC_OUTPUT_PORT]->hTunnelComponent != NULL)
    {
        eError = OMX_GetState(pComponentPrivate->
        							pCompPort[VIDDEC_OUTPUT_PORT]->
        									hTunnelComponent,&TunnelState);
        if(eError != OMX_ErrorNone)
        {
            LOG_API( "GetState Invalid return\n");
            goto EXIT;
        }
        OMX_PRCOMM2(pComponentPrivate->dbg, "TunnelState %d\n", TunnelState);
    }
    if((pComponentPrivate->eState == OMX_StateIdle) &&
    		(pComponentPrivate->eIdleToLoad == OMX_StateLoaded))
    {
        bTransIdle = OMX_TRUE;
    }
    else
    {
        bTransIdle = OMX_FALSE;
    }
    if (nPortIndex == pComponentPrivate->pInPortFormat->nPortIndex)
    {
        pInBufferCnt--;
        pInCompPort->nBufferCnt--;
        if (pInBufferCnt == 0)
        {
            pPortDefIn->bPopulated = OMX_FALSE;

            if (nPortIndex == VIDDEC_INPUT_PORT)
            {
                VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sInSemaphore);
            }
            else
            {
                VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sOutSemaphore);
            }

            if (bTransIdle)
            {
                i = 0;
            }
            else if ((!pPortDef->bEnabled &&
            				(pComponentPrivate->eState == OMX_StateIdle ||
                      pComponentPrivate->eState == OMX_StateExecuting
                      || pComponentPrivate->eState == OMX_StatePause)))
            {
                i = 0;
            }
            else
            {
                if (pComponentPrivate->eState != OMX_StateInvalid)
                {
                pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->
                										pHandle,
                                                        pComponentPrivate->
                                                        pHandle->
                                                        pApplicationPrivate,
                                                        OMX_EventError,
                                                        OMX_ErrorPortUnpopulated,
                                                        OMX_TI_ErrorMinor,
                                                        "Input Port Unpopulated");
                }
            }
        }
    }

    else if (nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
    {
        pOutBufferCnt--;
        pOutCompPort->nBufferCnt--;
        if (pOutBufferCnt == 0)
        {
            pPortDefOut->bPopulated = OMX_FALSE;

            if (nPortIndex == VIDDEC_INPUT_PORT)
            {
                VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sInSemaphore);
            }
            else
            {
                VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sOutSemaphore);
            }

            if ((pComponentPrivate->
            		pCompPort[VIDDEC_OUTPUT_PORT]->hTunnelComponent == NULL) &&
            						bTransIdle)
            {
                i = 0;
            }
            else if ((pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT]->
            				hTunnelComponent != NULL) &&
            						(TunnelState == OMX_StateIdle))
            {
                i = 0;
            }
            else if ((!pPortDef->bEnabled &&
            				(pComponentPrivate->eState == OMX_StateIdle ||
                     		 pComponentPrivate->eState == OMX_StateExecuting
                     		 || pComponentPrivate->eState == OMX_StatePause)))
            {
                i = 0;
            }
            else
            {
                if (pComponentPrivate->eState != OMX_StateInvalid)
                {
                pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->
                										pHandle,
                                                        pComponentPrivate->
                                                        pHandle->
                                                        pApplicationPrivate,
                                                        OMX_EventError,
                                                        OMX_ErrorPortUnpopulated,
                                                        OMX_TI_ErrorMinor,
                                                        "Output Port Unpopulated");
                }
            }
        }
    }

EXIT:
    LOG_API( "---Exiting eError 0x%x\n", eError);
    LOG_API("---Exiting\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_AllocateBuffer      							     */
/*                                                                           */
/*  Description   : This Function Allocate buffer for components use.		 */
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

static OMX_ERRORTYPE VIDDEC_AllocateBuffer (
								   OMX_IN OMX_HANDLETYPE            hComponent,
                                   OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffHead,
                                   OMX_IN OMX_U32                   nPortIndex,
                                   OMX_IN OMX_PTR                   pAppPrivate,
                                   OMX_IN OMX_U32                   nSizeBytes)
{
    OMX_ERRORTYPE                 eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*            pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE*     pComponentPrivate = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef          = NULL;
    VIDDEC_PORT_TYPE*             pCompPort         = NULL;
    OMX_U8*                       pTemp             = NULL;
    OMX_U8                        pBufferCnt        = 0;
    OMX_U32                       nTempSizeBytes    = 0;
    nTempSizeBytes = nSizeBytes;
	LOG_API("+++Entering\n");
#if DUMP_BUFFER_ALLOC_DETAILS
    FILE *fp = fopen("/tmp/buffAlloc.txt", "a");
    fprintf(fp, "VIDDEC_AllocateBuffer allocating buffer of size %d\n",
    					 nSizeBytes);
    fclose(fp);
#endif

    OMX_CONF_CHECK_CMD(hComponent, OMX_TRUE, OMX_TRUE);
    pHandle = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

#ifdef VIDDEC_WMVPOINTERFIXED
    if (pComponentPrivate->pInPortDef->
    			format.video.eCompressionFormat == OMX_VIDEO_CodingWMV &&
        nPortIndex == VIDDEC_INPUT_PORT &&
        pComponentPrivate->ProcessMode == 0)
    {
        nTempSizeBytes += VIDDEC_WMV_BUFFER_OFFSET;
        LOG_API(
        				"Adding extra wmv buffer add %d size %lu\n",
        							VIDDEC_WMV_BUFFER_OFFSET, nTempSizeBytes);
    }
#endif
    if (nPortIndex == pComponentPrivate->pInPortFormat->nPortIndex)
    {
        pCompPort  = pComponentPrivate->pCompPort[VIDDEC_INPUT_PORT];
        pBufferCnt = pCompPort->nBufferCnt;
        pPortDef   = pComponentPrivate->pInPortDef;
        LOG_API(
        				"OMX_AllocateBuffer >> INPUT nSizeBytes 0x%lx\n",
            						nSizeBytes);
    }
    else if (nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
    {
        pCompPort = pComponentPrivate->pCompPort[VIDDEC_OUTPUT_PORT];
        pBufferCnt = pCompPort->nBufferCnt;
        pPortDef = pComponentPrivate->pOutPortDef;
        LOG_API(
        				"OMX_AllocateBuffer >> OUTPUT nSizeBytes 0x%lx\n",
            				nSizeBytes);
    }
    else
    {
        eError = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    OMX_MALLOC_STRUCT(pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr,
    					OMX_BUFFERHEADERTYPE,
    					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel1]);
    if (!pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr)
    {
        eError = OMX_ErrorInsufficientResources;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pComponentPrivate->pHandle->
                                               	pApplicationPrivate,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               NULL);
        goto EXIT;
    }

    *pBuffHead = pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr;
    memset(*pBuffHead, 0, sizeof(OMX_BUFFERHEADERTYPE));
    OMX_CONF_INIT_STRUCT(pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr,
    							OMX_BUFFERHEADERTYPE, pComponentPrivate->dbg);
    OMX_MALLOC_STRUCT_DSPALIGN(pCompPort->pBufferPrivate[pBufferCnt]->
    								pBufferHdr->pBuffer, OMX_U8, nSizeBytes);
    if (!pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr->pBuffer)
    {
        eError = OMX_ErrorInsufficientResources;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pComponentPrivate->pHandle->
                                               		pApplicationPrivate,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               NULL);
        goto EXIT;
    }
   pTemp = (OMX_U8*)pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr->pBuffer;

#ifdef VIDDEC_WMVPOINTERFIXED
    if (pComponentPrivate->pInPortDef->
    				format.video.eCompressionFormat == OMX_VIDEO_CodingWMV &&
        nPortIndex == VIDDEC_INPUT_PORT &&
        pComponentPrivate->ProcessMode == 0)
    {
        pTemp += VIDDEC_WMV_BUFFER_OFFSET;
        pCompPort->pBufferPrivate[pBufferCnt]->pTempBuffer = (OMX_U8*)pTemp;
        (*pBuffHead)->nOffset = 0;
        LOG_API(
        				"Adding extra wmv padding %d pBuffer 0x%p\n",
        					VIDDEC_WMV_BUFFER_OFFSET,
           							 pTemp);
    }
#endif
    pCompPort->pBufferPrivate[pBufferCnt]->
    						pBufferHdr->pBuffer = (OMX_U8*)pTemp;
    (*pBuffHead)->pBuffer 						= (pCompPort->
    						pBufferPrivate[pBufferCnt]->pBufferHdr->pBuffer);
    (*pBuffHead)->nAllocLen 					= nTempSizeBytes;
    (*pBuffHead)->pAppPrivate 					= pAppPrivate;
    (*pBuffHead)->pPlatformPrivate 				= NULL;
    (*pBuffHead)->pInputPortPrivate 			= NULL;
    (*pBuffHead)->pOutputPortPrivate 			= NULL;
    (*pBuffHead)->nFlags 						= VIDDEC_CLEARFLAGS;
    (*pBuffHead)->pMarkData 					= NULL;

#ifdef __PERF_INSTRUMENTATION__
    PERF_ReceivedFrame(pComponentPrivate->pPERFcomp,
                       (*pBuffHead)->pBuffer,
                       (*pBuffHead)->nAllocLen,
                       PERF_ModuleMemory);
#endif


    if (pCompPort->hTunnelComponent != NULL)
    {
        if (pPortDef->eDir == OMX_DirInput)
        {
            (*pBuffHead)->nInputPortIndex  = nPortIndex;
            (*pBuffHead)->nOutputPortIndex = pCompPort->nTunnelPort;
        }
        else
        {
            (*pBuffHead)->nInputPortIndex  = pCompPort->nTunnelPort;
            (*pBuffHead)->nOutputPortIndex = nPortIndex;
        }
    }
    else
    {
        if (nPortIndex == VIDDEC_INPUT_PORT)
        {
            (*pBuffHead)->nInputPortIndex  = VIDDEC_INPUT_PORT;
            (*pBuffHead)->nOutputPortIndex = VIDDEC_NOPORT;
        }
        else
        {
            (*pBuffHead)->nInputPortIndex  = VIDDEC_NOPORT;
            (*pBuffHead)->nOutputPortIndex = VIDDEC_OUTPUT_PORT;
        }
    }

    if (nPortIndex == VIDDEC_INPUT_PORT)
    {
       (*pBuffHead)->pInputPortPrivate  = pCompPort->pBufferPrivate[pBufferCnt];
       (*pBuffHead)->pOutputPortPrivate = NULL;
    }
    else
    {
       (*pBuffHead)->pOutputPortPrivate = pCompPort->pBufferPrivate[pBufferCnt];
       (*pBuffHead)->pInputPortPrivate  = NULL;
    }
    pCompPort->pBufferPrivate[pBufferCnt]->bAllocByComponent = OMX_TRUE;

    if (pCompPort->hTunnelComponent != NULL)
    {
        pCompPort->pBufferPrivate[pBufferCnt]->eBufferOwner =
        									VIDDEC_BUFFER_WITH_TUNNELEDCOMP;
    }
    else
    {
        pCompPort->pBufferPrivate[pBufferCnt]->eBufferOwner =
        									VIDDEC_BUFFER_WITH_CLIENT;
    }

    pPortDef->nBufferSize = nTempSizeBytes;
    LOG_API(
    			"pBuffHead 0x%p nAllocLen 0x%lx pBuffer %p eBufferOwner %d\n",
       			 *pBuffHead, (*pBuffHead)->nAllocLen,
        			pCompPort->pBufferPrivate[pBufferCnt]->pBufferHdr->pBuffer,
        				pCompPort->pBufferPrivate[pBufferCnt]->eBufferOwner);

    eError = VIDDEC_Allocate_DSPResources(pComponentPrivate, nPortIndex);
    if (eError != OMX_ErrorNone)
    {
        OMX_PRDSP4(pComponentPrivate->dbg, "Error: Allocating DSP resources\n");
        eError = OMX_ErrorInsufficientResources;
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                               pComponentPrivate->pHandle->
                                               	pApplicationPrivate,
                                               OMX_EventError,
                                               eError,
                                               OMX_TI_ErrorMajor,
                                               NULL);
        goto EXIT;
    }

    pCompPort->nBufferCnt++;
    pCompPort->pBufferPrivate[pBufferCnt]->nNumber = pCompPort->nBufferCnt;

    LOG_API(
    	"eBufferOwner 0x%x nBufferCountActual %lu nBufferCnt %u nnumber %lu\n",
       	pCompPort->pBufferPrivate[pBufferCnt]->eBufferOwner,
        pPortDef->nBufferCountActual,
        pCompPort->nBufferCnt,pCompPort->pBufferPrivate[pBufferCnt]->nNumber);

    if (pCompPort->nBufferCnt == pPortDef->nBufferCountActual)
    {
        pPortDef->bPopulated = OMX_TRUE;

            if (nPortIndex == VIDDEC_INPUT_PORT)
            {
                VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sInSemaphore);
            }
            else
            {
                VIDDEC_PTHREAD_SEMAPHORE_POST(pComponentPrivate->sOutSemaphore);
            }
    }

EXIT:
    LOG_API( "---Exiting eError 0x%x\n", eError);
    LOG_API("---Exiting\n");
    return eError;
}

#ifdef KHRONOS_1_1
/*****************************************************************************/
/*                                                                           */
/*  Function Name : IsTIOMXComponent        							     */
/*                                                                           */
/*  Description   : Check if the component is TI component					 */
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
static OMX_BOOL IsTIOMXComponent(OMX_HANDLETYPE hComp,
								 struct         OMX_TI_Debug *dbg)
{
    OMX_ERRORTYPE    eError                  = OMX_ErrorNone;
    OMX_STRING       pTunnelcComponentName   = NULL;
    OMX_VERSIONTYPE* pTunnelComponentVersion = NULL;
    OMX_VERSIONTYPE* pSpecVersion            = NULL;
    OMX_UUIDTYPE*    pComponentUUID          = NULL;
    char            *pSubstring              = NULL;
    OMX_BOOL         bResult                 = OMX_TRUE;

    pTunnelcComponentName = malloc(128);
    LOG_API("+++Entering\n");
    if (pTunnelcComponentName == NULL)
    {
        eError = OMX_ErrorInsufficientResources;
        LOG_API(
        			"Error in Video Decoder OMX_ErrorInsufficientResources\n");
        goto EXIT;
    }

    pTunnelComponentVersion = malloc(sizeof(OMX_VERSIONTYPE));
    if (pTunnelComponentVersion == NULL)
    {
        LOG_API(
        			"Error in Video Decoder OMX_ErrorInsufficientResources\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pSpecVersion = malloc(sizeof(OMX_VERSIONTYPE));
    if (pSpecVersion == NULL)
    {
        LOG_API(
        			"Error in Video Decoder OMX_ErrorInsufficientResources\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pComponentUUID = malloc(sizeof(OMX_UUIDTYPE));
    if (pComponentUUID == NULL)
    {
        LOG_API(
        			"Error in Video Decoder OMX_ErrorInsufficientResources\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    eError = OMX_GetComponentVersion (hComp, pTunnelcComponentName,
    									pTunnelComponentVersion, pSpecVersion,
    										pComponentUUID);

    /*           Check if tunneled component is a TI component               */
    pSubstring = strstr(pTunnelcComponentName, "OMX.TI.");
    if(pSubstring == NULL)
    {
                bResult = OMX_FALSE;
    }

EXIT:
    free(pTunnelcComponentName);
    free(pTunnelComponentVersion);
    free(pSpecVersion);
    free(pComponentUUID);

    return bResult;
}
#endif

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_VerifyTunnelConnection						     */
/*                                                                           */
/*  Description   : This Function Verifies Tunnel Connection				 */
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
static OMX_ERRORTYPE VIDDEC_VerifyTunnelConnection (
								  VIDDEC_PORT_TYPE             *pPort,
                                  OMX_HANDLETYPE                hTunneledComp,
                                  OMX_PARAM_PORTDEFINITIONTYPE* pPortDef)
{
    OMX_PARAM_PORTDEFINITIONTYPE sPortDef;
    OMX_ERRORTYPE eError              = OMX_ErrorNone;

    sPortDef.nSize                    = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    sPortDef.nVersion.s.nVersionMajor = VERSION_MAJOR;
    sPortDef.nVersion.s.nVersionMinor = VERSION_MINOR;
    sPortDef.nVersion.s.nRevision     = VERSION_REVISION;
    sPortDef.nVersion.s.nStep         = VERSION_STEP;
    sPortDef.nPortIndex               = pPort->nTunnelPort;

    eError = OMX_GetParameter(hTunneledComp, OMX_IndexParamPortDefinition,
    											&sPortDef);
    LOG_API("+++Entering\n");
    if (eError != OMX_ErrorNone)
    {
        return eError;
    }

    switch (pPortDef->eDomain)
    {
        case OMX_PortDomainOther:
            if (sPortDef.format.other.eFormat != pPortDef->format.other.eFormat)
            {
                pPort->hTunnelComponent = 0;
                pPort->nTunnelPort      = 0;
                return OMX_ErrorPortsNotCompatible;
            }
            break;
        case OMX_PortDomainAudio:
            if (sPortDef.format.audio.eEncoding != pPortDef->
            										format.audio.eEncoding)
            {
                pPort->hTunnelComponent = 0;
                pPort->nTunnelPort      = 0;
                return OMX_ErrorPortsNotCompatible;
            }
            break;
        case OMX_PortDomainVideo:
            if (sPortDef.format.video.eCompressionFormat != pPortDef->
            								format.video.eCompressionFormat)
            {
                pPort->hTunnelComponent = 0;
                pPort->nTunnelPort      = 0;
                return OMX_ErrorPortsNotCompatible;
            }
            break;
        case OMX_PortDomainImage:
            if (sPortDef.format.image.eCompressionFormat != pPortDef->
            								format.image.eCompressionFormat)
            {
                pPort->hTunnelComponent = 0;
                pPort->nTunnelPort      = 0;
                return OMX_ErrorPortsNotCompatible;
            }
            break;
        default:
            pPort->hTunnelComponent = 0;
            pPort->nTunnelPort      = 0;
            return OMX_ErrorPortsNotCompatible;
    }
    LOG_API("---Exiting\n");
    return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_ComponentTunnelRequest						     */
/*                                                                           */
/*  Description   : This Function Handle Component Tunnel request.			 */
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
static OMX_ERRORTYPE VIDDEC_ComponentTunnelRequest (
								OMX_IN OMX_HANDLETYPE          hComponent,
                                OMX_IN OMX_U32                 nPort,
                                OMX_IN OMX_HANDLETYPE          hTunneledComp,
                                OMX_IN OMX_U32                 nTunneledPort,
                                OMX_INOUT OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    OMX_ERRORTYPE                 eError            = OMX_ErrorNone;
    OMX_COMPONENTTYPE*            pHandle           = NULL;
    VIDDEC_COMPONENT_PRIVATE*     pComponentPrivate = NULL;
    OMX_PARAM_BUFFERSUPPLIERTYPE  sBufferSupplier;
    VIDDEC_PORT_TYPE             *pPort             = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef          = NULL;

    OMX_CONF_CHECK_CMD(hComponent, OMX_TRUE, OMX_TRUE);

    pHandle           = (OMX_COMPONENTTYPE*)hComponent;
    pComponentPrivate = (VIDDEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

    pPort    = pComponentPrivate->pCompPort[nPort];
    pPortDef = pComponentPrivate->pInPortDef;

    LOG_API("+++Entering\n");
    if (nPort == pComponentPrivate->pInPortFormat->nPortIndex)
    {
        pPortDef = pComponentPrivate->pInPortDef;
    }
    else if (nPort == pComponentPrivate->pOutPortFormat->nPortIndex)
    {
        pPortDef = pComponentPrivate->pOutPortDef;
    }

    if (pTunnelSetup == NULL || hTunneledComp == 0)
    {
        pPort->hTunnelComponent = NULL;
        pPort->nTunnelPort = 0;
        pPort->eSupplierSetting = OMX_BufferSupplyUnspecified;
    }
    else {
        if (pPortDef->eDir != OMX_DirInput && pPortDef->eDir != OMX_DirOutput)
        {
            return OMX_ErrorBadParameter;
        }

#ifdef KHRONOS_1_1
        /*            Check if the other component is developed by TI        */
        if(IsTIOMXComponent(hTunneledComp, &pComponentPrivate->dbg) != OMX_TRUE)
        {
            eError = OMX_ErrorTunnelingUnsupported;
            goto EXIT;
        }
#endif
        pPort->hTunnelComponent = hTunneledComp;
        pPort->nTunnelPort = nTunneledPort;

        if (pPortDef->eDir == OMX_DirOutput)
        {
            pTunnelSetup->eSupplier = pPort->eSupplierSetting;
        }
        else {
            /*              Component is the input (sink of data)            */
            eError = VIDDEC_VerifyTunnelConnection(pPort, hTunneledComp,
            														pPortDef);
            if (OMX_ErrorNone != eError)
            {
                LOG_API(
                				"Error: PP VerifyTunnelConnection failed\n");
            /*           Invalid connection formats. Return eError           */
                return OMX_ErrorPortsNotCompatible;
            }

            pPort->eSupplierSetting = pTunnelSetup->eSupplier;
            if (OMX_BufferSupplyUnspecified == pPort->eSupplierSetting)
            {
                pPort->eSupplierSetting = pTunnelSetup->eSupplier =
                										OMX_BufferSupplyOutput;
            }

            /*            Tell the output port who the supplier is            */
            sBufferSupplier.nSize                    =
            							sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE);
            sBufferSupplier.nVersion.s.nVersionMajor = VERSION_MAJOR;
            sBufferSupplier.nVersion.s.nVersionMinor = VERSION_MAJOR;
            sBufferSupplier.nVersion.s.nRevision     = VERSION_REVISION;
            sBufferSupplier.nVersion.s.nStep         = VERSION_STEP;
            sBufferSupplier.nPortIndex               = nTunneledPort;
            sBufferSupplier.eBufferSupplier          = pPort->eSupplierSetting;

            eError = OMX_SetParameter(hTunneledComp,
            							OMX_IndexParamCompBufferSupplier,
            									&sBufferSupplier);

            eError = OMX_GetParameter(hTunneledComp,
            							OMX_IndexParamCompBufferSupplier,
            									&sBufferSupplier);

            if (sBufferSupplier.eBufferSupplier != pPort->eSupplierSetting)
            {
                LOG_API(
          	"SetParameter: OMX_IndexParamCompBufferSupplier failed to change setting\n" );
                return OMX_ErrorUndefined;
            }
        }
    }

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_Allocate_DSPResources						     */
/*                                                                           */
/*  Description   : This Function Allocate DSP Resources.					 */
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


static OMX_ERRORTYPE VIDDEC_Allocate_DSPResources(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
													OMX_IN OMX_U32 nPortIndex)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    /*                  todo remove array, fix it to variable,               */
    /*                it means just one variable for both index              */
    void *pUalgOutParams[1];
    OMX_U8* pTemp                            = NULL;
    OMX_U8 nBufferCnt                        = pComponentPrivate->
    											pCompPort[nPortIndex]->
    												nBufferCnt;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = pComponentPrivate->
    											pInPortDef;
    VIDDEC_BUFFER_PRIVATE* pBufferPrivate    = NULL;

    pBufferPrivate = (VIDDEC_BUFFER_PRIVATE*)(pComponentPrivate->
    											pCompPort[nPortIndex]->
    											pBufferPrivate[nBufferCnt]);

    LOG_API("+++Entering\n");
    if (nPortIndex == pComponentPrivate->pInPortFormat->nPortIndex)
    {
        if (pPortDefIn->
        		format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
            pPortDefIn->
            	format.video.eCompressionFormat == OMX_VIDEO_CodingAVC ||
            pPortDefIn->format.video.eCompressionFormat == OMX_VIDEO_CodingWMV)
        {
          eError = VIDDEC_allocate_SN_InputArgument(pComponentPrivate,
          										    pBufferPrivate);
          if(eError != OMX_ErrorNone) goto EXIT;
        }
        else
        {
            LOG_API(
            				"Error: Invalid Compression Type\n");
            goto EXIT;
        }
    }
    else if (nPortIndex == pComponentPrivate->pOutPortFormat->nPortIndex)
    {
        if (pPortDefIn->
        			format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
            pPortDefIn->
            		format.video.eCompressionFormat == OMX_VIDEO_CodingAVC ||
            pPortDefIn->
            		format.video.eCompressionFormat == OMX_VIDEO_CodingWMV)
        {

            OMX_MALLOC_STRUCT_DSPALIGN(pUalgOutParams[0],
            								ITTVDEC_UALGOutputParam_t,
            								sizeof(ITTVDEC_UALGOutputParam_t));

            if ((!pUalgOutParams[0]))
            {
                LOG_API("Error: Malloc failed\n");
                eError = OMX_ErrorInsufficientResources;
                goto EXIT;
            }
            pTemp = (OMX_U8*)pUalgOutParams[0];
            /*                pTemp += VIDDEC_PADDING_HALF;                  */
            pUalgOutParams[0] = pTemp;
            pBufferPrivate->pUalgParam = (void *)(pUalgOutParams[0]);

            if(pPortDefIn->
            		format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
                pBufferPrivate->
                	nUalgParamSize = sizeof(MPEG4VDEC_UALGOutputParam);
            else if(pPortDefIn->
            		format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
                pBufferPrivate->nUalgParamSize = sizeof(H264VDEC_UALGOutputParam);
            else
                pBufferPrivate->nUalgParamSize = sizeof(WMV9VDEC_UALGOutputParam);

        }
        else
        {
            LOG_API(
            				"Error: Invalid Compression Type\n");
            eError = OMX_ErrorUnsupportedSetting;
            goto EXIT;
        }
    }
    else
    {
        eError = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}
/*****************************************************************************/
/*                                                                           */
/*  Function Name : VIDDEC_GetExtensionIndex     						     */
/*                                                                           */
/*  Description   : This Function Retrieves Extension Index.				 */
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
static OMX_ERRORTYPE VIDDEC_GetExtensionIndex(OMX_IN OMX_HANDLETYPE hComponent,
											  OMX_IN OMX_STRING cParameterName,
											  OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    unsigned int nIndex;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;

    OMX_CONF_CHECK_CMD(hComponent, OMX_TRUE, OMX_TRUE);
    LOG_API("+++Entering\n");
    for(nIndex = 0;
    		nIndex < (sizeof(sVideoDecCustomParams)/sizeof(VIDDEC_CUSTOM_PARAM));
    			nIndex++)
    {
        if(strcmp((char *)cParameterName,
        	(char *)&(sVideoDecCustomParams[nIndex].cCustomParamName)) == 0)
        {
            *pIndexType = sVideoDecCustomParams[nIndex].nCustomParamIndex;
            eError = OMX_ErrorNone;
            break;
        }
    }
EXIT:
    LOG_API("---Exiting\n");
    return eError;
}

#ifdef KHRONOS_1_1
/*****************************************************************************/
/*                                                                           */
/*  Function Name : ComponentRoleEnum									     */
/*                                                                           */
/*  Description   : This Function Loads Default Value.						 */
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
static OMX_ERRORTYPE ComponentRoleEnum(
        								OMX_IN OMX_HANDLETYPE hComponent,
                						OMX_OUT OMX_U8 		 *cRole,
                						OMX_IN OMX_U32 		  nIndex)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate;

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    pComponentPrivate    = (VIDDEC_COMPONENT_PRIVATE *)
    					(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    LOG_API("+++Entering\n");
    memset(cRole, 0x0, OMX_MAX_STRINGNAME_SIZE);
    switch (nIndex)
    {
        case VIDDEC_DEFAULT_INPUT_INDEX_H263:
            eError = VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_H263);
            if( pComponentPrivate->pOutPortFormat->eColorFormat !=
            						VIDDEC_COLORFORMAT420)
            {
                eError = VIDDEC_Load_Defaults(pComponentPrivate,
                			VIDDEC_INIT_PLANAR420);
            }
            strcpy((char*)cRole, VIDDEC_COMPONENTROLES_H263);
            break;
        case VIDDEC_DEFAULT_INPUT_INDEX_H264:
            eError = VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_H264);
            if( pComponentPrivate->pOutPortFormat->eColorFormat !=
            										VIDDEC_COLORFORMAT420)
            {
                eError = VIDDEC_Load_Defaults(pComponentPrivate,
                								VIDDEC_INIT_PLANAR420);

            }
            strcpy((char*)cRole, VIDDEC_COMPONENTROLES_H264);
            break;
        case VIDDEC_DEFAULT_INPUT_INDEX_MPEG4:
            eError = VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_MPEG4);
            if( pComponentPrivate->pOutPortFormat->eColorFormat !=
            										VIDDEC_COLORFORMAT420)
            {
                eError = VIDDEC_Load_Defaults(pComponentPrivate,
                									VIDDEC_INIT_PLANAR420);
            }
            strcpy((char*)cRole, VIDDEC_COMPONENTROLES_MPEG4);
            break;
        case VIDDEC_DEFAULT_INPUT_INDEX_WMV9:
            eError = VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_WMV9);
            if( pComponentPrivate->pOutPortFormat->eColorFormat !=
            										VIDDEC_COLORFORMAT420)
            {
                eError = VIDDEC_Load_Defaults(pComponentPrivate,
                									VIDDEC_INIT_PLANAR420);
            }
            strcpy((char*)cRole, VIDDEC_COMPONENTROLES_WMV9);
            break;
        default:
            eError = OMX_ErrorNoMore;
            break;
    }
    if(eError != OMX_ErrorNone)
    {
        goto EXIT;
    }

EXIT:
	LOG_API("---Exiting\n");
    return eError;
}
#endif

