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

#ifndef __IA_ENHAACPLUSENC_ERROR_CODES_H__
#define __IA_ENHAACPLUSENC_ERROR_CODES_H__

/*****************************************************************************/
/* Constant hashdefines                                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Ittiam enhaacplus_enc ErrorCode Definitions                             */
/*****************************************************************************/

/*****************************************************************************/
/* Class 0: API Errors                                                       */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_ENHAACPLUSENC_API_NONFATAL_NO_ERROR					0x00000000
#define IA_ENHAACPLUSENC_API_NONFATAL_CMD_NOT_SUPPORTED			0x00000001
#define IA_ENHAACPLUSENC_API_NONFATAL_CMD_TYPE_NOT_SUPPORTED	0x00000002
#define IA_ENHAACPLUSENC_API_NONFATAL_AACPLUS_NOT_AVAIL			0x00000003
#define IA_ENHAACPLUSENC_API_NONFATAL_CRC_DEACTIVATED			0x00000004
/* Fatal Errors */
#define IA_ENHAACPLUSENC_API_FATAL_INVALID_MEMTAB_INDEX			0xFFFF8000
#define IA_ENHAACPLUSENC_API_FATAL_INVALID_LIB_ID_STRINGS_IDX	0xFFFF8001
#define IA_ENHAACPLUSENC_API_FATAL_MEM_ALLOC					0xFFFF8002
#define IA_ENHAACPLUSENC_API_FATAL_INVALID_CONFIG_PARAM			0xFFFF8003
#define IA_ENHAACPLUSENC_API_FATAL_INVALID_EXECUTE_TYPE			0xFFFF8004
#define IA_ENHAACPLUSENC_API_FATAL_INVALID_CMD					0xFFFF8005
#define IA_ENHAACPLUSENC_API_FATAL_MEM_ALIGN					0xFFFF8006
#define IA_ENHAACPLUSENC_API_FATAL_DNSAMP_INIT_FAILED			0xFFFF8007
#define IA_ENHAACPLUSENC_API_FATAL_AAC_INIT_FAILED				0xFFFF8008
/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_ENHAACPLUSENC_CONFIG_NONFATAL_PNS_NOT_ENABLED		0x00000800
/* Fatal Errors */
#define IA_ENHAACPLUSENC_CONFIG_FATAL_SAMP_FREQ				    0xFFFF8800
#define IA_ENHAACPLUSENC_CONFIG_FATAL_NUM_CHANNELS			    0xFFFF8801
#define IA_ENHAACPLUSENC_CONFIG_FATAL_BITRATE					0xFFFF8802
#define IA_ENHAACPLUSENC_CONFIG_FATAL_CHMODE					0xFFFF8803
#define IA_ENHAACPLUSENC_CONFIG_FATAL_AAC_CLASSIC				0xFFFF8804
#define IA_ENHAACPLUSENC_CONFIG_FATAL_USE_SPEECH_CONF			0xFFFF8805
#define IA_ENHAACPLUSENC_CONFIG_FATAL_PCM_WDSZ				    0xFFFF8806
#define IA_ENHAACPLUSENC_CONFIG_FATAL_USE_ADIF				    0xFFFF8807
#define IA_ENHAACPLUSENC_CONFIG_FATAL_USE_ADTS					0xFFFF8808
#define IA_ENHAACPLUSENC_CONFIG_FATAL_USE_STEREO_PRE_PRO		0xFFFF8809
#define IA_ENHAACPLUSENC_CONFIG_FATAL_QUALITY_LEVEL			    0xFFFF880A
#define IA_ENHAACPLUSENC_CONFIG_FATAL_USE_TNS					0xFFFF880B
#define IA_ENHAACPLUSENC_CONFIG_FATAL_AAC_CLASSIC_WITH_PS       0xFFFF880C
#define IA_ENHAACPLUSENC_CONFIG_FATAL_CHANNELS_MASK			    0xFFFF880D
#define IA_ENHAACPLUSENC_CONFIG_FATAL_AAC_LC_WITH_SBR           0xFFFF880E
#define IA_ENHAACPLUSENC_CONFIG_FATAL_HEAAC_WITH_PS             0xFFFF880F
#define IA_ENHAACPLUSENC_CONFIG_FATAL_WRITE_PCE	                0xFFFF8810
#define IA_ENHAACPLUSENC_CONFIG_FATAL_USE_CRC					0xFFFF8811

#ifdef MPEG_SURROUND
#define IA_MPSAACPLUSENC_CONFIG_FATAL_TREE_CONFIG				0xFFFF8812
#define IA_MPSAACPLUSENC_CONFIG_FATAL_EMBED_MODE				0xFFFF8813
#define IA_MPSAACPLUSENC_CONFIG_FATAL_NUM_CHANNELS				0xFFFF8814
#endif

#define IA_ENHAACPLUSENC_CONFIG_FATAL_INVALID_ENC_MODE			0xFFFF8815
#define IA_ENHAACPLUSENC_CONFIG_FATAL_INVALID_INPUT_FORMAT		0xFFFF8816
#define IA_ENHAACPLUSENC_CONFIG_FATAL_INVALID_PCM_WIDTH			0xFFFF8817

#endif /* __IA_ENHAACPLUSENC_ERROR_CODES_H__ */
