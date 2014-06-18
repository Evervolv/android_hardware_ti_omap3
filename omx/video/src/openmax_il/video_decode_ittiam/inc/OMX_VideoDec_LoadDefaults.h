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
/*  File Name         : OMX_VideoDec_LoadDefaults.h                          */
/*                                                                           */
/*  Description       : Contains macros used in OMX_VideoDec_LoadDefaults.h  */
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

#ifndef OMX_VIDDEC_DEFAULT__H
#define OMX_VIDDEC_DEFAULT__H

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define VIDDEC_BUFFERMINCOUNT                   VIDDEC_ONE
#define VIDDEC_PORT_ENABLED                     OMX_TRUE
#define VIDDEC_PORT_POPULATED                   OMX_FALSE
#define VIDDEC_PORT_DOMAIN                      OMX_PortDomainVideo

#define VIDDEC_DEFAULT_INPUT_BUFFER_SIZE        614400
#define VIDDEC_DEFAULT_OUTPUT_BUFFER_SIZE       614400

#define VIDDEC_DEFAULT_WIDTH                    640
#define VIDDEC_DEFAULT_HEIGHT                   480

#define VIDDEC_INPUT_PORT_COMPRESSIONFORMAT     OMX_VIDEO_CodingMPEG4
#define VIDDEC_OUTPUT_PORT_COMPRESSIONFORMAT    OMX_VIDEO_CodingUnused
#define VIDDEC_INPUT_PORT_BUFFERSUPPLIER        VIDDEC_ZERO
#define VIDDEC_OUTPUT_PORT_BUFFERSUPPLIER       VIDDEC_ZERO

#define VIDDEC_MIMETYPEH263                     "H263"
#define VIDDEC_MIMETYPEH264                     "H264"
#define VIDDEC_MIMETYPEMPEG4                    "MPEG4"
#define VIDDEC_MIMETYPEWMV                      "WMV"
#define VIDDEC_MIMETYPEYUV                      "YUV"

#define VIDDEC_INPUT_PORT_NATIVERENDER          NULL
#define VIDDEC_INPUT_PORT_STRIDE                VIDDEC_MINUS
#define VIDDEC_INPUT_PORT_SLICEHEIGHT           VIDDEC_MINUS

#ifdef __STD_COMPONENT__
    #define VIDDEC_INPUT_PORT_BITRATE           (64000)
    #define VIDDEC_INPUT_PORT_FRAMERATE         (15 << 16)
#else
    #define VIDDEC_INPUT_PORT_BITRATE            VIDDEC_MINUS
    #define VIDDEC_INPUT_PORT_FRAMERATE          VIDDEC_MINUS
#endif

#define VIDDEC_INPUT_PORT_FLAGERRORCONCEALMENT  OMX_FALSE

#define VIDDEC_OUTPUT_PORT_NATIVERENDER         NULL
#define VIDDEC_OUTPUT_PORT_STRIDE               VIDDEC_ZERO
#define VIDDEC_OUTPUT_PORT_SLICEHEIGHT          VIDDEC_ZERO
#define VIDDEC_OUTPUT_PORT_BITRATE              VIDDEC_ZERO
#define VIDDEC_OUTPUT_PORT_FRAMERATE            VIDDEC_ZERO
#define VIDDEC_OUTPUT_PORT_FLAGERRORCONCEALMENT OMX_FALSE

#define VIDDEC_DEFAULT_MPEG4_PORTINDEX              VIDDEC_INPUT_PORT
#define VIDDEC_DEFAULT_MPEG4_SLICEHEADERSPACING     VIDDEC_ZERO
#define VIDDEC_DEFAULT_MPEG4_SVH                    OMX_FALSE
#define VIDDEC_DEFAULT_MPEG4_GOV                    OMX_FALSE
#define VIDDEC_DEFAULT_MPEG4_PFRAMES                VIDDEC_MINUS
#define VIDDEC_DEFAULT_MPEG4_BFRAMES                VIDDEC_MINUS
#define VIDDEC_DEFAULT_MPEG4_IDCVLCTHRESHOLD        VIDDEC_MINUS
#define VIDDEC_DEFAULT_MPEG4_ACPRED                 OMX_FALSE
#define VIDDEC_DEFAULT_MPEG4_TIMEINCRES             VIDDEC_MINUS
#define VIDDEC_DEFAULT_MPEG4_PROFILE                VIDDEC_ONE
#define VIDDEC_DEFAULT_MPEG4_LEVEL                  OMX_VIDEO_MPEG4Level1
#define VIDDEC_DEFAULT_MPEG4_ALLOWEDPICTURETYPES    VIDDEC_MINUS
#define VIDDEC_DEFAULT_MPEG4_HEADEREXTENSION        VIDDEC_ONE
#define VIDDEC_DEFAULT_MPEG4_REVERSIBLEVLC          OMX_FALSE
#define VIDDEC_DEFAULT_MPEG4_MAXPACKETSIZE      VIDDEC_DEFAULT_INPUT_BUFFER_SIZE

#define VIDDEC_DEFAULT_H264_PORTINDEX                 VIDDEC_INPUT_PORT
#define VIDDEC_DEFAULT_H264_SLICEHEADERSPACING        VIDDEC_ZERO
#define VIDDEC_DEFAULT_H264_PFRAMES                   VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_BFRAMES                   VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_USEHADAMARD               OMX_FALSE
#define VIDDEC_DEFAULT_H264_REFFRAMES                 VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_REFIDX10ACTIVEMINUS1      VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_REFIDX11ACTIVEMINUS1      VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_ENABLEUEP                 OMX_FALSE
#define VIDDEC_DEFAULT_H264_ENABLEFMO                 OMX_FALSE
#define VIDDEC_DEFAULT_H264_ENABLEASO                 OMX_FALSE
#define VIDDEC_DEFAULT_H264_ENABLERS                  OMX_FALSE
#define VIDDEC_DEFAULT_H264_PROFILE                   OMX_VIDEO_AVCProfileMax
#define VIDDEC_DEFAULT_H264_LEVEL                     OMX_VIDEO_AVCLevelMax
#define VIDDEC_DEFAULT_H264_ALLOWEDPICTURETYPES       VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_FRAMEMBSONLY              OMX_FALSE
#define VIDDEC_DEFAULT_H264_MBAFF                     OMX_FALSE
#define VIDDEC_DEFAULT_H264_ENTROPYCODINGCABAC        OMX_FALSE
#define VIDDEC_DEFAULT_H264_WEIGHTEDPPREDICTION       OMX_FALSE
#define VIDDEC_DEFAULT_H264_WEIGHTEDBIPREDICITONMODE  VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_CONSTIPRED                OMX_FALSE
#define VIDDEC_DEFAULT_H264_DIRECT8X8INFERENCE        OMX_FALSE
#define VIDDEC_DEFAULT_H264_DIRECTSPATIALTEMPORAL     OMX_FALSE
#define VIDDEC_DEFAULT_H264_CABACINITIDC              VIDDEC_MINUS
#define VIDDEC_DEFAULT_H264_LOOPFILTERMODE        OMX_VIDEO_AVCLoopFilterDisable

#define VIDDEC_DEFAULT_WMV_PORTINDEX                  VIDDEC_INPUT_PORT
#define VIDDEC_DEFAULT_WMV_FORMAT                     OMX_VIDEO_WMVFormat9

#define H264VDEC_SN_MAX_NALUNITS                      1200

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
/* Called during initialization to load the default values */
OMX_ERRORTYPE VIDDEC_Load_Defaults (VIDDEC_COMPONENT_PRIVATE* pComponentPrivate,
									OMX_S32                   nPassing);

#endif /* OMX_VIDDEC_DEFAULT__H */
