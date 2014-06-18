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

#ifndef OMX_VIDEOENC_CUSTOMCMD_H
#define OMX_VIDEOENC_CUSTOMCMD_H

#define VIDENC_PARAM_VBVSIZE               "OMX.TI.VideoEncode.Param.VBVSize";
#define VIDENC_PARAM_DEBLOCK_FILTER        "OMX.TI.VideoEncode.Param.DeblockFilter";
#define VIDENC_CONFIG_FORCE_I_FRAME        "OMX.TI.VideoEncode.Config.ForceIFrame";
#define VIDENC_CONFIG_INTRA_FRAME_INTERVAL "OMX.TI.VideoEncode.Config.IntraFrameInterval";
#define VIDENC_CONFIG_TARGET_FRAMERATE     "OMX.TI.VideoEncode.Config.TargetFrameRate";
#define VIDENC_CONFIG_QPI                  "OMX.TI.VideoEncode.Config.QPI";
#define VIDENC_CONFIG_AIRRATE              "OMX.TI.VideoEncode.Config.AIRRate";
#define VIDENC_CONFIG_TARGET_BITRATE       "OMX.TI.VideoEncode.Config.TargetBitRate";

#endif /* OMX_VIDEOENC_CUSTOMCMD_H */
