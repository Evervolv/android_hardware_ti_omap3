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
/*  File Name         : OMX_IttiamVideoDec_DSP.h                             */
/*                                                                           */
/*  Description       : This file contains macros, structures, enums etc     */
/*						which are required to be used by the DSP side        */
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

#ifndef OMX_VIDDEC_DSP__H
#define OMX_VIDDEC_DSP__H

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define MAX_NUMBER_OF_IM_BUFS (16)
#define STRING_UUID_LENGHT    (37)
#define LIMIT_CORTEX_LOAD     (10)

#define OMX_BUFFERFLAG_SYNCFRAME 							0x00000020
#define VIDDEC_BUFFERFLAG_FRAMETYPE_MASK                    0xF0000000
#define VIDDEC_BUFFERFLAG_FRAMETYPE_I_FRAME                 0x10000000
#define VIDDEC_BUFFERFLAG_FRAMETYPE_P_FRAME                 0x20000000
#define VIDDEC_BUFFERFLAG_FRAMETYPE_B_FRAME                 0x40000000
#define VIDDEC_BUFFERFLAG_FRAMETYPE_IDR_FRAME               0x80000000

#define VIDDEC_BUFFERFLAG_EXTENDERROR_MASK                  0x0FFFF000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_DIRTY                 0x000FF000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_APPLIEDCONCEALMENT    0x00200000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_INSUFFICIENTDATA      0x00400000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_CORRUPTEDDATA         0x00800000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_CORRUPTEDHEADER       0x01000000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_UNSUPPORTEDINPUT      0x02000000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_UNSUPPORTEDPARAM      0x04000000
#define VIDDEC_BUFFERFLAG_EXTENDERROR_FATALERROR            0x08000000

#define RINGIO_NODE_DLL      "ringio.dll64P"
#define USN_DLL              "usn.dll64P"

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#define CEILING_1000X(x)       ((OMX_U32)(x) + (1000-(OMX_U32)(x)%1000))
#define VIDDEC_ISFLAGSET(x,y)  (((x)>>(y)) & 0x1)

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/
typedef enum
{
    USN_DSPACK_STOP          = 0x0200,
    USN_DSPACK_PAUSE         = 0x0300,
    USN_DSPACK_ALGCTRL       = 0x0400,
    USN_DSPACK_STRMCTRL      = 0x0500,
    USN_DSPMSG_BUFF_FREE     = 0x0600,
    USN_DSPACK_SET_STRM_NODE = 0x0700,
    USN_DSPACK_GET_NODE_PTR  = 0x0800,
    USN_DSPMSG_EVENT         = 0x0E00
}USN_NodeToHostCmd;

typedef enum
{
    USN_ERR_NONE,
    USN_ERR_WARNING,
    USN_ERR_PROCESS,
    USN_ERR_PAUSE,
    USN_ERR_STOP,
    USN_ERR_ALGCTRL,
    USN_ERR_STRMCTRL,
    USN_ERR_UNKNOWN_MSG
} USN_ErrTypes;

typedef enum
{
    IUALG_OK                  = 0x0000,
    IUALG_WARN_CONCEALED      = 0x0100,
    IUALG_WARN_UNDERFLOW      = 0x0200,
    IUALG_WARN_OVERFLOW       = 0x0300,
    IUALG_WARN_ENDOFDATA      = 0x0400,
    IUALG_WARN_PLAYCOMPLETED  = 0x0500,
    IUALG_ERR_BAD_HANDLE      = 0x0F00,
    IUALG_ERR_DATA_CORRUPT    = 0x0F01,
    IUALG_ERR_NOT_SUPPORTED   = 0x0F02,
    IUALG_ERR_ARGUMENT        = 0x0F03,
    IUALG_ERR_NOT_READY       = 0x0F04,
    IUALG_ERR_GENERAL         = 0x0FFF
}IUALG_Event;

typedef enum
{
    USN_STRMCMD_PLAY,
    USN_STRMCMD_PAUSE,
    USN_STRMCMD_STOP,
    USN_STRMCMD_SETCODECPARAMS,
    USN_STRMCMD_IDLE,
    USN_STRMCMD_FLUSH
}USN_StrmCmd;

/* XDM supported frame types for video */
typedef enum
{
    VIDDEC_I_FRAME =0,
    VIDDEC_P_FRAME,
    VIDDEC_B_FRAME,
    VIDDEC_IDR_FRAME
} VIDDEC_FrameType;

/*
 *  Extended error enumeration for XDM compliant encoders and decoders:
 *
 *  Bit 16-32 : Reserved
 *  Bit XDM_FATALERROR:       1 => Fatal error (Stop decoding)
 *                            0 => Recoverable error
 *  Bit XDM_UNSUPPORTEDPARAM: 1 => Unsupported input parameter or configuration
 *                            0 => Ignore
 *  Bit XDM_UNSUPPORTEDINPUT: 1 => Unsupported feature/ parameter in input,
 *                            0 => Ignore
 *  Bit XDM_CORRUPTEDHEADER:  1 => Header problem/ corruption,
 *                            0 => Ignore
 *  Bit XDM_CORRUPTEDDATA:    1 => Data problem /corruption,
 *                            0 => Ignore
 *  Bit XDM_INSUFFICIENTDATA: 1 => Insufficient data,
 *                            0 => Ignore
 *  Bit  XDM_APPLIEDCONCEALMENT:  1 => Applied concelement,
 *                            0=> Ignore
 *  Bit  8 :    Reserved
 *  Bit 7-0 :   Codec & Implementation specific
 *
 *  Notes :
 *  =====
 *  1) Algorithm will set multiple bits to 1 based on conditions.
 *     e.g. it will set bits XDM_FATALERROR (fatal) and XDM_UNSUPPORTEDPARAM
 *     (unsupported params) in case of unsupported run time parameters
 *  2) Some erors are applicable to decoders only.
 */
typedef enum
{
    VIDDEC_XDM_APPLIEDCONCEALMENT = 9,
    VIDDEC_XDM_INSUFFICIENTDATA   = 10,
    VIDDEC_XDM_CORRUPTEDDATA      = 11,
    VIDDEC_XDM_CORRUPTEDHEADER    = 12,
    VIDDEC_XDM_UNSUPPORTEDINPUT   = 13,
    VIDDEC_XDM_UNSUPPORTEDPARAM   = 14,
    VIDDEC_XDM_FATALERROR         = 15
} VIDDEC_XDM_ErrorBit;

/*****************************************************************************/
/* Structures                                                                 */
/*****************************************************************************/


static const struct DSP_UUID USN_UUID =
{
    0x79A3C8B3, 0x95F2, 0x403F, 0x9A, 0x4B,
    {0xCF, 0x80, 0x57, 0x73, 0x05, 0x41}
};

static const struct DSP_UUID RINGIO_TI_UUID =
{
    0x47698bfb, 0xa7ee, 0x417e, 0xa6, 0x7a,
    {0x41, 0xc0, 0x27, 0x9e, 0xb8, 0x05}
};




/* This contains the first elements fo _SNCreatePhArg for each codec -        */
/* Each codec should use the elements below as first elements in input param  */
/* structure definitions. Before trying to extend the create time parameters, */
/* check the size limiation in LCML for create params                         */
typedef struct
{
    OMX_U16 unNumOfStreams;
    OMX_U16 unInputStreamID;
    OMX_U16 unInputBufferType;
    OMX_U16 unlInputNumBufsPerStream;
    OMX_U16 unOutputStreamID;
    OMX_U16 unOutputBufferType;
    OMX_U16 unOutputNumBufsPerStream;
    OMX_U16 unReserved;
    OMX_U32 ulMaxWidth;
    OMX_U32 ulMaxHeight;
    OMX_U32 ulYUVFormat;
    OMX_U32 ulMaxFrameRate;
    OMX_U32 ulMaxBitRate;
    OMX_U32 ulDataEndianness;
    OMX_U32 ulProfile;
    OMX_S32 ulMaxLevel;
    OMX_U32 ulProcessMode;
    OMX_U32 ulDeblock;
    OMX_U32 ulDisplayWidth;
    OMX_U16 endArgs;
}ITTVDEC_SNCreatePhArg;

/* This contains the first elements fo UALGInputParam for each codec - */
/* Each codec should use the elements below as first elements in       */
/* input param structure definitions                                   */
typedef struct
{
    int  bLastBuffer; /* Set to 1 if buffer is last buffer */
    int  i4_num_im_bufs;
    int  ai4_im_buf_size[MAX_NUMBER_OF_IM_BUFS];
    int  i4_profile_enable;
    int  i4_checksum_enable;
}ITTVDEC_UALGInputParam;


/* This contains the first elements fo UALGOutputParam for each codec - */
/* Each codec should use the elements below as first elements in input  */
/* param structure definitions                                          */
typedef struct
{
    OMX_U32 ulDisplayID;
    OMX_U32 uBytesConsumed;
    OMX_S32 iErrorCode;
    OMX_S32 dspProcessTime;
    OMX_S32 dspExtendedError;
    OMX_U32 ulDecodedFrameType;
    OMX_S32 ulLastFrame;
    OMX_S32 framewidth;
    OMX_S32 frameheight;
    OMX_S32 framepitch;
    OMX_S32 checksum[4];
}ITTVDEC_UALGOutputParam;

typedef struct
{
    ITTVDEC_UALGInputParam UALGInputParam; /* Common structure containing     */
										   /* first few elements              */
    WMV9Alg_context        sIttAlgContext; /* Used to send Codec context info */
}WMV9VDEC_UALGInputParam;

typedef struct WMV9DEC_SNCreatePhArg
{
    ITTVDEC_SNCreatePhArg create_params;
} WMV9VDEC_SNCreatePhArg;

typedef struct
{
    ITTVDEC_UALGOutputParam out_params;
} WMV9VDEC_UALGOutputParam;

typedef struct
{
    ITTVDEC_UALGInputParam UALGInputParam; /* Common structure containing     */
										   /* first few elements              */
    Mpeg4Alg_context       sIttAlgContext; /* Used to send Codec context info */
    int  				   i4_export_qp_map;
}MPEG4VDEC_UALGInputParam;

typedef struct
{
    ITTVDEC_SNCreatePhArg create_params;
} MPEG4VDEC_SNCreatePhArg;


typedef struct
{
    ITTVDEC_UALGOutputParam out_params;
    OMX_S32 				picSignalType;
    OMX_U8 				    u1_qpmap[3652];
} MPEG4VDEC_UALGOutputParam;

typedef struct
{
    ITTVDEC_UALGInputParam UALGInputParam; /* Common structure containing     */
										   /* first few elements              */
    H264Alg_context        sIttAlgContext; /* Used to send Codec context info */
}H264VDEC_UALGInputParam;


typedef struct H264VDEC_SNCreatePhArg
{
    ITTVDEC_SNCreatePhArg create_params;
} H264VDEC_SNCreatePhArg;

typedef struct
{
    ITTVDEC_UALGOutputParam out_params;
} H264VDEC_UALGOutputParam;

/* Used for computing the maximum size of input structures */
/* Hence a union                                           */
typedef union
{
    MPEG4VDEC_UALGInputParam    mp4_input;
    WMV9VDEC_UALGInputParam     wmv9_input;
    H264VDEC_UALGInputParam     h264_input;
}ITTVDEC_UALGInputParam_t;

/* Used for computing the maximum size of input structures */
/* Hence a union                                           */
typedef union
{
    MPEG4VDEC_UALGOutputParam    mp4_output;
    WMV9VDEC_UALGOutputParam     wmv9_output;
    H264VDEC_UALGOutputParam     h264_output;
}ITTVDEC_UALGOutputParam_t;






#endif /* OMX_VIDDEC_DSP__H */

