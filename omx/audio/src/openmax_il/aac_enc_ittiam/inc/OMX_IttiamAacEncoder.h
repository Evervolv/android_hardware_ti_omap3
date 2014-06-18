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

#ifndef OMX_AACENCODER_H
#define OMX_AACENCODER_H


#include <OMX_Component.h>
#include <pthread.h>

/*
 *     M A C R O S
 */
#define AACENC_TIMEOUT (1000) /* millisecs, default timeout used to come out of blocking calls*/
#define NUM_AACENC_INPUT_BUFFERS 1
#define NUM_AACENC_INPUT_BUFFERS_DASF 2
#define NUM_AACENC_OUTPUT_BUFFERS 1
#define INPUT_AACENC_BUFFER_SIZE 8192
#define INPUT_AACENC_BUFFER_SIZE_DASF 8192
#define OUTPUT_AACENC_BUFFER_SIZE 9200
#define SAMPLING_FREQUENCY 44100
#define MAX_NUM_OF_BUFS 10
#define NUM_OF_PORTS 2

/* ======================================================================= */
/**
 * @def    Mem test application
 */
/* ======================================================================= */
#undef AACENC_DEBUGMEM

#ifdef AACENC_DEBUGMEM
#define newmalloc(x) mymalloc(__LINE__,__FILE__,x)
#define newfree(z) myfree(z,__LINE__,__FILE__)
#else
#define newmalloc(x) malloc(x)
#define newfree(z) free(z)
#endif

typedef enum OMX_AACENC_INDEXAUDIOTYPE {
    OMX_IndexCustomAacEncTNSConfig = 0xFF000001,
    OMX_IndexCustomAacEncQualityLevel,
    OMX_IndexCustomAacEncFrameMode
}OMX_AACENC_INDEXAUDIOTYPE;

#endif /* OMX_AACENCODER_H */


