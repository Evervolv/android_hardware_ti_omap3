/*****************************************************************************/
/*                                                                           */
/*                             Standard Files                                */
/*                  ITTIAM SYSTEMS PVT LTD, BANGALORE                        */
/*                          COPYRIGHT(C) 2006                                */
/*                                                                           */
/*  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and */
/*  is protected under Indian  Copyright Law as an unpublished work. Its use */
/*  and  disclosure  is  limited by  the terms  and  conditions of a license */
/*  agreement. It may not be copied or otherwise  reproduced or disclosed to */
/*  persons outside the licensee's organization except in accordance with the*/
/*  the  terms  and  conditions   of  such  an  agreement.  All  copies  and */
/*  reproductions shall be the property of Ittiam Systems Private Limited and*/
/*  must bear this notice in its entirety.                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : ittiam_datatypes.h                                   */
/*                                                                           */
/*  Description       : This file has the definitions of the data types used */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         06 04 2006   Malavika        Draft                                */
/*                                                                           */
/*****************************************************************************/

#ifndef OMX_VIDEOENC_UTILS__H
#define OMX_VIDEOENC_UTILS__H

#include "LCML_DspCodec.h"
#include "LCML_Types.h"
#include "LCML_CodecInterface.h"
#ifdef RESOURCE_MANAGER_ENABLED
#include <ResourceManagerProxyAPI.h>
#endif
#include "OMX_VideoEnc_DSP.h"
#ifdef __PERF_INSTRUMENTATION__
    #include "perf.h"
    #endif
#include <OMX_TI_Debug.h>
#include <OMX_Component.h>
#ifdef LOG_TAG
    #undef LOG_TAG
#endif
#define LOG_TAG "TI_720P_OMX_VideoEnc_Utils.h"

#include "OMX_TI_Common.h"
#include <utils/Log.h>
//#include "overlay_common.h"

/* this is the max of VIDENC_MAX_NUM_OF_IN_BUFFERS and VIDENC_MAX_NUM_OF_OUT_BUFFERS */
/*ittiam changes*/
#define NUM_SF_NATIVE_BUFFERS_REQUESTED   6
#define VIDENC_MAX_NUM_OF_BUFFERS     (NUM_SF_NATIVE_BUFFERS_REQUESTED + 4)
#define VIDENC_MAX_NUM_OF_IN_BUFFERS  (NUM_SF_NATIVE_BUFFERS_REQUESTED + 4)
#define VIDENC_MAX_NUM_OF_OUT_BUFFERS (NUM_SF_NATIVE_BUFFERS_REQUESTED + 4)
#define VIDENC_NUM_OF_IN_BUFFERS      NUM_SF_NATIVE_BUFFERS_REQUESTED
#define VIDENC_NUM_OF_OUT_BUFFERS     NUM_SF_NATIVE_BUFFERS_REQUESTED
#define VIDENC_NUM_OF_PORTS 2

#ifdef __ANALYSE_MEMORY__
#define PADDING_SIZE   512
#else
#define PADDING_SIZE   0
#endif

#if 1
    #define GPP_PRIVATE_NODE_HEAP
#endif

#define VIDENC_NUM_CUSTOM_INDEXES 24

#if 1
    #define __KHRONOS_CONF__
#endif

#if 1
    #define __KHRONOS_CONF_1_1__
#endif

#define KHRONOS_1_2

#define VIDENC_MAX_COMPONENT_TIMEOUT 0xFFFFFFFF
#define OMX_NOPORT 0xFFFFFFFE
#define MAXNUMSLCGPS 8  /*< max. number of slice groups*/
/* Remove after OMX 1.1 migration */
#ifndef __KHRONOS_CONF_1_1__
    #define OMX_BUFFERFLAG_SYNCFRAME 0x00000040
#endif
#define OMX_LFRAMETYPE_H264 1
#define OMX_CFRAMETYPE_MPEG4 1
/*Select Timeout */
#define  VIDENC_TIMEOUT_SEC 120;
#define  VIDENC_TIMEOUT_USEC 0;

/*
* Definition of capabilities index and structure
* Needed to inform OpenCore about component capabilities.
*/
#define PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX 0xFF7A347

typedef struct PV_OMXComponentCapabilityFlagsType
{
    /* OMX COMPONENT CAPABILITY RELATED MEMBERS*/
    OMX_BOOL iIsOMXComponentMultiThreaded;
    OMX_BOOL iOMXComponentSupportsExternalOutputBufferAlloc;
    OMX_BOOL iOMXComponentSupportsExternalInputBufferAlloc;
    OMX_BOOL iOMXComponentSupportsMovableInputBuffers;
    OMX_BOOL iOMXComponentSupportsPartialFrames;
    OMX_BOOL iOMXComponentUsesNALStartCode;
    OMX_BOOL iOMXComponentCanHandleIncompleteFrames;
    OMX_BOOL iOMXComponentUsesFullAVCFrames;
} PV_OMXComponentCapabilityFlagsType;

/*
 * Redirects control flow in an error situation.
 * The OMX_CONF_CMD_BAIL label is defined inside the calling function.
 */
#define OMX_CONF_BAIL_IF_ERROR(_eError)                     \
do {                                                        \
    if(_eError != OMX_ErrorNone) {                          \
        goto OMX_CONF_CMD_BAIL;                             \
        }                                                   \
} while(0)

#define OMX_VIDENC_BAIL_IF_ERROR(_eError, _hComp)           \
do {                                                        \
    if(_eError != OMX_ErrorNone) {  \
        _eError = OMX_VIDENC_HandleError(_hComp, _eError);  \
        if(_eError != OMX_ErrorNone) {                      \
            OMX_ERROR5(_hComp->dbg, "*Fatal Error : %x\n", _eError); \
            goto OMX_CONF_CMD_BAIL;                         \
        }                                                   \
    }                                                       \
} while(0)

/*
 * Sets error type and redirects control flow to error handling and cleanup section
 */
#define OMX_CONF_SET_ERROR_BAIL(_eError, _eCode)\
do {                                                        \
    _eError = _eCode;                                       \
    goto OMX_CONF_CMD_BAIL;                                 \
} while(0)

#define OMX_VIDENC_SET_ERROR_BAIL(_eError, _eCode, _hComp)\
do {                                                        \
    _eError = _eCode;                                       \
    OMX_ERROR5(_hComp->dbg, "*Fatal Error : %x\n", eError); \
    OMX_VIDENC_HandleError(_hComp, _eError);                \
    goto OMX_CONF_CMD_BAIL;                                 \
} while(0)

/*
 * Checking paramaters for non-NULL values.
 * The macro takes three parameters because inside the code the highest
 *   number of parameters passed for checking in a single instance is three.
 * In case one or two parameters are passed, the ramaining parameters
 *   are set to 1 (or a nonzero value).
 */
#define OMX_CONF_CHECK_CMD(_ptr1, _ptr2, _ptr3)             \
do {                                                        \
    if(!_ptr1 || !_ptr2 || !_ptr3){                         \
        eError = OMX_ErrorBadParameter;                     \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
} while(0)

/*
* Initialize the Circular Buffer data. The Tail and Head pointers are NULL.
*The number of nodes inside the circular buffer is equal to zero.
*Also the number of nodes that contains BufferData is iqual zero.
*It should be in the ComponentInit call of the Component.
*/
#define OMX_CONF_CIRCULAR_BUFFER_INIT(_pPrivateData_)       \
do {                                                            \
    (_pPrivateData_)->sCircularBuffer.pHead = NULL;         \
    (_pPrivateData_)->sCircularBuffer.pTail = NULL;         \
    (_pPrivateData_)->sCircularBuffer.nElements = 0;        \
        (_pPrivateData_)->sCircularBuffer.nFillElements = 0;    \
} while(0)

/*
*Restart the Circular Buffer. The tail points to the same node as the head. The
*number of fill elements is set to zero. It should be put in the Idle->Execution
*transition.
*/
#define OMX_CONF_CIRCULAR_BUFFER_RESTART(_sCircular_)       \
do {                                                            \
    (_sCircular_).pTail = (_sCircular_).pHead;              \
        (_sCircular_).nFillElements = 0;                        \
} while(0)

/*
*Add node to the Circular Buffer.  Should be use when UseBuffer or AllocateBuffer
*is call. The new node is insert in the head of the list. The it will go the last node
*and rewrite pNext with the new address of the Head.
*/
#define OMX_CONF_CIRCULAR_BUFFER_ADD_NODE(_pPrivateData_, _sCircular_)\
do {                                                        \
    if((_sCircular_).nElements < VIDENC_MAX_NUM_OF_BUFFERS) \
    {                                                       \
        OMX_U8 _i_ = 0;                                      \
        OMX_CONF_CIRCULAR_BUFFER_NODE* pTmp = (_sCircular_).pHead;\
        VIDENC_MALLOC( (_sCircular_).pHead,                 \
                        sizeof(OMX_CONF_CIRCULAR_BUFFER_NODE),\
                        OMX_CONF_CIRCULAR_BUFFER_NODE,      \
                        (_pPrivateData_)->pMemoryListHead,  \
                        (_pPrivateData_)->dbg);             \
        (_sCircular_).nElements++;                          \
        if(!pTmp){                                           \
            (_sCircular_).pHead->pNext = (_sCircular_).pHead;\
            (_sCircular_).pTail = (_sCircular_).pHead;      \
        }                                                   \
        else{                                               \
            (_sCircular_).pHead->pNext = pTmp;              \
            for(_i_=2 ; _i_ < (_sCircular_).nElements; _i_++) \
                    pTmp = pTmp->pNext;                     \
            pTmp->pNext = (_sCircular_).pHead;              \
        }                                                   \
    }                                                       \
} while(0)

/*
* Will move the Tail of the Cirular Buffer to the next element. In the tail resides the last buffer to enter
*the component from the Application layer. It will get all the Data to be propageted from
* the pBufferHeader and write it in the node. Then it will move the Tail to the next element.
*It should be put in the function that handles the filled buffer from the application.
*/
#define OMX_CONF_CIRCULAR_BUFFER_MOVE_TAIL(_pBufHead_, _sCircular_, _pPrivateData_)\
do {                                                        \
    if((_pPrivateData_)->pMarkBuf){                        \
        (_sCircular_).pTail->pMarkData = (_pPrivateData_)->pMarkBuf->pMarkData;\
        (_sCircular_).pTail->hMarkTargetComponent = (_pPrivateData_)->pMarkBuf->hMarkTargetComponent;\
        (_pPrivateData_)->pMarkBuf = NULL;                  \
    }                                                       \
    else{                                                   \
        (_sCircular_).pTail->pMarkData = (_pBufHead_)->pMarkData; \
        (_sCircular_).pTail->hMarkTargetComponent = (_pBufHead_)->hMarkTargetComponent;\
    }                                                       \
    (_sCircular_).pTail->nTickCount = (_pBufHead_)->nTickCount;\
    (_sCircular_).pTail->nTimeStamp = (_pBufHead_)->nTimeStamp;\
    (_sCircular_).pTail->nFlags = (_pBufHead_)->nFlags;      \
    (_sCircular_).pTail = (_sCircular_).pTail->pNext;       \
    (_sCircular_).nFillElements++;                          \
    if(((_sCircular_).pTail == (_sCircular_).pHead) &&      \
       ((_sCircular_).nFillElements != 0)){                 \
        OMX_TRACE2((_pPrivateData_)->dbg, "**Warning:Circular Buffer Full.\n"); \
    }                                                       \
} while(0)

/*
*Will move the Head of the Circular Buffer to the next element. In the head is the Data of the first Buffer
*to enter to the Application layer. It will propagate the Data and put it in the pBufferHeader
*that goes to the Application layer. Then it will move the Head to the Next element.
*It should be put in the function that handles the filled buffers that comes from the DSP.
*/
#define OMX_CONF_CIRCULAR_BUFFER_MOVE_HEAD(_pBufHead_, _sCircular_, _pPrivateData_) \
do {                                                         \
    (_pBufHead_)->pMarkData = (_sCircular_).pHead->pMarkData;\
    (_pBufHead_)->hMarkTargetComponent = (_sCircular_).pHead->hMarkTargetComponent;\
    (_pBufHead_)->nTickCount = (_sCircular_).pHead->nTickCount;\
    (_pBufHead_)->nTimeStamp = (_sCircular_).pHead->nTimeStamp;\
    (_pBufHead_)->nFlags = (_sCircular_).pHead->nFlags;      \
    (_sCircular_).pHead = (_sCircular_).pHead->pNext;       \
    (_sCircular_).nFillElements--;                          \
    if(((_sCircular_).pTail == (_sCircular_).pHead) &&      \
       ((_sCircular_).nFillElements == 0)){                 \
        OMX_TRACE1((_pPrivateData_)->dbg, "**Note:Circular Buffer Empty.\n"); \
    }                                                       \
} while(0)

/*
*This Macro will delete a node from the Circular Buffer. It will rearrenge the conections
*between the nodes, and restart the CircularBuffer. The Tail and Head will point to the same
*location and the nFillElement will be set to 0. It should be in the FreeBuffer call.
*/
#define OMX_CONF_CIRCULAR_BUFFER_DELETE_NODE(_pPrivateData_, _sCircular_)\
do {                                                        \
    OMX_CONF_CIRCULAR_BUFFER_NODE* pTmp1 = (_sCircular_).pHead;\
    OMX_CONF_CIRCULAR_BUFFER_NODE* pTmp2 = NULL;            \
    if(((_sCircular_).pHead != NULL) &&                     \
       ((_sCircular_).pTail != NULL)){                      \
        while(pTmp1->pNext != (_sCircular_).pHead){         \
            pTmp2 = pTmp1;                                  \
            pTmp1 = pTmp1->pNext;                           \
        }                                                   \
        VIDENC_FREE(pTmp1,(_pPrivateData_)->pMemoryListHead, (_pPrivateData_)->dbg); \
        (_sCircular_).nElements--;                          \
        (_sCircular_).nFillElements = 0;                    \
        if(pTmp2 != NULL){                                  \
            pTmp2->pNext = (_sCircular_).pHead;             \
            (_sCircular_).pTail = (_sCircular_).pHead;      \
        }                                                   \
        else {                                              \
            (_sCircular_).pHead = NULL;                     \
            (_sCircular_).pTail = NULL;                     \
        }                                                   \
    }                                                       \
} while(0)

/*
 * Checking for version compliance.
 * If the nSize of the OMX structure is not set, raises bad parameter error.
 * In case of version mismatch, raises a version mismatch error.
 */


#define OMX_CONF_CHK_VERSION(_s_, _name_, _e_)              \
do {                                                        \
    if((_s_)->nSize != sizeof(_name_)) _e_ = OMX_ErrorBadParameter; \
    if(((_s_)->nVersion.s.nVersionMajor != 0x1)||           \
       ((_s_)->nVersion.s.nVersionMinor != 0x0)||           \
       ((_s_)->nVersion.s.nRevision != 0x0)||               \
       ((_s_)->nVersion.s.nStep != 0x0)) _e_ = OMX_ErrorVersionMismatch;\
    if(_e_ != OMX_ErrorNone) goto OMX_CONF_CMD_BAIL;        \
} while(0)

/*
 * Initializes a data structure using a pointer to the structure.
 * The initialization of OMX structures always sets up the nSize and nVersion fields
 *   of the structure.
 */
#define OMX_CONF_INIT_STRUCT(_s_, _name_)       \
do {                                            \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x0;      \
    (_s_)->nVersion.s.nRevision     = 0x0;      \
    (_s_)->nVersion.s.nStep         = 0x0;      \
} while(0)


/* Event Handler Macro*/
#define OMX_VIDENC_EVENT_HANDLER(_hComponent_, _eEvent_, _nData1_, _nData2_, _pEventData_) \
do {                                                        \
    if((_hComponent_)->bHideEvents != OMX_TRUE )            \
        (_hComponent_)->sCbData.EventHandler((_hComponent_)->pHandle, \
                                            (_hComponent_)->pHandle->pApplicationPrivate, \
                                            _eEvent_,       \
                                            _nData1_,       \
                                            _nData2_,       \
                                            _pEventData_);  \
                                                            \
        OMX_PRINT1((_hComponent_)->dbg, "EventHandler : %lx : %lx : %lx \n", (OMX_U32) (_eEvent_), (OMX_U32) (_nData1_), (OMX_U32) (_nData2_)); \
                                                            \
} while(0)

#if 1
#define VIDENC_MALLOC(_p_, _s_, _c_, _h_, dbg)              \
do {                                                        \
    _p_ = malloc(_s_ + PADDING_SIZE); 		                 \
     if (_p_ == NULL) {                                      \
        OMX_TRACE4(dbg, "malloc() error.\n");               \
        eError = OMX_ErrorInsufficientResources;            \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
    else {                                                  \
        OMX_TRACE1(dbg, "malloc() -> %p\n", _p_); \
    }                                                       \
    memset((_p_), 0x0, (_s_ + PADDING_SIZE));                                \
    if ((_p_) == NULL) {                                    \
        OMX_TRACE4(dbg, "memset() error.\n");               \
        eError = OMX_ErrorUndefined;                        \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
    eError = OMX_VIDENC_ListAdd(&(dbg), _h_, _p_, _s_);          \
    if (eError == OMX_ErrorInsufficientResources) {         \
        OMX_TRACE4(dbg, "malloc() error.\n");               \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
    _p_ = (char *)_p_ + PADDING_SIZE/2;                     \
     _p_ = (_c_ *)_p_;                                      \
  } while(0)

#define VIDENC_FREE(_p_, _h_, dbg)                          \
do {                                                        \
    OMX_VIDENC_ListRemove((&dbg), _h_, _p_);                \
    _p_ = NULL;                                             \
} while(0)
#endif


#if defined(DSPALIGN_128BYTE)

// wrapper to call OMX_MALLOC_SIZE_DSPALIGN
#define VIDENC_MALLOC_DSPALIGN(_p_, _s_, _c_, _h_, dbg)      \
do {                                                        \
LOGD("line = %d func = %s DSPALIGN ", __LINE__, __FUNCTION__);      \
    OMX_MALLOC_SIZE_DSPALIGN(_p_,_s_,_c_);                  \
    if (_p_ == NULL) {                                      \
        OMX_TRACE4(dbg, "OMX_MALLOC_SIZE_DSPALIGN() error.\n");               \
        eError = OMX_ErrorInsufficientResources;            \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
    else {                                                  \
        OMX_TRACE1(dbg, "OMX_MALLOC_SIZE_DSPALIGN() -> %p\n", _p_); \
    }                                                       \
    memset((_p_), 0x0, _s_);                                \
    if ((_p_) == NULL) {                                    \
        OMX_TRACE4(dbg, "memset() error.\n");               \
        eError = OMX_ErrorUndefined;                        \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
    eError = OMX_VIDENC_ListAdd(&(dbg), _h_, _p_, 0);          \
    if (eError == OMX_ErrorInsufficientResources) {         \
        OMX_TRACE4(dbg, "OMX_MALLOC_SIZE_DSPALIGN() error.\n");               \
        goto OMX_CONF_CMD_BAIL;                             \
    }                                                       \
} while(0)

#else /* defined(DSPALIGN_128BYTE) */

/*
 This is defined in TI_OMX_Common.h post TI Android BSP inc 2.5
 TODO: to be deleted after migration to inc 2.5 BSP is complete
*/

/* ======================================================================= */
/**
 * @def    OMX_GET_SIZE_DSPALIGN Macro to get the DSP aligned size
 */
/* ======================================================================= */
#define OMX_GET_SIZE_DSPALIGN(_size_)            \
    ((_size_+DSP_CACHE_ALIGNMENT-1) & ~(DSP_CACHE_ALIGNMENT-1))

/*
   temporary implementation wrapper
   TODO: delete this after migration to TI Android BSP inc 2.5 is complete
*/
#define VIDENC_MALLOC_DSPALIGN(_p_, _s_, _c_, _h_, dbg)      \
  VIDENC_MALLOC(_p_, (_s_ + 256), _c_, _h_, dbg);       \
  {                                                     \
    OMX_U8* _temp_ = (OMX_U8*)_p_;                      \
    _temp_ += 128;                                      \
    _p_ = _temp_;                                       \
  }

/*
   This are defined in LCML_DspCodec.h post TI Android BSP inc 2.5
   TODO: to be deleted after migration to inc 2.5 BSP is complete
*/
enum{
  NO_ALIGNMENT_CHECK = 0x4000,
  ALIGNMENT_CHECK= 0x8000,
  MAX_ALIGNMENT_CHECK
};

#endif /* defined(DSPALIGN_128BYTE) */


/* Codec related initialization parameters */
#define MAX_SEARCH_RANGE_X   64
#define MAX_SEARCH_RANGE_Y   16
#define MAX_SEARCH_RANGE_H264_Y   8

#define ENABLE_SEI_VUI 1

#define PIXEL_ASPECT_RATIO_X  1
#define PIXEL_ASPECT_RATIO_Y  1
#define PIXEL_ASPECT_RATIO_RANGE  0

#define SET_DEINTERLACE_PRESET(x, y) (x = ((x & 0xFFFFFFC7) | ((y & 0x7) << 3)))



#define MAX_BUFFERS_TO_DSP 2

/*****************************************************************************/
/* Structure Definitions                                                     */
/*****************************************************************************/

typedef struct VIDENC_NODE
{
    char 	*pData;
    int     size;
    struct VIDENC_NODE* pNext;
}VIDENC_NODE;

typedef enum VIDEOENC_PORT_INDEX
{
    VIDENC_INPUT_PORT = 0x0,
    VIDENC_OUTPUT_PORT
} VIDEOENC_PORT_INDEX;

/* Custom set/get param */
typedef struct VIDENC_CUSTOM_DEFINITION
{
    OMX_U8 cCustomName[128];
    OMX_INDEXTYPE nCustomIndex;
} VIDENC_CUSTOM_DEFINITION;

typedef struct OMX_CONF_CIRCULAR_BUFFER_NODE
{
    OMX_HANDLETYPE hMarkTargetComponent;
    OMX_PTR pMarkData;
    OMX_U32 nTickCount;
    OMX_TICKS nTimeStamp;
    OMX_U32 nFlags;
    struct OMX_CONF_CIRCULAR_BUFFER_NODE* pNext;
} OMX_CONF_CIRCULAR_BUFFER_NODE;

typedef struct OMX_CONF_CIRCULAR_BUFFER
{
    struct OMX_CONF_CIRCULAR_BUFFER_NODE* pHead;
    struct OMX_CONF_CIRCULAR_BUFFER_NODE* pTail;
    OMX_U8 nElements;
    OMX_U8 nFillElements;
} OMX_CONF_CIRCULAR_BUFFER;

typedef enum VIDENC_CUSTOM_INDEX {
    #ifdef KHRONOS_1_2
        VideoEncodeCustomParamIndexVBVSize = OMX_IndexVendorStartUnused,
    #else
        VideoEncodeCustomParamIndexVBVSize = OMX_IndexIndexVendorStartUnused,
    #endif
    VideoEncodeCustomParamIndexDeblockFilter,
    VideoEncodeCustomConfigIndexForceIFrame,
    VideoEncodeCustomConfigIndexIntraFrameInterval,
    VideoEncodeCustomConfigIndexTargetFrameRate,
    VideoEncodeCustomConfigIndexQPI,
    VideoEncodeCustomConfigIndexAIRRate,
    VideoEncodeCustomConfigIndexTargetBitRate,
    /*Segment mode Metadata*/
    VideoEncodeCustomConfigIndexMVDataEnable,
    VideoEncodeCustomConfigIndexResyncDataEnable,
    /*ASO*/
    VideoEncodeCustomConfigIndexNumSliceASO,
    VideoEncodeCustomConfigIndexAsoSliceOrder,
    /*FMO*/
    VideoEncodeCustomConfigIndexNumSliceGroups,
    VideoEncodeCustomConfigIndexSliceGroupMapType,
    VideoEncodeCustomConfigIndexSliceGroupChangeDirectionFlag,
    VideoEncodeCustomConfigIndexSliceGroupChangeRate,
    VideoEncodeCustomConfigIndexSliceGroupChangeCycle,
    VideoEncodeCustomConfigIndexSliceGroupParams,
    /*others*/
    VideoEncodeCustomConfigIndexMIRRate,
    VideoEncodeCustomConfigIndexMaxMVperMB,
    VideoEncodeCustomConfigIndexIntra4x4EnableIdc,
    /*only for H264*/
    VideoEncodeCustomParamIndexEncodingPreset,
    VideoEncodeCustomParamIndexNALFormat,
    //Ittiam changes
    VideoEncoderStoreMetadatInBuffers,
    /* debug config */
    VideoEncodeCustomConfigIndexDebug
} VIDENC_CUSTOM_INDEX;

typedef enum VIDENC_BUFFER_OWNER
{
    VIDENC_BUFFER_WITH_CLIENT = 0x0,
    VIDENC_BUFFER_WITH_COMPONENT,
    VIDENC_BUFFER_WITH_DSP,
    VIDENC_BUFFER_WITH_TUNNELEDCOMP
} VIDENC_BUFFER_OWNER;

typedef enum VIDENC_AVC_NAL_FORMAT
{
    VIDENC_AVC_NAL_UNIT = 0,    /*Default, one buffer per frame, no NAL mode*/
    VIDENC_AVC_NAL_SLICE,       /*One NAL unit per buffer, one or more NAL units conforms a Frame*/
    VIDENC_AVC_NAL_FRAME        /*One frame per buffer, one or more NAL units inside the buffer*/
}VIDENC_AVC_NAL_FORMAT;

typedef struct VIDENC_BUFFER_PRIVATE
{
    OMX_PTR pMetaData;/*pointer to metadata structure, this structure is used when MPEG4 segment mode is enabled  */
    OMX_BUFFERHEADERTYPE* pBufferHdr;
    OMX_PTR pUalgParam;
    VIDENC_BUFFER_OWNER eBufferOwner;
    OMX_BOOL bAllocByComponent;
    OMX_BOOL bReadFromPipe;
} VIDENC_BUFFER_PRIVATE;

typedef struct VIDENC_MPEG4_SEGMENTMODE_METADATA
{
    unsigned int mvDataSize;/*unsigned int*/
    unsigned int numPackets;/*unsigned int*/
    OMX_PTR pMVData;/*pointer to unsigned char MVData[3264]*/
    OMX_PTR pResyncData;/*pointer to unsigned char ResyncData[5408]*/
}VIDENC_MPEG4_SEGMENTMODE_METADATA;

typedef enum VIDENC_BUFFER_TYPE
{
    VirtualPointers,    /*Used when buffer pointers come from the normal A9 virtual space */
    GrallocPointers,    /*Used when buffer pointers come from Gralloc allocations */
    IONPointers,        /*Used when buffer pointers come from ION allocations */
    EncoderMetadataPointers    /*Used when buffer pointers come from Stagefright in camcorder usecase */
} VIDENC_BUFFER_TYPE;

typedef struct OMX_VIDEO_STOREMETADATAINBUFFERSPARAMS {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bStoreMetaData;
} OMX_VIDEO_STOREMETADATAINBUFFERSPARAMS;


typedef struct VIDEOENC_PORT_TYPE
{
    OMX_U32 nBufferCnt;
    OMX_U32 nTunnelPort;
    OMX_HANDLETYPE hTunnelComponent;
    OMX_BUFFERSUPPLIERTYPE eSupplierSetting;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDef;
    OMX_VIDEO_PARAM_PORTFORMATTYPE* pPortFormat;

#ifdef __KHRONOS_CONF_1_1__
    OMX_VIDEO_PARAM_PROFILELEVELTYPE* pProfileType;
    OMX_CONFIG_FRAMERATETYPE* pFrameRateConfig;
    OMX_VIDEO_CONFIG_BITRATETYPE* pBitRateTypeConfig;
    OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE* pErrorCorrectionType;
    OMX_VIDEO_PARAM_INTRAREFRESHTYPE* pIntraRefreshType;
#endif

    OMX_VIDEO_PARAM_BITRATETYPE* pBitRateType;
    VIDENC_BUFFER_PRIVATE* pBufferPrivate[VIDENC_MAX_NUM_OF_BUFFERS];
    VIDENC_BUFFER_TYPE VIDEncBufferType;
} VIDEOENC_PORT_TYPE;

#ifndef KHRONOS_1_2
typedef enum OMX_EXTRADATATYPE
{
        OMX_ExtraDataNone = 0,
        OMX_ExtraDataQuantization
    } OMX_EXTRADATATYPE;
#endif
typedef struct OMX_OTHER_EXTRADATATYPE_1_1_2
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_EXTRADATATYPE eType;
    OMX_U32 nDataSize;
    OMX_U8 data[1];
} OMX_OTHER_EXTRADATATYPE_1_1_2;

typedef struct VIDEO_PROFILE_LEVEL
{
    OMX_S32  nProfile;
    OMX_S32  nLevel;
} VIDEO_PROFILE_LEVEL_TYPE;

/*
 * State of the component with regard to buffer handling
 * INIT                 - this is the state when the component has not been issued a input-output pair yet
 * HEADER_DECODE_DONE   - this is the state post header decoding
 *                          in this state, the first output-buffer is returned with header data
 *                                         the input buffer is stuck with us
 * STAGGER              - the component is creating a stagger before entering the steady state
 *                          in this state, the first input buffer has been issued to the DSP
 *                                         arm-encoder has not yet been issued a buffer yet
 *                                         The component will be in this state till "m" buffers
 *                                            have been issued to the dsp alone
 * STEADY               - this is the steady state, post staggering
 *                          in this state (m = stagger stages):
 *                                         dsp is issued Nth input and output
 *                                         arm is requested to process (N-m)th input and output
 *                                         wait for the (N-m)th input/output to be processed
 */
typedef enum
{
    OMX_SPLIT_VIDENC_STATE_INIT                 = 0x00,
    OMX_SPLIT_VIDENC_STATE_HEADER_ENCODE_DONE   = 0x01,
    OMX_SPLIT_VIDENC_STATE_STAGGER              = 0x02,
    OMX_SPLIT_VIDENC_STATE_STEADY               = 0x03,
    OMX_SPLIT_VIDENC_STATE_INVALID
} OMX_SPLIT_VIDENC_STATE_T;

/**
 * The VIDENC_COMPONENT_PRIVATE data structure is used to store component's
 *                              private data.
 */
typedef struct VIDENC_COMPONENT_PRIVATE
{
    OMX_PORT_PARAM_TYPE* pPortParamType;
    VIDEOENC_PORT_TYPE* pCompPort[VIDENC_NUM_OF_PORTS];
#ifdef __KHRONOS_CONF_1_1__
    OMX_PORT_PARAM_TYPE* pPortAudioType;
    OMX_PORT_PARAM_TYPE* pPortImageType;
    OMX_PORT_PARAM_TYPE* pPortOtherType;
#endif

    OMX_PRIORITYMGMTTYPE* pPriorityMgmt;
    OMX_VIDEO_PARAM_AVCTYPE* pH264;
    OMX_VIDEO_CONFIG_AVCINTRAPERIOD*  pH264IntraPeriod;  /* for intraFrameInterval */
    OMX_VIDEO_PARAM_MOTIONVECTORTYPE* pMotionVector;     /* for searchRange, maxMVperMB, qpi */
    OMX_VIDEO_PARAM_MPEG4TYPE* pMpeg4;
    OMX_VIDEO_PARAM_H263TYPE* pH263;
    OMX_VIDEO_PARAM_BITRATETYPE* pVidParamBitrate;
    OMX_VIDEO_PARAM_QUANTIZATIONTYPE* pQuantization;

    OMX_CALLBACKTYPE sCbData;
    OMX_COMPONENTTYPE* pHandle;
    OMX_STATETYPE eState;
    OMX_VERSIONTYPE ComponentVersion;
    OMX_VERSIONTYPE SpecVersion;
    OMX_STRING cComponentName;
    int nFree_oPipe[2];
    int nFilled_iPipe[2];
    int nCmdPipe[2];
    int nCmdDataPipe[2];

    int nFilled_oPipe[2];
    int nFree_iPipe[2];

    /* Pipes for sending and receiving DSP ACK events */
    int nVidenc_DSP_wait_comm_thread[2];
    int nDSP_wait_videnc_comm_thread[2];

    int nDsp_wait_thread_event_pipe[2];
    int nDsp_wait_thread_data_pipe[2];

    int nCallback_DSPWait_comm_thread[2];
    int nCallback_thread_data_pipe[2];
    int nCallback_thread_event_pipe[2];

    IVIDENC1_Handle        pCodecHandle;
    IVIDENC1_DynamicParams sDynParams;
    int nFrameCnt;
#ifdef __PERF_INSTRUMENTATION__
    PERF_OBJHANDLE pPERF, pPERFcomp;
    OMX_U32 nLcml_nCntIp;
    OMX_U32 nLcml_nCntOpReceived;
#endif
    unsigned int nVBVSize;
    OMX_MARKTYPE* pMarkBuf;
    OMX_PTR pMarkData;
    OMX_HANDLETYPE hMarkTargetComponent;
    OMX_U32 nFlags;
    OMX_U32 nCounter;
    /* these are duplicates */
    unsigned int nIntraFrameInterval;  /* should be OMX_VIDEO_CONFIG_AVCINTRAPERIOD */
    unsigned int nTargetFrameRate;  /* should be OMX_CONFIG_FRAMERATETYPE */
    unsigned int nQPI;              /* same as OMX_VIDEO_PARAM_QUANTIZATIONTYPE */
    unsigned int nAIRRate;          /* same as OMX_VIDEO_PARAM_INTRAREFRESHTYPE */
    unsigned int nTargetBitRate;    /* should be OMX_VIDEO_CONFIG_BITRATETYPE */
    OMX_U32 nMIRRate;


    OMX_U32  nInBufferSize;
    OMX_U32  nOutBufferSize;
    OMX_COLOR_FORMATTYPE colorFormats[3];

    pthread_mutex_t mVideoEncodeBufferMutex;
    pthread_mutex_t mbuffersTriggeredMutex;

    OMX_BOOL bDeblockFilter;
    OMX_BOOL bCodecStarted;
    OMX_BOOL bCodecLoaded;
    OMX_BOOL bDSPStopAck;
    OMX_BOOL bForceIFrame;
    OMX_BOOL bFlushComplete;
    OMX_BOOL bHideEvents;
    OMX_BOOL bHandlingFatalError;
    OMX_BOOL bUnresponsiveDsp;
    VIDENC_NODE*  pMemoryListHead;
    OMX_CONF_CIRCULAR_BUFFER sCircularBuffer;
    OMX_BOOL bIsStopped;

#ifdef __KHRONOS_CONF__
#ifdef __KHRONOS_CONF_1_1__
    OMX_PARAM_COMPONENTROLETYPE componentRole;
#endif
    OMX_BOOL bPassingIdleToLoaded;
    OMX_BOOL bErrorLcmlHandle;

    pthread_t ComponentThread;
    pthread_t DSPWaitThread;
    pthread_t CallbackThread;
#endif

/*ASO*/
    OMX_U32 numSliceASO;
    OMX_U32 asoSliceOrder[MAXNUMSLCGPS];
/*FMO*/
    OMX_U32 numSliceGroups;
    OMX_U32 sliceGroupMapType;
    OMX_U32 sliceGroupChangeDirectionFlag;
    OMX_U32 sliceGroupChangeRate;
    OMX_U32 sliceGroupChangeCycle;
    OMX_U32 sliceGroupParams[MAXNUMSLCGPS];
    pthread_mutex_t videoe_mutex;   /* pthread_cond_t  control_cond; */
    pthread_mutex_t videoe_mutex_app;
    pthread_cond_t  populate_cond;
    pthread_cond_t  unpopulate_cond;
    pthread_cond_t  stop_cond;
    pthread_cond_t  flush_cond;
    unsigned int nEncodingPreset;
    VIDENC_AVC_NAL_FORMAT AVCNALFormat;
    OMX_BOOL bMVDataEnable;
    OMX_BOOL bResyncDataEnable;
    IH264VENC_Intra4x4Params intra4x4EnableIdc;
    OMX_U32 maxMVperMB;
    #ifdef RESOURCE_MANAGER_ENABLED
    RMPROXY_CALLBACKTYPE cRMCallBack;
    #endif
    OMX_BOOL bPreempted;
    OMX_VIDEO_CODINGTYPE compressionFormats[3];
    struct OMX_TI_Debug dbg;
    PV_OMXComponentCapabilityFlagsType* pCapabilityFlags;
    OMX_BOOL bControlCallNeeded;
    OMX_BOOL bControlCallNeededForQp;
    OMX_BOOL bControlCallNeededForBitrate;
    OMX_BOOL bHeaderGenerate;
    OMX_BOOL bReturnLastBufferHandled;

    OMX_U32                 dsp720PEncoderBufferTrigIndex;
    IVIDEO1_BufDescIn       sInBufs[MAX_BUFFERS_TO_DSP];
    XDM_BufDesc             sOutBufs[MAX_BUFFERS_TO_DSP];
    IVIDENC1_InArgs         sInArgs[MAX_BUFFERS_TO_DSP];
    IVIDENC1_OutArgs        sOutArgs[MAX_BUFFERS_TO_DSP];

    /* Reference count for pending state change requests */
    OMX_U32 nPendingStateChangeRequests;
    pthread_mutex_t mutexStateChangeRequest;
    pthread_cond_t StateChangeCondition;


    /* Fuunction pointer for dsp trigger */
    XDAS_Int32 (* pf_dsp_trig)(IVIDENC1_Handle handle,
                            IVIDEO1_BufDescIn *inBufs,
                            XDM_BufDesc *outBufs,
                            IVIDENC1_InArgs *inArgs,
                            IVIDENC1_OutArgs *outArgs,
                            WORD32 i4_state);

    /* Function pointer for arm process */
    XDAS_Int32 (* pf_arm_process)(IVIDENC1_Handle handle, WORD32 i4_state);

    /* Function pointer for dsp wait */
    XDAS_Int32 (* pf_dsp_wait)(IVIDENC1_Handle handle, WORD32 i4_state);

    /* Added flag for H264 sps and pps deque calls for header generation */
    OMX_BOOL  bSPSdequed;
    OMX_BOOL  bPPSdequed;


    /* Count of ETB, EBD, FTB and FBD calls */
    OMX_U32 num_inp_buf_issued;
    OMX_U32 num_inp_buf_returned;
    OMX_U32 num_out_buf_issued;
    OMX_U32 num_out_buf_returned;

    OMX_U32 buffersTriggered;

    /* Mutex and conditions for the exit criteria of the threads */
    pthread_cond_t  m_Callback_Thread_Exit_Cond;
    pthread_mutex_t m_Callback_Thread_Exit_mutex;

    pthread_cond_t  m_DSPWait_Thread_Exit_Cond;
    pthread_mutex_t m_DSPWait_Thread_Exit_mutex;

    OMX_BOOL bCallbackThreadFlushing;
    OMX_BOOL bDSPWaitThreadFlushing;

#ifdef __OPENCORE_STORE_INPUT_BUFFER_ADDRESS__
	OMX_U32  *inpBufferAddress[2][NUM_SF_NATIVE_BUFFERS_REQUESTED];
#endif
} VIDENC_COMPONENT_PRIVATE;

typedef struct FRAME_ENCODE_BUFFER
{
    OMX_BUFFERHEADERTYPE *pInputBuffer;
    OMX_BUFFERHEADERTYPE *pOutputBuffer;
    int index;
}FRAME_ENCODE_BUFFER;

typedef OMX_ERRORTYPE (*fpo)(OMX_HANDLETYPE);

/*--------function prototypes ---------------------------------*/

OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE hComp);


OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSet(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_U32 nParam1);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSet (VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_U32 nParam1);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetIdle(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetLoaded (VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetExecuting(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetPause (VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandFlush(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_U32 nParam1, OMX_BOOL bInternalFlush);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandDisablePort(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_U32 nParam1);

OMX_ERRORTYPE OMX_VIDENC_HandleCommandEnablePort(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_U32 nParam1);

//OMX_ERRORTYPE OMX_VIDENC_Process_FilledInBuf(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_Process_FilledOutBuf(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_BUFFERHEADERTYPE* pBufHead);

OMX_ERRORTYPE OMX_VIDENC_Process_FreeInBuf(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_BUFFERHEADERTYPE* pBufHead);

//OMX_ERRORTYPE OMX_VIDENC_Process_FreeOutBuf(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_InitLCML(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_InitDSP_H264Enc(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_VIDENC_InitDSP_Mpeg4Enc(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

/*
* function to issue process call to the split encoder
*/
OMX_ERRORTYPE OMX_VIDENC_process(VIDENC_COMPONENT_PRIVATE *pComponentPrivate,
                                 OMX_BUFFERHEADERTYPE *pInpBufHead,
                                 OMX_BUFFERHEADERTYPE *pOutBufHead);


/*
 * This method with purge all the buffers held by the component including the ones in stagger queue
 * This method will be called in case the component thread is starved for either input or output buffers
 *   for a long enough time (timeout interval set in the thread)
 */
OMX_ERRORTYPE OMX_VIDENC_HandleReturnBuffers(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);


OMX_ERRORTYPE OMX_VIDENC_Allocate_DSPResources (OMX_IN VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                                   OMX_IN OMX_U32 nPortIndex);
OMX_ERRORTYPE OMX_VIDENC_EmptyDataPipes (void* pThreadData);

OMX_ERRORTYPE OMX_VIDENC_ListCreate(struct OMX_TI_Debug *dbg, struct VIDENC_NODE** pListHead);

OMX_ERRORTYPE OMX_VIDENC_ListAdd(struct OMX_TI_Debug *dbg, struct VIDENC_NODE* pListHead, OMX_PTR pData, int size);

OMX_ERRORTYPE OMX_VIDENC_ListRemove(struct OMX_TI_Debug *dbg, struct VIDENC_NODE* pListHead, OMX_PTR pData);

OMX_ERRORTYPE OMX_VIDENC_ListDestroy(struct OMX_TI_Debug *dbg, struct VIDENC_NODE* pListHead);

OMX_ERRORTYPE OMX_VIDENC_HandleError(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_ERRORTYPE eError);
#ifdef RESOURCE_MANAGER_ENABLED
void OMX_VIDENC_ResourceManagerCallBack(RMPROXY_COMMANDDATATYPE cbData);
#endif

void OMX_VIDENC_FatalErrorRecover(VIDENC_COMPONENT_PRIVATE* pComponentPrivate);

#endif
