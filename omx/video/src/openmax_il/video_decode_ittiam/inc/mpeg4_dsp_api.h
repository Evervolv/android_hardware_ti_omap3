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
/*  File Name         : mpeg4_dsp_api.h                                      */
/*                                                                           */
/*  Description       : This file contains macros, structures, enums etc     */
/*						which are required by the mpeg4 DSP side SN          */
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

#ifndef OMX_VIDDEC_DSP_MPEG4__H
#define OMX_VIDDEC_DSP_MPEG4__H

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define OMX_MP4DEC_NUM_DLLS   	  (4)
#define STRING_MP4DSOCKET_TI_UUID "7E4B8541_47A1_11D6_B156_00B0D017674B"
#define MP4_DEC_NODE_DLL     	  "720p_mp4vdec_sn.dll64P"

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/
static const struct DSP_UUID MP4DSOCKET_TI_UUID =
{
    0x7e4b8541, 0x47a1, 0x11d6, 0xb1, 0x56,
    {0x00, 0xb0, 0xd0, 0x17, 0x67, 0x4b}
};

typedef struct
{
    OMX_S32 size;
    OMX_S32 numBytes;
    OMX_S32 inputID;
    OMX_S32 bytesConsumed;
    OMX_S32 chromaFormat;
    OMX_S32 display_width;
    OMX_S32 decoded_frame_ht;
    OMX_S32 decoded_frame_wd;
    OMX_S32 extended_error;
    OMX_S32 vop_coding_type;
} Mpeg4Alg_context;

typedef struct MP4VDEC_UALGDynamicParams
{
  OMX_S32 size;
  OMX_U32 ulDecodeHeader;
  OMX_U32 ulDisplayWidth;
  OMX_U32 ulFrameSkipMode;
  OMX_U32 ulPPType;
}MP4VDEC_UALGDynamicParams;

#endif /* OMX_VIDDEC_DSP_MPEG4__H */
