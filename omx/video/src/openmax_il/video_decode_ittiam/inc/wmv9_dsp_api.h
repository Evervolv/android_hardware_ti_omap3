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
/*  File Name         : wmv9_dsp_api.h                                       */
/*                                                                           */
/*  Description       : This file contains macros, structures, enums etc     */
/*						which are required by the wmv9 DSP side SN           */
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

#ifndef OMX_VIDDEC_DSP_WMV9__H
#define OMX_VIDDEC_DSP_WMV9__H

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define OMX_WMVDEC_NUM_DLLS       (4)
#define STRING_WMVDSOCKET_TI_UUID "8E4B8541_47A1_11D6_B156_00B0D01767FF"
#define WMV_DEC_NODE_DLL          "720p_wmv9vdec_sn.dll64P"

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/
static const struct DSP_UUID WMVDSOCKET_TI_UUID =
{
   0x8e4b8541, 0x47a1, 0x11d6, 0xb1, 0x56,
   {0x00, 0xb0, 0xd0, 0x17, 0x67, 0xFF}
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
    OMX_S32 extendedError;
} WMV9Alg_context;

/* WMV9DEC_UALGDynamicParams - This structure defines the run-time algorithm  */
/* specific and UALG parameters which can be configured.					  */
/* @param ulDecodeHeader    :Parse header.									  */
/* @param ulDisplayWidth    :Pitch value. Used as pitch only if this value	  */
/* is greater than Video width.												  */
/* @param ulFrameSkipMode   :Frame skip mode.								  */
/* @param ulPPType          :Post-processing type required.                   */
typedef struct WMV9DEC_UALGDynamicParams
{
    OMX_S32 size;
    OMX_U32 ulDecodeHeader;
    OMX_U32 ulDisplayWidth;
    OMX_U32 ulFrameSkipMode;
    OMX_U32 ulPPType;
    OMX_U16 usIsElementaryStream;
}WMV9DEC_UALGDynamicParams;

#endif /* OMX_VIDDEC_DSP_WMV9__H  */
