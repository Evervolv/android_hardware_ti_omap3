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
/*  File Name         : OMX_IttiamVideoDec_Utils.h                           */
/*                                                                           */
/*  Description       :                                    				     */
/*																			 */
/*                                                                           */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          01 12 2010   Ittiam          Draft                               */
/*                                                                           */
/*****************************************************************************/

#ifndef OMX_VIDDEC_UTILS__H
#define OMX_VIDDEC_UTILS__H

/*****************************************************************************/
/* File includes                                                             */
/*****************************************************************************/
/* System Include Files */

//sasken start
//#include "properties.h"
//#include "native_handle.h"
#include "hardware/gralloc.h"
//#include "window.h"
//sasken stop

#define _XOPEN_SOURCE 600
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <memory.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dbapi.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>
#include <stdio.h>

//Sasken changes
//Define port indices in video decoder proxy
#define OMX_VIDEODECODER_INPUT_PORT 0
#define OMX_VIDEODECODER_OUTPUT_PORT 1
#define OMX_VER_MAJOR 0x1
#define OMX_VER_MINOR 0x1
#define MAX_SUB_ALLOCS 3

#include <sched.h>
#include <OMX_Core.h>
#include <OMX_TI_Debug.h>
#include "OMX_IttiamVidDec_CustomCmd.h"
#include "OMX_TI_Common.h"
/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Macros used to enable profiling and dumping                              */
/*****************************************************************************/
#define MAX_PROFILE 			  100 /* The number of frames for which    */
						                /*  profiling is done                */
#define DUMP_YUV                     0  /* Enable to dump the output         */
#define INP_DATA_BUF_PROFILE         0  /* Enable to profile the ETB calls   */
#define OUT_DATA_BUF_PROFILE         0  /* Enable to profile the FTB calls   */
#define INP_DATA_BUF_DONE_PROFILE    0  /* Enable to profile the EBD calls   */
#define OUT_DATA_BUF_DONE_PROFILE    0  /* Enable to profile the FBD calls   */
#define DUMP_TIMESTAMPS              0  /* Enable to Dump the I/O timestamps */
#define DUMP_DSP_PROCESS_TIME        0  /* Enable to dump the DSP side nos   */
#define DUMP_DSP_OUT_PARAMS          0  /* Enable to dump the DSP o/p params */
#define LCML_OUT_BUF_ISSUE_PROFILE   0  /* Enable to profile the o/p buffer  */
#define ARM_PROCESSING_PROFILE       0  /* Enable ARM proc profile           */
#define DSP_PROCESSING_PROFILE        0 /* Enable DSP proc profile           */
#define DSP_START_PROCESSING_PROFILE 0  /* Enable IM buf profile             */
#define CHECK_SUM_ENABLE             0	/* Enable Checksum                   */
#define DUMP_INP_TO_ARM              0  /* Dump the input to the ARM proc    */

#define VIDDEC_FLAGGED_EOS
#define VIDDEC_WMVPOINTERFIXED
#define __STD_COMPONENT__
#define KHRONOS_1_1
#define KHRONOS_1_2

#define VIDDEC_MEMLEVELS                     5
#define VIDDEC_MEMUSAGE 				     0

#define VIDDEC_COMPONENTROLES_H263           "video_decoder.h263"
#define VIDDEC_COMPONENTROLES_H264           "video_decoder.avc"
#define VIDDEC_COMPONENTROLES_MPEG4          "video_decoder.mpeg4"
#define VIDDEC_COMPONENTROLES_WMV9           "video_decoder.wmv"

#ifdef ANDROID
#define MAX_PRIVATE_IN_BUFFERS              6//NUM_OVERLAY_BUFFERS_REQUESTED
#define MAX_PRIVATE_OUT_BUFFERS             6//NUM_OVERLAY_BUFFERS_REQUESTED
#define MAX_PRIVATE_BUFFERS                 6//NUM_OVERLAY_BUFFERS_REQUESTED
#else
#define MAX_PRIVATE_IN_BUFFERS              4
#define MAX_PRIVATE_OUT_BUFFERS             4
#define MAX_PRIVATE_BUFFERS                 4
#endif

#define NUM_OF_PORTS                        2
#define VIDDEC_MAX_NAMESIZE                 128
#define VIDDEC_NOPORT                       0xfffffffe
#define VIDDEC_MPU                          50

#define VERSION_MAJOR                       1
#define VERSION_MINOR                       0
#define VERSION_REVISION                    0
#define VERSION_STEP                        0

/* Note: OMX_COLOR_FormatYUV420PackedPlanar is not working with OpenCore */
#define VIDDEC_COLORFORMAT420               OMX_COLOR_FormatYUV420Planar
#define VIDDEC_COLORFORMAT422               OMX_COLOR_FormatCbYCrY
#define VIDDEC_COLORFORMATUNUSED            OMX_COLOR_FormatUnused

/* RM frequencies to be requested for different formats, profiles and resolns */

#define VIDDEC_RM_FREC_MPEG4_QCIF                     30
#define VIDDEC_RM_FREC_MPEG4_CIF                      80
#define VIDDEC_RM_FREC_MPEG4_VGA                      165
/* For MPEG4 codecs request for 660 from the DSP */
/* This is in accordance to the RAC 31st Mar 10  */
/* Request for 600 MHz although you want 660 MHz */
#define VIDDEC_RM_FREC_MPEG4_720P                     600//660//430//600

#define VIDDEC_RM_FREC_H263_QCIF                      25
#define VIDDEC_RM_FREC_H263_CIF                       60
#define VIDDEC_RM_FREC_H263_VGA                       165

#define VIDDEC_RM_FREC_H264_QCIF                      85
#define VIDDEC_RM_FREC_H264_CIF                       160
#define VIDDEC_RM_FREC_H264_VGA                       260

/* For H264 codecs request for the highest OPP   */
/* This is in accordance to the RAC 31st Mar 10  */
/* Request for 600 MHz although you want 660 MHz */
#define VIDDEC_RM_FREC_H264_BP_720P                   620
#define VIDDEC_RM_FREC_H264_MP_720P                   750
#define VIDDEC_RM_FREC_H264_HP_720P                   750

#define VIDDEC_RM_FREC_WMV_QCIF                       55
#define VIDDEC_RM_FREC_WMV_CIF                        100
#define VIDDEC_RM_FREC_WMV_VGA                        300
#define VIDDEC_RM_FREC_WMV_720P                       600//660//430//600

/* Widths and Heights of different standard resolutions */
#define VIDDEC_MIN_WIDTH                              176
#define VIDDEC_MIN_HEIGHT                             144

#define VIDDEC_QCIF_WIDTH                             176
#define VIDDEC_QCIF_HEIGHT                            144

#define VIDDEC_QVGA_WIDTH                             320
#define VIDDEC_QVGA_HEIGHT                            240

#define VIDDEC_CIF_WIDTH                              352
#define VIDDEC_CIF_HEIGHT                             288

#define VIDDEC_VGA_WIDTH                              640
#define VIDDEC_VGA_HEIGHT                             480

#define VIDDEC_D1MAX_WIDTH                            720
#define VIDDEC_D1MAX_HEIGHT                           576

#define VIDDEC_MAX_QUEUE_SIZE                         256
#define VIDDEC_WMV_BUFFER_OFFSET                      255 - 4
#define VIDDEC_WMV_ELEMSTREAM                         0
#define VIDDEC_WMV_RCVSTREAM                          1

#define VIDDEC_SN_WMV_ELEMSTREAM                      1
#define VIDDEC_SN_WMV_RCVSTREAM                       2

#define VIDDEC_ZERO                              	  0
#define VIDDEC_ONE                               	  1
#define VIDDEC_MINUS                                 -1

#define VIDDEC_PIPE_WRITE                             VIDDEC_ONE
#define VIDDEC_PIPE_READ                              VIDDEC_ZERO

#define VIDDEC_FACTORFORMAT422                        (2)
#define VIDDEC_FACTORFORMAT420                        (1.5)
#define VIDDEC_CLEARFLAGS                             (0)

/* YUV ouput chroma format */
#define WMV9VIDDEC_YUVFORMAT_PLANAR420            	  (1)
#define WMV9VIDDEC_YUVFORMAT_INTERLEAVED422			  (4)
#define MP4VIDDEC_YUVFORMAT_PLANAR420 				  (1)
#define MP4VIDDEC_YUVFORMAT_INTERLEAVED422 			  (4)
#define H264VIDDEC_YUVFORMAT_PLANAR420 				  (0)
#define H264VIDDEC_YUVFORMAT_INTERLEAVED422 		  (1)

/* Macros for Circular Buffer used for storing timestamps */
#define VIDDEC_CBUFFER_LOCK
#define MAX_MULTIPLY                          (16)
#define CBUFFER_SIZE                          MAX_PRIVATE_BUFFERS * MAX_MULTIPLY

/* H264 Decoder profile */
#define H264_PROFILE_IDC_BASELINE 66
#define H264_PROFILE_IDC_MAIN 77
#define H264_PROFILE_IDC_EXTENDED 88
#define H264_PROFILE_IDC_HIGH 100

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#define VIDDEC_WAIT_CODE() sched_yield()

/*****************************************************************************/
/*  Function Name : OMX_MALLOC_STRUCT()                                      */
/*  Description   : 													     */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_MALLOC_STRUCT(_pStruct_, _sName_, _memusage_)           		\
    _pStruct_ = (_sName_*)malloc(sizeof(_sName_));                 		    \
    if(_pStruct_ == NULL){                                          		\
            eError = OMX_ErrorInsufficientResources;                		\
                goto EXIT;                                          		\
    }                                                               		\
    memset((_pStruct_), 0x0, sizeof(_sName_));

/*****************************************************************************/
/*  Function Name : OMX_MALLOC_STRUCT_SIZED()                                */
/*  Description   : 													     */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_MALLOC_STRUCT_SIZED(_pStruct_, _sName_, _nSize_, _memusage_)    \
    _pStruct_ = (_sName_*)malloc(_nSize_);                                  \
    if(_pStruct_ == NULL){                                                  \
            eError = OMX_ErrorInsufficientResources;                        \
                goto EXIT;                                                  \
    }                                                                       \
    memset((_pStruct_), 0x0, _nSize_);


/*****************************************************************************/
/*  Function Name : OMX_FREE() Free memory                                   */
/*  Description   : This method will free memory and set pointer to NULL     */
/*  Inputs        :  @param _pBuffer_     Pointer to free                    */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_FREE(_pBuffer_)                                                 \
    if(_pBuffer_ != NULL){                                                  \
        free(_pBuffer_);                                                    \
        _pBuffer_ = NULL;                                                   \
    }

/*****************************************************************************/
/*  Function Name : OMX_MALLOC_STRUCT_DSPALIGN()                             */
/*  Description   : 128byte aligned malloc.  This is to call the generic     */
/*                  DSPALIGN call in OMX_TI_Common.h                         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_MALLOC_STRUCT_DSPALIGN(_pStruct_, _sName_, _nSize_)             \
    OMX_MALLOC_SIZE_DSPALIGN(_pStruct_, _nSize_,_sName_)                    \
    if(_pStruct_ == NULL){                                                  \
            eError = OMX_ErrorInsufficientResources;                        \
                goto EXIT;                                                  \
    }                                                                       \
    memset((_pStruct_), 0x0, _nSize_);

/*****************************************************************************/
/*  Function Name : OMX_CONF_INIT_STRUCT()                                   */
/*  Description   : 													     */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_CONF_INIT_STRUCT(_s_, _name_, dbg)                               \
    memset((_s_), 0x0, sizeof(_name_));                                      \
    (_s_)->nSize = sizeof(_name_);                                           \
    (_s_)->nVersion.s.nVersionMajor = VERSION_MAJOR;                         \
    (_s_)->nVersion.s.nVersionMinor = VERSION_MINOR;                         \
    (_s_)->nVersion.s.nRevision = VERSION_REVISION;                          \
    (_s_)->nVersion.s.nStep = VERSION_STEP;                                  \
    OMX_PRINT0(dbg, "INIT_STRUCT Major 0x%x Minor 0x%x nRevision 0x%x nStep 0x%x\n", \
        (_s_)->nVersion.s.nVersionMajor, (_s_)->nVersion.s.nVersionMinor,    \
        (_s_)->nVersion.s.nRevision, (_s_)->nVersion.s.nStep);

/*****************************************************************************/
/*  Function Name : OMX_CONF_CHK_VERSION()                                   */
/*  Description   : 													     */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_CONF_CHK_VERSION(_s_, _name_, _e_, dbg)                           \
    if((_s_)->nSize != sizeof(_name_)) _e_ = OMX_ErrorBadParameter;           \
    if(((_s_)->nVersion.s.nVersionMajor != VERSION_MAJOR)||                   \
       ((_s_)->nVersion.s.nVersionMinor != VERSION_MINOR)||                   \
       ((_s_)->nVersion.s.nRevision != VERSION_REVISION)||                    \
       ((_s_)->nVersion.s.nStep != VERSION_STEP)) _e_ = OMX_ErrorVersionMismatch; \
    if(_e_ != OMX_ErrorNone) goto EXIT;

/*****************************************************************************/
/*  Function Name : OMX_CONF_CHECK_CMD()                                     */
/*  Description   : 													     */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3) \
{                                               \
    if(!_ptr1 || !_ptr2 || !_ptr3){             \
        eError = OMX_ErrorBadParameter;         \
        goto EXIT;                              \
    }                                           \
}

/*****************************************************************************/
/*  Function Name : OMX_CONF_SET_ERROR_BAIL()                                */
/*  Description   : 													     */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_CONF_SET_ERROR_BAIL(_eError, _eCode)\
{                                               \
    _eError = _eCode;                           \
    goto EXIT;                                  \
}

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_INIT()                              */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_INIT(_mutex_)   			 \
    if(!((_mutex_).bInitialized)) {           			 \
        pthread_mutex_init (&((_mutex_).mutex), NULL);   \
        pthread_cond_init (&(_mutex_).condition, NULL);  \
        (_mutex_).bInitialized = OMX_TRUE;   			 \
        (_mutex_).bSignaled = OMX_FALSE;     			 \
        (_mutex_).bEnabled = OMX_FALSE;      			 \
    }

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_DESTROY()                           */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_DESTROY(_mutex_)           \
    if((_mutex_).bInitialized) {                        \
        pthread_mutex_destroy (&((_mutex_).mutex));     \
        pthread_cond_destroy (&(_mutex_).condition);    \
        (_mutex_).bInitialized = OMX_FALSE;  		 	\
        (_mutex_).bEnabled = OMX_FALSE;      			\
    }
/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_LOCK()                              */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_LOCK(_mutex_)   \
    VIDDEC_PTHREAD_MUTEX_INIT ((_mutex_));   \
    (_mutex_).bSignaled = OMX_FALSE;         \
    (_mutex_).nErrorExist = 0; 				 \
    (_mutex_).nErrorExist = pthread_mutex_lock (&(_mutex_).mutex);

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_UNLOCK()                            */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_UNLOCK(_mutex_)  	 \
    VIDDEC_PTHREAD_MUTEX_INIT ((_mutex_));       \
    (_mutex_).nErrorExist = 0;					 \
    (_mutex_).nErrorExist = pthread_mutex_unlock (&(_mutex_).mutex);

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_TRYLOCK()                           */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_TRYLOCK(_mutex_) 	\
    VIDDEC_PTHREAD_MUTEX_INIT ((_mutex_));      \
    (_mutex_).nErrorExist = 0; 					\
    (_mutex_).nErrorExist = pthread_mutex_trylock (&(_mutex_).mutex);

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_SIGNAL()                            */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_SIGNAL(_mutex_)  	\
    VIDDEC_PTHREAD_MUTEX_INIT ((_mutex_));      \
    (_mutex_).nErrorExist = 0; 					\
    (_mutex_).nErrorExist = pthread_cond_signal (&(_mutex_).condition);

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_MUTEX_WAIT()                              */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_MUTEX_WAIT(_mutex_)   \
    VIDDEC_PTHREAD_MUTEX_INIT ((_mutex_));   \
    (_mutex_).bEnabled = OMX_TRUE;           \
     (_mutex_).nErrorExist = 0; 			 \
    (_mutex_).nErrorExist = pthread_cond_wait (&(_mutex_).condition, &(_mutex_).mutex);  \
        (_mutex_).bSignaled = OMX_FALSE;     \
        (_mutex_).bEnabled = OMX_FALSE;      \

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_SEMAPHORE_INIT()                          */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_SEMAPHORE_INIT(_semaphore_)      \
    if(!((_semaphore_).bInitialized)) {            		\
        sem_init (&((_semaphore_).sSemaphore), 0, 0);   \
        (_semaphore_).bInitialized = OMX_TRUE;   		\
        (_semaphore_).bEnabled = OMX_FALSE;      		\
        (_semaphore_).bSignaled = OMX_FALSE;        	\
    }

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_SEMAPHORE_DESTROY()                       */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_SEMAPHORE_DESTROY(_semaphore_) \
    if((_semaphore_).bInitialized) {             	  \
        sem_destroy (&(_semaphore_).sSemaphore);      \
        (_semaphore_).bInitialized = OMX_FALSE;  	  \
        (_semaphore_).bSignaled = OMX_FALSE;     	  \
        (_semaphore_).bEnabled = OMX_FALSE;      	  \
    }

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_SEMAPHORE_POST()                          */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_SEMAPHORE_POST(_semaphore_)     \
    VIDDEC_PTHREAD_SEMAPHORE_INIT ((_semaphore_));     \
    if((_semaphore_).bEnabled) {     				   \
        sem_post (&(_semaphore_).sSemaphore);          \
        (_semaphore_).bEnabled = OMX_FALSE;      	   \
    }               								   \
    else {          								   \
        (_semaphore_).bSignaled = OMX_TRUE;     	   \
        (_semaphore_).bEnabled = OMX_FALSE;     	   \
    }

/*****************************************************************************/
/*  Function Name : VIDDEC_PTHREAD_SEMAPHORE_WAIT()                          */
/*  Description   : 	           									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define VIDDEC_PTHREAD_SEMAPHORE_WAIT(_semaphore_)  	\
    VIDDEC_PTHREAD_SEMAPHORE_INIT ((_semaphore_));      \
    if(!(_semaphore_).bSignaled) {     					\
        (_semaphore_).bEnabled = OMX_TRUE;     			\
        sem_wait (&(_semaphore_).sSemaphore);  		    \
    }      											    \
    else {  											\
        (_semaphore_).bEnabled = OMX_FALSE;   		    \
        (_semaphore_).bSignaled = OMX_FALSE;  		    \
    }

/*****************************************************************************/
/*  Function Name : OMX_WMV_INSERT_CODEC_DATA()                              */
/*  Description   : This method will insert the codec data to the 1st frame  */
/*  Inputs        : _pBuffHead_ is a pointer to the buffer                   */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_WMV_INSERT_CODEC_DATA(_pBuffHead_)                    					    \
    {                                                                                   \
        OMX_U8* _pTempBuffer_ = NULL;                                                   \
        /* Copy frame data in a temporary buffer*/                                      \
        OMX_MALLOC_STRUCT_SIZED(_pTempBuffer_, OMX_U8, _pBuffHead_->nFilledLen, NULL);  \
        memcpy (_pTempBuffer_, _pBuffHead_->pBuffer, _pBuffHead_->nFilledLen);          \
                                                                                        \
        /* Add frame start code */           \
        (*(_pBuffHead_->pBuffer++)) = 0x00;  \
        (*(_pBuffHead_->pBuffer++)) = 0x00;  \
        (*(_pBuffHead_->pBuffer++)) = 0x01;  \
        (*(_pBuffHead_->pBuffer++)) = 0x0d;  \
                                             \
        /* Insert again the frame buffer */  \
        memcpy (_pBuffHead_->pBuffer, _pTempBuffer_, _pBuffHead_->nFilledLen); \
        /* pTempBuffer no longer need*/                                        \
        OMX_FREE(_pTempBuffer_);                                               \
																			   \
        _pBuffHead_->pBuffer    -= 4; \
        _pBuffHead_->nFilledLen += 4; \
    }

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/
typedef OMX_ERRORTYPE (*VIDDEC_fpo)(OMX_HANDLETYPE);

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/
typedef enum VIDDEC_ENUM_MEMLEVELS{
    VIDDDEC_Enum_MemLevel0 = 0,
    VIDDDEC_Enum_MemLevel1,
    VIDDDEC_Enum_MemLevel2,
    VIDDDEC_Enum_MemLevel3,
    VIDDDEC_Enum_MemLevel4
}VIDDEC_ENUM_MEMLEVELS;

typedef enum VIDDEC_CUSTOM_PARAM_INDEX
{
#ifdef KHRONOS_1_2
    VideoDecodeCustomParamProcessMode = (OMX_IndexVendorStartUnused + 1),
#else
    VideoDecodeCustomParamProcessMode = (OMX_IndexIndexVendorStartUnused + 1),
#endif
    VideoDecodeCustomParamH264BitStreamFormat,
    VideoDecodeCustomParamWMVProfile,
    VideoDecodeCustomParamWMVFileType,
    VideoDecodeCustomParamParserEnabled,
    VideoDecodeCustomParamIsNALBigEndian,
	// Added by sasken for 3630 ICS
    VideoDecodeEnableAndroidNativeBuffers,
    VideoDecodeGetAndroidNativeBufferUsage,  
    VideoDecodeGetAndroiduseAndroidNativeBuffer2,
	// Added by sasken for 3630 ICS
#ifdef VIDDEC_FLAGGED_EOS
    VideoDecodeCustomParambUseFlaggedEos,
#else
    VideoDecodeCustomParamIsNALBigEndian,
#endif
    VideoDecodeCustomConfigDebug,
    VideoDecodeCustomConfigCacheableBuffers

#ifdef ANDROID /*To be used by opencore multimedia framework*/
    ,
    PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX = 0xFF7A347
#endif
} VIDDEC_CUSTOM_PARAM_INDEX;

typedef enum VIDDEC_LCML_STATES
{
    VidDec_LCML_State_Unload = 0,
    VidDec_LCML_State_Load,
    VidDec_LCML_State_Init,
    VidDec_LCML_State_Pause,
    VidDec_LCML_State_Start,
    VidDec_LCML_State_Stop,
    VidDec_LCML_State_Destroy
} VIDDEC_LCML_STATES;

typedef enum VIDDEC_RMPROXY_STATES
{
    VidDec_RMPROXY_State_Unload = 0,
    VidDec_RMPROXY_State_Load,
    VidDec_RMPROXY_State_Registered,
} VIDDEC_RMPROXY_STATES;

typedef enum VIDDEC_PORT_INDEX
{
    VIDDEC_INPUT_PORT,
    VIDDEC_OUTPUT_PORT
}VIDDEC_PORT_INDEX;

typedef enum VIDDEC_DEFAULT_INPUT_INDEX
{
    VIDDEC_DEFAULT_INPUT_INDEX_MPEG4,
    VIDDEC_DEFAULT_INPUT_INDEX_H264,
    VIDDEC_DEFAULT_INPUT_INDEX_H263,
    VIDDEC_DEFAULT_INPUT_INDEX_WMV9,
    VIDDEC_DEFAULT_INPUT_INDEX_MAX = 0x7ffffff
}VIDDEC_DEFAULT_INPUT_INDEX;

typedef enum VIDDEC_DEFAULT_OUTPUT_INDEX
{
    VIDDEC_DEFAULT_OUTPUT_INDEX_INTERLEAVED422,
    VIDDEC_DEFAULT_OUTPUT_INDEX_PLANAR420,
    VIDDEC_DEFAULT_OUTPUT_INDEX_MAX = 0x7ffffff
}VIDDEC_DEFAULT_OUTPUT_INDEX;

typedef enum VIDDEC_BUFFER_OWNER
{
    VIDDEC_BUFFER_WITH_CLIENT = 0x0,
    VIDDEC_BUFFER_WITH_COMPONENT,
    VIDDEC_BUFFER_WITH_DSP,
    VIDDEC_BUFFER_WITH_TUNNELEDCOMP
} VIDDEC_BUFFER_OWNER;

typedef enum VIDDEC_TYPE_ALLOCATE
{
    VIDDEC_TALLOC_USEBUFFER,
    VIDDEC_TALLOC_ALLOCBUFFER
}VIDDEC_TYPE_ALLOCATE;

typedef enum VIDDEC_INIT_VALUE
{
    VIDDEC_INIT_ALL,
    VIDDEC_INIT_STRUCTS,
    VIDDEC_INIT_VARS,
    VIDDEC_INIT_H263,
    VIDDEC_INIT_H264,
    VIDDEC_INIT_MPEG4,
    VIDDEC_INIT_WMV9,

    VIDDEC_INIT_PLANAR420,
    VIDDEC_INIT_INTERLEAVED422,
    VIDDEC_INIT_IDLEEXECUTING,
    VIIDE_INIT_MAX = 0x7ffffff
}VIDDEC_INIT_VALUE;

typedef enum VIDDEC_WMV_PROFILES
{
    VIDDEC_WMV_PROFILE0,
    VIDDEC_WMV_PROFILE1,
    VIDDEC_WMV_PROFILE2,
    VIDDEC_WMV_PROFILE3,
    VIDDEC_WMV_PROFILE4,
    VIDDEC_WMV_PROFILE5,
    VIDDEC_WMV_PROFILE6,
    VIDDEC_WMV_PROFILE7,
    VIDDEC_WMV_PROFILE8,
    VIDDEC_WMV_PROFILEMAX
}VIDDEC_WMV_PROFILES;

typedef enum VIDDEC_CBUFFER_TYPE
{
    VIDDEC_CBUFFER_MARKDATA,
    VIDDEC_CBUFFER_TIMESTAMP,
    VIDDEC_CBUFFER_CMDMARKDATA
} VIDDEC_CBUFFER_TYPE;

typedef enum VIDDEC_QUEUE_TYPES
{
    VIDDEC_QUEUE_OMX_U32,
    VIDDEC_QUEUE_OMX_MARKTYPE
} VIDDEC_QUEUE_TYPES;


typedef struct OMX_CONFIG_MACROBLOCKERRORMAPTYPE_TI
{
    OMX_U32		    nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 		nPortIndex;
    OMX_U32 		nErrMapSize;         /* Size of the Error Map in bytes */
    OMX_U8 		    ErrMap[(864 * 480) / 256];  /* Error map hint */
} OMX_CONFIG_MACROBLOCKERRORMAPTYPE_TI;

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/
typedef struct VIDDEC_CUSTOM_PARAM
{
    unsigned char cCustomParamName[128];
    OMX_INDEXTYPE nCustomParamIndex;
} VIDDEC_CUSTOM_PARAM;

typedef struct VIDDEC_BUFFER_PRIVATE
{
    OMX_BUFFERHEADERTYPE  *pBufferHdr;
    OMX_PTR				  pUalgParam;
    OMX_U32 			  nUalgParamSize;
    VIDDEC_BUFFER_OWNER   eBufferOwner;
    VIDDEC_TYPE_ALLOCATE  bAllocByComponent;
    OMX_U32				  nNumber;
#ifdef VIDDEC_WMVPOINTERFIXED
    OMX_U8				  *pTempBuffer;
#endif
} VIDDEC_BUFFER_PRIVATE;

typedef struct VIDDEC_QUEUE_TYPE {
	OMX_PTR Elements;
	OMX_U32 CounterElements[VIDDEC_MAX_QUEUE_SIZE];
	OMX_U32 nHead;
	OMX_U32 nTail;
	OMX_U32 nElements;
	OMX_U32 nErrorCount;
	pthread_mutex_t mMutex;
}VIDDEC_QUEUE_TYPE;

typedef struct OMX_PARAM_WMVFILETYPE {
    OMX_U32         nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32         nWmvFileType;
} OMX_PARAM_WMVFILETYPE;

typedef struct VIDDEC_CIRCULAR_BUFFER {
    OMX_PTR             pElement[CBUFFER_SIZE];
    VIDDEC_CBUFFER_TYPE nType;
#ifdef VIDDEC_CBUFFER_LOCK
    pthread_mutex_t*    m_lock;
#endif
    OMX_U8 nTail;
    OMX_U8 nHead;
    OMX_U8 nCount;
} VIDDEC_CIRCULAR_BUFFER;

typedef struct VIDDEC_CBUFFER_BUFFERFLAGS{
    OMX_TICKS       nTimeStamp;
    OMX_U32         nFlags;
    OMX_U32         nTickCount;
    OMX_PTR         pMarkData;
    OMX_HANDLETYPE  hMarkTargetComponent;
} VIDDEC_CBUFFER_BUFFERFLAGS;


//Sasken
// -----------------
typedef enum VIDDEC_BUFFER_TYPE
{
		VirtualPointers,   						//Used when buffer pointers come from the normal A9 virtual space
		GrallocPointers,  	 						//Used when buffer pointers come from Gralloc allocations
		IONPointers,       							//Used when buffer pointers come from ION allocations
		EncoderMetadataPointers		//Used when buffer pointers come from Stagefright in camcorder usecase
} VIDDEC_BUFFER_TYPE;
// -----------------


typedef struct VIDDEC_PORT_TYPE
{
    OMX_HANDLETYPE         hTunnelComponent;
    OMX_U32                nTunnelPort;
    OMX_BUFFERSUPPLIERTYPE eSupplierSetting;
    OMX_U8                 nBufferCnt;
    VIDDEC_BUFFER_PRIVATE  *pBufferPrivate[MAX_PRIVATE_BUFFERS];
    VIDDEC_CIRCULAR_BUFFER eTimeStamp;
	
	// Sasken
	// --------------------
	VIDDEC_BUFFER_TYPE VIDDECBufferType;   // Used when buffer pointers come from the normal A9 virtual space
	OMX_U32 IsBuffer2D;   											// Used when buffer pointers come from Gralloc allocations*/
	// --------------------

} VIDDEC_PORT_TYPE;

typedef struct VIDDEC_MUTEX{
    OMX_BOOL	    bEnabled;
    OMX_BOOL 		bSignaled;
    OMX_BOOL 		bInitialized;
    OMX_S32         nErrorExist;
    pthread_mutex_t mutex;
    pthread_cond_t  condition;
} VIDDEC_MUTEX;

typedef struct VIDDEC_SEMAPHORE{
    OMX_BOOL bEnabled;
    OMX_BOOL bSignaled;
    OMX_BOOL bInitialized;
    OMX_S32  nErrorExist;
    sem_t    sSemaphore;
} VIDDEC_SEMAPHORE;

// Added by Sasken for 3630 ICS
/**
* A pointer to this struct is passed to the OMX_SetParameter when the extension
* index for the 'OMX.google.android.index.enableAndroidNativeBuffers' extension
* is given.
* The corresponding extension Index is OMX_TI_IndexUseNativeBuffers.
* This will be used to inform OMX about the presence of gralloc pointers instead
* of virtual pointers
*/
typedef struct OMX_TI_PARAMUSENATIVEBUFFER {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bEnable;
} OMX_TI_PARAMUSENATIVEBUFFER;

/**
* A pointer to this struct is passed to OMX_GetParameter when the extension
* index for the 'OMX.google.android.index.getAndroidNativeBufferUsage'
* extension is given.
* The corresponding extension Index is OMX_TI_IndexAndroidNativeBufferUsage.
* The usage bits returned from this query will be used to allocate the Gralloc
* buffers that get passed to the useAndroidNativeBuffer command.
*/
typedef struct OMX_TI_PARAMNATIVEBUFFERUSAGE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U32 nUsage;
} OMX_TI_PARAMNATIVEBUFFERUSAGE;

typedef struct VIDEO_PROFILE_LEVEL
{
    OMX_S32  nProfile;
    OMX_S32  nLevel;
} VIDEO_PROFILE_LEVEL_TYPE;

// End of sasken addition


#ifdef ANDROID
/* Opencore specific, refer to OpenMax Call Sequences document section 3.2 */
typedef struct PV_OMXComponentCapabilityFlagsType
{
	/* OMX COMPONENT CAPABILITY RELATED MEMBERS */
    OMX_BOOL iIsOMXComponentMultiThreaded;
    OMX_BOOL iOMXComponentSupportsExternalOutputBufferAlloc;
    OMX_BOOL iOMXComponentSupportsExternalInputBufferAlloc;
    OMX_BOOL iOMXComponentSupportsMovableInputBuffers;
    OMX_BOOL iOMXComponentSupportsPartialFrames;
    OMX_BOOL iOMXComponentUsesNALStartCodes;
    OMX_BOOL iOMXComponentCanHandleIncompleteFrames;
    OMX_BOOL iOMXComponentUsesFullAVCFrames;
} PV_OMXComponentCapabilityFlagsType;
#endif

/* Data structure used for everything .... */
typedef struct VIDDEC_COMPONENT_PRIVATE
{
    OMX_PARAM_PORTDEFINITIONTYPE    *pInPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE    *pOutPortDef;
    OMX_VIDEO_PARAM_PORTFORMATTYPE  *pInPortFormat;
    OMX_VIDEO_PARAM_PORTFORMATTYPE  *pOutPortFormat;
    OMX_PRIORITYMGMTTYPE            *pPriorityMgmt;
    OMX_PARAM_BUFFERSUPPLIERTYPE    *pInBufSupplier;
    OMX_PARAM_BUFFERSUPPLIERTYPE    *pOutBufSupplier;
    OMX_PORT_PARAM_TYPE             *pPortParamType;
    OMX_PARAM_DEBLOCKINGTYPE        *pDeblockingParamType;
    OMX_CALLBACKTYPE                cbInfo;
    OMX_PARAM_PORTDEFINITIONTYPE    *pPortDef[NUM_OF_PORTS];
    OMX_U8					        numPorts;
    OMX_COMPONENTTYPE               *pHandle;
    OMX_STATETYPE                   eState;
    OMX_VERSIONTYPE                 pComponentVersion;
    OMX_VERSIONTYPE 			    pSpecVersion;
    OMX_STRING                      cComponentName;

    OMX_VIDEO_PARAM_AVCTYPE   *pH264;
    OMX_VIDEO_PARAM_MPEG4TYPE *pMpeg4;
    OMX_VIDEO_PARAM_H263TYPE  *pH263;
    OMX_VIDEO_PARAM_WMVTYPE   *pWMV;

#ifdef __STD_COMPONENT__
    OMX_PORT_PARAM_TYPE *pPortParamTypeAudio;
    OMX_PORT_PARAM_TYPE *pPortParamTypeImage;
    OMX_PORT_PARAM_TYPE *pPortParamTypeOthers;

#ifdef KHRONOS_1_1
    OMX_U32             nCurrentMPEG4ProfileIndex;
#endif /* KHRONOS_1_1 */

#endif /* __STD_COMPONENT__ */

 #ifdef VIDDEC_FLAGGED_EOS
    OMX_BOOL 			 bUseFlaggedEos;
    OMX_BUFFERHEADERTYPE pTempBuffHead;
#endif

#ifdef KHRONOS_1_1
    OMX_PARAM_COMPONENTROLETYPE componentRole;
    /* MBError Reporting variables */
    OMX_CONFIG_MBERRORREPORTINGTYPE      eMBErrorReport;
    OMX_CONFIG_MACROBLOCKERRORMAPTYPE_TI eMBErrorMapType[MAX_PRIVATE_BUFFERS];
    OMX_U8 								 cMBErrorIndexIn;
    OMX_U8							     cMBErrorIndexOut;
#endif

#ifdef ANDROID /* Specific flag for opencore mmframework */
    PV_OMXComponentCapabilityFlagsType *pPVCapabilityFlags;
#endif

#ifdef __PERF_INSTRUMENTATION__
    PERF_OBJHANDLE pPERF, pPERFcomp;
    OMX_U32 	   lcml_nCntOpReceived;
    OMX_U32 	   lcml_nCntIp;
#endif

    VIDDEC_RMPROXY_STATES eRMProxyState;  /* State of RM         */
#ifdef RESOURCE_MANAGER_ENABLED
    RMPROXY_CALLBACKTYPE rmproxyCallback; /* used by RM callback */
#endif

   /* LCML member variables */
    void               *pLcmlHandle;
    void               *pModLCML;
    LCML_DSP_INTERFACE *pLCML;
    VIDDEC_LCML_STATES eLCMLState;
    OMX_BOOL           bLCMLHalted;
    OMX_BOOL           bLCMLOut;
	OMX_BOOL           bIsDSPInitialised;
    OMX_U16            arr[100];

    /* Variables used to handle CommandMarkBuffer and propagate */
    /*  the marked data											*/
    OMX_U8       nInMarkBufIndex;
    OMX_U8       nOutMarkBufIndex;
    OMX_MARKTYPE arrMarkBufIndex[VIDDEC_MAX_QUEUE_SIZE];
    OMX_U8       nInCmdMarkBufIndex;
    OMX_U8       nOutCmdMarkBufIndex;
    OMX_MARKTYPE arrCmdMarkBufIndex[VIDDEC_MAX_QUEUE_SIZE];
    OMX_U8       nInBufIndex;
    OMX_U8       nOutBufIndex;
    OMX_U64      arrBufIndex[VIDDEC_MAX_QUEUE_SIZE];
    OMX_MARKTYPE MTbuffMark;

	/* Flags and temporary variables */
    OMX_STATETYPE eIdleToLoad;
    OMX_STATETYPE eExecuteToIdle;
    OMX_BOOL      bPlayCompleted;
    OMX_BOOL      iEndofInputSent;
    OMX_BOOL      iEndofInput;
    OMX_BOOL      bPipeCleaned;
    OMX_BOOL      bParserEnabled;
    OMX_BOOL      bBuffFound;
    OMX_BOOL      bFlushOut;
    OMX_BOOL      bFirstHeader;
    OMX_BOOL      bDynamicConfigurationInProgress;
    OMX_BOOL      bInPortSettingsChanged;
    OMX_BOOL      bOutPortSettingsChanged;
    OMX_U32		  ProcessMode;
    OMX_U32 	  H264BitStreamFormat;
    OMX_BOOL 	  bConfigBufferCompleteAVC; /* Used to handle config buffer */
										    /* fragmentation on AVC         */
    OMX_PTR  	  pInternalConfigBufferAVC;
    OMX_U32  	  nInternalConfigBufferFilledAVC;
    OMX_BOOL 	  bIsNALBigEndian;
    OMX_U32 	  nMemUsage[VIDDEC_MEMLEVELS];
    OMX_BOOL 	  isOpBufFlushComplete;
    OMX_BOOL 	  sendEOSToDSP;
    OMX_BOOL 	  EOSHasBeenSentToAPP;
    OMX_U32  	  frameCount;
    OMX_U32       nPendingStateChangeRequests; /* Reference count for pending */
											   /* state change requests       */

    pthread_t        ComponentThread; /* Video Decoder thread */
    void             *pSplit;         /* Accessed by ASC thread for everything*/
    VIDDEC_PORT_TYPE *pCompPort[NUM_OF_PORTS];
    int				 free_inpBuf_Q[2];
    int 			 free_outBuf_Q[2];
    int 			 filled_outBuf_Q[2];
    int 			 cmdPipe[2];
    int 			 cmdDataPipe[2];
    struct OMX_TI_Debug dbg;

    /* These variables track the number of buffers with the component */
    OMX_U8 bInputBCountDsp;
    OMX_U8 bOutputBCountDsp;
    OMX_U8 bInputBCountApp;
    OMX_U8 bOutputBCountApp;

    OMX_U32 nLevelIdc;
    OMX_U32 nNumRefFrames;

    OMX_VIDEO_AVCPROFILETYPE AVCProfileType;
    
    OMX_U32 isLevelCheckDone;
    OMX_U32 isRefFrameCheckDone;

/* The mutexes protect the buffer count modification across threads */
    pthread_mutex_t InputBCountDspMutex;
    pthread_mutex_t OutputBCountDspMutex;
    pthread_mutex_t InputBCountAppMutex;
    pthread_mutex_t OutputBCountAppMutex;

    pthread_mutex_t mutexStateChangeRequest;
    pthread_cond_t  StateChangeCondition;

    VIDDEC_MUTEX     sMutex;
    VIDDEC_MUTEX     sDynConfigMutex;
    VIDDEC_SEMAPHORE sInSemaphore;
    VIDDEC_SEMAPHORE sOutSemaphore;

    VIDDEC_CBUFFER_BUFFERFLAGS aBufferFlags[CBUFFER_SIZE];
    VIDDEC_WMV_PROFILES wmvProfile;
    OMX_U32 			nWMVFileType;
	
	//Sasken changes
    OMX_U32 nTotalBuffers;
    gralloc_module_t const *grallocModule;
    OMX_U32 count_ion_buff;
    OMX_U8 *ion_handle[6];
    OMX_U8 *out_buff[6];

#if 0
	PROXY_EMPTYBUFFER_DONE proxyEmptyBufferDone;
	PROXY_FILLBUFFER_DONE proxyFillBufferDone;
	PROXY_EVENTHANDLER proxyEventHandler;

#endif


//#ifdef USE_ION
		int ion_fd;
		OMX_BOOL bUseIon;
		OMX_BOOL bMapIonBuffers;
		OMX_U8 mmap_fd[6];
		OMX_BOOL controlCallNeeded;
		OMX_U32 nCropWidth;
		OMX_U32 nCropHeight;
		
} VIDDEC_COMPONENT_PRIVATE;

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
OMX_ERRORTYPE OMX_ComponentInit (OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE VIDDEC_Start_ComponentThread (OMX_HANDLETYPE pHandle);

OMX_ERRORTYPE VIDDEC_Stop_ComponentThread(OMX_HANDLETYPE pComponent);

/* Functions to handle OMX-IL Commands */
OMX_ERRORTYPE VIDDEC_HandleCommandStateSet (OMX_HANDLETYPE pHandle,
											OMX_U32        nParam1);

OMX_ERRORTYPE VIDDEC_DisablePort (VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								  OMX_U32 				   nParam1);

OMX_ERRORTYPE VIDDEC_EnablePort (VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								 OMX_U32 				  nParam1);

OMX_ERRORTYPE VIDDEC_HandleCommandMarkBuffer(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								OMX_U32                  nParam1,
								OMX_PTR                  pCmdData);

OMX_ERRORTYPE VIDDEC_HandleCommandFlush(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								OMX_U32 				  nParam1,
								OMX_BOOL 				  bPass);

OMX_ERRORTYPE VIDDEC_Handle_InvalidState (
								VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToIdle(OMX_HANDLETYPE phandle);

OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToExecute(OMX_HANDLETYPE phandle);

OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToLoaded(OMX_HANDLETYPE phandle);

OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToPause(OMX_HANDLETYPE phandle);

OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToInvalid(OMX_HANDLETYPE phandle);

OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToWaitForResources(
													OMX_HANDLETYPE phandle);

/* Functions to handle buffer movements */
OMX_ERRORTYPE VIDDEC_HandleDataBuf_FromApp (
                                    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									OMX_BUFFERHEADERTYPE     *pBuffHead,
									OMX_U32                  *bdoNotProcess);

OMX_ERRORTYPE VIDDEC_HandleDataBuf_FromDsp(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

OMX_ERRORTYPE VIDDEC_HandleFreeDataBuf(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

OMX_ERRORTYPE VIDDEC_HandleFreeOutputBufferFromApp
							(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

OMX_ERRORTYPE VIDDEC_ReturnBuffers(VIDDEC_COMPONENT_PRIVATE	 *pComponentPrivate,
								   OMX_U32                   nParam1,
								   OMX_BOOL 				 bRetDSP);

OMX_ERRORTYPE VIDDEC_Propagate_Mark(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									OMX_BUFFERHEADERTYPE     *pBuffHead);

/* Functions of the circular buffer storing the timestamps */
OMX_ERRORTYPE VIDDEC_CircBuf_Init(VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate,
								  VIDDEC_CBUFFER_TYPE       nTypeIndex,
								  VIDDEC_PORT_INDEX         nPortIndex);

OMX_ERRORTYPE VIDDEC_CircBuf_Flush(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								   VIDDEC_CBUFFER_TYPE      nTypeIndex,
								   VIDDEC_PORT_INDEX        nPortIndex);

OMX_ERRORTYPE VIDDEC_CircBuf_DeInit(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									VIDDEC_CBUFFER_TYPE      nTypeIndex,
									VIDDEC_PORT_INDEX        nPortIndex);

OMX_U8 VIDDEC_CircBuf_GetHead(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							  VIDDEC_CBUFFER_TYPE 	    nTypeIndex,
							  VIDDEC_PORT_INDEX		    nPortIndex);

OMX_ERRORTYPE VIDDEC_CircBuf_Add(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								 VIDDEC_CBUFFER_TYPE      nTypeIndex,
								 VIDDEC_PORT_INDEX        nPortIndex,
								 OMX_PTR                  pElement);

OMX_ERRORTYPE VIDDEC_CircBuf_Remove(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
									VIDDEC_CBUFFER_TYPE      nTypeIndex,
									VIDDEC_PORT_INDEX        nPortIndex,
									OMX_PTR					 *pElement);

OMX_ERRORTYPE VIDDEC_CircBuf_Count(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								   VIDDEC_CBUFFER_TYPE      nTypeIndex,
								   VIDDEC_PORT_INDEX        nPortIndex,
								   OMX_U8* 					pCount);

/* Resource Manager functions */
OMX_U32 VIDDEC_GetRMFrecuency(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE VIDDEC_RM_free_resources(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

#ifdef RESOURCE_MANAGER_ENABLED
void VIDDEC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData);
#endif

/* Called during DSP Hang */
OMX_ERRORTYPE VIDDEC_CloseCodec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

char *OMXIndex2StringArray(OMX_U32 index);

OMX_ERRORTYPE VIDDEC_FatalErrorRecover(
								VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

#endif
