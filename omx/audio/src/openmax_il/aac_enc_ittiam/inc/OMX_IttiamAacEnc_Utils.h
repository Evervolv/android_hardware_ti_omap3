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

#ifndef OMX_AACENC_UTILS__H
#define OMX_AACENC_UTILS__H

#include <OMX_Component.h>
#include <OMX_TI_Common.h>
#include <OMX_TI_Debug.h>
#include <ia_type_def.h>
#include <ia_heaacv2_enc_omx.h>
#include "OMX_IttiamAacEncoder.h"
/* #include <ResourceManagerProxyAPI.h> */

#define AACENC_MAJOR_VER 0x0001
#define AACENC_MINOR_VER 0x0001
#define NOT_USED 10
#define NORMAL_BUFFER 0
#define OMX_AACENC_DEFAULT_SEGMENT (0)
#define OMX_AACENC_SN_TIMEOUT (1000)
#define OMX_AACENC_SN_PRIORITY (10)
#define OMX_AACENC_NUM_DLLS (2)
#define AACENC_CPU_USAGE 45
#define _ERROR_PROPAGATION__
#define MPEG4AACENC_MAX_OUTPUT_FRAMES 24

#ifndef ANDROID
    #define ANDROID
#endif

#ifdef ANDROID
    #undef LOG_TAG
    #define LOG_TAG "OMX_AACENC"

/* PV opencore capability custom parameter index */
    #define PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX 0xFF7A347
#endif

#define EXTRA_BYTES 128
#define DSP_CACHE_ALIGNMENT 256

#ifdef __PERF_INSTRUMENTATION__
    #include "perf.h"
#endif
#include <OMX_Component.h>

#define OMX_CONF_INIT_STRUCT(_s_, _name_)   \
    memset((_s_), 0x0, sizeof(_name_)); \
    (_s_)->nSize = sizeof(_name_);      \
    (_s_)->nVersion.s.nVersionMajor = AACENC_MAJOR_VER;  \
    (_s_)->nVersion.s.nVersionMinor = AACENC_MINOR_VER;  \
    (_s_)->nVersion.s.nRevision = 0x0001;      \
    (_s_)->nVersion.s.nStep = 0x0

#define OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3) \
{                       \
    if(!_ptr1 || !_ptr2 || !_ptr3){     \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d :: Error bad parameter \n",__LINE__);\
        eError = OMX_ErrorBadParameter;     \
    goto EXIT;          \
    }                       \
}

#define OMX_CONF_SET_ERROR_BAIL(_eError, _eCode)\
{                       \
    _eError = _eCode;               \
    goto OMX_CONF_CMD_BAIL;         \
}

#define OMX_MALLOC_STRUCT(_pStruct_, _sName_)   \
    _pStruct_ = (_sName_*)newmalloc(sizeof(_sName_));   \
    if(_pStruct_ == NULL){  \
        eError = OMX_ErrorInsufficientResources;    \
        goto EXIT;  \
    } \
    memset(_pStruct_,0,sizeof(_sName_));

#define OMX_MALLOC_STRUCT_SIZE(_ptr_, _size_,_name_)    \
    _ptr_ = (_name_ *)newmalloc(_size_);    \
    if(_ptr_ == NULL){  \
        eError = OMX_ErrorInsufficientResources;    \
        goto EXIT;  \
    } \
    memset(_ptr_,0,_size_);


#define OMX_MEMFREE_STRUCT(_pStruct_)\
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d :: FREEING MEMORY = %p\n",__LINE__,_pStruct_);\
    if(_pStruct_ != NULL){\
        newfree(_pStruct_);\
        _pStruct_ = NULL;\
    }

#define OMX_CLOSE_PIPE(_pStruct_,err)\
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d :: CLOSING PIPE \n",__LINE__);\
    err = close (_pStruct_);\
    if(0 != err && OMX_ErrorNone == eError){\
        eError = OMX_ErrorHardware;\
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d :: Error while closing pipe\n",__LINE__);\
        goto EXIT;\
    }

#define OMX_DPRINT_ADDRESS(_s_, _ptr_)  \
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%s = %p\n", _s_, _ptr_);


#undef SWAT_ANALYSIS

/* debug message */
#define AACENC_DEBUG
#define AACENC_ERROR


#ifndef UNDER_CE
#ifdef  AACENC_ERROR
     #define AACENC_EPRINT(...) fprintf(stderr,__VA_ARGS__)
#else
     #define AACENC_EPRINT(...)
#endif /* AAC_ERROR*/


#ifdef  AACENC_DEBUG
     #define AACENC_DPRINT(...) fprintf(stderr,__VA_ARGS__)
#else
     #define AACENC_DPRINT(...)
#endif

#else /*UNDER_CE*/

#ifdef  AACENC_DEBUG
 #define AACENC_DPRINT(STR, ARG...) printf()
#else
#endif

#endif /*UNDER_CE*/


typedef struct PV_OMXComponentCapabilityFlagsType
{
        ////////////////// OMX COMPONENT CAPABILITY RELATED MEMBERS (for opencore compatability)
        OMX_BOOL iIsOMXComponentMultiThreaded;
        OMX_BOOL iOMXComponentSupportsExternalOutputBufferAlloc;
        OMX_BOOL iOMXComponentSupportsExternalInputBufferAlloc;
        OMX_BOOL iOMXComponentSupportsMovableInputBuffers;
        OMX_BOOL iOMXComponentSupportsPartialFrames;
        OMX_BOOL iOMXComponentNeedsNALStartCode;
        OMX_BOOL iOMXComponentCanHandleIncompleteFrames;
} PV_OMXComponentCapabilityFlagsType;

/*This enum must not be changed.*/
typedef enum COMP_PORT_TYPE
{
    INPUT_PORT = 0,
    OUTPUT_PORT
}COMP_PORT_TYPE;

typedef enum
{
    DIRECTION_INPUT,
    DIRECTION_OUTPUT
}BUFFER_Dir;

typedef struct BUFFS
{
    char BufHeader;
    char Buffer;
}BUFFS;

typedef struct BUFFERHEADERTYPE_INFO
{
    OMX_BUFFERHEADERTYPE* pBufHeader[MAX_NUM_OF_BUFS];
    BUFFS bBufOwner[MAX_NUM_OF_BUFS];
}BUFFERHEADERTYPE_INFO;

typedef OMX_ERRORTYPE (*fpo)(OMX_HANDLETYPE);

typedef struct AACENC_AudioCodecParams
{
    unsigned long  iSamplingRate;
    unsigned long  iStrmId;
    unsigned short iAudioFormat;
}AACENC_AudioCodecParams;

/* enum AACENC_BOOL_TYPE                        */
/* brief Enumerated type for Boolean decision   */

typedef enum
{
    AACENC_FALSE = 0,     /*!< To indicate False  */
    AACENC_TRUE  = 1      /*!< To indicate True   */
} AACENC_BOOL_TYPE;

/*! \enum AACENC_OBJ_TYP  */
/*! \brief enumerated type for output format */
typedef enum
{
  AACENC_OBJ_TYP_LC           = 2,   /*!< AAC Low Complexity  */
  AACENC_OBJ_TYP_HEAAC        = 5,   /*!< HE AAC              */
  AACENC_OBJ_TYP_PS           = 29   /*!< AAC LC + SBR + PS   */
} AACENC_OBJ_TYP;

#if 0
/*! \enum  AACENC_TRANSPORT_TYPE */
/*! \brief Enumerated type for output file format */
typedef enum
{
  AACENC_TT_RAW    = 0,              /*!< Raw Output Format   */
  AACENC_TT_ADIF   = 1,              /*!< ADIF File Format    */
  AACENC_TT_ADTS   = 2               /*!< ADTS File Format    */
} AACENC_TRANSPORT_TYPE;

#endif

typedef enum {
    IUALG_CMD_STOP          = 0,
    IUALG_CMD_PAUSE         = 1,
    IUALG_CMD_GETSTATUS     = 2,
    IUALG_CMD_SETSTATUS     = 3,
    IUALG_CMD_USERCMDSTART  = 100
}IUALG_Cmd;

typedef enum {
    IAUDIO_BLOCK=0,
    IAUDIO_INTERLEAVED
} IAUDIO_AacFormat;



typedef struct _BUFFERLIST BUFFERLIST;

struct _BUFFERLIST
{
    OMX_BUFFERHEADERTYPE sBufHdr;                   /* actual buffer header */
    OMX_BUFFERHEADERTYPE *pBufHdr[MAX_NUM_OF_BUFS]; /* records buffer header send by client */
    OMX_U32 bufferOwner[MAX_NUM_OF_BUFS];
    OMX_U16 numBuffers;
    OMX_U32 bBufferPending[MAX_NUM_OF_BUFS];
    BUFFERLIST *pNextBuf;
    BUFFERLIST *pPrevBuf;
};

#ifdef UNDER_CE
    #ifndef _OMX_EVENT_
        #define _OMX_EVENT_
        typedef struct OMX_Event {
            HANDLE event;
        } OMX_Event;
    #endif
    int OMX_CreateEvent(OMX_Event *event);
    int OMX_SignalEvent(OMX_Event *event);
    int OMX_WaitForEvent(OMX_Event *event);
    int OMX_DestroyEvent(OMX_Event *event);
#endif



typedef struct AACENC_COMPONENT_PRIVATE
{
    /** Array of pointers to BUFFERHEADERTYPE structues
       This pBufHeader[INPUT_PORT] will point to all the
       BUFFERHEADERTYPE structures related to input port,
       not just one structure. Same is for output port
       also. */
    OMX_BUFFERHEADERTYPE* pBufHeader[NUM_OF_PORTS];

    BUFFERHEADERTYPE_INFO BufInfo[NUM_OF_PORTS];

    OMX_CALLBACKTYPE cbInfo;
    /** Handle for use with async callbacks */
    OMX_PORT_PARAM_TYPE sPortParam;

    OMX_PRIORITYMGMTTYPE* sPriorityMgmt;

/*  RMPROXY_CALLBACKTYPE rmproxyCallback; */
    OMX_BOOL bPreempted;

    OMX_AUDIO_PARAM_PORTFORMATTYPE sInPortFormat;

    OMX_AUDIO_PARAM_PORTFORMATTYPE sOutPortFormat;

    OMX_U32 bIsBufferOwned[NUM_OF_PORTS];
    /** This will contain info like how many buffers
        are there for input/output ports, their size etc, but not
        BUFFERHEADERTYPE POINTERS. */
    OMX_U32 abc;

    OMX_U32 def;

    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef[NUM_OF_PORTS];

    OMX_AUDIO_PARAM_AACPROFILETYPE* aacParams[NUM_OF_PORTS];

    OMX_AUDIO_PARAM_PCMMODETYPE* pcmParam[NUM_OF_PORTS];

    /** This is component handle */
    OMX_COMPONENTTYPE* pHandle;

    /** Current state of this component */
    OMX_STATETYPE curState;

    /** The component thread handle */
    pthread_t ComponentThread;



/** The pipes for sending output Buffers to the thread */
    int dataPipeInputBuf[2];
    int dataPipeOutputBuf[2];

    int i_init_done;

    OMX_S8 *pInternalInputBuffer;
    OMX_S8 *pInternalInputBuffer_frame;
    OMX_S32 nInternalFilledLen;
    OMX_S32 nInternalindex;

    fd_set rfds;

    /** The pipes for sending buffers to the thread */
    int cmdPipe[2];
    /** The pipes for sending buffers to the thread */
    int cmdDataPipe[2];

    /** Set to indicate component is stopping */
    OMX_U32 bIsStopping;

    /** Number of input buffers at runtime */
    OMX_U32 nRuntimeInputBuffers;

    /** Number of output buffers at runtime */
    OMX_U32 nRuntimeOutputBuffers;

    OMX_U32 bIsThreadstop;

    OMX_U32 bIsEOFSent;

    OMX_BOOL CustomConfiguration;

#ifdef __PERF_INSTRUMENTATION__
        PERF_OBJHANDLE pPERF, pPERFcomp;
#endif

    AACENC_AudioCodecParams* pParams;

    ia_heaacv2_enc_params_t ia_heaacv2_enc_params;

    ia_heaacv2_enc_input_args_t ia_heaacv2_enc_input_args;

    ia_heaacv2_enc_output_args_t ia_heaacv2_enc_output_args;

    OMX_PTR  pHeaacv2EncHandle;

    OMX_S32  i_frame_count;

    OMX_TICKS  nLatestTimeStamp;

    /* samplesEncoded parameter is meant to store the total samples encoded since arrival of previous
    timestamp, i.e. nLatestTimeStamp */
    OMX_U32 samplesEncoded;
//--[[ GB Patch START : junghyun.you@lge.com [2012.05.31]
    OMX_TICKS temp_TS; // Tushar - [] - For calculating o/p buffer timestamp instead of copying i/p buffer timestamp
//--]] GB Patch END
    OMX_U16 ulSamplingRate;

    OMX_U16 unNumChannels;

    OMX_U32 unBitrate;

    OMX_U16 nObjectType;

    OMX_U32 bitRateMode;

    OMX_U16 File_Format;

    OMX_U32 EmptybufferdoneCount;

    OMX_U32 EmptythisbufferCount;

    OMX_U32 FillbufferdoneCount;

    OMX_U32 FillthisbufferCount;

    OMX_U32 bPortDefsAllocated;

    OMX_U32 bCompThreadStarted;

    OMX_PTR pMarkData;

    OMX_MARKTYPE *pMarkBuf;

    OMX_HANDLETYPE hMarkTargetComponent;

    OMX_U32 bBypassDSP;

    BUFFERLIST *pInputBufferList;

    BUFFERLIST *pOutputBufferList;


    OMX_U32 nVersion;

    OMX_U16 framemode;
    OMX_U32 bNewOutputBufRequired;
    OMX_U32 EOSfromApp;
    OMX_U32 StateTransitiontoIdleFromExecOngoing;

    OMX_BUFFERHEADERTYPE *pOutputBufHeader;

    OMX_STRING cComponentName;

    OMX_VERSIONTYPE ComponentVersion;

    OMX_U32 streamID;

    OMX_U32 bInputBufferHeaderAllocated;
    /** Stores input buffers while paused */
    OMX_BUFFERHEADERTYPE *pInputBufHdrPending[MAX_NUM_OF_BUFS];

    /** Number of input buffers received while paused */
    OMX_U32 nNumInputBufPending;

    /** Stores output buffers while paused */
    OMX_BUFFERHEADERTYPE *pOutputBufHdrPending[MAX_NUM_OF_BUFS];

    /** Number of output buffers received while paused */
    OMX_U32 nNumOutputBufPending;


    OMX_U8 PendingInPausedBufs;
    OMX_BUFFERHEADERTYPE *pInBufHdrPausedPending[MAX_NUM_OF_BUFS];
    OMX_U8 PendingOutPausedBufs;
    OMX_BUFFERHEADERTYPE *pOutBufHdrPausedPending[MAX_NUM_OF_BUFS];



    OMX_U32 bPlayCompleteFlag;

    /** Flag set when a disable command is pending */
    OMX_U32 bDisableCommandPending;

    /** Parameter for pending disable command */
    OMX_U32 bDisableCommandParam;

    /** Flag set when a disable command is pending */
    OMX_U32 bEnableCommandPending;

    /** Parameter for pending disable command */
    OMX_U32 nEnableCommandParam;

    /** Flag to set when socket node stop callback should not transition
        component to OMX_StateIdle */
    OMX_U32 bNoIdleOnStop;

    /** Flag set when idle command is pending */
    /* OMX_U32 bIdleCommandPending; */

    /** Flag set when pause command is pending */
    OMX_U32 bPauseCommandPending;

    /** Flag set when socket node is stopped */
    OMX_U32 bDspStoppedWhileExecuting;

    /** Number of outstanding FillBufferDone() calls */
    OMX_S32 nOutStandingFillDones;

    /** Number of outstanding EmptyBufferDone() calls */
    OMX_S32 nOutStandingEmptyDones;

    OMX_BUFFERHEADERTYPE *LastOutputBufferHdrQueued;

#ifndef UNDER_CE
    pthread_mutex_t AlloBuf_mutex;
    pthread_cond_t AlloBuf_threshold;
    OMX_U8 AlloBuf_waitingsignal;

    pthread_mutex_t codecStop_mutex;
    pthread_cond_t codecStop_threshold;
    OMX_U8 codecStop_waitingsignal;

    pthread_mutex_t codecFlush_mutex;
    pthread_cond_t codecFlush_threshold;
    OMX_U8 codecFlush_waitingsignal;

pthread_mutex_t InLoaded_mutex;
    pthread_cond_t InLoaded_threshold;
    OMX_U8 InLoaded_readytoidle;

    pthread_mutex_t InIdle_mutex;
    pthread_cond_t InIdle_threshold;
    OMX_U8 InIdle_goingtoloaded;

    OMX_U8 nUnhandledFillThisBuffers;
    OMX_U8 nUnhandledEmptyThisBuffers;
    OMX_BOOL bFlushOutputPortCommandPending;
    OMX_BOOL bFlushInputPortCommandPending;

#else
    OMX_Event AlloBuf_event;
    OMX_U8 AlloBuf_waitingsignal;

    OMX_Event InLoaded_event;
    OMX_U8 InLoaded_readytoidle;

    OMX_Event InIdle_event;
    OMX_U8 InIdle_goingtoloaded;


#endif
    OMX_BOOL bLoadedCommandPending;
    OMX_BOOL bIsInvalidState;
    void* PtrCollector[6];

    OMX_BUFFERHEADERTYPE *LastOutbuf;
    OMX_PARAM_COMPONENTROLETYPE componentRole;
    OMX_U16 FramesPer_OutputBuffer;

    OMX_BOOL bCodecDestroyed;

    OMX_STRING* sDeviceString;
    OMX_BOOL bFirstOutputBuffer;

    /** Keep buffer timestamps **/
    OMX_S64 timestampBufIndex[MAX_NUM_OF_BUFS];
    /** Index to arrBufIndex[], used for input buffer timestamps */
    OMX_U8 IpBufindex;
    /** Index to arrBufIndex[], used for output buffer timestamps */
    OMX_U8 OpBufindex;

    /** Keep buffer tickcount **/
    OMX_U32 tickcountBufIndex[MAX_NUM_OF_BUFS];

    PV_OMXComponentCapabilityFlagsType iPVCapabilityFlags;

    struct OMX_TI_Debug dbg;

} AACENC_COMPONENT_PRIVATE;



OMX_ERRORTYPE AACENCFill_InitParams(OMX_HANDLETYPE pHandle );


OMX_U32 AACENCHandleCommand(AACENC_COMPONENT_PRIVATE *pComponentPrivate);

OMX_ERRORTYPE AACENCHandleDataBuf_FromApp(AACENC_COMPONENT_PRIVATE *pComponentPrivate);

int AACEnc_GetSampleRateIndexL( const int aRate);


OMX_ERRORTYPE AACENC_CleanupInitParams(OMX_HANDLETYPE pHandle);


OMX_ERRORTYPE AACENCFill_InitParamsEx(OMX_HANDLETYPE pComponent);

OMX_ERRORTYPE AACENCWriteConfigHeader(AACENC_COMPONENT_PRIVATE *pComponentPrivate, OMX_BUFFERHEADERTYPE *pBufHdr);

/* void AACENC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData); */

#ifndef UNDER_CE
OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComp);
#else
/*  WinCE Implicit Export Syntax */
#define OMX_EXPORT __declspec(dllexport)
OMX_EXPORT OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComp);
#endif


OMX_ERRORTYPE AACENC_StartComponentThread(OMX_HANDLETYPE pHandle);

OMX_ERRORTYPE AACENC_StopComponentThread(OMX_HANDLETYPE pHandle);

OMX_ERRORTYPE AACENC_FreeCompResources(OMX_HANDLETYPE pComponent);

#endif



