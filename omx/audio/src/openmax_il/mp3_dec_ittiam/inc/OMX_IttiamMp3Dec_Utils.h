/*****************************************************************************/
/*                                                                           */
/*                          MP3 Decoder                                      */
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
/*  File Name        : OMX_IttiamMp3Dec_Utils.h                              */
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

#ifndef OMX_MP3DEC_UTILS__H
#define OMX_MP3DEC_UTILS__H

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
#include "ia_mp3_dec_omx.h"
#include "ia_mp3_dec_error_codes.h"

#ifndef ANDROID
    #define ANDROID
#endif

#ifdef ANDROID
    /* Log for Android system*/
    #undef LOG_TAG
    #define LOG_TAG "OMX_MP3DEC"

    /* PV opencore capability custom parameter index */
    #define PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX 0xFF7A347
#endif


#define MP3DEC_MAJOR_VER 0x1/* Major number of the component */
#define MP3DEC_MINOR_VER 0x1 /* Mnor number of the component */
#define NOT_USED 10 /* Value not used */
#define NORMAL_BUFFER 0 /* Marks a buffer as normal buffer */
#define OMX_MP3DEC_DEFAULT_SEGMENT (0) /* default segment ID of the component */
#define OMX_MP3DEC_SN_TIMEOUT (-1) /* tomeout value keep waiting until get the message */
#define OMX_MP3DEC_SN_PRIORITY (10) /* SN priority value */

#define MP3DEC_BUFHEADER_VERSION 0x0 /* Buffer Header structure version number */
#define MP3D_TIMEOUT (1000) /* default timeout in millisecs */
#define MP3_CPU 25

/* #define DSP_RENDERING_ON*/ /* Enable to use DASF functionality */
/* #define MP3DEC_MEMDEBUG */ /* Enable memory leaks debuf info */
//#define MP3DEC_DEBUG    /* See all debug statement of the component */
/* #define MP3DEC_MEMDETAILS */  /* See memory details of the component */
/* #define MP3DEC_BUFDETAILS */  /* See buffers details of the component */
// #define MP3DEC_STATEDETAILS /* See all state transitions of the component*/

#define MP3_APP_ID  100 /* Defines MP3 Dec App ID, App must use this value */
#define MP3D_MAX_NUM_OF_BUFS 10 /* Max number of buffers used */
//#define MP3D_NUM_INPUT_BUFFERS 4  /* Default number of input buffers */
//#define MP3D_NUM_OUTPUT_BUFFERS 4 /* Default number of output buffers */

//#define MP3D_INPUT_BUFFER_SIZE  2048*10 /* Default size of input buffer */
//#define MP3D_OUTPUT_BUFFER_SIZE 8192*10 /* Default size of output buffer */
#define MP3D_SAMPLING_FREQUENCY 44100

/*#define OUTPUT_PORT_MP3DEC 1
#define INPUT_PORT_MP3DEC 0*/

#define MP3D_MONO_STREAM  1 /* Mono stream index */
#define MP3D_STEREO_INTERLEAVED_STREAM  2 /* Stereo Interleaved stream index */
#define MP3D_STEREO_NONINTERLEAVED_STREAM  3 /* Stereo Non-Interleaved stream index */

#define EXTRA_BYTES 128 /* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256 /* For Cache alignment*/

#define MP3D_STEREO_STREAM  2

#define EXIT_COMPONENT_THRD  10

#define ENABLE_ITTIAM_MP3_DECODER
//#define ENABLE_ITTIAM_PROFILE
//#define ENABLE_ITTIAM_DUMP
/* ======================================================================= */
/**
 * @def    MP3_DEC__XXX_VER    Component version
 */
/* ======================================================================= */
#define MP3DEC_MAJOR_VER 1
#define MP3DEC_MINOR_VER 1
/* ======================================================================= */
/**
 * @def    NOT_USED_MP3DEC    Defines a value for "don't care" parameters
 */
/* ======================================================================= */
#define NOT_USED_MP3DEC 0
/* ======================================================================= */
/**
 * @def    NORMAL_BUFFER_MP3DEC    Defines the flag value with all flags turned off
 */
/* ======================================================================= */
#define NORMAL_BUFFER_MP3DEC 0
/* ======================================================================= */
/**
 * @def    OMX_MP3DEC_DEFAULT_SEGMENT    Default segment ID for the LCML
 */
/* ======================================================================= */
#define OMX_MP3DEC_DEFAULT_SEGMENT (0)
/* ======================================================================= */

#define MP3DEC_BUFHEADER_VERSION 0x1
/* ======================================================================= */

/* ======================================================================= */
#define MP3D_TIMEOUT (1000) /* millisecs */

#define DONT_CARE 0

/* ======================================================================= */
/**
 * @def    MP3DEC_CPU_USAGE for Resource Mannager (MHZ)
 */
/* ======================================================================= */
#define MP3DEC_CPU_USAGE 50


/* ======================================================================= */
/**
 * @def    MP3DEC_SBR_CONTENT  flag detection
 */
/* ======================================================================= */

#define MP3DEC_SBR_CONTENT 0x601


/* ======================================================================= */
/**
 * @def    MP3DEC_PS_CONTENT flag  detection
 */
/* ======================================================================= */

#define  MP3DEC_PS_CONTENT 0x602


/* ======================================================================= */
/**
 * @def    MP3DEC_DEBUG   Debug print macro
 */
/* ======================================================================= */

#undef MP3DEC_DEBUG
#define _ERROR_PROPAGATION__

#ifdef UNDER_CE

/* ======================================================================= */
/**
 * @def    DEBUG   Memory print macro
 */
/* ======================================================================= */
#if DEBUG
#define MP3DEC_DPRINT printf
#define MP3DEC_MEMPRINT printf
#define MP3DEC_STATEPRINT printf
#define MP3DEC_BUFPRINT printf
#define MP3DEC_MEMPRINT printf
#define MP3DEC_EPRINT printf
#else
#define MP3DEC_DPRINT
#define MP3DEC_MEMPRINT
#define MP3DEC_STATEPRINT
#define MP3DEC_BUFPRINT
#define MP3DEC_MEMPRINT
#define MP3DEC_EPRINT
#endif

#else /* for Linux */

#ifdef  MP3DEC_DEBUG
    #define MP3DEC_DPRINT printf    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d:: ",__FUNCTION__, __LINE__);\
                                //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                                    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n");

    #undef MP3DEC_BUFPRINT printf
    #undef MP3DEC_MEMPRINT printf
    #define MP3DEC_STATEPRINT printf
#else
    #define MP3DEC_DPRINT(...)
#endif

#ifdef MP3DEC_STATEDETAILS
    #define MP3DEC_STATEPRINT printf
#else
    #define MP3DEC_STATEPRINT(...)
#endif

#ifdef MP3DEC_BUFDETAILS
    #define MP3DEC_BUFPRINT printf
#else
    #define MP3DEC_BUFPRINT(...)
#endif

#ifdef MP3DEC_MEMDETAILS
    #define MP3DEC_MEMPRINT(...)  fprintf(stdout, "%s %d::  ",__FUNCTION__, __LINE__); \
                                  fprintf(stdout, __VA_ARGS__); \
                                  fprintf(stdout, "\n");
#else
    #define MP3DEC_MEMPRINT(...)
#endif

#define MP3DEC_EPRINT LOGE

                           /* __android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d::    ERROR",__FUNCTION__, __LINE__);\
                        __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                            __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n"); */

#endif

/* ======================================================================= */
/**
 * @def    MP3D_OMX_MALLOC   Macro to allocate Memory
 */
/* ======================================================================= */
#define MP3D_OMX_MALLOC(_pStruct_, _sName_)                         \
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
 * @def    MP3DEC_OMX_MALLOC_SIZE   Macro to allocate Memory
 */
/* ======================================================================= */
#define MP3DEC_OMX_MALLOC_SIZE(_ptr_, _size_,_name_)            \
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
 * @def    MP3DEC_OMX_ERROR_EXIT   Exit print and return macro
 */
/* ======================================================================= */
#define MP3DEC_OMX_ERROR_EXIT(_e_, _c_, _s_)                            \
    _e_ = _c_;                                                          \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d : Error Name: %s : Error Num = %x",__LINE__, _s_, _e_);  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    goto EXIT;

/* ======================================================================= */
/**
 * @def    MP3DEC_OMX_CONF_CHECK_CMD   Command check Macro
 */
/* ======================================================================= */
#define MP3DEC_OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3)  \
    {                                                   \
        if(!_ptr1 || !_ptr2 || !_ptr3){                 \
            eError = OMX_ErrorBadParameter;             \
            goto EXIT;                                  \
        }                                               \
    }

/* ======================================================================= */
/**
 * @def    MP3DEC_OMX_FREE   Macro to free the Memory
 */
/* ======================================================================= */
#define MP3DEC_OMX_FREE(ptr)                                            \
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
 * @def    MP3DEC_BUFDETAILS   Turns buffer messaging on and off
 */
/* ======================================================================= */
#undef MP3DEC_BUFDETAILS
/* ======================================================================= */
/**
 * @def    MP3DEC_STATEDETAILS   Turns state messaging on and off
 */
/* ======================================================================= */
#undef MP3DEC_STATEDETAILS
/* ======================================================================= */
/**
 * @def    MP3DEC_MEMDETAILS   Turns memory messaging on and off
 */
/* ======================================================================= */
#undef MP3DEC_MEMDETAILS

#define EXTRA_BYTES 128 /* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256 /* For Cache alignment*/
#define MP3DEC_OUTPUT_PORT 1
#define MP3DEC_INPUT_PORT 0
#define MP3DEC_APP_ID  100
#define MAX_NUM_OF_BUFS_MP3DEC 15
#define PARAMETRIC_STEREO_MP3DEC 1
#define NON_PARAMETRIC_STEREO_MP3DEC 0
/* ======================================================================= */
/**
 * @def    NUM_OF_PORTS_MP3DEC   Number of ports
 */
/* ======================================================================= */
#define NUM_OF_PORTS_MP3DEC 2
/* ======================================================================= */
/**
 * @def    STREAM_COUNT_MP3DEC   Number of streams
 */
/* ======================================================================= */
#define STREAM_COUNT_MP3DEC 2

/** Default timeout used to come out of blocking calls*/

/* ======================================================================= */
/**
 * @def    MP3D_NUM_INPUT_BUFFERS   Default number of input buffers
 *
 */
/* ======================================================================= */
#define MP3D_NUM_INPUT_BUFFERS 4
/* ======================================================================= */
/**
 * @def    MP3D_NUM_OUTPUT_BUFFERS   Default number of output buffers
 *
 */
/* ======================================================================= */
#define MP3D_NUM_OUTPUT_BUFFERS 4

/* ======================================================================= */
/**
 * @def    MP3D_INPUT_BUFFER_SIZE   Default input buffer size
 *
 */
/* ======================================================================= */
#define MIN_MP3D_INPUT_BUFFER_SIZE (1536)
#define MP3D_INPUT_BUFFER_SIZE MIN_MP3D_INPUT_BUFFER_SIZE*60 //90kB for DivX requirements
/* ======================================================================= */
/**
 * @def    MP3D_OUTPUT_BUFFER_SIZE   Default output buffer size
 *
 */
/* ======================================================================= */
#define MP3D_OUTPUT_BUFFER_SIZE 8192*10 //8192*10Is it not too big ?
#define MIN_MP3D_OUTPUT_BUFFER_SIZE (4608)

/* ======================================================================= */
/**
 * @def    Mem test application
 */
/* ======================================================================= */
#undef MP3DEC_DEBUGMEM

#ifdef MP3DEC_DEBUGMEM
#define newmalloc(x) mymalloc(__LINE__,__FILE__,x)
#define newfree(z) myfree(z,__LINE__,__FILE__)
#else
#define newmalloc(x) malloc(x)
#define newfree(z) free(z)
#endif

/* ======================================================================= */
/**
 * @def    MP3Dec macros for MONO,STEREO_INTERLEAVED,STEREO_NONINTERLEAVED
 */
/* ======================================================================= */
/*#define MP3D_STEREO_INTERLEAVED_STREAM     2
  #define MP3D_STEREO_NONINTERLEAVED_STREAM  3*/
/* ======================================================================= */
/**
 * @def    MP3Dec macros for MONO,STEREO_INTERLEAVED,STEREO_NONINTERLEAVED
 */
/* ======================================================================= */
/* Stream types supported*/
#define MONO_STREAM_MP3DEC                   1
#define STEREO_STREAM_STEREO     2

/**
 *
 * MP3 Decoder Profile:0 - MAIN, 1 - LC, 2 - SSR, 3 - LTP.
 */
/*typedef enum {
    EProfileMain,
    EProfileLC,
    EProfileSSR,
    EProfileLTP
}MP3Profile;*/
/* ======================================================================= */
/** COMP_PORT_TYPE_MP3DEC  Port types
 *
 *  @param  INPUT_PORT_MP3DEC                    Input port
 *
 *  @param  OUTPUT_PORT_MP3DEC               Output port
 */
/*  ==================================================================== */
/*This enum must not be changed. */
typedef enum COMP_PORT_TYPE_MP3DEC {
    INPUT_PORT_MP3DEC = 0,
    OUTPUT_PORT_MP3DEC
}COMP_PORT_TYPE_MP3DEC;
/* ======================================================================= */
/** OMX_INDEXAUDIOTYPE_MP3DEC  Defines the custom configuration settings
 *                              for the component
 *
 *  @param  OMX_IndexCustomMode16_24bit_MP3DEC  Sets the 16/24 mode
 *
 *  @param  OMX_IndexCustomModeProfile_MP3DEC  Sets the Profile mode
 *
 *  @param  OMX_IndexCustomModeSBR_MP3DEC  Sets the SBR mode
 *
 *  @param  OMX_IndexCustomModeDasfConfig_MP3DEC  Sets the DASF mode
 *
 *  @param  OMX_IndexCustomModeRAW_MP3DEC  Sets the RAW mode
 *
 *  @param  OMX_IndexCustomModePS_MP3DEC  Sets the ParametricStereo mode
 *
 */
/*  ==================================================================== */
typedef enum OMX_INDEXAUDIOTYPE_MP3DEC {
    OMX_IndexCustomFramemodeConfig = 0xFF000001,
    /*MP3D_OMX_IndexCustomModeDasfConfig,
    OMX_IndexCustomMp3DecHeaderInfoConfig,
    OMX_IndexCustomMp3DecStreamInfoConfig,
    OMX_IndexCustomMp3DecDataPath,*/
    OMX_IndexCustomDebug
}OMX_INDEXAUDIOTYPE_MP3DEC;

/* ======================================================================= */
/** IAUDIO_PcmFormat: This value is used by DSP.
 *
 * @param IAUDIO_BLOCK: It is used in DASF mode.
 *
 * @param IAUDIO_INTERLEAVED: It specifies interleaved format of SN.
 */
/* ==================================================================== */
typedef enum {
    EAUDIO_BLOCK =0,
    EAUDIO_INTERLEAVED
}TAUDIO_Mp3Format;

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
/** MP3_DEC_BUFFERLIST: This contains information about a buffer's owner whether
 * it is application or component, number of buffers owned etc.
 *
 * @see OMX_BUFFERHEADERTYPE
 */
/* ==================================================================== */
struct MP3_DEC_BUFFERLIST{
    /* Array of pointer to OMX buffer headers */
    OMX_BUFFERHEADERTYPE *pBufHdr[MAX_NUM_OF_BUFS_MP3DEC];
    /* Array that tells about owner of each buffer */
    OMX_U32 bufferOwner[MAX_NUM_OF_BUFS_MP3DEC];
    /* Tracks pending buffers */
    OMX_U32 bBufferPending[MAX_NUM_OF_BUFS_MP3DEC];
    /* Number of buffers  */
    OMX_U32 numBuffers;
};

typedef struct MP3_DEC_BUFFERLIST MP3DEC_BUFFERLIST;

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
/** MP3DEC_COMPONENT_PRIVATE: This is the major and main structure of the
 * component which contains all type of information of buffers, ports etc
 * contained in the component.
 *
 * @see OMX_BUFFERHEADERTYPE
 * @see OMX_AUDIO_PARAM_PORTFORMATTYPE
 * @see OMX_PARAM_PORTDEFINITIONTYPE
 * @see MP3D_LCML_BUFHEADERTYPE
 * @see OMX_PORT_PARAM_TYPE
 * @see OMX_PRIORITYMGMTTYPE
 * @see AUDIODEC_PORT_TYPE
 * @see MP3DEC_BUFFERLIST
 * @see LCML_STRMATTR
 * @see
 */
/* ==================================================================== */

typedef struct MP3DEC_COMPONENT_PRIVATE
{

    OMX_CALLBACKTYPE cbInfo;
    /** Handle for use with async callbacks */
    OMX_PORT_PARAM_TYPE* sPortParam;
    /* Input port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sInPortFormat;
    /* Output port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sOutPortFormat;
    /* Buffer owner information */
    OMX_U32 bIsBufferOwned[NUM_OF_PORTS_MP3DEC];

    /** This will contain info like how many buffers
        are there for input/output ports, their size etc, but not
        BUFFERHEADERTYPE POINTERS. */
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef[NUM_OF_PORTS_MP3DEC];
    /* Contains information that come from application */
    OMX_AUDIO_PARAM_MP3TYPE* mp3Params;

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

    /** Set to indicate component is stopping */
    OMX_U32 bIsEOFSent;

    /** Holds the value of dasf mode, 1: DASF mode or 0: File Mode  */
    OMX_U32 dasfmode;

#ifdef __PERF_INSTRUMENTATION__
    PERF_OBJHANDLE pPERF, pPERFcomp;
/*    OMX_U32 nLcml_nCntIp;
    OMX_U32 nLcml_nCntOpReceived;*/
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
    AUDIODEC_PORT_TYPE *pCompPort[NUM_OF_PORTS_MP3DEC];

    /* Checks whether or not buffer were allocated by appliction */
    OMX_U32 bufAlloced;

    /** Flag to check about execution of component thread */
    OMX_U16 bExitCompThrd;

    /** Pointer to list of input buffers */
    MP3DEC_BUFFERLIST *pInputBufferList;

    /** Pointer to list of output buffers */
    MP3DEC_BUFFERLIST *pOutputBufferList;

    /** Contains the version information */
    OMX_U32 nVersion;

    OMX_U16 framemode;

    OMX_STRING cComponentName;

    OMX_VERSIONTYPE ComponentVersion;

    OMX_U32 nOpBit;
  /*  OMX_U32 parameteric_stereo;
    OMX_U32 nProfile;
    OMX_U32 SBR;
    OMX_U32 RAW;*/
    OMX_U32 bInitParamsInitialized;
    /*MP3DEC_BUFFERLIST *pInputBufferListQueue;
    MP3DEC_BUFFERLIST *pOutputBufferListQueue;*/

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

#ifdef ENABLE_ITTIAM_MP3_DECODER

    FILE *m_fp;
    VOID *m_ittiam_handle;

    fd_set rfds;

    ia_mp3_dec_params_t  m_ittiam_dec_params;
    ia_mp3_dec_input_args_t  m_input_args;
    ia_mp3_dec_output_args_t m_output_args;


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
	//OMX_S32 iPortReconfigurationOngoing;
    OMX_BOOL m_shouldConvertToStereo;

//Shashank Noise Fix [
    OMX_S32 m_numberOfOutputFramesToDrop;
    OMX_BOOL m_bIsFirstTime;
//Shashank Noise Fix ]

#ifdef ENABLE_ITTIAM_PROFILE
    UWORD32 m_avgDecodeTimeMS;
    UWORD32 m_peakDecodeTimeMS;
    UWORD32 m_totalDecodeTimeMS;
    UWORD32 m_numOfDecodes;
#endif

#endif

} MP3DEC_COMPONENT_PRIVATE;

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
 *  @see          Mp3Dec_StartCompThread()
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
 * Mp3Dec_StartCompThread() starts the component thread. This is internal
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
OMX_ERRORTYPE Mp3Dec_StartCompThread(OMX_HANDLETYPE pHandle);
/* ================================================================================= * */
/**
 * MP3DEC_GetBufferDirection() function determines whether it is input buffer or
 * output buffer.
 *
 * @param *pBufHeader This is pointer to buffer header whose direction needs to
 *                    be determined.
 *
 * @param *eDir  This is output argument which stores the direction of buffer.
 *
 * @pre          None
 *
 * @post         None
 *
 *  @return      OMX_ErrorNone = Successful processing.
 *               OMX_ErrorBadParameter = In case of invalid buffer
 *
 *  @see         None
 */
/* ================================================================================ * */
OMX_ERRORTYPE MP3DEC_GetBufferDirection(OMX_BUFFERHEADERTYPE *pBufHeader,
                                        OMX_DIRTYPE *eDir);
/* ================================================================================= * */
/**
 * MP3DEC_HandleCommand() function handles the command sent by the application.
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
OMX_U32 MP3DEC_HandleCommand (MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);
/* ================================================================================= * */
/**
 * MP3DEC_HandleDataBuf_FromApp() function handles the input and output buffers
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
OMX_ERRORTYPE MP3DEC_HandleDataBuf_FromApp(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);

/* void MP3DEC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData); */

void* MP3DEC_ComponentThread (void* pThreadData);




OMX_ERRORTYPE MP3DEC_HandleCommandStateSet(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate,
                                           OMX_U32                   commandData);
OMX_ERRORTYPE MP3DEC_SetStateToIdle(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE MP3DEC_SetStateToExecuting(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE MP3DEC_SetStateToPause(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE MP3DEC_SetStateToLoaded(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE MP3DEC_HandleCommandPortDisable(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_U32                  commandData);
OMX_ERRORTYPE MP3DEC_HandleCommandPortEnable(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_U32                  commandData);

OMX_ERRORTYPE MP3DEC_InitializeTheComponent(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE MP3DEC_CloseTheComponent(MP3DEC_COMPONENT_PRIVATE *pComponentPrivate);

#endif
