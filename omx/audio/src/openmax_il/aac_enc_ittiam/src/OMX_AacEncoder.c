/*****************************************************************************/
/*                                                                           */
/*                        Ittiam Audio Standards                             */
/*                                                                           */
/*                   ITTIAM SYSTEMS PVT LTD, BANGALORE                       */
/*                          COPYRIGHT(C) 2004                                */
/*                                                                           */
/*  This program is proprietary to Ittiam Systems Pvt. Ltd. and is protected */
/*  under Indian Copyright Act as an unpublished work.Its use and disclosure */
/*  is  limited by  the terms and conditions of a license  agreement. It may */
/*  be copied or  otherwise reproduced or  disclosed  to persons outside the */
/*  licensee 's  organization  except  in  accordance  with  the  terms  and */
/*  conditions of  such an agreement. All  copies and reproductions shall be */
/*  the  property  of Ittiam Systems Pvt.  Ltd. and  must  bear  this notice */
/*  in its entirety.                                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name        : ia_enhaacplus_enc_error_codes.h                     */
/*                                                                           */
/*  Description      : Module specific error codes definitions               */
/*                                                                           */
/*  List of Functions: None                                                  */
/*                                                                           */
/*  Issues / Problems: None                                                  */
/*                                                                           */
/*  Revision History :                                                       */
/*                                                                           */
/*        DD MM YYYY       Author                Changes                     */
/*        29 07 2005       Ittiam                Created                     */
/*                                                                           */
/*****************************************************************************/


/* ------compilation control switches -------------------------*/
/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#ifdef UNDER_CE
#include <windows.h>
#include <oaf_osal.h>
#include <omx_core.h>
#else
#include <wchar.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>
#endif
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <dbapi.h>
#include <dlfcn.h>




/*-------program files ----------------------------------------*/

#ifndef UNDER_CE
#ifdef DSP_RENDERING_ON
#include <AudioManagerAPI.h>
#endif
#endif
#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM
#include <ResourceManagerProxyAPI.h>
#endif
#include <OMX_Component.h>
#include "OMX_IttiamAacEncoder.h"
#define  AAC_ENC_ROLE "audio_encoder.aac"
#include "OMX_IttiamAacEnc_Utils.h"


/****************************************************************
*  EXTERNAL REFERENCES NOTE : only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

static OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComp, OMX_CALLBACKTYPE* pCallBacks, OMX_PTR pAppData);
static OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComp, OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion, OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* pComponentUUID);
static OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComp, OMX_COMMANDTYPE nCommand, OMX_U32 nParam,OMX_PTR pCmdData);
static OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComp, OMX_INDEXTYPE nParamIndex, OMX_PTR ComponentParamStruct);
static OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComp, OMX_INDEXTYPE nParamIndex, OMX_PTR ComponentParamStruct);
static OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComp, OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
static OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComp, OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
static OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComp, OMX_BUFFERHEADERTYPE* pBuffer);
static OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComp, OMX_BUFFERHEADERTYPE* pBuffer);
static OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComp, OMX_STATETYPE* pState);
static OMX_ERRORTYPE ComponentTunnelRequest(OMX_HANDLETYPE hComp, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup);
static OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE pHandle);
static OMX_ERRORTYPE AllocateBuffer(OMX_IN OMX_HANDLETYPE hComponent, OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer, OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_PTR pAppPrivate, OMX_IN OMX_U32 nSizeBytes);
static OMX_ERRORTYPE FreeBuffer(OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
static OMX_ERRORTYPE UseBuffer(OMX_IN OMX_HANDLETYPE hComponent, OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_PTR pAppPrivate, OMX_IN OMX_U32 nSizeBytes, OMX_IN OMX_U8* pBuffer);
static OMX_ERRORTYPE GetExtensionIndex(OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_STRING cParameterName, OMX_OUT OMX_INDEXTYPE* pIndexType);
static OMX_ERRORTYPE ComponentRoleEnum( OMX_IN OMX_HANDLETYPE hComponent,
                                                  OMX_OUT OMX_U8 *cRole,
                                                  OMX_IN OMX_U32 nIndex);


/*-------------------------------------------------------------------*/
/**
  * OMX_ComponentInit() Set the all the function pointers of component
  *
  * This method will update the component function pointer to the handle
  *
  * @param hComp         handle for this instance of the component
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_ErrorInsufficientResources If the malloc fails
  **/
/*-------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComp)
{

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(hComp,1,1);                              /* checking for NULL pointers */
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef_ip = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef_op = NULL;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_AUDIO_PARAM_AACPROFILETYPE *aac_ip = NULL;
    OMX_AUDIO_PARAM_AACPROFILETYPE *aac_op = NULL;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE*) hComp;
    OMX_AUDIO_PARAM_PCMMODETYPE *aac_pcm_ip = NULL;
    OMX_AUDIO_PARAM_PCMMODETYPE *aac_pcm_op = NULL;
    int i;

/* For Android Tests */
    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC : Entering OMX_ComponentInit\n", __LINE__);

    /*Set the all component function pointer to the handle*/
    pHandle->SetCallbacks           = SetCallbacks;
    pHandle->GetComponentVersion    = GetComponentVersion;
    pHandle->SendCommand            = SendCommand;
    pHandle->GetParameter           = GetParameter;
    pHandle->GetExtensionIndex      = GetExtensionIndex;
    pHandle->SetParameter           = SetParameter;
    pHandle->GetConfig              = GetConfig;
    pHandle->SetConfig              = SetConfig;
    pHandle->GetState               = GetState;
    pHandle->EmptyThisBuffer        = EmptyThisBuffer;
    pHandle->FillThisBuffer         = FillThisBuffer;
    pHandle->ComponentTunnelRequest = ComponentTunnelRequest;
    pHandle->ComponentDeInit        = ComponentDeInit;
    pHandle->AllocateBuffer         = AllocateBuffer;
    pHandle->FreeBuffer             = FreeBuffer;
    pHandle->UseBuffer              = UseBuffer;
    pHandle->ComponentRoleEnum      = ComponentRoleEnum;

    OMX_MALLOC_STRUCT(pHandle->pComponentPrivate, AACENC_COMPONENT_PRIVATE);
    ((AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate)->pHandle = pHandle;
    OMXDBG_PRINT(stderr, PRINT, 1, 0, "AACENC: pComponentPrivate %p \n",pHandle->pComponentPrivate);

    /* Initialize component data structures to default values */
    ((AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate)->sPortParam.nPorts = 0x2;
    ((AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate)->sPortParam.nStartPortNumber = 0x0;


    /* ---------start of MX_AUDIO_PARAM_AACPROFILETYPE --------- */

    OMX_MALLOC_STRUCT(aac_ip, OMX_AUDIO_PARAM_AACPROFILETYPE);
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "AACENC: aac_ip %p \n", aac_ip);
    OMX_MALLOC_STRUCT(aac_op, OMX_AUDIO_PARAM_AACPROFILETYPE);
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "AACENC: aac_op %p \n",aac_op);

    ((AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate)->aacParams[INPUT_PORT] = aac_ip;
    ((AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate)->aacParams[OUTPUT_PORT] = aac_op;

    aac_op->nSize               = sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE);
    aac_op->nChannels           = 2;
    aac_op->nSampleRate         = 44100;
    aac_op->eAACProfile         = OMX_AUDIO_AACObjectLC;
    aac_op->eAACStreamFormat    = OMX_AUDIO_AACStreamFormatMP2ADTS;         /* For khronos only : should  be MP4ADTS*/
    aac_op->nBitRate            = 128000;
    aac_op->eChannelMode        = OMX_AUDIO_ChannelModeStereo;
    aac_op->nPortIndex          = 1;
    aac_op->nFrameLength        = 0;
    aac_op->nAudioBandWidth     = 0;                                        /* ? */

    /* ---------end of MX_AUDIO_PARAM_AACPROFILETYPE --------- */


    /* ---------start of OMX_AUDIO_PARAM_PCMMODETYPE --------- */

    OMX_MALLOC_STRUCT(aac_pcm_ip, OMX_AUDIO_PARAM_PCMMODETYPE);
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "AACENC: aac_pcm_ip %p \n ",aac_pcm_ip);
    OMX_MALLOC_STRUCT(aac_pcm_op, OMX_AUDIO_PARAM_PCMMODETYPE);

    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d :: AACENC: %p\n",__LINE__,aac_pcm_op);
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "AACENC: aac_pcm_op%p \n ",aac_pcm_op);

    aac_pcm_ip->nSize               = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
    aac_pcm_ip->nBitPerSample       = 16;                           /*Will be remapped for SN. 16:2,  24:3*/
    aac_pcm_ip->nPortIndex          = 0;
    aac_pcm_ip->nChannels           = 1;                            /*Will be remapped for SN.  0:mono, 1:stereo*/
    aac_pcm_ip->eNumData            = OMX_NumericalDataSigned;
    aac_pcm_ip->nSamplingRate       = 8000;
    aac_pcm_ip->ePCMMode            = OMX_AUDIO_PCMModeLinear;
    aac_pcm_ip->bInterleaved        = OMX_TRUE;

    ((AACENC_COMPONENT_PRIVATE*) pHandle->pComponentPrivate)->pcmParam[INPUT_PORT] = aac_pcm_ip;
    ((AACENC_COMPONENT_PRIVATE*) pHandle->pComponentPrivate)->pcmParam[OUTPUT_PORT] = aac_pcm_op;

    /* ---------end of OMX_AUDIO_PARAM_PCMMODETYPE --------- */


    pComponentPrivate = pHandle->pComponentPrivate;
    OMX_DBG_INIT(pComponentPrivate->dbg, "OMX_DBG_AACENC");

#ifdef ANDROID /* leave this now, we may need them later. */
    pComponentPrivate->iPVCapabilityFlags.iIsOMXComponentMultiThreaded = OMX_TRUE; /* this should be true always for TI components */
    pComponentPrivate->iPVCapabilityFlags.iOMXComponentNeedsNALStartCode = OMX_FALSE; /* used only for H.264, leave this as false */
    pComponentPrivate->iPVCapabilityFlags.iOMXComponentSupportsExternalOutputBufferAlloc = OMX_FALSE; /* N/C */
    pComponentPrivate->iPVCapabilityFlags.iOMXComponentSupportsExternalInputBufferAlloc = OMX_FALSE; /* N/C */
    pComponentPrivate->iPVCapabilityFlags.iOMXComponentSupportsMovableInputBuffers = OMX_FALSE; /* experiment with this */
    pComponentPrivate->iPVCapabilityFlags.iOMXComponentSupportsPartialFrames = OMX_FALSE; /* N/C */
    pComponentPrivate->iPVCapabilityFlags.iOMXComponentCanHandleIncompleteFrames = OMX_FALSE; /* N/C */
#endif

#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->pPERF = PERF_Create(PERF_FOURCC('A','A','E','_'),
                                               PERF_ModuleLLMM |
                                               PERF_ModuleAudioDecode);
#endif

    OMX_MALLOC_STRUCT(pComponentPrivate->pInputBufferList, BUFFERLIST);

    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pInputBufferList %p\n ", pComponentPrivate->pInputBufferList);
    pComponentPrivate->pInputBufferList->numBuffers = 0; /* initialize number of buffers */

    OMX_MALLOC_STRUCT(pComponentPrivate->pOutputBufferList, BUFFERLIST);
    pComponentPrivate->pOutputBufferList->numBuffers = 0; /* initialize number of buffers */
    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pOutputBufferList %p\n ", pComponentPrivate->pOutputBufferList);

    for (i=0; i < MAX_NUM_OF_BUFS; i++)
    {
        pComponentPrivate->pOutputBufferList->pBufHdr[i] = NULL;
        pComponentPrivate->pInputBufferList->pBufHdr[i]  = NULL;
        pComponentPrivate->tickcountBufIndex[i] = 0;
        pComponentPrivate->timestampBufIndex[i]  = 0;
    }

    pComponentPrivate->IpBufindex = 0;
    pComponentPrivate->OpBufindex = 0;

    OMX_MALLOC_STRUCT_SIZE(pComponentPrivate->sDeviceString, 100*sizeof(OMX_STRING), void);

    /* Initialize device string to the default value */
    strcpy((char*)pComponentPrivate->sDeviceString,"/rtmdn:i2:o1/codec\0");

    /*Safety value for frames per output buffer ( for Khronos)  */
    pComponentPrivate->FramesPer_OutputBuffer = 1;
    pComponentPrivate->CustomConfiguration = OMX_FALSE;

    pComponentPrivate->unNumChannels                = 2;
    pComponentPrivate->ulSamplingRate               = 44100;
    pComponentPrivate->unBitrate                    = 128000;
    pComponentPrivate->nObjectType                  = 2;
    pComponentPrivate->bitRateMode                  = 0;
    pComponentPrivate->File_Format                  = 2;
    pComponentPrivate->EmptybufferdoneCount         = 0;
    pComponentPrivate->EmptythisbufferCount         = 0;
    pComponentPrivate->FillbufferdoneCount          = 0;
    pComponentPrivate->FillthisbufferCount          = 0;

    pComponentPrivate->bPortDefsAllocated           = 0;
    pComponentPrivate->bCompThreadStarted           = 0;
    pComponentPrivate->bPlayCompleteFlag            = 0;
  OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->bPlayCompleteFlag = %ld\n",__LINE__,pComponentPrivate->bPlayCompleteFlag);
    pComponentPrivate->pMarkBuf                     = NULL;
    pComponentPrivate->pMarkData                    = NULL;
    pComponentPrivate->pParams                      = NULL;
    pComponentPrivate->LastOutputBufferHdrQueued    = NULL;
    pComponentPrivate->bDspStoppedWhileExecuting    = OMX_FALSE;
    pComponentPrivate->nOutStandingEmptyDones       = 0;
    pComponentPrivate->nOutStandingFillDones        = 0;
    pComponentPrivate->bPauseCommandPending         = OMX_FALSE;
    pComponentPrivate->bEnableCommandPending        = 0;
    pComponentPrivate->bDisableCommandPending       = 0;
    pComponentPrivate->nNumOutputBufPending         = 0;
    pComponentPrivate->bLoadedCommandPending        = OMX_FALSE;
    pComponentPrivate->bFirstOutputBuffer = 1;

    pComponentPrivate->nUnhandledFillThisBuffers=0;
    pComponentPrivate->nUnhandledEmptyThisBuffers = 0;

    pComponentPrivate->bFlushOutputPortCommandPending = OMX_FALSE;
    pComponentPrivate->bFlushInputPortCommandPending = OMX_FALSE;

    pComponentPrivate->nNumInputBufPending          = 0;
    pComponentPrivate->nNumOutputBufPending         = 0;

    pComponentPrivate->PendingInPausedBufs          = 0;
    pComponentPrivate->PendingOutPausedBufs         = 0;

//--[[ GB Patch START : junghyun.you@lge.com [2012.05.31]
    // Tushar - [ - For calculating o/p buffer timestamp instead of copying i/p buffer timestamp
    /*Timestamp calculation holding variable*/
    pComponentPrivate->temp_TS = 0;
    // Tushar - ] - For calculating o/p buffer timestamp instead of copying i/p buffer timestamp
//--]] GB Patch END
    /* Port format type */
    pComponentPrivate->sOutPortFormat.eEncoding     = OMX_AUDIO_CodingAAC;
    pComponentPrivate->sOutPortFormat.nIndex        = 0;/*OMX_IndexParamAudioAac;*/
    pComponentPrivate->sOutPortFormat.nPortIndex    = OUTPUT_PORT;

    pComponentPrivate->sInPortFormat.eEncoding      = OMX_AUDIO_CodingPCM;
    pComponentPrivate->sInPortFormat.nIndex         = 1;/*OMX_IndexParamAudioPcm;  */
    pComponentPrivate->sInPortFormat.nPortIndex     = INPUT_PORT;

    pComponentPrivate->i_frame_count            = 0;
    pComponentPrivate->bCodecDestroyed              = OMX_FALSE;

    pComponentPrivate->framemode          = 1;
    pComponentPrivate->bNewOutputBufRequired  = 1;
    pComponentPrivate->EOSfromApp             = 0;
    pComponentPrivate->StateTransitiontoIdleFromExecOngoing = 0;

    /* initialize role name */
    strcpy((char *)pComponentPrivate->componentRole.cRole, "audio_encoder.aac");

#ifndef UNDER_CE
    pthread_mutex_init(&pComponentPrivate->AlloBuf_mutex, NULL);
    pthread_cond_init (&pComponentPrivate->AlloBuf_threshold, NULL);
    pComponentPrivate->AlloBuf_waitingsignal = 0;

    pthread_mutex_init(&pComponentPrivate->codecStop_mutex, NULL);
    pthread_cond_init (&pComponentPrivate->codecStop_threshold, NULL);
    pComponentPrivate->codecStop_waitingsignal = 0;

    pthread_mutex_init(&pComponentPrivate->codecFlush_mutex, NULL);
    pthread_cond_init (&pComponentPrivate->codecFlush_threshold, NULL);
    pComponentPrivate->codecFlush_waitingsignal = 0;

    pthread_mutex_init(&pComponentPrivate->InLoaded_mutex, NULL);
    pthread_cond_init (&pComponentPrivate->InLoaded_threshold, NULL);
    pComponentPrivate->InLoaded_readytoidle = 0;

    pthread_mutex_init(&pComponentPrivate->InIdle_mutex, NULL);
    pthread_cond_init (&pComponentPrivate->InIdle_threshold, NULL);
    pComponentPrivate->InIdle_goingtoloaded = 0;
#else
    OMX_CreateEvent(&(pComponentPrivate->AlloBuf_event));
    pComponentPrivate->AlloBuf_waitingsignal = 0;

    OMX_CreateEvent(&(pComponentPrivate->InLoaded_event));
    pComponentPrivate->InLoaded_readytoidle = 0;

    OMX_CreateEvent(&(pComponentPrivate->InIdle_event));
    pComponentPrivate->InIdle_goingtoloaded = 0;
#endif

    /* port definition, input port */
    OMX_MALLOC_STRUCT(pPortDef_ip, OMX_PARAM_PORTDEFINITIONTYPE);
    OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: pPortDef_ip %p \n",pPortDef_ip );


    /* port definition, output port */
    OMX_MALLOC_STRUCT(pPortDef_op, OMX_PARAM_PORTDEFINITIONTYPE);
    OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: pPortDef_op %p,  size: %x \n",pPortDef_op, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));


    ((AACENC_COMPONENT_PRIVATE*) pHandle->pComponentPrivate)->pPortDef[INPUT_PORT] = pPortDef_ip;
    ((AACENC_COMPONENT_PRIVATE*) pHandle->pComponentPrivate)->pPortDef[OUTPUT_PORT] = pPortDef_op;


    pPortDef_ip->nSize                  = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    pPortDef_ip->nPortIndex             = 0x0;
    pPortDef_ip->nBufferCountActual     = NUM_AACENC_INPUT_BUFFERS;
    pPortDef_ip->nBufferCountMin        = NUM_AACENC_INPUT_BUFFERS;
    pPortDef_ip->eDir                   = OMX_DirInput;
    pPortDef_ip->bEnabled               = OMX_TRUE;
    pPortDef_ip->nBufferSize            = INPUT_AACENC_BUFFER_SIZE;
    pPortDef_ip->bPopulated             = 0;
    pPortDef_ip->format.audio.eEncoding =OMX_AUDIO_CodingPCM;
    pPortDef_ip->eDomain                = OMX_PortDomainAudio;

    pPortDef_op->nSize                  = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    pPortDef_op->nPortIndex             = 0x1;
    pPortDef_op->nBufferCountActual     = NUM_AACENC_OUTPUT_BUFFERS; /*4*/
    pPortDef_op->nBufferCountMin        = NUM_AACENC_OUTPUT_BUFFERS;
    pPortDef_op->eDir                   = OMX_DirOutput;
    pPortDef_op->bEnabled               = OMX_TRUE;
    pPortDef_op->nBufferSize            = OUTPUT_AACENC_BUFFER_SIZE;
    pPortDef_op->bPopulated             = 0;
    pPortDef_op->format.audio.eEncoding = OMX_AUDIO_CodingAAC;
    pPortDef_op->eDomain                = OMX_PortDomainAudio;

    pComponentPrivate->bIsInvalidState = OMX_FALSE;

    pComponentPrivate->bPreempted = OMX_FALSE;

#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM
    /* start Resource Manager Proxy */
    eError = RMProxy_NewInitalize();
    if (eError != OMX_ErrorNone)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error returned from loading ResourceManagerProxy thread\n",__LINE__);
        goto EXIT;
    }
#endif

    eError = AACENC_StartComponentThread(pHandle);
    if (eError != OMX_ErrorNone)
    {
      OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error returned from the Component\n",__LINE__);
      goto EXIT;
    }


#ifdef __PERF_INSTRUMENTATION__
    PERF_ThreadCreated(pComponentPrivate->pPERF, pComponentPrivate->ComponentThread,
                       PERF_FOURCC('A','A','E','T'));
#endif

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting OMX_ComponentInit\n", __LINE__);
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  SetCallbacks() Sets application callbacks to the component
  *
  * This method will update application callbacks
  * to the component. So that component can make use of those call back
  * while sending buffers to the application. And also it will copy the
  * application private data to component memory
  *
  * @param pComponent    handle for this instance of the component
  * @param pCallBacks    application callbacks
  * @param pAppData      Application private data
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE SetCallbacks (OMX_HANDLETYPE pComponent,
                                   OMX_CALLBACKTYPE* pCallBacks,
                                   OMX_PTR pAppData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(pComponent,1,1);                 /* Checking for NULL pointers:  pAppData is NULL for Khronos */
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE*)pComponent;

    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Entering SetCallbacks\n", __LINE__);
    if (pCallBacks == NULL)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
        eError = OMX_ErrorBadParameter;
        OMX_ERROR2(pComponentPrivate->dbg, "%d :: Received the empty callbacks from the application\n",__LINE__);
        goto EXIT;
    }

    /*Copy the callbacks of the application to the component private*/
    memcpy (&(pComponentPrivate->cbInfo), pCallBacks, sizeof(OMX_CALLBACKTYPE));
    /*copy the application private data to component memory */
    pHandle->pApplicationPrivate = pAppData;
    pComponentPrivate->curState = OMX_StateLoaded;

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting SetCallbacks\n", __LINE__);
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  GetComponentVersion() This will return the component version
  *
  * This method will retrun the component version
  *
  * @param hComp               handle for this instance of the component
  * @param pCompnentName       Name of the component
  * @param pCompnentVersion    handle for this instance of the component
  * @param pSpecVersion        application callbacks
  * @param pCompnentUUID
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE GetComponentVersion (OMX_HANDLETYPE hComp,
                                          OMX_STRING pComponentName,
                                          OMX_VERSIONTYPE* pComponentVersion,
                                          OMX_VERSIONTYPE* pSpecVersion,
                                          OMX_UUIDTYPE* pComponentUUID)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE*) hComp;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Entering GetComponentVersion\n", __LINE__);

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

    /* Copy component version structure */
    if(pComponentVersion != NULL && pComponentName != NULL)
    {
        strcpy(pComponentName, pComponentPrivate->cComponentName);
        memcpy(pComponentVersion, &(pComponentPrivate->ComponentVersion.s), sizeof(pComponentPrivate->ComponentVersion.s));
    }
    else
    {
        eError = OMX_ErrorBadParameter;
    }

EXIT:

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting GetComponentVersion\n", __LINE__);
    return eError;
}
/*-------------------------------------------------------------------*/
/**
  *  SendCommand() used to send the commands to the component
  *
  * This method will be used by the application.
  *
  * @param phandle         handle for this instance of the component
  * @param Cmd             Command to be sent to the component
  * @param nParam          indicates commmad is sent using this method
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE SendCommand (OMX_HANDLETYPE phandle,
                                  OMX_COMMANDTYPE Cmd,
                                  OMX_U32 nParam,OMX_PTR pCmdData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(phandle,1,1);        /*NOTE: Cmd,  pCmdData, nParam  are  NULL  for khronos*/
    int nRet = 0;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)phandle;
    AACENC_COMPONENT_PRIVATE *pCompPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

#ifdef _ERROR_PROPAGATION__
    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Entering SendCommand\n", __LINE__);
    if (pCompPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: SendCommand() : InvalidState\n", __LINE__);
        goto EXIT;
    }
#else
    OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Entering SendCommand()\n", __LINE__);
    if(pCompPrivate->curState == OMX_StateInvalid)
    {
           OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand\n",__LINE__);
           eError = OMX_ErrorInvalidState;
           OMX_ERROR4(pCompPrivate->dbg, "%d :: Error Notofication Sent to App\n",__LINE__);
           pCompPrivate->cbInfo.EventHandler(pHandle, pHandle->pApplicationPrivate,
                                             OMX_EventError,
                                             OMX_ErrorInvalidState,
                                             OMX_TI_ErrorMinor,
                                             "Invalid State");
           goto EXIT;
    }
#endif

#ifdef __PERF_INSTRUMENTATION__
    PERF_SendingCommand(pCompPrivate->pPERF,Cmd,
                        (Cmd == OMX_CommandMarkBuffer) ? ((OMX_U32) pCmdData) : nParam,
                        PERF_ModuleComponent);
#endif

    switch(Cmd)
    {
        case OMX_CommandStateSet:
            if (nParam == OMX_StateLoaded)
            {
                pCompPrivate->bLoadedCommandPending = OMX_TRUE;
            }

                OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand\n",__LINE__);
                OMX_PRSTATE1(pCompPrivate->dbg, "%d :: AACENC: pCompPrivate->curState = %d\n",__LINE__,pCompPrivate->curState);
                if(pCompPrivate->curState == OMX_StateLoaded)
                {
                    if((nParam == OMX_StateExecuting) || (nParam == OMX_StatePause))
                    {
                        pCompPrivate->cbInfo.EventHandler(pHandle,
                                                          pHandle->pApplicationPrivate,
                                                          OMX_EventError,
                                                          OMX_ErrorIncorrectStateTransition,
                                                          OMX_TI_ErrorMinor,
                                                          NULL);
                        goto EXIT;
                    }

                    if(nParam == OMX_StateInvalid)
                    {
                        OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand\n",__LINE__);
                        OMX_PRSTATE2(pCompPrivate->dbg, "AACENC: State changed to OMX_StateInvalid Line %d\n",__LINE__);
                        pCompPrivate->curState = OMX_StateInvalid;
                        pCompPrivate->cbInfo.EventHandler(pHandle,
                                                          pHandle->pApplicationPrivate,
                                                          OMX_EventError,
                                                          OMX_ErrorInvalidState,
                                                          OMX_TI_ErrorMinor,
                                                          NULL);
                        goto EXIT;
                    }
                }
                break;

        case OMX_CommandFlush:
                OMX_PRINT1(pCompPrivate->dbg, "%d :: IAACENC: nside SendCommand\n",__LINE__);
                if(nParam > 1 && nParam != -1)
                {
                    eError = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }
                break;

        case OMX_CommandPortDisable:
                OMX_PRDSP2(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand OMX_CommandPortDisable\n",__LINE__);
                break;

        case OMX_CommandPortEnable:
                OMX_PRDSP2(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand OMX_CommandPortEnable\n",__LINE__);
                break;

        case OMX_CommandMarkBuffer:
                OMX_PRDSP2(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand OMX_CommandMarkBuffer\n",__LINE__);
                if (nParam > 0)
                {
                    eError = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }
                break;

        default:
                OMX_ERROR4(pCompPrivate->dbg, "%d :: Error: Command Received Default error\n",__LINE__);
                pCompPrivate->cbInfo.EventHandler(pHandle, pHandle->pApplicationPrivate,
                                                  OMX_EventError,
                                                  OMX_ErrorBadParameter,
                                                  OMX_TI_ErrorMinor,
                                                  "Invalid Command");
                break;

    }

    if ( (pCompPrivate->curState == OMX_StateExecuting) && (nParam == OMX_StateIdle)  )
    {
        pCompPrivate->StateTransitiontoIdleFromExecOngoing = 1;
    }



    OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand\n",__LINE__);
    nRet = write (pCompPrivate->cmdPipe[1], &Cmd, sizeof(Cmd));
    OMX_PRCOMM2(pCompPrivate->dbg, "%d :: AACENC: Cmd pipe has been writen. nRet = %d  \n",__LINE__,nRet);
    OMX_PRCOMM2(pCompPrivate->dbg, "%d :: AACENC: pCompPrivate->cmdPipe[1] = %d  \n",__LINE__,pCompPrivate->cmdPipe[1]);
    OMX_PRCOMM2(pCompPrivate->dbg, "%d :: AACENC: &Cmd = %p  \n",__LINE__,&Cmd);

    if (nRet == -1)
    {
        OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Inside SendCommand\n",__LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    if (Cmd == OMX_CommandMarkBuffer)
    {
        nRet = write(pCompPrivate->cmdDataPipe[1], &pCmdData, sizeof(OMX_PTR));
    }
    else
    {
        nRet = write(pCompPrivate->cmdDataPipe[1], &nParam, sizeof(OMX_U32));
    }

    if (nRet == -1)
    {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

EXIT:
    OMX_PRINT1(pCompPrivate->dbg, "%d :: AACENC: Exiting SendCommand()\n", __LINE__);
    return eError;
}
/*-------------------------------------------------------------------*/
/**
  *  GetParameter() Gets the current configurations of the component
  *
  * @param hComp         handle for this instance of the component
  * @param nParamIndex
  * @param ComponentParameterStructure
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE GetParameter (OMX_HANDLETYPE hComp, OMX_INDEXTYPE nParamIndex, OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(hComp,1,1);
    AACENC_COMPONENT_PRIVATE  *pComponentPrivate = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE *pParameterStructure = NULL;

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Entering GetParameter\n", __LINE__);
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d :: AACENC: Inside the GetParameter nParamIndex = %x\n",__LINE__, nParamIndex);
    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComp)->pComponentPrivate);

    pParameterStructure = (OMX_PARAM_PORTDEFINITIONTYPE*)ComponentParameterStructure;
    if (pParameterStructure == NULL)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: OMX_ErrorBadParameter Inside the GetParameter Line\n",__LINE__);
        eError = OMX_ErrorBadParameter;
        goto EXIT;

    }

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#else
    if(pComponentPrivate->curState == OMX_StateInvalid)
    {
        pComponentPrivate->cbInfo.EventHandler(hComp,
                                                ((OMX_COMPONENTTYPE *)hComp)->pApplicationPrivate,
                                                OMX_EventError,
                                                OMX_ErrorIncorrectStateOperation,
                                                OMX_TI_ErrorMinor,
                                                "Invalid State");
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: AACENC: Inside the GetParameter Line\n",__LINE__);
    }
#endif
    switch(nParamIndex)
    {

        case OMX_IndexParamAudioInit:
             memcpy(ComponentParameterStructure, &pComponentPrivate->sPortParam, sizeof(OMX_PORT_PARAM_TYPE));
             break;

        case OMX_IndexParamPortDefinition:
            if(((OMX_PARAM_PORTDEFINITIONTYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->pPortDef[INPUT_PORT]->nPortIndex)
            {
                OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENC: Inside the GetParameter Line\n",__LINE__);
                memcpy(ComponentParameterStructure,pComponentPrivate->pPortDef[INPUT_PORT], sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            }
            else if(((OMX_PARAM_PORTDEFINITIONTYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->pPortDef[OUTPUT_PORT]->nPortIndex)
            {
                memcpy(ComponentParameterStructure, pComponentPrivate->pPortDef[OUTPUT_PORT], sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
                OMX_PRDSP1(pComponentPrivate->dbg, "%d :: AACENC: Inside the GetParameter \n",__LINE__);
            }
            else
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: BadPortIndex Inside the GetParameter \n",__LINE__);
                eError = OMX_ErrorBadPortIndex;
            }
            break;

        case OMX_IndexParamAudioPortFormat:
            if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->pPortDef[INPUT_PORT]->nPortIndex)
            {
                if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(ComponentParameterStructure))->nIndex > pComponentPrivate->sInPortFormat.nIndex)
                {
                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: ErrorNoMore Inside the GetParameter Line\n",__LINE__);
                    eError = OMX_ErrorNoMore;
                }
                else
                {
                    OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENC: About to copy Inside GetParameter \n",__LINE__);
                    memcpy(ComponentParameterStructure, &pComponentPrivate->sInPortFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                }
            }
            else if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->pPortDef[OUTPUT_PORT]->nPortIndex)
            {
                    if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(ComponentParameterStructure))->nIndex > pComponentPrivate->sOutPortFormat.nIndex)
                    {
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: ErrorNoMore Inside the GetParameter Line\n",__LINE__);
                        eError = OMX_ErrorNoMore;
                    }
                    else
                    {
                        OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENC: About to copy Inside GetParameter \n",__LINE__);
                        memcpy(ComponentParameterStructure, &pComponentPrivate->sOutPortFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                    }
            }
            else
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: BadPortIndex Inside the GetParameter Line\n",__LINE__);
                eError = OMX_ErrorBadPortIndex;
            }
            break;

        case OMX_IndexParamAudioAac:
            OMX_PRDSP2(pComponentPrivate->dbg, "AACENC: OMX_IndexParamAudioAac : nPortIndex = %d\n", (int)((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex);
            OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: acParams[INPUT_PORT]->nPortIndex = %d\n", (int)pComponentPrivate->aacParams[INPUT_PORT]->nPortIndex);
            OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: acParams[OUPUT_PORT]->nPortIndex = %d\n", (int)pComponentPrivate->aacParams[OUTPUT_PORT]->nPortIndex);

            if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->aacParams[INPUT_PORT]->nPortIndex)
            {

                if (pComponentPrivate->CustomConfiguration ) /* For Testapp: An  Index  was providded. Getting the required Structure */
                                                             /* The flag is set in Setconfig() function*/
                {
                    OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: OMX_IndexParamAudioAac :input port \n");
                    memcpy(ComponentParameterStructure,pComponentPrivate->aacParams[INPUT_PORT], sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));

                }
                else            /*for Khronos:  Getting the default structure (Ouput) for an  index not providded*/
                {
                    OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: OMX_IndexParamAudioAac :output port \n");
                    memcpy(ComponentParameterStructure, pComponentPrivate->aacParams[OUTPUT_PORT], sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
                }
            }
            else if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->aacParams[OUTPUT_PORT]->nPortIndex)
            {
                OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: OMX_IndexParamAudioAac :output port \n");
                memcpy(ComponentParameterStructure, pComponentPrivate->aacParams[OUTPUT_PORT], sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));

            }
            else
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: BadPortIndex Inside the GetParameter Line\n",__LINE__);
                eError = OMX_ErrorBadPortIndex;
            }
            break;


        case OMX_IndexParamAudioPcm:
            if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->pcmParam[INPUT_PORT]->nPortIndex)
            {
                memcpy(ComponentParameterStructure,pComponentPrivate->pcmParam[INPUT_PORT], sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
                OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: OMX_IndexParamAudioPcm :input port \n");
            }
            else if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex == pComponentPrivate->pcmParam[OUTPUT_PORT]->nPortIndex)
            {
                memcpy(ComponentParameterStructure, pComponentPrivate->pcmParam[OUTPUT_PORT], sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
                OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: OMX_IndexParamAudioPcm :output port \n");
            }
            else
            {
                eError = OMX_ErrorBadPortIndex;
            }
            break;

        case OMX_IndexParamCompBufferSupplier:
         if(((OMX_PARAM_BUFFERSUPPLIERTYPE *)(ComponentParameterStructure))->nPortIndex == OMX_DirInput)
         {
             OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: GetParameter OMX_IndexParamCompBufferSupplier \n");

         }
         else if(((OMX_PARAM_BUFFERSUPPLIERTYPE *)(ComponentParameterStructure))->nPortIndex == OMX_DirOutput)
         {
             OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: GetParameter OMX_IndexParamCompBufferSupplier \n");
         }
         else
         {
             OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: OMX_ErrorBadPortIndex from GetParameter",__LINE__);
             eError = OMX_ErrorBadPortIndex;
         }

            break;

#ifdef ANDROID
    case (OMX_INDEXTYPE) PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX:
    {
    OMX_PRDSP2(pComponentPrivate->dbg, "Entering PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX::%d\n", __LINE__);
        PV_OMXComponentCapabilityFlagsType* pCap_flags = (PV_OMXComponentCapabilityFlagsType *) ComponentParameterStructure;
        if (NULL == pCap_flags)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: ERROR PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX\n", __LINE__);
            eError =  OMX_ErrorBadParameter;
            goto EXIT;
        }
        OMX_PRDSP2(pComponentPrivate->dbg, "%d :: Copying PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX\n", __LINE__);
        memcpy(pCap_flags, &(pComponentPrivate->iPVCapabilityFlags), sizeof(PV_OMXComponentCapabilityFlagsType));
    eError = OMX_ErrorNone;
    }
    break;
#endif


        case OMX_IndexParamPriorityMgmt:
            break;


        case OMX_IndexParamVideoInit:
               break;

        case OMX_IndexParamImageInit:
               break;

        case OMX_IndexParamOtherInit:
               break;

        default:
            eError = OMX_ErrorUnsupportedIndex;
            break;
    }
EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting GetParameter:: %x  error :: %x \n",__LINE__,nParamIndex,eError);
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  SetParameter() Sets configuration paramets to the component
  *
  * @param hComp         handle for this instance of the component
  * @param nParamIndex
  * @param pCompParam
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE SetParameter (OMX_HANDLETYPE hComp, OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(hComp,1,1);
    AACENC_COMPONENT_PRIVATE  *pComponentPrivate = NULL;
    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComp)->pComponentPrivate);
    OMX_PARAM_COMPONENTROLETYPE  *pRole = NULL;
    OMX_PARAM_BUFFERSUPPLIERTYPE sBufferSupplier;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Entering the SetParameter()\n",__LINE__);
    OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: Inside the SetParameter nParamIndex = %x\n",__LINE__, nParamIndex);

    if (pCompParam == NULL)
    {
        eError = OMX_ErrorBadParameter;
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter \n",__LINE__);
        goto EXIT;
    }

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

    switch(nParamIndex)
    {
        case OMX_IndexParamAudioPortFormat:
            {
                OMX_AUDIO_PARAM_PORTFORMATTYPE *pComponentParam = (OMX_AUDIO_PARAM_PORTFORMATTYPE *)pCompParam;
                OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: pCompParam = index %d\n",__LINE__,(int)pComponentParam->nIndex);
                OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pCompParam = nportindex  %d\n",__LINE__,(int)pComponentParam->nPortIndex);
                /* for input port */
                if (pComponentParam->nPortIndex == 0)
                {
                    OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: OMX_IndexParamAudioPortFormat - index 0 \n",__LINE__);

                }
                else if (pComponentParam->nPortIndex == 1)
                {
                    OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: OMX_IndexParamAudioPortFormat - index 1 \n",__LINE__);
                }
               else
               {
                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Wrong Port Index Parameter\n", __LINE__);
                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
                    eError = OMX_ErrorBadParameter;
                    goto EXIT;
               }
               break;
            }

        case OMX_IndexParamAudioAac:
            OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENC: Inside the SetParameter - OMX_IndexParamAudioAac\n", __LINE__);
            if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(pCompParam))->nPortIndex ==
                                     pComponentPrivate->pPortDef[OUTPUT_PORT]->nPortIndex)
            {
                memcpy(pComponentPrivate->aacParams[OUTPUT_PORT], pCompParam,
                                               sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
                OMX_PRDSP2(pComponentPrivate->dbg, "AACENC: nSampleRate %ld\n",pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate);

            }
            else if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(pCompParam))->nPortIndex ==
                                     pComponentPrivate->pPortDef[INPUT_PORT]->nPortIndex)
            {
                                        memcpy(pComponentPrivate->aacParams[INPUT_PORT], pCompParam,
                                               sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
            }
            else
            {
                eError = OMX_ErrorBadPortIndex;
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in SetParameter - OMX_IndexParamAudioAac = %x\n", __LINE__, eError);
            }
        break;

        case OMX_IndexParamAudioPcm:
            if(((OMX_AUDIO_PARAM_PCMMODETYPE *)(pCompParam))->nPortIndex ==
                    pComponentPrivate->pcmParam[INPUT_PORT]->nPortIndex)
            {
                memcpy(pComponentPrivate->pcmParam[INPUT_PORT],pCompParam,
                       sizeof(OMX_AUDIO_PARAM_PCMMODETYPE)
                      );
                OMX_PRDSP2(pComponentPrivate->dbg, "AACENC: Interleave mode %ld\n",pComponentPrivate->pcmParam[INPUT_PORT]->bInterleaved);
            }
            else if(((OMX_AUDIO_PARAM_PCMMODETYPE *)(pCompParam))->nPortIndex ==
                    pComponentPrivate->pcmParam[OUTPUT_PORT]->nPortIndex)
            {
                memcpy(pComponentPrivate->pcmParam[OUTPUT_PORT],pCompParam,
                       sizeof(OMX_AUDIO_PARAM_PCMMODETYPE)
                      );

            }
            else
            {
                eError = OMX_ErrorBadPortIndex;
            }
        break;

        case OMX_IndexParamPortDefinition:
            if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pCompParam))->nPortIndex == pComponentPrivate->pPortDef[INPUT_PORT]->nPortIndex)
            {
                memcpy(pComponentPrivate->pPortDef[INPUT_PORT], pCompParam, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            }
            else if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pCompParam))->nPortIndex == pComponentPrivate->pPortDef[OUTPUT_PORT]->nPortIndex)
            {
                memcpy(pComponentPrivate->pPortDef[OUTPUT_PORT], pCompParam, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

            }
            else
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Wrong Port Index Parameter\n", __LINE__);
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
                eError = OMX_ErrorBadPortIndex;
                goto EXIT;
            }
            break;

        case OMX_IndexParamPriorityMgmt:
            if (pComponentPrivate->curState != OMX_StateLoaded)
            {
                eError = OMX_ErrorIncorrectStateOperation;
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorIncorrectStateOperation\n",__LINE__);
            }
            break;

        case OMX_IndexParamStandardComponentRole:
            if (pCompParam)
            {
                pRole = (OMX_PARAM_COMPONENTROLETYPE *)pCompParam;
                memcpy(&(pComponentPrivate->componentRole), (void *)pRole, sizeof(OMX_PARAM_COMPONENTROLETYPE));
            }
            else
            {
                eError = OMX_ErrorBadParameter;
            }
            break;


         case OMX_IndexParamCompBufferSupplier:
            if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pCompParam))->nPortIndex ==
                                    pComponentPrivate->pPortDef[INPUT_PORT]->nPortIndex)
            {
                    OMX_PRBUFFER1(pComponentPrivate->dbg, "AACENC: SetParameter OMX_IndexParamCompBufferSupplier \n");
                                   sBufferSupplier.eBufferSupplier = OMX_BufferSupplyInput;
                                   memcpy(&sBufferSupplier, pCompParam, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));

            }
            else if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pCompParam))->nPortIndex ==
                              pComponentPrivate->pPortDef[OUTPUT_PORT]->nPortIndex)
            {
                OMX_PRBUFFER1(pComponentPrivate->dbg, "AACENC: SetParameter OMX_IndexParamCompBufferSupplier \n");
                sBufferSupplier.eBufferSupplier = OMX_BufferSupplyOutput;
                memcpy(&sBufferSupplier, pCompParam, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
            }
            else
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d:: Error: OMX_ErrorBadPortIndex from SetParameter",__LINE__);
                eError = OMX_ErrorBadPortIndex;
            }
            break;




        default:
            break;
    }
EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting SetParameter:: %x\n",__LINE__,nParamIndex);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting the SetParameter() returned eError = %d\n",__LINE__, eError);
    return eError;
}
/*-------------------------------------------------------------------*/
/**
  *  GetConfig() Gets the current configuration of to the component
  *
  * @param hComp         handle for this instance of the component
  * @param nConfigIndex
  * @param ComponentConfigStructure
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE GetConfig (OMX_HANDLETYPE hComp, OMX_INDEXTYPE nConfigIndex, OMX_PTR ComponentConfigStructure)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate =  NULL;

    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)
            (((OMX_COMPONENTTYPE*)hComp)->pComponentPrivate);

    if ( nConfigIndex == OMX_IndexCustomAacEncTNSConfig)
     {
       memcpy(ComponentConfigStructure,&(pComponentPrivate->ia_heaacv2_enc_params.useTns), sizeof(OMX_S32));
     }
    if ( nConfigIndex == OMX_IndexCustomAacEncQualityLevel)
     {
       memcpy(ComponentConfigStructure,&(pComponentPrivate->ia_heaacv2_enc_params.invQuant), sizeof(OMX_S32));
     }
    if ( nConfigIndex == OMX_IndexCustomAacEncFrameMode)
     {
       memcpy(ComponentConfigStructure,&(pComponentPrivate->framemode), sizeof(OMX_S32));
     }
#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

EXIT:    return eError;
}
/*-------------------------------------------------------------------*/
/**
  *  SetConfig() Sets the configuration of the component
  *
  * @param hComp         handle for this instance of the component
  * @param nConfigIndex
  * @param ComponentConfigStructure
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE SetConfig (OMX_HANDLETYPE hComp, OMX_INDEXTYPE nConfigIndex, OMX_PTR ComponentConfigStructure)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE*)hComp;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_S32 *value = (OMX_S32 *)ComponentConfigStructure;



    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Entering SetConfig\n", __LINE__);
    if (pHandle == NULL)
    {
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d :: AACENC: Invalid HANDLE OMX_ErrorBadParameter \n",__LINE__);
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

    switch (nConfigIndex)
    {
        case OMX_IndexCustomAacEncTNSConfig:
        {
           if ( (*value != 0) && (*value != 1))
            {
                eError = OMX_ErrorBadParameter;
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: OMX_ErrorBadParameter from SetConfig\n",__LINE__);
                goto EXIT;
            }
            pComponentPrivate->ia_heaacv2_enc_params.useTns = *value ; // User input for TNS
            OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC TNS %d \n", __LINE__,(pComponentPrivate->ia_heaacv2_enc_params.useTns));
            break;
        }
        case  OMX_IndexCustomAacEncQualityLevel:
        {
            if ( (*value != 0) && (*value != 1) && (*value != 2))
            {
                eError = OMX_ErrorBadParameter;
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: OMX_ErrorBadParameter from SetConfig\n",__LINE__);
                goto EXIT;
            }
            pComponentPrivate->ia_heaacv2_enc_params.invQuant = *value; // User input for quality level
            OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC Quality Level %d \n", __LINE__,(pComponentPrivate->ia_heaacv2_enc_params.invQuant));
            break;
        }
        case  OMX_IndexCustomAacEncFrameMode:
        {
            if ( (*value != 0) && (*value != 1))
            {
                eError = OMX_ErrorBadParameter;
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: OMX_ErrorBadParameter from SetConfig\n",__LINE__);
                goto EXIT;
            }
            pComponentPrivate->framemode = *((OMX_U16 *)value); // User input for frame mode
            OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC FrameMode %d \n", __LINE__,(pComponentPrivate->framemode));
            break;
        }

        default:
        {
/*          eError = OMX_ErrorUnsupportedIndex; */
        break;
    }
    }
EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting SetConfig\n", __LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Returning = 0x%x\n",__LINE__,eError);
    return eError;


}
/*-------------------------------------------------------------------*/
/**
  *  GetState() Gets the current state of the component
  *
  * @param pCompomponent handle for this instance of the component
  * @param pState
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE GetState (OMX_HANDLETYPE pComponent, OMX_STATETYPE* pState)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_CONF_CHECK_CMD(pComponent,1,1);
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Entering GetState\n", __LINE__);
    if (!pState)
    {
        eError = OMX_ErrorBadParameter;
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d :: Error: About to return OMX_ErrorBadParameter \n",__LINE__);
        goto EXIT;
    }

    if (pHandle && pHandle->pComponentPrivate)
    {
        *pState =  ((AACENC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate)->curState;
    }
    else
    {
        *pState = OMX_StateLoaded;
    }
    eError = OMX_ErrorNone;

EXIT:
    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Exiting GetState\n", __LINE__);
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  EmptyThisBuffer() This callback is used to send the input buffer to
  *  component
  *
  * @param pComponent       handle for this instance of the component
  * @param nPortIndex       input port index
  * @param pBuffer          buffer to be sent to codec
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE EmptyThisBuffer (OMX_HANDLETYPE pComponent,
                                      OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(pComponent,pBuffer,1);
    int ret = 0;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef = NULL;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Entering EmptyThisBuffer\n", __LINE__);

    pPortDef = ((AACENC_COMPONENT_PRIVATE*)
                    pComponentPrivate)->pPortDef[INPUT_PORT];

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

#ifdef __PERF_INSTRUMENTATION__
        PERF_ReceivedFrame(pComponentPrivate->pPERF,pBuffer->pBuffer, pBuffer->nFilledLen,
                           PERF_ModuleHLMM);
#endif

    OMX_PRBUFFER1(pComponentPrivate->dbg, "AACENC: pBuffer->nSize %d \n",(int)pBuffer->nSize);
    OMX_PRBUFFER1(pComponentPrivate->dbg, "AACENC: size OMX_BUFFERHEADERTYPE %d \n",sizeof(OMX_BUFFERHEADERTYPE));

    if(!pPortDef->bEnabled)
    {
        eError = OMX_ErrorIncorrectStateOperation;
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorIncorrectStateOperation\n",__LINE__);
        goto EXIT;
    }
    if (pBuffer == NULL)
    {
        eError = OMX_ErrorBadParameter;
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
        goto EXIT;
    }
    if (pBuffer->nSize != sizeof(OMX_BUFFERHEADERTYPE))
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }


    if (pBuffer->nVersion.nVersion != pComponentPrivate->nVersion)
    {
        eError = OMX_ErrorVersionMismatch;
        goto EXIT;
    }

    if (pBuffer->nInputPortIndex != INPUT_PORT)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadPortIndex\n",__LINE__);
        eError  = OMX_ErrorBadPortIndex;
        goto EXIT;
    }
    OMX_PRBUFFER2(pComponentPrivate->dbg, "\n------------------------------------------\n\n");
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: Component Sending Filled ip buff %p  to Component Thread\n", __LINE__,pBuffer);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "\n------------------------------------------\n\n");

    pComponentPrivate->pMarkData = pBuffer->pMarkData;
    pComponentPrivate->hMarkTargetComponent = pBuffer->hMarkTargetComponent;

    pComponentPrivate->nUnhandledEmptyThisBuffers++;
    ret = write (pComponentPrivate->dataPipeInputBuf[1], &pBuffer, sizeof(OMX_BUFFERHEADERTYPE*));
    if (ret == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Writing to the Input Buffer Data pipe\n", __LINE__);
        eError = OMX_ErrorHardware;
        goto EXIT;
    }
    pComponentPrivate->EmptythisbufferCount++;

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting EmptyThisBuffer\n", __LINE__);
    return eError;
}
/*-------------------------------------------------------------------*/
/**
  *  FillThisBuffer() This callback is used to send the output buffer to
  *  the component
  *
  * @param pComponent    handle for this instance of the component
  * @param nPortIndex    output port number
  * @param pBuffer       buffer to be sent to codec
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE FillThisBuffer (OMX_HANDLETYPE pComponent,
                                     OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(pComponent,pBuffer,1);
    int ret = 0;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate =
                         (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef = NULL;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Entering FillThisBuffer\n", __LINE__);

    OMX_PRBUFFER2(pComponentPrivate->dbg, "\n------------------------------------------\n\n");
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: Component Sending Emptied op buff %p to Component Thread\n",__LINE__,pBuffer);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "\n------------------------------------------\n\n");
    pPortDef = ((AACENC_COMPONENT_PRIVATE*)
                    pComponentPrivate)->pPortDef[OUTPUT_PORT];

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

#ifdef __PERF_INSTRUMENTATION__
    PERF_ReceivedFrame(pComponentPrivate->pPERF,pBuffer->pBuffer,0,PERF_ModuleHLMM);
#endif

    if(!pPortDef->bEnabled)
    {
        eError = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }
    if (pBuffer == NULL)
    {
        eError = OMX_ErrorBadParameter;
        OMX_ERROR4(pComponentPrivate->dbg, " %d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
        goto EXIT;
    }
    if (pBuffer->nSize != sizeof(OMX_BUFFERHEADERTYPE))
    {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    if (pBuffer->nVersion.nVersion != pComponentPrivate->nVersion)
    {
        eError = OMX_ErrorVersionMismatch;
        goto EXIT;
    }

    if (pBuffer->nOutputPortIndex != OUTPUT_PORT)
    {
        eError  = OMX_ErrorBadPortIndex;
        goto EXIT;
    }
    pBuffer->nFilledLen = 0;
    /*Filling the Output buffer with zero */
#ifndef UNDER_CE
    /*memset(pBuffer->pBuffer, 0, pBuffer->nAllocLen);*/
#endif

    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pMarkBuf = %p\n",__LINE__, pComponentPrivate->pMarkBuf);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pMarkData = %p\n",__LINE__, pComponentPrivate->pMarkData);
    if(pComponentPrivate->pMarkBuf)
    {
        pBuffer->hMarkTargetComponent = pComponentPrivate->pMarkBuf->hMarkTargetComponent;
        pBuffer->pMarkData = pComponentPrivate->pMarkBuf->pMarkData;
        pComponentPrivate->pMarkBuf = NULL;
    }

    if (pComponentPrivate->pMarkData)
    {
        pBuffer->hMarkTargetComponent = pComponentPrivate->hMarkTargetComponent;
        pBuffer->pMarkData = pComponentPrivate->pMarkData;
        pComponentPrivate->pMarkData = NULL;
    }

    pComponentPrivate->nUnhandledFillThisBuffers++;
    ret = write (pComponentPrivate->dataPipeOutputBuf[1], &pBuffer, sizeof (OMX_BUFFERHEADERTYPE*));
    if (ret == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Writing to the Output Buffer Data pipe\n", __LINE__);
        eError = OMX_ErrorHardware;
        goto EXIT;
    }
    pComponentPrivate->FillthisbufferCount++;

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting FillThisBuffer error= %d \n", __LINE__,eError);
    return eError;
}
/*-------------------------------------------------------------------*/
/**
  * OMX_ComponentDeinit() this methold will de init the component
  *
  * @param pComp         handle for this instance of the component
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE pHandle)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(pHandle,1,1);
    OMX_COMPONENTTYPE *pComponent = (OMX_COMPONENTTYPE *)pHandle;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pComponent->pComponentPrivate;
    struct OMX_TI_Debug dbg;
    dbg = pComponentPrivate->dbg;

    OMX_PRINT1(dbg, "%d :: AACENC: ComponentDeInit\n",__LINE__);

#ifdef __PERF_INSTRUMENTATION__
    PERF_Boundary(pComponentPrivate->pPERF,PERF_BoundaryStart | PERF_BoundaryCleanup);
#endif


#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM
    eError = RMProxy_NewSendCommand(pHandle, RMProxy_FreeResource, OMX_AAC_Encoder_COMPONENT, 0, 3456, NULL);
    if (eError != OMX_ErrorNone)
    {
         OMX_ERROR4(dbg, "%d :: Error returned from destroy ResourceManagerProxy thread\n",
                                                        __LINE__);
    }

    eError = RMProxy_Deinitalize();
    if (eError != OMX_ErrorNone)
    {
         OMX_ERROR4(dbg, "%d :: Error returned from destroy ResourceManagerProxy thread\n",__LINE__);
    }
#endif

#ifdef SWAT_ANALYSIS
    SWATAPI_ReleaseHandle(pComponentPrivate->pSwatInfo->pSwatApiHandle);
    SWAT_Boundary(pComponentPrivate->pSwatInfo->pSwatObjHandle,
                          pComponentPrivate->pSwatInfo->ctUC,
                          SWAT_BoundaryComplete | SWAT_BoundaryCleanup);
    OMX_PRINT2(dbg, "%d :: AACENC: Instrumentation: SWAT_BoundaryComplete Done\n",__LINE__);
    SWAT_Done(pComponentPrivate->pSwatInfo->pSwatObjHandle);
#endif

    OMX_PRINT2(dbg, "%d :: AACENC: Inside ComponentDeInit point A \n",__LINE__);
    pComponentPrivate->bIsThreadstop = 1;
    eError = AACENC_StopComponentThread(pHandle);
    OMX_PRINT2(dbg, "%d :: AACENC: Inside ComponentDeInit Point B \n",__LINE__);
    /* Wait for thread to exit so we can get the status into "error" */

    /* close the pipe handles */
    AACENC_FreeCompResources(pHandle);

#ifdef __PERF_INSTRUMENTATION__
        PERF_Boundary(pComponentPrivate->pPERF,
                      PERF_BoundaryComplete | PERF_BoundaryCleanup);
        PERF_Done(pComponentPrivate->pPERF);
#endif

    OMX_MEMFREE_STRUCT(pComponentPrivate->pInputBufferList);
    OMX_MEMFREE_STRUCT(pComponentPrivate->pOutputBufferList);
    OMX_PRBUFFER2(dbg, "%d :: AACENC: After AACENC_FreeCompResources\n",__LINE__);
    OMX_PRBUFFER2(dbg, "%d :: AACENC: [FREE] %p\n",__LINE__,pComponentPrivate);

    if (pComponentPrivate->sDeviceString != NULL)
    {
        OMX_MEMFREE_STRUCT(pComponentPrivate->sDeviceString);
    }
    OMX_MEMFREE_STRUCT(pComponentPrivate);
    pComponentPrivate = NULL;

EXIT:
    OMX_DBG_CLOSE(dbg);
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  ComponentTunnelRequest() this method is not implemented in 1.5
  *
  * This method will update application callbacks
  * the application.
  *
  * @param pComp         handle for this instance of the component
  * @param pCallBacks    application callbacks
  * @param ptr
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE ComponentTunnelRequest (OMX_HANDLETYPE hComp,
                                             OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp,
                                             OMX_U32 nTunneledPort,
                                             OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    eError = OMX_ErrorNotImplemented;
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  AllocateBuffer()

  * @param pComp         handle for this instance of the component
  * @param pCallBacks    application callbacks
  * @param ptr
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/

static OMX_ERRORTYPE AllocateBuffer (OMX_IN OMX_HANDLETYPE hComponent,
                   OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
                   OMX_IN OMX_U32 nPortIndex,
                   OMX_IN OMX_PTR pAppPrivate,
                   OMX_IN OMX_U32 nSizeBytes)

{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef= NULL;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate= NULL;

    OMX_BUFFERHEADERTYPE *pBufferHeader = NULL;
    OMX_CONF_CHECK_CMD(hComponent,pBuffer,1);

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Entering AllocateBuffer\n", __LINE__);
    OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: AACENC: pBuffer = %p\n", __LINE__,pBuffer);
    OMXDBG_PRINT(stderr, COMM, 2, 0, "AACENC: nPortIndex = %d - %p \n",(int)nPortIndex,&nPortIndex);
    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)
            (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    pPortDef = ((AACENC_COMPONENT_PRIVATE*)
                    pComponentPrivate)->pPortDef[nPortIndex];

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }
#endif

#ifdef __PERF_INSTRUMENTATION__
    PERF_ReceivedBuffer(pComponentPrivate->pPERF,(*pBuffer)->pBuffer,nSizeBytes,PERF_ModuleMemory);
#endif

    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pPortDef = %p\n", __LINE__, pPortDef);
    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pPortDef->bEnabled = %d\n", __LINE__, pPortDef->bEnabled);
    while (1)
    {
        if(pPortDef->bEnabled)
        {
            break;
        }
        pComponentPrivate->AlloBuf_waitingsignal = 1;

#ifndef UNDER_CE
        pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
        pthread_cond_wait(&pComponentPrivate->AlloBuf_threshold, &pComponentPrivate->AlloBuf_mutex);
        pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);
#else
        OMX_WaitForEvent(&(pComponentPrivate->AlloBuf_event));
#endif
        break;

    }

    OMX_MALLOC_STRUCT(pBufferHeader, OMX_BUFFERHEADERTYPE);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pBufferHeader %p\n",pBufferHeader);

    OMX_MALLOC_STRUCT_SIZE(pBufferHeader->pBuffer, nSizeBytes + 256, OMX_U8);

    pBufferHeader->pBuffer += 128;
    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pBufferHeader->pbuffer %p to %p \n",pBufferHeader->pBuffer,(pBufferHeader->pBuffer + sizeof(pBufferHeader->pBuffer)) );

    if (nPortIndex == INPUT_PORT)
    {
        pBufferHeader->nInputPortIndex = nPortIndex;
        pBufferHeader->nOutputPortIndex = -1;
        pComponentPrivate->pInputBufferList->pBufHdr[pComponentPrivate->pInputBufferList->numBuffers] = pBufferHeader;
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pInputBufferList->pBufHdr[%d] = %p\n",__LINE__, pComponentPrivate->pInputBufferList->numBuffers,pComponentPrivate->pInputBufferList->pBufHdr[pComponentPrivate->pInputBufferList->numBuffers]);
        pComponentPrivate->pInputBufferList->bBufferPending[pComponentPrivate->pInputBufferList->numBuffers] = 0;
        pComponentPrivate->pInputBufferList->bufferOwner[pComponentPrivate->pInputBufferList->numBuffers++] = 1;
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pInputBufferList->numBuffers = %d \n",__LINE__, pComponentPrivate->pInputBufferList->numBuffers);
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pPortDef->nBufferCountMin = %ld \n",__LINE__, pPortDef->nBufferCountMin);
        if (pComponentPrivate->pInputBufferList->numBuffers == pPortDef->nBufferCountActual)
        {
            pPortDef->bPopulated = OMX_TRUE;
        }
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: INPUT PORT - pPortDef->bPopulated = %d\n",__LINE__, pPortDef->bPopulated);
    }
    else if (nPortIndex == OUTPUT_PORT)
    {
        pBufferHeader->nInputPortIndex = -1;
        pBufferHeader->nOutputPortIndex = nPortIndex;
        pComponentPrivate->pOutputBufferList->pBufHdr[pComponentPrivate->pOutputBufferList->numBuffers] = pBufferHeader;
        pComponentPrivate->pOutputBufferList->bBufferPending[pComponentPrivate->pOutputBufferList->numBuffers] = 0;
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pOutputBufferList->pBufHdr[%d] = %p\n",__LINE__, pComponentPrivate->pOutputBufferList->numBuffers,pComponentPrivate->pOutputBufferList->pBufHdr[pComponentPrivate->pOutputBufferList->numBuffers]);
        pComponentPrivate->pOutputBufferList->bufferOwner[pComponentPrivate->pOutputBufferList->numBuffers++] = 1;
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pOutputBufferList->numBuffers = %d \n",__LINE__, pComponentPrivate->pOutputBufferList->numBuffers);
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pPortDef->nBufferCountMin = %ld \n",__LINE__, pPortDef->nBufferCountMin);
        if (pComponentPrivate->pOutputBufferList->numBuffers == pPortDef->nBufferCountActual)
        {
            pPortDef->bPopulated = OMX_TRUE;
        }
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: OUTPUT PORT - pPortDef->bPopulated = %d\n",__LINE__, pPortDef->bPopulated);
    }
    else
    {
        eError = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

  if((pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated == pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled)&&
       (pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated == pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled) &&
       (pComponentPrivate->InLoaded_readytoidle))
    {
        pComponentPrivate->InLoaded_readytoidle = 0;
#ifndef UNDER_CE
        pthread_mutex_lock(&pComponentPrivate->InLoaded_mutex);
        pthread_cond_signal(&pComponentPrivate->InLoaded_threshold);
        pthread_mutex_unlock(&pComponentPrivate->InLoaded_mutex);
#else
        OMX_SignalEvent(&(pComponentPrivate->InLoaded_event));
#endif
    }

    pBufferHeader->pAppPrivate = pAppPrivate;
    pBufferHeader->pPlatformPrivate = pComponentPrivate;
    pBufferHeader->nAllocLen = nSizeBytes;
    pBufferHeader->nVersion.s.nVersionMajor = AACENC_MAJOR_VER;
    pBufferHeader->nVersion.s.nVersionMinor = AACENC_MINOR_VER;
    pComponentPrivate->nVersion = pBufferHeader->nVersion.nVersion;
    pBufferHeader->nSize = sizeof(OMX_BUFFERHEADERTYPE);
    *pBuffer = pBufferHeader;
    if (pComponentPrivate->bEnableCommandPending && pPortDef->bPopulated)
    {
        SendCommand (pComponentPrivate->pHandle,OMX_CommandPortEnable,pComponentPrivate->nEnableCommandParam,NULL);
    }

#ifdef __PERF_INSTRUMENTATION__
        PERF_ReceivedBuffer(pComponentPrivate->pPERF,(*pBuffer)->pBuffer, nSizeBytes,PERF_ModuleMemory);
#endif

EXIT:
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC:  AllocateBuffer returning eError =  %d\n",__LINE__,eError);

    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pBufferHeader = %p\n",pBufferHeader);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pBufferHeader->pBuffer = %p\n",pBufferHeader->pBuffer);
    return eError;
}

/*-------------------------------------------------------------------*/
/**
  *  FreeBuffer()

  * @param hComponent   handle for this instance of the component
  * @param pCallBacks   application callbacks
  * @param ptr
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
  **/
/*-------------------------------------------------------------------*/
static OMX_ERRORTYPE FreeBuffer(OMX_IN  OMX_HANDLETYPE hComponent,
                                OMX_IN  OMX_U32 nPortIndex,
                                OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(hComponent,1,pBuffer);
    AACENC_COMPONENT_PRIVATE * pComponentPrivate = NULL;
    OMX_BUFFERHEADERTYPE* buff = NULL;
    OMX_U8* tempBuff = NULL;
    int i =0;
    int inputIndex = -1;
    int outputIndex = -1;
    OMX_COMPONENTTYPE *pHandle = NULL;

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: FreeBuffer\n", __LINE__);
    OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: AACENC: pBuffer = %p\n",__LINE__,pBuffer);
    OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: AACENC: pBuffer->pBuffer = %p\n",__LINE__,pBuffer->pBuffer);
    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate = %p\n", __LINE__,pComponentPrivate);
    for (i=0; i < MAX_NUM_OF_BUFS; i++)
    {
        buff = pComponentPrivate->pInputBufferList->pBufHdr[i];
        if (buff == pBuffer)
        {
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: Found matching input buffer\n",__LINE__);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: buff = %p\n",__LINE__,buff);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pBuffer = %p\n",__LINE__,pBuffer);
            inputIndex = i;
            break;
        }
        else
        {
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: This is not a match\n",__LINE__);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: buff = %p\n",__LINE__,buff);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pBuffer = %p\n",__LINE__,pBuffer);
        }
    }

    for (i=0; i < MAX_NUM_OF_BUFS; i++)
    {
        buff = pComponentPrivate->pOutputBufferList->pBufHdr[i];
        if (buff == pBuffer)
        {
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: Found matching output buffer\n",__LINE__);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: buff = %p\n",__LINE__,buff);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pBuffer = %p\n",__LINE__,pBuffer);
            outputIndex = i;
            break;
        }
        else
        {
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: This is not a match\n",__LINE__);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: buff = %p\n",__LINE__,buff);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pBuffer = %p\n",__LINE__,pBuffer);
        }
    }


    if (inputIndex != -1)
    {
        if (pComponentPrivate->pInputBufferList->bufferOwner[inputIndex] == 1)
        {
            tempBuff = pComponentPrivate->pInputBufferList->pBufHdr[inputIndex]->pBuffer;
            if (tempBuff != 0)
            {
                tempBuff -= 128;
            }
            OMX_MEMFREE_STRUCT(tempBuff);
        }
#ifdef __PERF_INSTRUMENTATION__
            PERF_SendingBuffer(pComponentPrivate->pPERF,
                               pComponentPrivate->pInputBufferList->pBufHdr[inputIndex]->pBuffer,
                               pComponentPrivate->pInputBufferList->pBufHdr[inputIndex]->nAllocLen,
                               PERF_ModuleMemory );

#endif

        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: [FREE] %p\n",__LINE__,pComponentPrivate->pBufHeader[INPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->pInputBufferList->pBufHdr[inputIndex]);
        pComponentPrivate->pInputBufferList->numBuffers--;

        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pInputBufferList->numBuffers = %d \n",__LINE__,pComponentPrivate->pInputBufferList->numBuffers);
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pPortDef[INPUT_PORT]->nBufferCountMin = %ld \n",__LINE__,pComponentPrivate->pPortDef[INPUT_PORT]->nBufferCountMin);
        if (pComponentPrivate->pInputBufferList->numBuffers < pComponentPrivate->pPortDef[INPUT_PORT]->nBufferCountActual)
        {

            pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated = OMX_FALSE;
        }

        if(pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled &&
            pComponentPrivate->bLoadedCommandPending == OMX_FALSE &&
            (pComponentPrivate->curState == OMX_StateIdle ||
            pComponentPrivate->curState == OMX_StateExecuting ||
            pComponentPrivate->curState == OMX_StatePause))
        {
            OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: AACENC: PortUnpopulated\n",__LINE__);
            pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventError,
                                                    OMX_ErrorPortUnpopulated,
                                                    OMX_TI_ErrorMinor,
                                                    "Input Port Unpopulated");
        }
    }
    else if (outputIndex != -1)
    {
        if (pComponentPrivate->pOutputBufferList->bufferOwner[outputIndex] == 1)
        {
            tempBuff = pComponentPrivate->pOutputBufferList->pBufHdr[outputIndex]->pBuffer;
            if (tempBuff != 0)
            {
               tempBuff -= 128;
            }
            OMX_MEMFREE_STRUCT(tempBuff);
        }
#ifdef __PERF_INSTRUMENTATION__
            PERF_SendingBuffer(pComponentPrivate->pPERF,
                               pComponentPrivate->pOutputBufferList->pBufHdr[outputIndex]->pBuffer,
                               pComponentPrivate->pOutputBufferList->pBufHdr[outputIndex]->nAllocLen,
                               PERF_ModuleMemory);

#endif

        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: [FREE] %p\n",__LINE__,pComponentPrivate->pBufHeader[OUTPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->pOutputBufferList->pBufHdr[outputIndex]);
        pComponentPrivate->pOutputBufferList->pBufHdr[outputIndex] = NULL;
        pComponentPrivate->pOutputBufferList->numBuffers--;

        OMX_PRBUFFER1(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pOutputBufferList->numBuffers = %d \n",__LINE__,pComponentPrivate->pOutputBufferList->numBuffers);
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pPortDef[OUTPUT_PORT]->nBufferCountMin = %ld \n",__LINE__,pComponentPrivate->pPortDef[OUTPUT_PORT]->nBufferCountMin);
        if (pComponentPrivate->pOutputBufferList->numBuffers <
            pComponentPrivate->pPortDef[OUTPUT_PORT]->nBufferCountActual)
        {

            pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated = OMX_FALSE;
        }
        if(pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled &&
            pComponentPrivate->bLoadedCommandPending == OMX_FALSE &&
            (pComponentPrivate->curState == OMX_StateIdle ||
            pComponentPrivate->curState == OMX_StateExecuting ||
            pComponentPrivate->curState == OMX_StatePause))
        {
            OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: AACENC: PortUnpopulated\n",__LINE__);
            pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventError,
                                                    OMX_ErrorPortUnpopulated,
                                                    OMX_TI_ErrorMinor,
                                                    "Output Port Unpopulated");
        }
    }
    else
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Returning OMX_ErrorBadParameter\n",__LINE__);
        eError = OMX_ErrorBadParameter;
    }
    if ((!pComponentPrivate->pInputBufferList->numBuffers &&
         !pComponentPrivate->pOutputBufferList->numBuffers) &&
         pComponentPrivate->InIdle_goingtoloaded)
    {
        pComponentPrivate->InIdle_goingtoloaded = 0;
#ifndef UNDER_CE
        pthread_mutex_lock(&pComponentPrivate->InIdle_mutex);
        pthread_cond_signal(&pComponentPrivate->InIdle_threshold);
        pthread_mutex_unlock(&pComponentPrivate->InIdle_mutex);
#else
        OMX_SignalEvent(&(pComponentPrivate->InIdle_event));
#endif
    }

    if (pComponentPrivate->bDisableCommandPending && (pComponentPrivate->pInputBufferList->numBuffers + pComponentPrivate->pOutputBufferList->numBuffers == 0))
    {
        SendCommand (pComponentPrivate->pHandle,OMX_CommandPortDisable,pComponentPrivate->bDisableCommandParam,NULL);
    }

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting FreeBuffer\n", __LINE__);
EXIT:
    return eError;
}

/* ================================================================================= */
/**
* @fn UseBuffer() description for UseBuffer
UseBuffer().
Called by the OMX IL client to pass a buffer to be used.
*
*  @see         OMX_Core.h
*/
/* ================================================================================ */
static OMX_ERRORTYPE UseBuffer (OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDef= NULL;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_BUFFERHEADERTYPE *pBufferHeader = NULL;
    OMX_CONF_CHECK_CMD(hComponent,ppBufferHdr,pBuffer);

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: Entering UseBuffer\n", __LINE__);
    OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: AACENC: pBuffer = %p\n", __LINE__,pBuffer);
    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)
            (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

#ifdef _ERROR_PROPAGATION__
    if (pComponentPrivate->curState == OMX_StateInvalid)
    {
        eError = OMX_ErrorInvalidState;
        goto EXIT;
    }

#endif

#ifdef __PERF_INSTRUMENTATION__
        PERF_ReceivedBuffer(pComponentPrivate->pPERF,pBuffer, nSizeBytes,
                            PERF_ModuleHLMM);
#endif

    pPortDef = ((AACENC_COMPONENT_PRIVATE*)
                    pComponentPrivate)->pPortDef[nPortIndex];
    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pPortDef = %p\n", __LINE__,pPortDef);
    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: AACENC: pPortDef->bEnabled = %d\n", __LINE__,pPortDef->bEnabled);

    if(!pPortDef->bEnabled)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: In AllocateBuffer\n", __LINE__);
        eError = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    OMX_PRINT2(pComponentPrivate->dbg, "AACENC: nSizeBytes = %ld\n",nSizeBytes);
    OMX_PRBUFFER1(pComponentPrivate->dbg, "AACENC: pPortDef->nBufferSize = %ld\n",pPortDef->nBufferSize);
    OMX_PRBUFFER1(pComponentPrivate->dbg, "AACENC: pPortDef->bPopulated = %d\n",pPortDef->bPopulated);
    if(nSizeBytes != pPortDef->nBufferSize || pPortDef->bPopulated)
    {
        OMX_ERROR2(pComponentPrivate->dbg, "%d :: Error: In AllocateBuffer\n", __LINE__);
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: About to return OMX_ErrorBadParameter\n",__LINE__);
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    OMX_MALLOC_STRUCT(pBufferHeader, OMX_BUFFERHEADERTYPE);

    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: [ALLOC] %p\n",__LINE__,pBufferHeader);

    if (nPortIndex == OUTPUT_PORT)
    {
        pBufferHeader->nInputPortIndex = -1;
        pBufferHeader->nOutputPortIndex = nPortIndex;
        pComponentPrivate->pOutputBufferList->pBufHdr[pComponentPrivate->pOutputBufferList->numBuffers] = pBufferHeader;
        pComponentPrivate->pOutputBufferList->bBufferPending[pComponentPrivate->pOutputBufferList->numBuffers] = 0;
        pComponentPrivate->pOutputBufferList->bufferOwner[pComponentPrivate->pOutputBufferList->numBuffers++] = 0;
        if (pComponentPrivate->pOutputBufferList->numBuffers == pPortDef->nBufferCountActual)
        {
            pPortDef->bPopulated = OMX_TRUE;
        }
    }
    else
    {
        pBufferHeader->nInputPortIndex = nPortIndex;
        pBufferHeader->nOutputPortIndex = -1;
        pComponentPrivate->pInputBufferList->pBufHdr[pComponentPrivate->pInputBufferList->numBuffers] = pBufferHeader;
        pComponentPrivate->pInputBufferList->bBufferPending[pComponentPrivate->pInputBufferList->numBuffers] = 0;
        pComponentPrivate->pInputBufferList->bufferOwner[pComponentPrivate->pInputBufferList->numBuffers++] = 0;
        if (pComponentPrivate->pInputBufferList->numBuffers == pPortDef->nBufferCountActual)
        {
            pPortDef->bPopulated = OMX_TRUE;
        }
    }

    if((pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated == pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled)&&
       (pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated == pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled) &&
       (pComponentPrivate->InLoaded_readytoidle))
    {
       pComponentPrivate->InLoaded_readytoidle = 0;
#ifndef UNDER_CE
       pthread_mutex_lock(&pComponentPrivate->InLoaded_mutex);
       pthread_cond_signal(&pComponentPrivate->InLoaded_threshold);
       pthread_mutex_unlock(&pComponentPrivate->InLoaded_mutex);
#else
       OMX_SignalEvent(&(pComponentPrivate->InLoaded_event));
#endif
    }

    pBufferHeader->pAppPrivate = pAppPrivate;
    pBufferHeader->pPlatformPrivate = pComponentPrivate;
    pBufferHeader->nAllocLen = nSizeBytes;
    pBufferHeader->nVersion.s.nVersionMajor = AACENC_MAJOR_VER;
    pBufferHeader->nVersion.s.nVersionMinor = AACENC_MINOR_VER;
    pComponentPrivate->nVersion = pBufferHeader->nVersion.nVersion;
    pBufferHeader->pBuffer = pBuffer;
    pBufferHeader->nSize = sizeof(OMX_BUFFERHEADERTYPE);
    *ppBufferHdr = pBufferHeader;
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pBufferHeader = %p\n",__LINE__,pBufferHeader);
    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: UseBuffer : pBufferHeader = %p\n", __LINE__,pBufferHeader);

    if (pComponentPrivate->bEnableCommandPending)
    {
        OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d :: AACENC: UseBuffer : -> Send Comman OMX_CommandPortEnable \n", __LINE__);
        SendCommand (pComponentPrivate->pHandle,OMX_CommandPortEnable,pComponentPrivate->nEnableCommandParam,NULL);
    }

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "AACENC: [UseBuffer] pBufferHeader = %p\n",pBufferHeader);
    OMX_PRINT1(pComponentPrivate->dbg, "AACENC: [UseBuffer] pBufferHeader->pBuffer = %p\n",pBufferHeader->pBuffer);

    return eError;
}

/* ================================================================================= */
/**
* @fn GetExtensionIndex() description for GetExtensionIndex
GetExtensionIndex().
Returns index for vendor specific settings.
*
*  @see         OMX_Core.h
*/
/* ================================================================================ */
static OMX_ERRORTYPE GetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "AACENC: GetExtensionIndex\n");
    if ((strcmp(cParameterName,"OMX.ITTIAM.index.config.tns"))==0)
    {
        *pIndexType = OMX_IndexCustomAacEncTNSConfig;
        OMXDBG_PRINT(stderr, PRINT, 1, 0, "OMX_IndexCustomAacEncTNSConfig\n");
    }
    else if ((strcmp(cParameterName,"OMX.ITTIAM.index.config.aacenc.qualitylevel"))==0)
    {
        *pIndexType = OMX_IndexCustomAacEncQualityLevel;
        OMXDBG_PRINT(stderr, PRINT, 1, 0, "OMX_IndexCustomAacEncQualityLevel\n");
    }
    else if ((strcmp(cParameterName,"OMX.ITTIAM.index.config.aacenc.framemode"))==0)
    {
        *pIndexType = OMX_IndexCustomAacEncFrameMode;
        OMXDBG_PRINT(stderr, PRINT, 1, 0, "OMX_IndexCustomAacEncFrameMode\n");
    }
    else
    {
        eError = OMX_ErrorBadParameter;
        OMXDBG_PRINT(stderr, PRINT, 1, 0, "AACENC: Bad Index\n");
    }

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "AACENC: Exiting GetExtensionIndex\n");

    return eError;
}

/* ================================================================================= */
/**
* @fn ComponentRoleEnum() description for ComponentRoleEnum()

Returns the role at the given index
*
*  @see         OMX_Core.h
*/
/* ================================================================================ */
static OMX_ERRORTYPE ComponentRoleEnum(
      OMX_IN OMX_HANDLETYPE hComponent,
      OMX_OUT OMX_U8 *cRole,
      OMX_IN OMX_U32 nIndex)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    if(nIndex == 0)
    {
      memcpy(cRole, &pComponentPrivate->componentRole.cRole, sizeof(OMX_U8) * OMX_MAX_STRINGNAME_SIZE);
    }
    else
    {
      eError = OMX_ErrorNoMore;
      goto EXIT;
    }

EXIT:
    return eError;
};

#ifdef UNDER_CE
/* ================================================================================= */
/**
* @fns Sleep replace for WIN CE
*/
/* ================================================================================ */
int OMX_CreateEvent(OMX_Event *event){
    int ret = OMX_ErrorNone;
    HANDLE createdEvent = NULL;
    if(event == NULL){
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    event->event  = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(event->event == NULL)
        ret = (int)GetLastError();
EXIT:
    return ret;
}

int OMX_SignalEvent(OMX_Event *event){
     int ret = OMX_ErrorNone;
     if(event == NULL){
        ret = OMX_ErrorBadParameter;
        goto EXIT;
     }
     SetEvent(event->event);
     ret = (int)GetLastError();
EXIT:
    return ret;
}

int OMX_WaitForEvent(OMX_Event *event) {
     int ret = OMX_ErrorNone;
     if(event == NULL){
        ret = OMX_ErrorBadParameter;
        goto EXIT;
     }
     WaitForSingleObject(event->event, INFINITE);
     ret = (int)GetLastError();
EXIT:
     return ret;
}

int OMX_DestroyEvent(OMX_Event *event) {
     int ret = OMX_ErrorNone;
     if(event == NULL){
        ret = OMX_ErrorBadParameter;
        goto EXIT;
     }
     CloseHandle(event->event);
EXIT:
     return ret;
}
#endif
