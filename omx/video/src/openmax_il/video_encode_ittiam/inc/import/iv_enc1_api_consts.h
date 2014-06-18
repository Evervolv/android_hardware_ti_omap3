/*****************************************************************************/
/*                                                                           */
/*                            VIDEO ENCODER API                              */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2007                             */
/*                                                                           */
/*  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and */
/*  is protected under Indian  Copyright Law as an unpublished work. Its use */
/*  and  disclosure  is  limited by  the terms  and  conditions of a license */
/*  agreement. It may not be copied or otherwise  reproduced or disclosed to */
/*  persons outside the  licensee's organization  except in accordance  with */
/*  the  terms  and  conditions   of  such  an  agreement.  All  copies  and */
/*  reproductions shall be the property  of Ittiam  Systems Private  Limited */
/*  and must bear this notice in its entirety.                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : iv_enc1_api_consts.h                                 */
/*                                                                           */
/*  Description       : Describes the extensions to XDM1.0 enumerations and  */
/*                      macros.                                              */
/*                                                                           */
/*  List of Functions : None                                                 */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         27 09 2007   Ittiam          Original                             */
/*                                                                           */
/*****************************************************************************/

#ifndef _IV_ENC1_API_CONSTS_H_
#define _IV_ENC1_API_CONSTS_H_

/****************************************************************************/
/* ivea - Stands for: Ittiam Video Encoder Application                      */
/****************************************************************************/

/****************************************************************************/
/* MACROS                                                                   */
/****************************************************************************/
#define IVEA_MAX_DYN_PARAMS_TO_ENC      (10)
#define IVEA_MAX_ERROR_LENGTH           (1024)
#define IVEA_MAX_RFC_SIZE               128

/****************************************************************************/
/* Presets of dsp processing tools */
/****************************************************************************/
#define IMP4E_SET_CODING_TOOLS_TO_DEFAULT(x) (x) = 0

#define DSP_ENCODING_PERCENTAGE             96

#define IVEA_GET_PULLDOWN_PRESET(x)         (x & 0x7)
#define IVEA_SET_PULLDOWN_PRESET(x, y)      (x = ((x & 0xFFFFFFF8) | \
                                            (y & 0x7)))

#define IVEA_GET_DEINTERLACE_PRESET(x)      ((x & 0x00000038) >> 3)
#define IVEA_SET_DEINTERLACE_PRESET(x, y)   (x = ((x & 0xFFFFFFC7) | \
                                            ((y & 0x7) << 3)))

#define IVEA_GET_DEBLK_PRESET(x)      ((x) & 1)
#define IVEA_SET_DEBLK_PRESET(x, y)   (x = ((x) & ~1) | ((y & 0x1)))

#define IVEA_GET_QPEL_PRESET(x)      (((x) & 0x2) >> 1)
#define IVEA_SET_QPEL_PRESET(x, y)   (x = ((x) & (~0x2)) | \
                                            (((y) & 0x1) << 1))
                                            
#define IVEA_GET_MB_QP_ACT_PRESET(x)      (((x) & 0x4) >> 2)
#define IVEA_SET_MB_QP_ACT_PRESET(x, y)   (x = ((x) & (~0x4)) | \
                                            (((y) & 0x1) << 2))

#define IVEA_GET_IIR_FILT_PRESET(x)      (((x) & 0x8) >> 3)
#define IVEA_SET_IIR_FILT_PRESET(x, y)   (x = ((x) & (~0x8)) | \
                                            (((y) & 0x1) << 3))

typedef enum E_IVEA_PULLDOWN_PRESETS
{
    IVEA_DISABLE_PULLDOWN = 0,
    IVEA_ENABLE_PULLDOWN
}E_IVEA_PULLDOWN_PRESETS_T;

typedef enum E_IVEA_DEINTERLACE_PRESETS
{
    IVEA_DISABLE_DEINTERLACE        = 0,
    IVEA_SIMPLE_DEINTERLACE
}E_IVEA_DEINTERLACE_PRESETS_T;

typedef enum E_IVEA_DEBLK_PRESETS
{
    IVEA_DISABLE_DEBLK        = 0,
    IVEA_ENABLE_DEBLK         = 1
}E_IVEA_DEBLK_PRESETS_T;

typedef enum E_IVEA_MB_QPEL_PRESETS
{
    IVEA_DISABLE_QPEL_ME    = 0,
    IVEA_ENABLE_QPEL_ME     = 1 
}E_IVEA_MB_QPEL_PRESETS_T;

typedef enum E_IVEA_MB_QP_ACT_PRESETS
{
    IVEA_DISABLE_MB_QP_ACT  = 0,
    IVEA_ENABLE_MB_QP_ACT   = 1 
}E_IVEA_MB_QP_ACT_PRESETS_T;

typedef enum E_IVEA_IIR_FILT_PRESETS
{
    IVEA_DISABLE_IIR_FILT	= 0,
    IVEA_ENABLE_IIR_FILT	= 1	
}E_IVEA_IIR_FILT_PRESETS_T;

/****************************************************************************/
/* ENUMERATIONS                                                             */
/****************************************************************************/

/****************************************************************************/
/* TLV buffer ids                                                           */
/****************************************************************************/

typedef enum E_IVEA_TLV_BUF_TAGS
{
    IVEA_TAGS_START                     =   0x00000000,

    /* Codec agnostic, out tags */
    IVEA_TAGS_CODEC_AGNOSTIC_OUT_START  =   0x00400000,
    IVEA_TAG_CA_OUT_BUF_INFO            =   0x00400001,
    IVEA_TAGS_CODEC_AGNOSTIC_OUT_END    =   0x0040FFFF,

    /* Codec agnostic, input tags */
    IVEA_TAGS_CODEC_AGNOSTIC_INP_START  =   0x00800000,
    IVEA_TAG_CA_INP_USER_DATA           =   0x00800001,
    IVEA_TAG_CA_INP_PULL_DOWN_INFO      =   0x00800003,
    IVEA_TAGS_CODEC_AGNOSTIC_INP_END    =   0x0080FFFF,

    /* Codec agnostic I/O tags */
    IVEA_TAGS_CODEC_AGNOSTIC_IO_START   =   0x00C00000,
    IVEA_TAG_CA_IO_DATA_END             =   0x00C00001,
    IVEA_TAGS_CODEC_AGNOSTIC_IO_END     =   0x00C0FFFF,

    /* Codec specific tags */
    IVEA_TAGS_CODEC_SPECIFIC_START      =   0x01000000,

    IVEA_TAGS_END                       =   0xFFFFFFFF
}E_IVEA_TLV_BUF_TAGS_T;


/****************************************************************************/
/* enum E_ENC_CONTROL_CMD                                                   */
/* This is the extended set of control commands, common to all encoders     */
/* + All "GET"Params will be returned to application through status->data   */
/****************************************************************************/
typedef enum E_IVEA_CONTROL_CMD_EXT
{
    IVEA_EXT_CMDS_START         = 0x4000,

/****************************************************************************/
/*  One Time dynamic Param's: To be called before first process()           */
/****************************************************************************/

    IVEA_SET_RC_OTP_PRMS            =       IVEA_EXT_CMDS_START + 0x20,
    IVEA_GET_RC_OTP_PRMS            =       IVEA_EXT_CMDS_START + 0x21,

    IVEA_SET_RND_ACCESS_PTS_OTP     =       IVEA_EXT_CMDS_START + 0x50,
    IVEA_GET_RND_ACCESS_PTS_OTP     =       IVEA_EXT_CMDS_START + 0x51,

    IVEA_SET_SLICE_OTP_PRMS         =       IVEA_EXT_CMDS_START + 0x60,
    IVEA_GET_SLICE_OTP_PRMS         =       IVEA_EXT_CMDS_START + 0x61,

    IVEA_SET_SLICE_ENC_API_OTP_PRMS =       IVEA_EXT_CMDS_START + 0x70,
    IVEA_GET_SLICE_ENC_API_OTP_PRMS =       IVEA_EXT_CMDS_START + 0x71,

    IVEA_SET_PROFILE_LEVEL_PRMS     =       IVEA_EXT_CMDS_START + 0x80,
    IVEA_GET_PROFILE_LEVEL_PRMS     =       IVEA_EXT_CMDS_START + 0x81,

    IVEA_GET_MBINFO_OTP_PRMS        =       IVEA_EXT_CMDS_START + 0x91,

    IVEA_SET_INTRA_QUANT_MAT        =       IVEA_EXT_CMDS_START + 0xA0,

    IVEA_SET_INTER_QUANT_MAT        =       IVEA_EXT_CMDS_START + 0xB0,

    IVEA_SET_DEFAULT_INTRA_QUANT_MAT=       IVEA_EXT_CMDS_START + 0xC0,

    IVEA_SET_DEFAULT_INTER_QUANT_MAT=       IVEA_EXT_CMDS_START + 0xD0,

    IVEA_SET_INP_ASPECT_RATIO       =       IVEA_EXT_CMDS_START + 0xE0,

    IVEA_GET_INP_ASPECT_RATIO       =       IVEA_EXT_CMDS_START + 0xE1,

    IVEA_SET_TXFR_CTRL              =       IVEA_EXT_CMDS_START + 0xF0,

    IVEA_GET_TXFR_CTRL              =       IVEA_EXT_CMDS_START + 0xF1,


/****************************************************************************/
/* Truly Dynamic Param's : Can be called at any time after handle creation  */
/****************************************************************************/

    IVEA_SET_AIR_REFRESH_PERIOD     =       IVEA_EXT_CMDS_START + 0x2020,
    IVEA_GET_AIR_REFRESH_PERIOD     =       IVEA_EXT_CMDS_START + 0x2021,

    IVEA_SET_QUANT_PARAMS           =       IVEA_EXT_CMDS_START + 0x2030,
    IVEA_GET_QUANT_PARAMS           =       IVEA_EXT_CMDS_START + 0x2031,

    IVEA_SET_CBR_PARAMS             =       IVEA_EXT_CMDS_START + 0x2040,
    IVEA_GET_CBR_PARAMS             =       IVEA_EXT_CMDS_START + 0x2041,

    IVEA_SET_VBR_PARAMS             =       IVEA_EXT_CMDS_START + 0x2050,
    IVEA_GET_VBR_PARAMS             =       IVEA_EXT_CMDS_START + 0x2051,

    IVEA_GET_ERR_INFO               =       IVEA_EXT_CMDS_START + 0x2061,

    IVEA_SET_PKT_PRMS               =       IVEA_EXT_CMDS_START + 0x2070,
    IVEA_GET_PKT_PRMS               =       IVEA_EXT_CMDS_START + 0x2071,

    IVEA_SET_TGT_DIM                =       IVEA_EXT_CMDS_START + 0x2080,
    IVEA_GET_TGT_DIM                =       IVEA_EXT_CMDS_START + 0x2081,

    IVEA_USER_DATA_ON               =       IVEA_EXT_CMDS_START + 0x2090,

    IVEA_USER_DATA_OFF              =       IVEA_EXT_CMDS_START + 0x20A0,

    IVEA_EXT_CMDS_END               =       IVEA_EXT_CMDS_START + 0x4000,

/****************************************************************************/
/*  Algorithm/Codec specific commands                                       */
/****************************************************************************/

    IVEA_EXT_CODEC_SPECIFIC_CMDS_START  =   0x8000,

    IVEA_GET_PAR_PRMS                   =   IVEA_EXT_CODEC_SPECIFIC_CMDS_START + 0x10,
    IVEA_SET_PAR_PRMS                   =   IVEA_EXT_CODEC_SPECIFIC_CMDS_START + 0x11,

    IVEA_SET_IIR_PARAMS                 =   IVEA_EXT_CODEC_SPECIFIC_CMDS_START + 0x12,
    IVEA_OTP_GET_SEI_VUI_PARAMS         =	IVEA_EXT_CODEC_SPECIFIC_CMDS_START +0x13,
    IVEA_OTP_SET_SEI_VUI_PARAMS         =	IVEA_EXT_CODEC_SPECIFIC_CMDS_START +0x14,

    IVEA_EXT_CODEC_SPECIFIC_CMDS_END    =   0xC000

}E_IVEA_CONTROL_CMD_EXT_T;

/****************************************************************************/
/*  Extensions to rate control presets: IVIDEO_RateControlPreset            */
/****************************************************************************/

typedef enum E_IVEA_RC_PRESETS
{
    /* Codec independant rc presets extension */
    IVEA_CA_RC_PRESET_START         =       0x00000010,
    /* Non low-delay CBR rate control */
    IVEA_RC_CBR                     =       0x00000011,

    /**************************************************************************/
    /* Variable bit-rate transmission for low latency streaming in presence of*/
    /* periodic I-frames, needed for random access when the stream is stored  */
    /* for later playback                                                     */
    /**************************************************************************/
    IVEA_VBR_LOW_LATENCY            =       0x00000021,

    /**************************************************************************/
    /* Codec independent rc presets extension ends                           */
    /**************************************************************************/
    IVEA_CA_RC_PRESET_END           =       0x000000FF,

    /* Codec specific rc preset extensions */
    IVEA_CS_RC_PRESET_START         =       0x00000100,
    IVEA_CS_RC_PRESET_END           =       0x000001FF

}E_IVEA_RC_PRESETS_T;

/****************************************************************************/
/*  Extensions to Chroma formats: XDM_ChromaFormat                          */
/****************************************************************************/

typedef enum E_IVEA_CHROMA_FORMAT
{
    /* 4 byte little endian formats */
    IVEA_YUV_422P_LE_32 = 0x00000102,

    /* Y : Planar , U,V : Interleaved */
    IVEA_YUV_420PILE   = 0x00000103,

    /* Y : Planar , U,V : Interleaved */
    IVEA_YUV_422PILE    = 0x00000104,

    /* Y, U,V : Interleaved  Y0-U-Y1-V*/
    IVEA_YUV_422I_LE_16 = 0x00000105

}E_IVEA_CHROMA_FORMAT_T;


/****************************************************************************/
/*  Extensions to Frame Field formats                                       */
/****************************************************************************/
typedef enum E_IVEA_FRM_FLD_RPT_EXT {

    IVEA_FRAME                    = 0,
    IVEA_TOP_FIELD                = 1,
    IVEA_BOTTOM_FIELD             = 2,
    IVEA_TOP_BOTTOM_FIELD         = 3,
    IVEA_BOTTOM_TOP_FIELD         = 4,
    IVEA_TOP_BOTTOM_TOP_FIELD     = 5,
    IVEA_BOTTOM_TOP_BOTTOM_FIELD  = 6,
    IVEA_FRAME_DOUBLING           = 7,
    IVEA_FRAME_TRIPLING           = 8

} E_IVEA_FRM_FLD_RPT_EXT_T;

#endif /* _IV_ENC1_API_CONSTS_H_ */


