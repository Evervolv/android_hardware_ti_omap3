/*****************************************************************************/
/*                                                                           */
/*                        Enhanced aacPlus Decoder                           */
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
/*  File Name        : OMX_IttiamAacDec_Utils.h                              */
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

#ifndef OMX_AACDEC_UTILS__H
#define OMX_AACDEC_UTILS__H

#include <OMX_Component.h>
#include <OMX_TI_Common.h>
#include <OMX_TI_Debug.h>
#include <pthread.h>
#include <sched.h>


#define AUDIO_MANAGER

#ifdef __PERF_INSTRUMENTATION__
#include "perf.h"
#endif

/*------- Ittiam Header Files -----------------------------------------------*/
#include "ia_heaacv2_dec_omx_struct_def.h"
#include "ia_enhaacplus_dec_error_codes.h"

#ifndef ANDROID
    #define ANDROID
#endif

#ifdef ANDROID
    /* Log for Android system*/
    #undef LOG_TAG
    #define LOG_TAG "OMX_AACDEC"

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
 * @def    AAC_DEC__XXX_VER    Component version
 */
/* ======================================================================= */
#define AACDEC_MAJOR_VER 1
#define AACDEC_MINOR_VER 1
/* ======================================================================= */

#define AACDEC_BUFHEADER_VERSION 0x1
/* ======================================================================= */
/**
 * @def    AACDEC_DEBUG   Debug print macro
 */
/* ======================================================================= */

#undef AACDEC_DEBUG
#define _ERROR_PROPAGATION__

#ifdef UNDER_CE

/* ======================================================================= */
/**
 * @def    DEBUG   Memory print macro
 */
/* ======================================================================= */
#if DEBUG
#define AACDEC_DPRINT printf
#define AACDEC_MEMPRINT printf
#define AACDEC_STATEPRINT printf
#define AACDEC_BUFPRINT printf
#define AACDEC_MEMPRINT printf
#define AACDEC_EPRINT printf
#else
#define AACDEC_DPRINT
#define AACDEC_MEMPRINT
#define AACDEC_STATEPRINT
#define AACDEC_BUFPRINT
#define AACDEC_MEMPRINT
#define AACDEC_EPRINT
#endif

#else /* for Linux */

#ifdef  AACDEC_DEBUG
    #define AACDEC_DPRINT printf    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d:: ",__FUNCTION__, __LINE__);\
                                //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                                    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n");

    #undef AACDEC_BUFPRINT printf
    #undef AACDEC_MEMPRINT printf
    #define AACDEC_STATEPRINT printf
#else
    #define AACDEC_DPRINT(...)
#endif

#ifdef AACDEC_STATEDETAILS
    #define AACDEC_STATEPRINT printf
#else
    #define AACDEC_STATEPRINT(...)
#endif

#ifdef AACDEC_BUFDETAILS
    #define AACDEC_BUFPRINT printf
#else
    #define AACDEC_BUFPRINT(...)
#endif

#ifdef AACDEC_MEMDETAILS
    #define AACDEC_MEMPRINT(...)  fprintf(stdout, "%s %d::  ",__FUNCTION__, __LINE__); \
                                  fprintf(stdout, __VA_ARGS__); \
                                  fprintf(stdout, "\n");
#else
    #define AACDEC_MEMPRINT(...)
#endif

#define AACDEC_EPRINT LOGE

                           /* __android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d::    ERROR",__FUNCTION__, __LINE__);\
                        __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                            __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n"); */

#endif

/* ======================================================================= */
/**
 * @def    AACD_OMX_MALLOC   Macro to allocate Memory
 */
/* ======================================================================= */
#define AACD_OMX_MALLOC(_pStruct_, _sName_)                         \
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
 * @def    AACDEC_OMX_MALLOC_SIZE   Macro to allocate Memory
 */
/* ======================================================================= */
#define AACDEC_OMX_MALLOC_SIZE(_ptr_, _size_,_name_)            \
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
 * @def    AACDEC_OMX_ERROR_EXIT   Exit print and return macro
 */
/* ======================================================================= */
#define AACDEC_OMX_ERROR_EXIT(_e_, _c_, _s_)                            \
    _e_ = _c_;                                                          \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d : Error Name: %s : Error Num = %x",__LINE__, _s_, _e_);  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    goto EXIT;

/* ======================================================================= */
/**
 * @def    AACDEC_OMX_CONF_CHECK_CMD   Command check Macro
 */
/* ======================================================================= */
#define AACDEC_OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3)  \
    {                                                   \
        if(!_ptr1 || !_ptr2 || !_ptr3){                 \
            eError = OMX_ErrorBadParameter;             \
            goto EXIT;                                  \
        }                                               \
    }

/* ======================================================================= */
/**
 * @def    AACDEC_OMX_FREE   Macro to free the Memory
 */
/* ======================================================================= */
#define AACDEC_OMX_FREE(ptr)                                            \
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
 * @def    AACDEC_BUFDETAILS   Turns buffer messaging on and off
 */
/* ======================================================================= */
#undef AACDEC_BUFDETAILS
/* ======================================================================= */
/**
 * @def    AACDEC_STATEDETAILS   Turns state messaging on and off
 */
/* ======================================================================= */
#undef AACDEC_STATEDETAILS
/* ======================================================================= */
/**
 * @def    AACDEC_MEMDETAILS   Turns memory messaging on and off
 */
/* ======================================================================= */
#undef AACDEC_MEMDETAILS

#define EXTRA_BYTES 128 /* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256 /* For Cache alignment*/
#define AACDEC_OUTPUT_PORT 1
#define AACDEC_INPUT_PORT 0
#define MAX_NUM_OF_BUFS_AACDEC 15
/* ======================================================================= */
/**
 * @def    NUM_OF_PORTS_AACDEC   Number of ports
 */
/* ======================================================================= */
#define NUM_OF_PORTS_AACDEC 2

/* ======================================================================= */
/**
 * @def    AACD_NUM_INPUT_BUFFERS   Default number of input buffers
 *
 */
/* ======================================================================= */
#define AACD_NUM_INPUT_BUFFERS 4
/* ======================================================================= */
/**
 * @def    AACD_NUM_OUTPUT_BUFFERS   Default number of output buffers
 *
 */
/* ======================================================================= */
#define AACD_NUM_OUTPUT_BUFFERS 4

/* ======================================================================= */
/**
 * @def    AACD_INPUT_BUFFER_SIZE   Default input buffer size
 *
 */
/* ======================================================================= */
#define AACD_INPUT_BUFFER_SIZE 1536 // - STEREO
//#define AACD_INPUT_BUFFER_SIZE 6144 //Multichannel
/* ======================================================================= */
/**
 * @def    AACD_OUTPUT_BUFFER_SIZE   Default output buffer size
 *
 */
/* ======================================================================= */
#define MIN_AACD_OUTPUT_BUFFER_SIZE 8192 // Stereo
#define AACD_OUTPUT_BUFFER_SIZE 8192


//#define MIN_AACD_OUTPUT_BUFFER_SIZE 24576
//#define AACD_OUTPUT_BUFFER_SIZE 24576*2
/* ======================================================================= */
/**
 * @def    AACD_SAMPLING_FREQUENCY   Sampling frequency
 */
/* ======================================================================= */
#define AACD_SAMPLING_FREQUENCY 44100

/* ======================================================================= */
/**
 * @def    Mem test application
 */
/* ======================================================================= */
#undef AACDEC_DEBUGMEM

#ifdef AACDEC_DEBUGMEM
#define newmalloc(x) mymalloc(__LINE__,__FILE__,x)
#define newfree(z) myfree(z,__LINE__,__FILE__)
#else
#define newmalloc(x) malloc(x)
#define newfree(z) free(z)
#endif

/* ======================================================================= */
/**
 * @def    AACDec macros for MONO,STEREO_INTERLEAVED,STEREO_NONINTERLEAVED
 */
/* ======================================================================= */
/* Stream types supported*/
#define MONO_STREAM_AACDEC                   1
#define STEREO_INTERLEAVED_STREAM_AACDEC     2
#define STEREO_NONINTERLEAVED_STREAM_AACDEC  3

/**
 *
 * AAC Decoder Profile:0 - MAIN, 1 - LC, 2 - SSR, 3 - LTP.
 */
typedef enum {
    EProfileMain,
    EProfileLC,
    EProfileSSR,
    EProfileLTP
}AACProfile;
/* ======================================================================= */
/** COMP_PORT_TYPE_AACDEC  Port types
 *
 *  @param  INPUT_PORT_AACDEC                    Input port
 *
 *  @param  OUTPUT_PORT_AACDEC               Output port
 */
/*  ==================================================================== */
/*This enum must not be changed. */
typedef enum COMP_PORT_TYPE_AACDEC {
    INPUT_PORT_AACDEC = 0,
    OUTPUT_PORT_AACDEC
}COMP_PORT_TYPE_AACDEC;
/* ======================================================================= */
/** OMX_INDEXAUDIOTYPE_AACDEC  Defines the custom configuration settings
 *                              for the component
 *
 *  @param  OMX_IndexCustomMode16_24bit_AACDEC  Sets the 16/24 mode
 *
 *  @param  OMX_IndexCustomModeProfile_AACDEC  Sets the Profile mode
 *
 *  @param  OMX_IndexCustomModeSBR_AACDEC  Sets the SBR mode
 *
 *  @param  OMX_IndexCustomModeDasfConfig_AACDEC  Sets the DASF mode
 *
 *  @param  OMX_IndexCustomModeRAW_AACDEC  Sets the RAW mode
 *
 *  @param  OMX_IndexCustomModePS_AACDEC  Sets the ParametricStereo mode
 *
 */
/*  ==================================================================== */
typedef enum OMX_INDEXAUDIOTYPE_AACDEC {
    OMX_IndexCustomFramemodeConfig = 0xFF000001,
    OMX_IndexCustomAacDecStreamConfig,
    OMX_IndexCustomAacDecDownmixtoStereo,
    OMX_IndexCustomAacDecDataPath,
    OMX_IndexCustomDebug
}OMX_INDEXAUDIOTYPE_AACDEC;


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
/** AAC_DEC_BUFFERLIST: This contains information about a buffer's owner whether
 * it is application or component, number of buffers owned etc.
 *
 * @see OMX_BUFFERHEADERTYPE
 */
/* ==================================================================== */
struct AAC_DEC_BUFFERLIST{
    /* Array of pointer to OMX buffer headers */
    OMX_BUFFERHEADERTYPE *pBufHdr[MAX_NUM_OF_BUFS_AACDEC];
    /* Array that tells about owner of each buffer */
    OMX_U32 bufferOwner[MAX_NUM_OF_BUFS_AACDEC];
    /* Tracks pending buffers */
    OMX_U32 bBufferPending[MAX_NUM_OF_BUFS_AACDEC];
    /* Number of buffers  */
    OMX_U32 numBuffers;
};

typedef struct AAC_DEC_BUFFERLIST AACDEC_BUFFERLIST;

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
/** AACDEC_COMPONENT_PRIVATE: This is the major and main structure of the
 * component which contains all type of information of buffers, ports etc
 * contained in the component.
 *
 * @see OMX_BUFFERHEADERTYPE
 * @see OMX_AUDIO_PARAM_PORTFORMATTYPE
 * @see OMX_PARAM_PORTDEFINITIONTYPE
 * @see AACD_LCML_BUFHEADERTYPE
 * @see OMX_PORT_PARAM_TYPE
 * @see OMX_PRIORITYMGMTTYPE
 * @see AUDIODEC_PORT_TYPE
 * @see AACDEC_BUFFERLIST
 * @see LCML_STRMATTR
 * @see
 */
/* ==================================================================== */

typedef struct AACDEC_COMPONENT_PRIVATE
{

    OMX_CALLBACKTYPE cbInfo;
    /** Handle for use with async callbacks */
    OMX_PORT_PARAM_TYPE* sPortParam;
    /* Input port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sInPortFormat;
    /* Output port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sOutPortFormat;
    /* Buffer owner information */
    OMX_U32 bIsBufferOwned[NUM_OF_PORTS_AACDEC];

    /** This will contain info like how many buffers
        are there for input/output ports, their size etc, but not
        BUFFERHEADERTYPE POINTERS. */
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef[NUM_OF_PORTS_AACDEC];
    /* Contains information that come from application */
    OMX_AUDIO_PARAM_AACPROFILETYPE* aacParams;

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
    AUDIODEC_PORT_TYPE *pCompPort[NUM_OF_PORTS_AACDEC];

    /* Checks whether or not buffer were allocated by appliction */
    OMX_U32 bufAlloced;

    /** Flag to check about execution of component thread */
    OMX_U16 bExitCompThrd;

    /** Pointer to list of input buffers */
    AACDEC_BUFFERLIST *pInputBufferList;

    /** Pointer to list of output buffers */
    AACDEC_BUFFERLIST *pOutputBufferList;

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
    OMX_BOOL reconfigInputPort;
    OMX_BOOL reconfigOutputPort;
    OMX_U8 OutPendingPR;

    struct OMX_TI_Debug dbg;

    VOID *m_ittiam_handle;
    fd_set rfds;

    ia_heaacv2_dec_params_t  m_ittiam_dec_params;
    ia_heaacv2_input_args_t  m_input_args;
    ia_heaacv2_output_args_t m_output_args;

    OMX_S32 inputBufFilledLen ;
    VOID    *m_inputBuffer;
    UWORD32 m_inputBufferSize;
    WORD32 m_inputBytesPresent;

    VOID    *m_outputBuffer;
	UWORD32 m_outputBufferSize;
    UWORD32 m_outputBytesPresent;

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

} AACDEC_COMPONENT_PRIVATE;

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
 *  @see          AacDec_StartCompThread()
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
 * AacDec_StartCompThread() starts the component thread. This is internal
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
OMX_ERRORTYPE AacDec_StartCompThread(OMX_HANDLETYPE pHandle);
/* ================================================================================= * */
/**
 * AACDEC_HandleCommand() function handles the command sent by the application.
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
OMX_U32 AACDEC_HandleCommand (AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
/* ================================================================================= * */
/**
 * AACDEC_HandleDataBuf_FromApp() function handles the input and output buffers
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
OMX_ERRORTYPE AACDEC_HandleDataBuf_FromApp(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
void* AACDEC_ComponentThread (void* pThreadData);
OMX_ERRORTYPE AACDEC_HandleCommandStateSet(AACDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                           OMX_S32                   commandData);
OMX_ERRORTYPE AACDEC_SetStateToIdle(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE AACDEC_SetStateToExecuting(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE AACDEC_SetStateToPause(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE AACDEC_SetStateToLoaded(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE AACDEC_HandleCommandPortDisable(AACDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData);
OMX_ERRORTYPE AACDEC_HandleCommandPortEnable(AACDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData);

OMX_ERRORTYPE AACDEC_InitializeTheComponent(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE AACDEC_CloseTheComponent(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE AACDEC_FreeCompResources(OMX_HANDLETYPE pComponent);
OMX_ERRORTYPE ReturnOutputBuffers(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE ReturnInputBuffers(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE ReturnInputOutputBuffers(AACDEC_COMPONENT_PRIVATE *pComponentPrivate);

#endif
