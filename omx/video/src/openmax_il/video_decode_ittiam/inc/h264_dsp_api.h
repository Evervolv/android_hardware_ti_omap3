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
/*  File Name         : h264_dsp_api.h                                       */
/*                                                                           */
/*  Description       : This file contains macros, structures, enums etc     */
/*						which are required by the h.264 DSP side SN          */
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

#ifndef OMX_VIDDEC_DSP_H264__H
#define OMX_VIDDEC_DSP_H264__H

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define OMX_H264DEC_NUM_DLLS          (4)
#define STRING_H264VDSOCKET_TI_UUID   "CB1E9F0F_9D5A_4434_8449_1FED2F992DF7"
#define H264_DEC_NODE_DLL    		  "720p_h264vdec_sn.dll64P"

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/
static const struct DSP_UUID H264VDSOCKET_TI_UUID =
{
    0xCB1E9F0F, 0x9D5A, 0x4434, 0x84, 0x49,
    {0x1F, 0xED, 0x2F, 0x99, 0x2D, 0xF7}
};

typedef struct
{
    OMX_S32  size;
    OMX_S32  i4_inputID;
    OMX_S32  i4_bytesConsumed;
    OMX_S32  i4_extendedError;
    OMX_S32  i4_validity_of_picture;
    OMX_U32 u4_mbs_parsed_asc;
    OMX_S32  i4_num_mb_skip;
    OMX_U16 au2_size_imbuf[4];
    OMX_U32 u4_imbuf2_offset;
} H264Alg_context;

typedef struct H264_Iualg_Cmd_SetStatus
{
    OMX_S32 size;
    OMX_U32 ulDecodeHeader;
    OMX_U32 ulDisplayWidth;
    OMX_U32 ulFrameSkipMode;
    OMX_U32 ulPPType;
} H264_Iualg_Cmd_SetStatus;

#endif /* OMX_VIDDEC_DSP_H264__H */
