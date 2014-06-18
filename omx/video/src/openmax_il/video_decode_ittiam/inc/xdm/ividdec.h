/*
 *  Copyright 2005 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== ividdec.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the video decoder algorithms.
 *  Version 0.5
 *
 */
#ifndef IVIDDEC_
#define IVIDDEC_

#include <ialg.h>
#include <xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IVIDDEC_CID      "ti.xdais.xdm.IVIDDEC"

#define IVIDDEC_ERUNTIME  XDM_ERUNTIME
#define IVIDDEC_EOK       XDM_EOK
#define IVIDDEC_EFAIL     XDM_EFAIL

/*
 *  ======== IVIDDEC_Obj ========
 *  This structure must be the first field of all VIDDEC instance objects.
 *
 */
typedef struct IVIDDEC_Obj {
    struct IVIDDEC_Fxns *fxns;
} IVIDDEC_Obj;

/*
 *  ======== IVIDDEC_Handle ========
 *  This handle is used to reference all VIDDEC instance objects.
 */
typedef struct IVIDDEC_Obj  *IVIDDEC_Handle;


/*
 *  ======== IVIDDEC_Params ========
 *  This structure defines the creation time parameters for all VIDDEC instance
 *  objects. 
 */
typedef struct IVIDDEC_Params {
    XDAS_Int32 size;              /*  size of this structure                  */
    XDAS_Int32 maxHeight;         /*  Maximum video height to be supported    */
    XDAS_Int32 maxWidth;          /*  Maximum video width to be supported     */
    XDAS_Int32 maxFrameRate ;     /*  Maximum Framerate * 1000 to be supported*/
    XDAS_Int32 maxBitRate;        /*  Maximum Bitrate to be supported  
                                   *  (in bits per second.)                   
                                   */ 
    XDAS_Int32 dataEndianness;    /*  Endianness  of input data as defined in 
                                   *  type: XDM_DataFormat 
                                   */ 
    XDAS_Int32 forceChromaFormat; /*  Set to XDM::XDM_ChromaFormat
                                   *  Set to XDM_DEFAULT to avoid re-sampling 
                                   */ 
} IVIDDEC_Params;

/*  ======== IVIDDEC_DynamicParams ========
 *  This structure defines the run time configurable parameters of IVIDDEC 
 *  instance objects (via control API).
 *
 *  It is not necessary that given implementation support all dynamic 
 *  parameters to be configurable at run time. If a particular codec
 *  does not support run-time updates to a parameter that  the application
 *  is attempting to change at runtime, the codec may indicate this as an
 *  error. 
 */

typedef struct IVIDDEC_DynamicParams {
    XDAS_Int32 size;          /*  size of this structure  */
    XDAS_Int32 decodeHeader;  /*  XDM_DECODE_AU or XDM_PARSE_HEADER */
    XDAS_Int32 displayWidth;  /*  DEFAULT (0): use imagewidth as pitch  
                               *  else use given Display width for pitch
                               *  only if Displaywidth greater than    
                               *  imagewidth of input. 
                               */
    XDAS_Int32 frameSkipMode; /*  Frame skip mode. 
                               *  IVIDEO_NO_SKIP,IVIDEO_SKIP_P, IVIDEO_SKIP_B,
                               *  or IVIDEO_SKIP_I (only in case of H.264)
                               */
} IVIDDEC_DynamicParams;

/*
 *  ======== IVIDDEC_InArgs ========
 *  This structure defines the input arguments for all VIDDEC instance 
 *  process function.
 */
typedef struct IVIDDEC_InArgs {
    XDAS_Int32 size;     /* size of this structure  */
    XDAS_Int32 numBytes; /* Size of valid input data in bytes in input buffer */
    XDAS_Int32 inputID;  /* Application will pass this ID to algorithm and 
                          * decoder will attach this ID with corrosponding 
                          * output frames. 
                          * This is useful in case re-ordering (e.g B frames). 
                          * In case of lack of re-ordering Output ID in Status 
                          * will be same as Input ID  
                          */
} IVIDDEC_InArgs;

/*
 *  ======== IVIDDEC_Status ========
 *  This structure defines instance status parameters (read-only).
 */
typedef struct IVIDDEC_Status {
     XDAS_Int32      size;               /* size of this structure          */
     XDAS_Int32      extendedError;      /* Extended Error code. (see XDM_ErrorBit) */
     XDAS_Int32      outputHeight;       /* Output Height                   */
     XDAS_Int32      outputWidth;        /* Output Width                    */
     XDAS_Int32      frameRate;          /* Average frame rate* 1000        */
     XDAS_Int32      bitRate;            /* Average Bit rate in bits/second */
     XDAS_Int32      contentType;        /* Video Content: IVIDEO_PROGRESSIVE
                                          * or IVIDEO_INTERLACED 
                                          */
     XDAS_Int32      outputChromaFormat; /* Chroma format of output: 
                                          * of type IVIDEO_CHROMAFORMAT
                                          */
     XDM_AlgBufInfo  bufInfo;            /* Input & output buffer info      */
} IVIDDEC_Status;


/*
 *  ======== IVIDDEC_OutArgs ========
 *  This structure defines the run time output arguments for all VIDDEC 
 *  instance objects. 
 */
typedef struct IVIDDEC_OutArgs {
  XDAS_Int32 size;               /*  size of this structure                */
  XDAS_Int32 extendedError;      /*  Extended Error code                   */
  XDAS_Int32 bytesConsumed;      /*  The bytes consumed per given call     */
  XDAS_Int32 decodedFrameType;   /*  Frame type: IVIDEO_I_FRAME, 
                                  *  IVIDEO_P_FRAME, IVIDEO_B_FRAME, or 
                                  *  IVIDEO_IDR_FRAME                    
                                 */
  XDAS_Int32 outputID;           /*  Output ID : Refer to comment 
                                  *  VIDDEC_InArgs::InputId 
                                 */
  IVIDEO_BufDesc displayBufs;     /* decoder fills this structure to denote
                                     the buffer pointers for current displayable
                                     frames. In case of sequences having I & P
                                     frames only, This values are identical to
                                     outBufs passed in Process call 
                                  */
                                                                    
} IVIDDEC_OutArgs;


/*
 *  ======== IVIDDEC_Cmd ========
 *  This structure defines the control commands for the VIDDEC module.
 */
typedef  IALG_Cmd IVIDDEC_Cmd;


/*
 *  ======== IVIDDEC_Fxns ========
 *  This structure defines all of the operations on VIDDEC objects.
 * 
 *
 *  process() - Basic video decoding call. It takes input and output buffer
 *              descriptors and structures for input and output arguments. 
 *              The algorithm may also modify the output buffer pointers.
 *              The return value is IALG_EOK for success or IALG_EFAIL
 *              in case of failure. The extendedError field of the outArgs
 *              contain further error conditions flagged by the algorithm.
 *
 *  control() - This will control behaviour of algorithm via Cmd ID. 
 *              All XDM Control Commands are supported:
 *                  XDM_GETSTATUS =0, XDM_SETPARAMS, XDM_RESET, XDM_SETDEFAULT,
 *                  XDM_FLUSH, XDM_GETBUFINFO
 *              See xdm.h for details. 
 *              The return value is IALG_EOK for success or IALG_EFAIL.
 *              In case of failure extended error code contains details. 
 */
typedef struct IVIDDEC_Fxns {
    IALG_Fxns   ialg;
    XDAS_Int32 (*process)(IVIDDEC_Handle handle, XDM_BufDesc *inBufs, 
               XDM_BufDesc *outBufs, IVIDDEC_InArgs *inargs, 
               IVIDDEC_OutArgs *outargs);
    XDAS_Int32 (*control)(IVIDDEC_Handle handle, IVIDDEC_Cmd id, 
               IVIDDEC_DynamicParams *params, IVIDDEC_Status *status);
} IVIDDEC_Fxns;

#ifdef __cplusplus
}
#endif

#endif  /* IVIDDEC_ */
