/*****************************************************************************/
/*                                                                           */
/*                          BSAC Decoder 	                                 */
/*                                                                           */
/*                   ITTIAM SYSTEMS PVT LTD, BANGALORE                       */
/*                          COPYRIGHT(C) 2009                                */
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
/*  File Name        : IttiamOMX.h                                           */
/*                                                                           */
/*  Description      :                                                       */
/*                                                                           */
/*  List of Functions: None                                                  */
/*                                                                           */
/*  Issues / Problems: None                                                  */
/*                                                                           */
/*  Revision History :                                                       */
/*                                                                           */
/*****************************************************************************/

#ifndef ITTIAMOMX1__H
#define ITTIAMOMX1__H



typedef struct OMX_AUDIO_PARAM_BSACTYPE {
    OMX_U32 		nSize;                 /**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;              /**< OMX specification version information */
    OMX_U32 		nPortIndex;            /**< port that this structure applies to */
    OMX_S32      	nSampleRate;
	OMX_S32      	nChannels;
 } OMX_AUDIO_PARAM_BSACTYPE;

/** Enumeration used to define the possible audio codings.
 *  If "OMX_AUDIO_CodingUnused" is selected, the coding selection must
 *  be done in a vendor specific way.  Since this is for an audio
 *  processing element this enum is relevant.  However, for another
 *  type of component other enums would be in this area.
 */

#define OMX_AUDIO_CodingBSAC (OMX_AUDIO_CODINGTYPE)(OMX_AUDIO_CodingVendorStartUnused + 1) /**< Any variant of BSAC encoded data */
#define OMX_AUDIO_CodingWMAPRO  0x7F000001
#define OMX_AUDIO_CodingWMALSL  0x7F000001

#define OMX_IndexParamAudioBsac		(OMX_INDEXTYPE)(OMX_IndexVendorStartUnused+1)

#endif//ITTIAMOMX__H
