/*****************************************************************************/
/*                                                                           */
/*                        Enhanced pcmPlus Decoder                           */
/*                                                                           */
/*                   ITTIAM SYSTEMS PVT LTD, BANGALORE                       */
/*                          COPYRIGHT(C) 2009                                */
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
/*  File Name        : OMX_IttiamPcmDec_Utils.h                              */
/*                                                                           */
/*  Description      :                                                       */
/*                                                                           */
/*  List of Functions: None                                                  */
/*                                                                           */
/*  Issues / Problems: None                                                  */
/*                                                                           */
/*  Revision History :                                                       */
/*                                                                           */
/*****************************************************************************/

#ifndef OMX_PCMDEC_UTILS__H
#define OMX_PCMDEC_UTILS__H

#include <OMX_Component.h>
#include <OMX_TI_Common.h>
#include <OMX_TI_Debug.h>
#include <pthread.h>
#include <sched.h>


#define AUDIO_MANAGER

#ifdef __PERF_INSTRUMENTATION__
#include "perf.h"
#endif

#ifndef ANDROID
    #define ANDROID
#endif

#ifdef ANDROID
    /* Log for Android system*/
    #undef LOG_TAG
    #define LOG_TAG "OMX_PCMDEC"

    /* PV opencore capability custom parameter index */
    #define PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX 0xFF7A347
#endif

#define OBJECTTYPE_LC 2
#define OBJECTTYPE_HE 5
#define OBJECTTYPE_HE2 29

#define EXIT_COMPONENT_THRD  10


//#define ENABLE_ITTIAM_PROFILE

/* ======================================================================= */
/**
 * @def    PCM_DEC__XXX_VER    Component version
 */
/* ======================================================================= */
#define PCMDEC_MAJOR_VER 1
#define PCMDEC_MINOR_VER 1
/* ======================================================================= */

#define PCMDEC_BUFHEADER_VERSION 0x1
/* ======================================================================= */
/**
 * @def    PCMDEC_DEBUG   Debug print macro
 */
/* ======================================================================= */

#undef PCMDEC_DEBUG
#define _ERROR_PROPAGATION__

#ifdef UNDER_CE

/* ======================================================================= */
/**
 * @def    DEBUG   Memory print macro
 */
/* ======================================================================= */
#if DEBUG
#define PCMDEC_DPRINT printf
#define PCMDEC_MEMPRINT printf
#define PCMDEC_STATEPRINT printf
#define PCMDEC_BUFPRINT printf
#define PCMDEC_MEMPRINT printf
#define PCMDEC_EPRINT printf
#else
#define PCMDEC_DPRINT
#define PCMDEC_MEMPRINT
#define PCMDEC_STATEPRINT
#define PCMDEC_BUFPRINT
#define PCMDEC_MEMPRINT
#define PCMDEC_EPRINT
#endif

#else /* for Linux */

#ifdef  PCMDEC_DEBUG
    #define PCMDEC_DPRINT printf    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d:: ",__FUNCTION__, __LINE__);\
                                //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                                    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n");

    #undef PCMDEC_BUFPRINT printf
    #undef PCMDEC_MEMPRINT printf
    #define PCMDEC_STATEPRINT printf
#else
    #define PCMDEC_DPRINT(...)
#endif

#ifdef PCMDEC_STATEDETAILS
    #define PCMDEC_STATEPRINT printf
#else
    #define PCMDEC_STATEPRINT(...)
#endif

#ifdef PCMDEC_BUFDETAILS
    #define PCMDEC_BUFPRINT printf
#else
    #define PCMDEC_BUFPRINT(...)
#endif

#ifdef PCMDEC_MEMDETAILS
    #define PCMDEC_MEMPRINT(...)  fprintf(stdout, "%s %d::  ",__FUNCTION__, __LINE__); \
                                  fprintf(stdout, __VA_ARGS__); \
                                  fprintf(stdout, "\n");
#else
    #define PCMDEC_MEMPRINT(...)
#endif

#define PCMDEC_EPRINT LOGE

                           /* __android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d::    ERROR",__FUNCTION__, __LINE__);\
                        __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                            __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n"); */

#endif

/* ======================================================================= */
/**
 * @def    PCMD_OMX_MALLOC   Macro to allocate Memory
 */
/* ======================================================================= */
#define PCMD_OMX_MALLOC(_pStruct_, _sName_)                         \
    _pStruct_ = (_sName_*)newmalloc(sizeof(_sName_));               \
    if(_pStruct_ == NULL){                                          \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "***********************************\n");            \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d :: Malloc Failed\n",__LINE__);                   \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "***********************************\n");            \
        eError = OMX_ErrorInsufficientResources;                    \
        goto EXIT;                                                  \
    }                                                               \
    memset(_pStruct_,0,sizeof(_sName_));                            \
    OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: Malloced = %p\n",__LINE__,_pStruct_);

/* ======================================================================= */
/**
 * @def    PCMDEC_OMX_MALLOC_SIZE   Macro to allocate Memory
 */
/* ======================================================================= */
#define PCMDEC_OMX_MALLOC_SIZE(_ptr_, _size_,_name_)            \
    _ptr_ = (_name_*)newmalloc(_size_);                         \
    if(_ptr_ == NULL){                                          \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "***********************************\n");        \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d :: Malloc Failed\n",__LINE__);               \
        OMXDBG_PRINT(stderr, ERROR, 4, 0, "***********************************\n");        \
        eError = OMX_ErrorInsufficientResources;                \
        goto EXIT;                                              \
    }                                                           \
    memset(_ptr_,0,_size_);                                     \
    OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: Malloced = %p\n",__LINE__,_ptr_);

/* ======================================================================= */
/**
 * @def    PCMDEC_OMX_ERROR_EXIT   Exit print and return macro
 */
/* ======================================================================= */
#define PCMDEC_OMX_ERROR_EXIT(_e_, _c_, _s_)                            \
    _e_ = _c_;                                                          \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d : Error Name: %s : Error Num = %x",__LINE__, _s_, _e_);  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    goto EXIT;

/* ======================================================================= */
/**
 * @def    PCMDEC_OMX_CONF_CHECK_CMD   Command check Macro
 */
/* ======================================================================= */
#define PCMDEC_OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3)  \
    {                                                   \
        if(!_ptr1 || !_ptr2 || !_ptr3){                 \
            eError = OMX_ErrorBadParameter;             \
            goto EXIT;                                  \
        }                                               \
    }

/* ======================================================================= */
/**
 * @def    PCMDEC_OMX_FREE   Macro to free the Memory
 */
/* ======================================================================= */
#define PCMDEC_OMX_FREE(ptr)                                            \
    if(NULL != ptr) {                                                   \
        OMXDBG_PRINT(stderr, BUFFER, 2, 0, "%d :: Freeing Address = %p\n",__LINE__,ptr);   \
        newfree(ptr);                                                   \
        ptr = NULL;                                                     \
    }

/* ======================================================================= */
/**
 * @def    OMX_CONF_INIT_STRUCT   Macro to Initialise the structure variables
 */
/* ======================================================================= */
#define OMX_CONF_INIT_STRUCT(_s_, _name_)       \
    memset((_s_), 0x0, sizeof(_name_));         \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 1;      \
    (_s_)->nVersion.s.nVersionMinor = 1;      \
    (_s_)->nVersion.s.nRevision = 0x0;          \
    (_s_)->nVersion.s.nStep = 0x0

/* ======================================================================= */
/**
 * @def    PCMDEC_BUFDETAILS   Turns buffer messaging on and off
 */
/* ======================================================================= */
#undef PCMDEC_BUFDETAILS
/* ======================================================================= */
/**
 * @def    PCMDEC_STATEDETAILS   Turns state messaging on and off
 */
/* ======================================================================= */
#undef PCMDEC_STATEDETAILS
/* ======================================================================= */
/**
 * @def    PCMDEC_MEMDETAILS   Turns memory messaging on and off
 */
/* ======================================================================= */
#undef PCMDEC_MEMDETAILS

#define EXTRA_BYTES 128 /* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256 /* For Cache alignment*/
#define PCMDEC_OUTPUT_PORT 1
#define PCMDEC_INPUT_PORT 0
#define MAX_NUM_OF_BUFS_PCMDEC 15
/* ======================================================================= */
/**
 * @def    NUM_OF_PORTS_PCMDEC   Number of ports
 */
/* ======================================================================= */
#define NUM_OF_PORTS_PCMDEC 2

/* ======================================================================= */
/**
 * @def    PCMD_NUM_INPUT_BUFFERS   Default number of input buffers
 *
 */
/* ======================================================================= */
#define PCMD_NUM_INPUT_BUFFERS 4
/* ======================================================================= */
/**
 * @def    PCMD_NUM_OUTPUT_BUFFERS   Default number of output buffers
 *
 */
/* ======================================================================= */
#define PCMD_NUM_OUTPUT_BUFFERS 4

/* ======================================================================= */
/**
 * @def    PCMD_INPUT_BUFFER_SIZE   Default input buffer size
 *
 */
/* ======================================================================= */
#define PCMD_INPUT_BUFFER_SIZE 1536 // - STEREO
//#define PCMD_INPUT_BUFFER_SIZE 6144 //Multichannel
/* ======================================================================= */
/**
 * @def    PCMD_OUTPUT_BUFFER_SIZE   Default output buffer size
 *
 */
/* ======================================================================= */
#define MIN_PCMD_OUTPUT_BUFFER_SIZE 8192 // Stereo
#define PCMD_OUTPUT_BUFFER_SIZE 8192


//#define MIN_AACD_OUTPUT_BUFFER_SIZE 24576
//#define PCMD_OUTPUT_BUFFER_SIZE 24576*2
/* ======================================================================= */
/**
 * @def    PCMD_SAMPLING_FREQUENCY   Sampling frequency
 */
/* ======================================================================= */
#define PCMD_SAMPLING_FREQUENCY 44100

/* ======================================================================= */
/**
 * @def    Mem test application
 */
/* ======================================================================= */
#undef PCMDEC_DEBUGMEM

#ifdef PCMDEC_DEBUGMEM
#define newmalloc(x) mymalloc(__LINE__,__FILE__,x)
#define newfree(z) myfree(z,__LINE__,__FILE__)
#else
#define newmalloc(x) malloc(x)
#define newfree(z) free(z)
#endif

/* ======================================================================= */
/**
 * @def    PCMDec macros for MONO,STEREO_INTERLEAVED,STEREO_NONINTERLEAVED
 */
/* ======================================================================= */
/* Stream types supported*/
#define MONO_STREAM_PCMDEC                   1
#define STEREO_INTERLEAVED_STREAM_PCMDEC     2
#define STEREO_NONINTERLEAVED_STREAM_PCMDEC  3

/**
 *
 * PCM Decoder Profile:0 - MAIN, 1 - LC, 2 - SSR, 3 - LTP.
 */
typedef enum {
    EProfileMain,
    EProfileLC,
    EProfileSSR,
    EProfileLTP
}PCMProfile;
/* ======================================================================= */
/** COMP_PORT_TYPE_PCMDEC  Port types
 *
 *  @param  INPUT_PORT_PCMDEC                    Input port
 *
 *  @param  OUTPUT_PORT_PCMDEC               Output port
 */
/*  ==================================================================== */
/*This enum must not be changed. */
typedef enum COMP_PORT_TYPE_PCMDEC {
    INPUT_PORT_PCMDEC = 0,
    OUTPUT_PORT_PCMDEC
}COMP_PORT_TYPE_PCMDEC;
/* ======================================================================= */
/** OMX_INDEXAUDIOTYPE_PCMDEC  Defines the custom configuration settings
 *                              for the component
 *
 *  @param  OMX_IndexCustomMode16_24bit_PCMDEC  Sets the 16/24 mode
 *
 *  @param  OMX_IndexCustomModeProfile_PCMDEC  Sets the Profile mode
 *
 *  @param  OMX_IndexCustomModeSBR_PCMDEC  Sets the SBR mode
 *
 *  @param  OMX_IndexCustomModeDasfConfig_PCMDEC  Sets the DASF mode
 *
 *  @param  OMX_IndexCustomModeRAW_PCMDEC  Sets the RAW mode
 *
 *  @param  OMX_IndexCustomModePS_PCMDEC  Sets the ParametricStereo mode
 *
 */
/*  ==================================================================== */
typedef enum OMX_INDEXAUDIOTYPE_PCMDEC {
    OMX_IndexCustomFramemodeConfig = 0xFF000001,
    OMX_IndexCustomPcmDecStreamConfig,
    OMX_IndexCustomPcmDecDownmixtoStereo,
    OMX_IndexCustomPcmDecDataPath,
    OMX_IndexCustomDebug
}OMX_INDEXAUDIOTYPE_PCMDEC;


/* ======================================================================= */
/** IAUDIO_PcmFormat: This value is used by DSP.
 *
 * @param IAUDIO_BLOCK: It is used in DASF mode.
 *
 * @param IAUDIO_INTERLEAVED: It specifies interleaved format of SN.
 */
/* ==================================================================== */
typedef enum {
    IAUDIO_BLOCK=0,
    IAUDIO_INTERLEAVED
} IAUDIO_PcmFormat;



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


/* Component Port Context */
typedef struct AUDIODEC_PORT_TYPE {
    /* Used in tunneling, this is handle of tunneled component */
    OMX_HANDLETYPE hTunnelComponent;
    /* Port which has to be tunneled */
    OMX_U32 nTunnelPort;
    /* Buffer Supplier Information */
    OMX_BUFFERSUPPLIERTYPE eSupplierSetting;
    /* Number of buffers */
    OMX_U8 nBufferCnt;
    /* Port format information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE* pPortFormat;
} AUDIODEC_PORT_TYPE;


/* ======================================================================= */
/** PCM_DEC_BUFFERLIST: This contains information about a buffer's owner whether
 * it is application or component, number of buffers owned etc.
 *
 * @see OMX_BUFFERHEADERTYPE
 */
/* ==================================================================== */
struct PCM_DEC_BUFFERLIST{
    /* Array of pointer to OMX buffer headers */
    OMX_BUFFERHEADERTYPE *pBufHdr[MAX_NUM_OF_BUFS_PCMDEC];
    /* Array that tells about owner of each buffer */
    OMX_U32 bufferOwner[MAX_NUM_OF_BUFS_PCMDEC];
    /* Tracks pending buffers */
    OMX_U32 bBufferPending[MAX_NUM_OF_BUFS_PCMDEC];
    /* Number of buffers  */
    OMX_U32 numBuffers;
};

typedef struct PCM_DEC_BUFFERLIST PCMDEC_BUFFERLIST;

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

/* ======================================================================= */
/** PCMDEC_COMPONENT_PRIVATE: This is the major and main structure of the
 * component which contains all type of information of buffers, ports etc
 * contained in the component.
 *
 * @see OMX_BUFFERHEADERTYPE
 * @see OMX_AUDIO_PARAM_PORTFORMATTYPE
 * @see OMX_PARAM_PORTDEFINITIONTYPE
 * @see PCMD_LCML_BUFHEADERTYPE
 * @see OMX_PORT_PARAM_TYPE
 * @see OMX_PRIORITYMGMTTYPE
 * @see AUDIODEC_PORT_TYPE
 * @see PCMDEC_BUFFERLIST
 * @see LCML_STRMATTR
 * @see
 */
/* ==================================================================== */

typedef struct PCMDEC_COMPONENT_PRIVATE
{

    OMX_CALLBACKTYPE cbInfo;
    /** Handle for use with async callbacks */
    OMX_PORT_PARAM_TYPE* sPortParam;
    /* Input port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sInPortFormat;
    /* Output port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sOutPortFormat;
    /* Buffer owner information */
    OMX_U32 bIsBufferOwned[NUM_OF_PORTS_PCMDEC];

    /** This will contain info like how many buffers
        are there for input/output ports, their size etc, but not
        BUFFERHEADERTYPE POINTERS. */
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef[NUM_OF_PORTS_PCMDEC];
    /* Contains information that come from application */
    OMX_AUDIO_PARAM_PCMMODETYPE* aacParams;

    OMX_AUDIO_PARAM_PCMMODETYPE* pcmParams;

    /** This is component handle */
    OMX_COMPONENTTYPE* pHandle;

    /** Current state of this component */
    OMX_STATETYPE curState;

    /** The component thread handle */
    pthread_t ComponentThread;

    /** The pipes for sending buffers to the thread */
    int dataPipeInputBuf[2];
    int dataPipeOutputBuf[2];

    /** The pipes for sending buffers to the thread */
    int cmdPipe[2];

    /** The pipes for sending command data to the thread */
    int cmdDataPipe[2];

#ifdef __PERF_INSTRUMENTATION__
    PERF_OBJHANDLE pPERF, pPERFcomp;
#endif

    /** Tells whether buffers on ports have been allocated */
    OMX_U32 bPortDefsAllocated;

    /** Tells whether component thread has started */
    OMX_U32 bCompThreadStarted;
    /** Marks the buffer data  */

    OMX_PTR pMarkData;
    /** Marks the buffer */
    OMX_MARKTYPE *pMarkBuf;
    /** Marks the target component */

    OMX_HANDLETYPE hMarkTargetComponent;
    /** Flag to track when input buffer's filled length is 0 */

    /** Pointer to port parameter structure */
    OMX_PORT_PARAM_TYPE* pPortParamType;

    /** Pointer to port priority management structure */
    OMX_PRIORITYMGMTTYPE* pPriorityMgmt;

    /* RMPROXY_CALLBACKTYPE rmproxyCallback; */
    OMX_BOOL bPreempted;

    /** Contains the port related info of both the ports */
    AUDIODEC_PORT_TYPE *pCompPort[NUM_OF_PORTS_PCMDEC];

    /* Checks whether or not buffer were allocated by appliction */
    OMX_U32 bufAlloced;

    /** Flag to check about execution of component thread */
    OMX_U16 bExitCompThrd;

    /** Pointer to list of input buffers */
    PCMDEC_BUFFERLIST *pInputBufferList;

    /** Pointer to list of output buffers */
    PCMDEC_BUFFERLIST *pOutputBufferList;

    /** Contains the version information */
    OMX_U32 nVersion;

    OMX_U16 framemode;
    OMX_U16 iDownmixtoStereo;

    OMX_U16 interleave_mode;

    OMX_S32 bDownSampleSbr;

    OMX_STRING cComponentName;

    OMX_VERSIONTYPE ComponentVersion;

    OMX_U32 nOpBit;
    OMX_U32 nProfile;
    OMX_U32 RAW;
    OMX_U32 bInitParamsInitialized;

    /** Flags to control port disable command **/
    OMX_U32 bDisableCommandPending;
    OMX_U32 bDisableCommandParam;

    /** Flags to control port enable command **/
    OMX_U32 bEnableCommandPending;
    OMX_U32 bEnableCommandParam;

    OMX_U32 nInvalidFrameCount;
    OMX_U32 numPendingBuffers;
    OMX_U32 bNoIdleOnStop;

    /* bIdleCommandPending;*/
    OMX_S32 nOutStandingFillDones;
    OMX_BOOL bIsInvalidState;
    OMX_STRING* sDeviceString;

    pthread_mutex_t AlloBuf_mutex;
    pthread_cond_t AlloBuf_threshold;
    OMX_U8 AlloBuf_waitingsignal;

    pthread_mutex_t InLoaded_mutex;
    pthread_cond_t InLoaded_threshold;
    OMX_U8 InLoaded_readytoidle;

    pthread_mutex_t InIdle_mutex;
    pthread_cond_t InIdle_threshold;
    OMX_U8 InIdle_goingtoloaded;

    pthread_mutex_t codecStop_mutex;
    pthread_cond_t codecStop_threshold;
    OMX_U8 codecStop_waitingsignal;

    pthread_mutex_t codecFlush_mutex;
    pthread_cond_t codecFlush_threshold;
    OMX_U8 codecFlush_waitingsignal;

    OMX_S8 nUnhandledFillThisBuffers;
    OMX_S8 nUnhandledEmptyThisBuffers;
    OMX_BOOL bFlushOutputPortCommandPending;
    OMX_BOOL bFlushInputPortCommandPending;

    OMX_BOOL bLoadedCommandPending;
    OMX_PARAM_COMPONENTROLETYPE *componentRole;

    /** Flag to flush SN after EOS in order to process more buffers after EOS**/
    /** Set to indicate component is stopping */
    OMX_U8 iEOS;
    OMX_U8 SendEOStoApp;
    OMX_U8 InputPortFlushed;
    OMX_U8 OutputPortFlushed;

    /** Flag to mark the first sent buffer**/
    OMX_U8 first_buff;
    /** First Time Stamp sent **/
    OMX_U64 first_TS;
    OMX_BOOL bNewOutputBufRequired;
    OMX_BUFFERHEADERTYPE *pOutputBufHeader;
    OMX_BUFFERHEADERTYPE *pInputBufHeader;

    PV_OMXComponentCapabilityFlagsType iPVCapabilityFlags;
    OMX_BOOL bConfigData;
    OMX_U8 OutPendingPR;

    struct OMX_TI_Debug dbg;

    fd_set rfds;

    OMX_S32 inputBufFilledLen ;
    OMX_U64 m_numSamplesProduced;

    OMX_S32 iFrameCount;
    OMX_S32 iLatestTimeStamp;
    OMX_S32 iPreviousTimeStamp;

#ifdef ENABLE_ITTIAM_PROFILE
    UWORD32 m_avgDecodeTimeMS;
    UWORD32 m_peakDecodeTimeMS;
    UWORD32 m_totalDecodeTimeMS;
    UWORD32 m_numOfDecodes;
#endif

    /* The config header needs to be stored */

    OMX_U8 ga_header[100];
    OMX_U32 ga_hdr_size;

} PCMDEC_COMPONENT_PRIVATE;

/* ================================================================================= * */
/**
 * OMX_ComponentInit() function is called by OMX Core to initialize the component
 * with default values of the component. Before calling this function OMX_Init
 * must have been called.
 *
 * @param *hComp This is component handle allocated by the OMX core.
 *
 * @pre          OMX_Init should be called by application.
 *
 * @post         Component has initialzed with default values.
 *
 *  @return      OMX_ErrorNone = Successful Inirialization of the component\n
 *               OMX_ErrorInsufficientResources = Not enough memory
 *
 *  @see          PcmDec_StartCompThread()
 */
/* ================================================================================ * */
#ifndef UNDER_CE
OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComp);
#else
/*  WinCE Implicit Export Syntax */
#define OMX_EXPORT __declspec(dllexport)
/* ===========================================================  */
/**
 *  OMX_ComponentInit()  Initializes component
 *
 *
 *  @param hComp            OMX Handle
 *
 *  @return OMX_ErrorNone = Successful
 *          Other error code = fail
 *
 */
/*================================================================== */
OMX_EXPORT OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComp);
#endif

/* ================================================================================= * */
/**
 * PcmDec_StartCompThread() starts the component thread. This is internal
 * function of the component.
 *
 * @param pHandle This is component handle allocated by the OMX core.
 *
 * @pre          None
 *
 * @post         None
 *
 *  @return      OMX_ErrorNone = Successful Inirialization of the component\n
 *               OMX_ErrorInsufficientResources = Not enough memory
 *
 *  @see         None
 */
/* ================================================================================ * */
OMX_ERRORTYPE PcmDec_StartCompThread(OMX_HANDLETYPE pHandle);
/* ================================================================================= * */
/**
 * PCMDEC_HandleCommand() function handles the command sent by the application.
 * All the state transitions, except from nothing to loaded state, of the
 * component are done by this function.
 *
 * @param pComponentPrivate  This is component's private date structure.
 *
 * @pre          None
 *
 * @post         None
 *
 *  @return      OMX_ErrorNone = Successful processing.
 *               OMX_ErrorInsufficientResources = Not enough memory
 *               OMX_ErrorHardware = Hardware error has occured lile LCML failed
 *               to do any said operartion.
 *
 *  @see         None
 */
/* ================================================================================ * */
OMX_U32 PCMDEC_HandleCommand (PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
/* ================================================================================= * */
/**
 * PCMDEC_HandleDataBuf_FromApp() function handles the input and output buffers
 * that come from the application. It is not direct function wich gets called by
 * the application rather, it gets called eventually.
 *
 * @param *pBufHeader This is the buffer header that needs to be processed.
 *
 * @param *pComponentPrivate  This is component's private date structure.
 *
 * @pre          None
 *
 * @post         None
 *
 *  @return      OMX_ErrorNone = Successful processing.
 *               OMX_ErrorInsufficientResources = Not enough memory
 *               OMX_ErrorHardware = Hardware error has occured lile LCML failed
 *               to do any said operartion.
 *
 *  @see         None
 */
/* ================================================================================ * */
OMX_ERRORTYPE PCMDEC_HandleDataBuf_FromApp(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
void* PCMDEC_ComponentThread (void* pThreadData);
OMX_ERRORTYPE PCMDEC_HandleCommandStateSet(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                           OMX_S32                   commandData);
OMX_ERRORTYPE PCMDEC_SetStateToIdle(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE PCMDEC_SetStateToExecuting(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE PCMDEC_SetStateToPause(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE PCMDEC_SetStateToLoaded(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE PCMDEC_HandleCommandPortDisable(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData);
OMX_ERRORTYPE PCMDEC_HandleCommandPortEnable(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData);

OMX_ERRORTYPE PCMDEC_InitializeTheComponent(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE PCMDEC_CloseTheComponent(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE PCMDEC_FreeCompResources(OMX_HANDLETYPE pComponent);
OMX_ERRORTYPE ReturnOutputBuffers(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE ReturnInputBuffers(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE ReturnInputOutputBuffers(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate);

#endif
