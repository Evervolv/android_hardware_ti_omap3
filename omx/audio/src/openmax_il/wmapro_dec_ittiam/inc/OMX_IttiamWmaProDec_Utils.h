/*****************************************************************************/
/*                                                                           */
/*                          WMA PRO Decoder                                  */
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
/*  File Name        : OMX_IttiamWmaProDec_Utils.h                              */
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

#ifndef OMX_WMAPRODEC_UTILS__H
#define OMX_WMAPRODEC_UTILS__H

#include <OMX_Component.h>
#include <OMX_TI_Common.h>
#include <OMX_TI_Debug.h>
#include <pthread.h>
#include <sched.h>

/*------------Dummy Debug defines starts-----------------------*/
/*------------Use LOGD for debug prints------------------------*/
#include <utils/Log.h>

/*struct OMX_TI_Debug
{
    FILE *out;    /* standard output file */
 //   FILE *err;    /* error output file - used for the error domain
                  //   and crucial messages */
 //   FILE *out_opened;  /* opened output file */
  //  FILE *err_opened;  /* opened error file */
  //  OMX_U32 mask; /* debug mask */
//};

#define OMX_DBG_INIT(dbg, tag_str)
#define OMX_DBG_INIT_MASK(dbg, tag_str)
#define OMX_DBG_SETCONFIG(dbg, pConfig)
#define OMX_DBG_GETCONFIG(dbg, pConfig)
#define OMX_DBG_CLOSE(dbg)

#define ERROR 0
#define SYSTEM 0
#define PRINT 0
#define BUFFER 0
#define MGR 0
#define COMM 0
#define STATE 0

#define OMXDBG_PRINT(file, domain, level, mask, format, list...)

/* Shortcuts */
#define OMX_ERROR0(dbg, format, list...) OMXDBG_PRINT((dbg).err, ERROR, 0, (dbg).mask, format, ##list)
#define OMX_ERROR1(dbg, format, list...) OMXDBG_PRINT((dbg).err, ERROR, 1, (dbg).mask, format, ##list)
#define OMX_ERROR2(dbg, format, list...) OMXDBG_PRINT((dbg).err, ERROR, 2, (dbg).mask, format, ##list)
#define OMX_ERROR3(dbg, format, list...) OMXDBG_PRINT((dbg).err, ERROR, 3, (dbg).mask, format, ##list)
#define OMX_ERROR4(dbg, format, list...) OMXDBG_PRINT((dbg).err, ERROR, 4, (dbg).mask, format, ##list)
#define OMX_ERROR5(dbg, format, list...) OMXDBG_PRINT((dbg).err, ERROR, 5, (dbg).mask, format, ##list)
#define OMX_TRACE0(dbg, format, list...) OMXDBG_PRINT((dbg).out, SYSTEM, 0, (dbg).mask, format, ##list)
#define OMX_TRACE1(dbg, format, list...) OMXDBG_PRINT((dbg).out, SYSTEM, 1, (dbg).mask, format, ##list)
#define OMX_TRACE2(dbg, format, list...) OMXDBG_PRINT((dbg).out, SYSTEM, 2, (dbg).mask, format, ##list)
#define OMX_TRACE3(dbg, format, list...) OMXDBG_PRINT((dbg).out, SYSTEM, 3, (dbg).mask, format, ##list)
#define OMX_TRACE4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, SYSTEM, 4, (dbg).mask, format, ##list)
#define OMX_TRACE5(dbg, format, list...) OMXDBG_PRINT((dbg).err, SYSTEM, 5, (dbg).mask, format, ##list)
#define OMX_PRINT0(dbg, format, list...) OMXDBG_PRINT((dbg).out, PRINT, 0, (dbg).mask, format, ##list)
#define OMX_PRINT1(dbg, format, list...) OMXDBG_PRINT((dbg).out, PRINT, 1, (dbg).mask, format, ##list)
#define OMX_PRINT2(dbg, format, list...) OMXDBG_PRINT((dbg).out, PRINT, 2, (dbg).mask, format, ##list)
#define OMX_PRINT3(dbg, format, list...) OMXDBG_PRINT((dbg).out, PRINT, 3, (dbg).mask, format, ##list)
#define OMX_PRINT4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, PRINT, 4, (dbg).mask, format, ##list)
#define OMX_PRINT5(dbg, format, list...) OMXDBG_PRINT((dbg).err, PRINT, 5, (dbg).mask, format, ##list)
#define OMX_PRBUFFER0(dbg, format, list...) OMXDBG_PRINT((dbg).out, BUFFER, 0, (dbg).mask, format, ##list)
#define OMX_PRBUFFER1(dbg, format, list...) OMXDBG_PRINT((dbg).out, BUFFER, 1, (dbg).mask, format, ##list)
#define OMX_PRBUFFER2(dbg, format, list...) OMXDBG_PRINT((dbg).out, BUFFER, 2, (dbg).mask, format, ##list)
#define OMX_PRBUFFER3(dbg, format, list...) OMXDBG_PRINT((dbg).out, BUFFER, 3, (dbg).mask, format, ##list)
#define OMX_PRBUFFER4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, BUFFER, 4, (dbg).mask, format, ##list)
#define OMX_PRBUFFER5(dbg, format, list...) OMXDBG_PRINT((dbg).err, BUFFER, 5, (dbg).mask, format, ##list)
#define OMX_PRMGR0(dbg, format, list...) OMXDBG_PRINT((dbg).out, MGR, 0, (dbg).mask, format, ##list)
#define OMX_PRMGR1(dbg, format, list...) OMXDBG_PRINT((dbg).out, MGR, 1, (dbg).mask, format, ##list)
#define OMX_PRMGR2(dbg, format, list...) OMXDBG_PRINT((dbg).out, MGR, 2, (dbg).mask, format, ##list)
#define OMX_PRMGR3(dbg, format, list...) OMXDBG_PRINT((dbg).out, MGR, 3, (dbg).mask, format, ##list)
#define OMX_PRMGR4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, MGR, 4, (dbg).mask, format, ##list)
#define OMX_PRMGR5(dbg, format, list...) OMXDBG_PRINT((dbg).err, MGR, 5, (dbg).mask, format, ##list)
#define OMX_PRDSP0(dbg, format, list...) OMXDBG_PRINT((dbg).out, DSP, 0, (dbg).mask, format, ##list)
#define OMX_PRDSP1(dbg, format, list...) OMXDBG_PRINT((dbg).out, DSP, 1, (dbg).mask, format, ##list)
#define OMX_PRDSP2(dbg, format, list...) OMXDBG_PRINT((dbg).out, DSP, 2, (dbg).mask, format, ##list)
#define OMX_PRDSP3(dbg, format, list...) OMXDBG_PRINT((dbg).out, DSP, 3, (dbg).mask, format, ##list)
#define OMX_PRDSP4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, DSP, 4, (dbg).mask, format, ##list)
#define OMX_PRDSP5(dbg, format, list...) OMXDBG_PRINT((dbg).err, DSP, 5, (dbg).mask, format, ##list)
#define OMX_PRCOMM0(dbg, format, list...) OMXDBG_PRINT((dbg).out, COMM, 0, (dbg).mask, format, ##list)
#define OMX_PRCOMM1(dbg, format, list...) OMXDBG_PRINT((dbg).out, COMM, 1, (dbg).mask, format, ##list)
#define OMX_PRCOMM2(dbg, format, list...) OMXDBG_PRINT((dbg).out, COMM, 2, (dbg).mask, format, ##list)
#define OMX_PRCOMM3(dbg, format, list...) OMXDBG_PRINT((dbg).out, COMM, 3, (dbg).mask, format, ##list)
#define OMX_PRCOMM4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, COMM, 4, (dbg).mask, format, ##list)
#define OMX_PRCOMM5(dbg, format, list...) OMXDBG_PRINT((dbg).err, COMM, 5, (dbg).mask, format, ##list)
#define OMX_PRSTATE0(dbg, format, list...) OMXDBG_PRINT((dbg).out, STATE, 0, (dbg).mask, format, ##list)
#define OMX_PRSTATE1(dbg, format, list...) OMXDBG_PRINT((dbg).out, STATE, 1, (dbg).mask, format, ##list)
#define OMX_PRSTATE2(dbg, format, list...) OMXDBG_PRINT((dbg).out, STATE, 2, (dbg).mask, format, ##list)
#define OMX_PRSTATE3(dbg, format, list...) OMXDBG_PRINT((dbg).out, STATE, 3, (dbg).mask, format, ##list)
#define OMX_PRSTATE4(dbg, format, list...) OMXDBG_PRINT((dbg).OMX_DBG_LEVEL4, STATE, 4, (dbg).mask, format, ##list)
#define OMX_PRSTATE5(dbg, format, list...) OMXDBG_PRINT((dbg).err, STATE, 5, (dbg).mask, format, ##list)

/*------------Debug defines ends-----------------------*/

#define AUDIO_MANAGER

#ifdef __PERF_INSTRUMENTATION__
#include "perf.h"
#endif

/*------- Ittiam Header Files -----------------------------------------------*/
#include "ia_wma_pro_dec_omx.h"

#ifndef ANDROID
    #define ANDROID
#endif

#ifdef ANDROID
    /* Log for Android system*/
    #undef LOG_TAG
    #define LOG_TAG "OMX_WMAPRODEC"

    /* PV opencore capability custom parameter index */
    #define PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX 0xFF7A347
#endif


#define WMAPRODEC_MAJOR_VER 0x1/* Major number of the component */
#define WMAPRODEC_MINOR_VER 0x1 /* Mnor number of the component */
#define NOT_USED 10 /* Value not used */
#define NORMAL_BUFFER 0 /* Marks a buffer as normal buffer */
#define OMX_WMAPRODEC_DEFAULT_SEGMENT (0) /* default segment ID of the component */
#define OMX_WMAPRODEC_SN_TIMEOUT (-1) /* tomeout value keep waiting until get the message */
#define OMX_WMAPRODEC_SN_PRIORITY (10) /* SN priority value */

#define WMAPRODEC_BUFHEADER_VERSION 0x0 /* Buffer Header structure version number */
#define WMAPROD_TIMEOUT (1000) /* default timeout in millisecs */
#define WMAPRO_CPU 25

/* #define DSP_RENDERING_ON*/ /* Enable to use DASF functionality */
/* #define WMAPRODEC_MEMDEBUG */ /* Enable memory leaks debuf info */
//#define WMAPRODEC_DEBUG    /* See all debug statement of the component */
/* #define WMAPRODEC_MEMDETAILS */  /* See memory details of the component */
/* #define WMAPRODEC_BUFDETAILS */  /* See buffers details of the component */
// #define WMAPRODEC_STATEDETAILS /* See all state transitions of the component*/

#define WMAPRO_APP_ID  100 /* Defines WMAPRO Dec App ID, App must use this value */
#define WMAPROD_MAX_NUM_OF_BUFS 10 /* Max number of buffers used */
//#define WMAPROD_NUM_INPUT_BUFFERS 4  /* Default number of input buffers */
//#define WMAPROD_NUM_OUTPUT_BUFFERS 4 /* Default number of output buffers */

//#define WMAPROD_INPUT_BUFFER_SIZE  2000*4 /* Default size of input buffer */
//#define WMAPROD_OUTPUT_BUFFER_SIZE 8192*10 /* Default size of output buffer */
#define WMAPROD_SAMPLING_FREQUENCY 44100

#define WMAPROD_MONO_STREAM  1 /* Mono stream index */
#define WMAPROD_STEREO_INTERLEAVED_STREAM  2 /* Stereo Interleaved stream index */
#define WMAPROD_STEREO_NONINTERLEAVED_STREAM  3 /* Stereo Non-Interleaved stream index */

#define EXTRA_BYTES 128 /* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256 /* For Cache alignment*/

#define WMAPROD_STEREO_STREAM  2

#define EXIT_COMPONENT_THRD  10

#define ENABLE_ITTIAM_WMAPRO_DECODER
//#define ENABLE_ITTIAM_PROFILE
//#define ENABLE_ITTIAM_DUMP
/* ======================================================================= */
/**
 * @def    WMA_DEC__XXX_VER    Component version
 */
/* ======================================================================= */
//#define WMAPRODEC_MAJOR_VER 1
//#define WMAPRODEC_MINOR_VER 1
/* ======================================================================= */
/**
 * @def    NOT_USED_WMAPRODEC    Defines a value for "don't care" parameters
 */
/* ======================================================================= */
#define NOT_USED_WMAPRODEC 0
/* ======================================================================= */
/**
 * @def    NORMAL_BUFFER_WMAPRODEC    Defines the flag value with all flags turned off
 */
/* ======================================================================= */
#define NORMAL_BUFFER_WMAPRODEC 0
/* ======================================================================= */
/**
 * @def    OMX_WMAPRODEC_DEFAULT_SEGMENT    Default segment ID for the LCML
 */
/* ======================================================================= */
#define OMX_WMAPRODEC_DEFAULT_SEGMENT (0)
/* ======================================================================= */

//#define WMAPRODEC_BUFHEADER_VERSION 0x1
/* ======================================================================= */

/* ======================================================================= */
#define WMAPROD_TIMEOUT (1000) /* millisecs */

#define DONT_CARE 0

/* ======================================================================= */
/**
 * @def    WMAPRODEC_CPU_USAGE for Resource Mannager (MHZ)
 */
/* ======================================================================= */
#define WMAPRODEC_CPU_USAGE 50


/* ======================================================================= */
/**
 * @def    WMAPRODEC_DEBUG   Debug print macro
 */
/* ======================================================================= */

#undef WMAPRODEC_DEBUG
#define _ERROR_PROPAGATION__

#ifdef UNDER_CE

/* ======================================================================= */
/**
 * @def    DEBUG   Memory print macro
 */
/* ======================================================================= */
#if DEBUG
#define WMAPRODEC_DPRINT printf
#define WMAPRODEC_MEMPRINT printf
#define WMAPRODEC_STATEPRINT printf
#define WMAPRODEC_BUFPRINT printf
#define WMAPRODEC_MEMPRINT printf
#define WMAPRODEC_EPRINT printf
#else
#define WMAPRODEC_DPRINT
#define WMAPRODEC_MEMPRINT
#define WMAPRODEC_STATEPRINT
#define WMAPRODEC_BUFPRINT
#define WMAPRODEC_MEMPRINT
#define WMAPRODEC_EPRINT
#endif

#else /* for Linux */

#ifdef  WMAPRODEC_DEBUG
    #define WMAPRODEC_DPRINT printf    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d:: ",__FUNCTION__, __LINE__);
                                //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);
                                    //__android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n");

    #undef WMAPRODEC_BUFPRINT printf
    #undef WMAPRODEC_MEMPRINT printf
    #define WMAPRODEC_STATEPRINT printf
#else
    #define WMAPRODEC_DPRINT(...)
#endif

#ifdef WMAPRODEC_STATEDETAILS
    #define WMAPRODEC_STATEPRINT printf
#else
    #define WMAPRODEC_STATEPRINT(...)
#endif

#ifdef WMAPRODEC_BUFDETAILS
    #define WMAPRODEC_BUFPRINT printf
#else
    #define WMAPRODEC_BUFPRINT(...)
#endif

#ifdef WMAPRODEC_MEMDETAILS
    #define WMAPRODEC_MEMPRINT(...)  fprintf(stdout, "%s %d::  ",__FUNCTION__, __LINE__); \
                                  fprintf(stdout, __VA_ARGS__); \
                                  fprintf(stdout, "\n");
#else
    #define WMAPRODEC_MEMPRINT(...)
#endif

#define WMAPRODEC_EPRINT LOGE

                           /* __android_log_print(ANDROID_LOG_VERBOSE, __FILE__,"%s %d::    ERROR",__FUNCTION__, __LINE__);\
                        __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, __VA_ARGS__);\
                            __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, "\n"); */

#endif

/* ======================================================================= */
/**
 * @def    WMAPROD_OMX_MALLOC   Macro to allocate Memory
 */
/* ======================================================================= */
#define WMAPROD_OMX_MALLOC(_pStruct_, _sName_)                         \
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
 * @def    WMAPRODEC_OMX_MALLOC_SIZE   Macro to allocate Memory
 */
/* ======================================================================= */
#define WMAPRODEC_OMX_MALLOC_SIZE(_ptr_, _size_,_name_)            \
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
 * @def    WMAPRODEC_OMX_ERROR_EXIT   Exit print and return macro
 */
/* ======================================================================= */
#define WMAPRODEC_OMX_ERROR_EXIT(_e_, _c_, _s_)                            \
    _e_ = _c_;                                                          \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "%d : Error Name: %s : Error Num = %x",__LINE__, _s_, _e_);  \
    OMXDBG_PRINT(stderr, ERROR, 4, 0, "\n**************** OMX ERROR ************************\n");  \
    goto EXIT;

/* ======================================================================= */
/**
 * @def    WMAPRODEC_OMX_CONF_CHECK_CMD   Command check Macro
 */
/* ======================================================================= */
#define WMAPRODEC_OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3)  \
    {                                                   \
        if(!_ptr1 || !_ptr2 || !_ptr3){                 \
            eError = OMX_ErrorBadParameter;             \
            goto EXIT;                                  \
        }                                               \
    }

/* ======================================================================= */
/**
 * @def    WMAPRODEC_OMX_FREE   Macro to free the Memory
 */
/* ======================================================================= */
#define WMAPRODEC_OMX_FREE(ptr)                                            \
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
 * @def    WMAPRODEC_BUFDETAILS   Turns buffer messaging on and off
 */
/* ======================================================================= */
#undef WMAPRODEC_BUFDETAILS
/* ======================================================================= */
/**
 * @def    WMAPRODEC_STATEDETAILS   Turns state messaging on and off
 */
/* ======================================================================= */
#undef WMAPRODEC_STATEDETAILS
/* ======================================================================= */
/**
 * @def    WMAPRODEC_MEMDETAILS   Turns memory messaging on and off
 */
/* ======================================================================= */
#undef WMAPRODEC_MEMDETAILS

#define EXTRA_BYTES 128 /* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256 /* For Cache alignment*/
#define WMAPRODEC_OUTPUT_PORT 1
#define WMAPRODEC_INPUT_PORT 0
#define WMAPRODEC_APP_ID  100
#define MAX_NUM_OF_BUFS_WMAPRODEC 15
#define PARAMETRIC_STEREO_WMAPRODEC 1
#define NON_PARAMETRIC_STEREO_WMAPRODEC 0
/* ======================================================================= */
/**
 * @def    NUM_OF_PORTS_WMAPRODEC   Number of ports
 */
/* ======================================================================= */
#define NUM_OF_PORTS_WMAPRODEC 2
/* ======================================================================= */
/**
 * @def    STREAM_COUNT_WMAPRODEC   Number of streams
 */
/* ======================================================================= */
#define STREAM_COUNT_WMAPRODEC 2

/** Default timeout used to come out of blocking calls*/

/* ======================================================================= */
/**
 * @def    WMAPROD_NUM_INPUT_BUFFERS   Default number of input buffers
 *
 */
/* ======================================================================= */
#define WMAPROD_NUM_INPUT_BUFFERS 4
/* ======================================================================= */
/**
 * @def    WMAPROD_NUM_OUTPUT_BUFFERS   Default number of output buffers
 *
 */
/* ======================================================================= */
#define WMAPROD_NUM_OUTPUT_BUFFERS 4

/* ======================================================================= */
/**
 * @def    WMAPROD_INPUT_BUFFER_SIZE   Default input buffer size
 *
 */
/* ======================================================================= */
#define MIN_MP3D_INPUT_BUFFER_SIZE (1024*40)
#define WMAPROD_INPUT_BUFFER_SIZE MIN_MP3D_INPUT_BUFFER_SIZE
/* ======================================================================= */
/**
 * @def    WMAPROD_OUTPUT_BUFFER_SIZE   Default output buffer size
 *
 */
/* ======================================================================= */
#define WMAPROD_OUTPUT_BUFFER_SIZE (8*4*2048*4)

/* ======================================================================= */
/**
 * @def    Mem test application
 */
/* ======================================================================= */
#undef WMAPRODEC_DEBUGMEM

#ifdef WMAPRODEC_DEBUGMEM
#define newmalloc(x) mymalloc(__LINE__,__FILE__,x)
#define newfree(z) myfree(z,__LINE__,__FILE__)
#else
#define newmalloc(x) malloc(x)
#define newfree(z) free(z)
#endif

/* ======================================================================= */
/**
 * @def    WMAPRODec macros for MONO,STEREO_INTERLEAVED,STEREO_NONINTERLEAVED
 */
/* ======================================================================= */
/*#define WMAPROD_STEREO_INTERLEAVED_STREAM     2
  #define WMAPROD_STEREO_NONINTERLEAVED_STREAM  3*/
/* ======================================================================= */
/**
 * @def    WMAPRODec macros for MONO,STEREO_INTERLEAVED,STEREO_NONINTERLEAVED
 */
/* ======================================================================= */
/* Stream types supported*/
#define MONO_STREAM_WMAPRODEC                   1
#define STEREO_STREAM_STEREO     2

/**
 *
 * WMA Decoder Profile:0 - MAIN, 1 - LC, 2 - SSR, 3 - LTP.
 */
/*typedef enum {
    EProfileMain,
    EProfileLC,
    EProfileSSR,
    EProfileLTP
}WMAProfile;*/
/* ======================================================================= */
/** COMP_PORT_TYPE_WMAPRODEC  Port types
 *
 *  @param  INPUT_PORT_WMAPRODEC                    Input port
 *
 *  @param  OUTPUT_PORT_WMAPRODEC               Output port
 */
/*  ==================================================================== */
/*This enum must not be changed. */
typedef enum COMP_PORT_TYPE_WMAPRODEC {
    INPUT_PORT_WMAPRODEC = 0,
    OUTPUT_PORT_WMAPRODEC
}COMP_PORT_TYPE_WMAPRODEC;
/* ======================================================================= */
/** OMX_INDEXAUDIOTYPE_WMAPRODEC  Defines the custom configuration settings
 *                              for the component
 *
 *  @param  OMX_IndexCustomMode16_24bit_WMAPRODEC  Sets the 16/24 mode
 *
 *  @param  OMX_IndexCustomModeProfile_WMAPRODEC  Sets the Profile mode
 *
 *  @param  OMX_IndexCustomModeSBR_WMAPRODEC  Sets the SBR mode
 *
 *  @param  OMX_IndexCustomModeDasfConfig_WMAPRODEC  Sets the DASF mode
 *
 *  @param  OMX_IndexCustomModeRAW_WMAPRODEC  Sets the RAW mode
 *
 *  @param  OMX_IndexCustomModePS_WMAPRODEC  Sets the ParametricStereo mode
 *
 */
/*  ==================================================================== */
typedef enum OMX_INDEXAUDIOTYPE_WMAPRODEC {
    OMX_IndexCustomFramemodeConfig = 0xFF000001,
    OMX_IndexCustomDownmixConfig,
    OMX_IndexCustomDebug,
}OMX_INDEXAUDIOTYPE_WMAPRODEC;

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
}TAUDIO_WmaProFormat;

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
/** WMAPRO_DEC_BUFFERLIST: This contains information about a buffer's owner whether
 * it is application or component, number of buffers owned etc.
 *
 * @see OMX_BUFFERHEADERTYPE
 */
/* ==================================================================== */
struct WMAPRO_DEC_BUFFERLIST{
    /* Array of pointer to OMX buffer headers */
    OMX_BUFFERHEADERTYPE *pBufHdr[MAX_NUM_OF_BUFS_WMAPRODEC];
    /* Array that tells about owner of each buffer */
    OMX_U32 bufferOwner[MAX_NUM_OF_BUFS_WMAPRODEC];
    /* Tracks pending buffers */
    OMX_U32 bBufferPending[MAX_NUM_OF_BUFS_WMAPRODEC];
    /* Number of buffers  */
    OMX_U32 numBuffers;
};

typedef struct WMAPRO_DEC_BUFFERLIST WMAPRODEC_BUFFERLIST;

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
/** WMAPRODEC_COMPONENT_PRIVATE: This is the major and main structure of the
 * component which contains all type of information of buffers, ports etc
 * contained in the component.
 *
 * @see OMX_BUFFERHEADERTYPE
 * @see OMX_AUDIO_PARAM_PORTFORMATTYPE
 * @see OMX_PARAM_PORTDEFINITIONTYPE
 * @see WMAPROD_LCML_BUFHEADERTYPE
 * @see OMX_PORT_PARAM_TYPE
 * @see OMX_PRIORITYMGMTTYPE
 * @see AUDIODEC_PORT_TYPE
 * @see WMAPRODEC_BUFFERLIST
 * @see LCML_STRMATTR
 * @see
 */
/* ==================================================================== */

typedef struct WMAPRODEC_COMPONENT_PRIVATE
{

    OMX_CALLBACKTYPE cbInfo;
    /** Handle for use with async callbacks */
    OMX_PORT_PARAM_TYPE* sPortParam;
    /* Input port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sInPortFormat;
    /* Output port information */
    OMX_AUDIO_PARAM_PORTFORMATTYPE sOutPortFormat;
    /* Buffer owner information */
    OMX_U32 bIsBufferOwned[NUM_OF_PORTS_WMAPRODEC];

    /** This will contain info like how many buffers
        are there for input/output ports, their size etc, but not
        BUFFERHEADERTYPE POINTERS. */
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef[NUM_OF_PORTS_WMAPRODEC];
    /* Contains information that come from application */
    OMX_AUDIO_PARAM_WMATYPE* wmaproParams;

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
    AUDIODEC_PORT_TYPE *pCompPort[NUM_OF_PORTS_WMAPRODEC];

    /* Checks whether or not buffer were allocated by appliction */
    OMX_U32 bufAlloced;

    /** Flag to check about execution of component thread */
    OMX_U16 bExitCompThrd;

    /** Pointer to list of input buffers */
    WMAPRODEC_BUFFERLIST *pInputBufferList;

    /** Pointer to list of output buffers */
    WMAPRODEC_BUFFERLIST *pOutputBufferList;

    /** Contains the version information */
    OMX_U32 nVersion;

    OMX_U16 framemode;

    OMX_S32 downmixflag;

    OMX_STRING cComponentName;

    OMX_VERSIONTYPE ComponentVersion;

    OMX_U32 nOpBit;
  /*  OMX_U32 parameteric_stereo;
    OMX_U32 nProfile;
    OMX_U32 SBR;
    OMX_U32 RAW;*/
    OMX_U32 bInitParamsInitialized;
    /*WMAPRODEC_BUFFERLIST *pInputBufferListQueue;
    WMAPRODEC_BUFFERLIST *pOutputBufferListQueue;*/

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


    FILE *m_fp;
    VOID *m_ittiam_handle;

    fd_set rfds;

    ia_wma_pro_dec_params_t  m_ittiam_dec_params;
    ia_wma_pro_dec_input_args_t  m_input_args;
    ia_wma_pro_dec_output_args_t m_output_args;


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
    OMX_S32 header_decode_done;

    //OMX_S32 iPortReconfigurationOngoing;


#ifdef ENABLE_ITTIAM_PROFILE
    UWORD32 m_avgDecodeTimeMS;
    UWORD32 m_peakDecodeTimeMS;
    UWORD32 m_totalDecodeTimeMS;
    UWORD32 m_numOfDecodes;
#endif

} WMAPRODEC_COMPONENT_PRIVATE;

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
 *  @see          WmaProDec_StartCompThread()
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
 * WmaProDec_StartCompThread() starts the component thread. This is internal
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
OMX_ERRORTYPE WmaProDec_StartCompThread(OMX_HANDLETYPE pHandle);
/* ================================================================================= * */
/**
 * WMAPRODEC_GetBufferDirection() function determines whether it is input buffer or
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
OMX_ERRORTYPE WMAPRODEC_GetBufferDirection(OMX_BUFFERHEADERTYPE *pBufHeader,
                                        OMX_DIRTYPE *eDir);
/* ================================================================================= * */
/**
 * WMAPRODEC_HandleCommand() function handles the command sent by the application.
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
OMX_U32 WMAPRODEC_HandleCommand (WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);
/* ================================================================================= * */
/**
 * WMAPRODEC_HandleDataBuf_FromApp() function handles the input and output buffers
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
OMX_ERRORTYPE WMAPRODEC_HandleDataBuf_FromApp(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);

/* void WMAPRODEC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData); */

void* WMAPRODEC_ComponentThread (void* pThreadData);




OMX_ERRORTYPE WMAPRODEC_HandleCommandStateSet(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate,
                                           OMX_S32                   commandData);
OMX_ERRORTYPE WMAPRODEC_SetStateToIdle(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE WMAPRODEC_SetStateToExecuting(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE WMAPRODEC_SetStateToPause(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE WMAPRODEC_SetStateToLoaded(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE WMAPRODEC_HandleCommandPortDisable(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData);
OMX_ERRORTYPE WMAPRODEC_HandleCommandPortEnable(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData);

OMX_ERRORTYPE WMAPRODEC_InitializeTheComponent(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);
OMX_ERRORTYPE WMAPRODEC_CloseTheComponent(WMAPRODEC_COMPONENT_PRIVATE *pComponentPrivate);

#endif
