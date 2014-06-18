/*
 *****************************************************************************
 *                    Ittiam Video Decoder OMX Plug-in
 *                  ITTIAM SYSTEMS PVT LTD, BANGALORE
 *                           COPYRIGHT(C) 2009
 *  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and
 *  is protected under Indian  Copyright Law as an unpublished work. Its use
 *  and  disclosure  is  limited by  the terms  and  conditions of a license
 *  agreement. It may not be copied or otherwise  reproduced or disclosed to
 *  persons outside the licensee's organization except in accordance with the
 *  terms  and  conditions   of  such  an  agreement.  All  copies  and
 *  reproductions shall be the property of Ittiam Systems Private Limited and
 *  must bear this notice in its entirety.
 *
 *****************************************************************************
 */
/**
 *****************************************************************************
 *
 *  @file     OMX_IttiamVidDec_CustomCmd.h
 *
 *  @brief
 *
 *****************************************************************************
 */


#ifndef OMX_VIDDEC_CUSTOMCMD_H
#define OMX_VIDDEC_CUSTOMCMD_H
#define VIDDEC_CUSTOMPARAM_PROCESSMODE "OMX.TI.VideoDecode.Param.ProcessMode"
#define VIDDEC_CUSTOMPARAM_H264BITSTREAMFORMAT "OMX.TI.VideoDecode.Param.H264BitStreamFormat"
#define VIDDEC_CUSTOMPARAM_WMVPROFILE "OMX.TI.VideoDecode.Param.WMVProfile"
#define VIDDEC_CUSTOMPARAM_WMVFILETYPE "OMX.TI.VideoDecode.Param.WMVFileType"
#define VIDDEC_CUSTOMPARAM_PARSERENABLED "OMX.TI.VideoDecode.Param.ParserEnabled"
#define VIDDEC_CUSTOMPARAM_ISNALBIGENDIAN "OMX.TI.VideoDecode.Param.IsNALBigEndian"
#define VIDDEC_CUSTOMCONFIG_DEBUG "OMX.TI.VideoDecode.Debug"

#define VIDDEC_CUSTOMCONFIG_CACHEABLEBUFFERS "OMX.TI.VideoDecode.CacheableBuffers"
#define VIDDEC_ENABLE_ANDROID_NATIVE_BUFFERS "OMX.google.android.index.enableAndroidNativeBuffers"
#define VIDDEC_GET_ANDROID_NATIVE_BUFFER_USAGE "OMX.google.android.index.getAndroidNativeBufferUsage"
#define VIDDEC_ANDROID_USE_ANDROID_NATIVE_BUFFER2 "OMX.google.android.index.useAndroidNativeBuffer2"

#define VIDDEC_CUSTOMPARAM_BUSEFLAGGEDEOS "OMX.TI.VideoDecode.Param.bUseFlaggedEos"



/*------- Program Header Files ----------------------------------------*/
#include <OMX_Component.h>

/*------- Structures ----------------------------------------*/

#endif /* OMX_VIDDEC_CUSTOMCMD_H */
