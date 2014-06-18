/*****************************************************************************/
/*                                                                           */
/*                        Enhanced aacPlus Decoder                           */
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
/*  File Name        : ia_enhaacplus_dec_error_codes.h                       */
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

#ifndef __IA_ENHAACPLUS_DEC_ERROR_CODES_H__
#define __IA_ENHAACPLUS_DEC_ERROR_CODES_H__

/*****************************************************************************/
/* Constant hashdefines                                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Ittiam enhaacplus_dec ErrorCode Definitions                               */
/*****************************************************************************/

/*****************************************************************************/
/* Class 0: API Errors                                                       */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_ENHAACPLUS_DEC_API_NONFATAL_NO_ERROR                 0x00000000
#define IA_ENHAACPLUS_DEC_API_NONFATAL_CMD_NOT_SUPPORTED        0x00000001
#define IA_ENHAACPLUS_DEC_API_NONFATAL_CMD_TYPE_NOT_SUPPORTED   0x00000002
/* Fatal Errors */
#define IA_ENHAACPLUS_DEC_API_FATAL_INVALID_MEMTAB_INDEX        0xFFFF8000
#define IA_ENHAACPLUS_DEC_API_FATAL_INVALID_LIB_ID_STRINGS_IDX  0xFFFF8001
#define IA_ENHAACPLUS_DEC_API_FATAL_MEM_ALLOC                   0xFFFF8002
#define IA_ENHAACPLUS_DEC_API_FATAL_INVALID_CONFIG_PARAM        0xFFFF8003
#define IA_ENHAACPLUS_DEC_API_FATAL_INVALID_EXECUTE_TYPE        0xFFFF8004
#define IA_ENHAACPLUS_DEC_API_FATAL_INVALID_CMD                 0xFFFF8005
#define IA_ENHAACPLUS_DEC_API_FATAL_MEM_ALIGN                   0xFFFF8006
/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_PCM_WDSZ      0x00000800
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_DOWNMIX       0x00000801
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_OUT08KHZ      0x00000802
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_OUT16KHZ      0x00000803
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_TOSTEREO      0x00000804
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_DSAMPLE       0x00000805
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_FRAMEOK       0x00000806
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_MP4FLAG       0x00000807
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_MAX_CHANNEL   0x00000808
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_COUP_CHANNEL  0x00000809
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_FEATURE_NOT_SUPPORTED 0x0000080A
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_SYNCFLAG      0x0000080B
#define IA_ENHAACPLUS_DEC_CONFIG_NONFATAL_INVALID_SBRUPFLAG     0x0000080C
/* Fatal Errors */
#define IA_ENHAACPLUS_DEC_CONFIG_FATAL_INVALID_SAMPLE_RATE      0xFFFF8800
/*****************************************************************************/
/* Class 2: Initialization Errors                                            */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_ENHAACPLUS_DEC_INIT_NONFATAL_BOTH_16AND08OUT         0x00001000
#define IA_ENHAACPLUS_DEC_INIT_NONFATAL_NO_UPSAMPLING           0x00001001
#define IA_ENHAACPLUS_DEC_INIT_NONFATAL_HEADER_NOT_AT_START     0x00001002
/* Fatal Errors */
#define IA_ENHAACPLUS_DEC_INIT_FATAL_DEC_INIT_FAIL              0xFFFF9000
#define IA_ENHAACPLUS_DEC_INIT_FATAL_EO_INPUT_REACHED           0xFFFF9001
#define IA_ENHAACPLUS_DEC_INIT_FATAL_STREAM_CHAN_GT_MAX         0xFFFF9002
/*****************************************************************************/
/* Class 3: Execution Errors                                                 */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_ENHAACPLUS_DEC_EXE_NONFATAL_ADTS_SYNC_LOST			0x00001800
#define IA_ENHAACPLUS_DEC_EXE_NONFATAL_SBR_TURNED_OFF			0x00001801
#define IA_ENHAACPLUS_DEC_EXE_NONFATAL_SBR_TURNED_ON			0x00001802
#define IA_ENHAACPLUS_DEC_EXE_NONFATAL_ADTS_HDR_CRC_FAIL		0x00001803
#define IA_ENHAACPLUS_DEC_EXE_NONFATAL_INSUFFICIENT_INPUT_BYTES 0x00001804


/* Fatal Errors */
#define IA_ENHAACPLUS_DEC_EXE_FATAL_DECODE_FRAME_ERROR			0xFFFF9800
#define IA_ENHAACPLUS_DEC_EXE_FATAL_INVALID_CODE_BOOK			0xFFFF9801
#define IA_ENHAACPLUS_DEC_EXE_FATAL_PREDICTION_DATA_PRESENT		0xFFFF9802
#define IA_ENHAACPLUS_DEC_EXE_FATAL_UNIMPLEMENTED_CCE			0xFFFF9803
//#define IA_ENHAACPLUS_DEC_EXE_FATAL_UNIMPLEMENTED_PCE			0xFFFF9804
//#define IA_ENHAACPLUS_DEC_EXE_FATAL_UNIMPLEMENTED_LFE			0xFFFF9805
#define IA_ENHAACPLUS_DEC_EXE_FATAL_GAIN_CONTROL_DATA_PRESENT	0xFFFF9804
#define IA_ENHAACPLUS_DEC_EXE_FATAL_TNS_RANGE_ERROR				0xFFFF9805
#define IA_ENHAACPLUS_DEC_EXE_FATAL_TNS_ORDER_ERROR				0xFFFF9806

#define IA_ENHAACPLUS_DEC_EXE_FATAL_ELE_INSTANCE_TAG_NOT_FOUND	0xFFFF9807
#define IA_ENHAACPLUS_DEC_EXE_FATAL_EXCEEDS_SFB_TRANSMITTED     0xFFFF9808
#define IA_ENHAACPLUS_DEC_EXE_FATAL_EXCEEDS_MAX_HUFFDEC_VAL     0xFFFF9809
#define IA_ENHAACPLUS_DEC_EXE_FATAL_CHANGED_ADTS_SF             0xFFFF980A

#endif /* __IA_ENHAACPLUS_DEC_ERROR_CODES_H__ */
