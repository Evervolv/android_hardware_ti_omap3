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
/*  File Name        : ia_mp3_dec_error_codes.h                              */
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

#ifndef __IA_MP3DEC_ERROR_CODES_H__
#define __IA_MP3DEC_ERROR_CODES_H__

/*****************************************************************************/
/* Constant hashdefines                                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Ittiam mp3_dec ErrorCode Definitions                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Class 0: API Errors                                                       */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_MP3DEC_API_NONFATAL_NO_ERROR							0x00000000
#define IA_MP3DEC_API_NONFATAL_CMD_NOT_SUPPORTED				0x00000001
#define IA_MP3DEC_API_NONFATAL_CMD_TYPE_NOT_SUPPORTED			0x00000002
/* Fatal Errors */
#define IA_MP3DEC_API_FATAL_INVALID_MEMTAB_INDEX				0xFFFF8000
#define IA_MP3DEC_API_FATAL_INVALID_LIB_ID_STRINGS_IDX			0xFFFF8001
#define IA_MP3DEC_API_FATAL_MEM_ALLOC							0xFFFF8002
#define IA_MP3DEC_API_FATAL_INVALID_LIB_PROCESS					0xFFFF8003
#define IA_MP3DEC_API_FATAL_INVALID_CONFIG_PARAM				0xFFFF8004
#define IA_MP3DEC_API_FATAL_INVALID_EXECUTE_TYPE				0xFFFF8005
#define IA_MP3DEC_API_FATAL_INVALID_CMD							0xFFFF8006
#define IA_MP3DEC_API_FATAL_RESUME_NOT_SUPPORTED				0xFFFF8007
#define IA_MP3DEC_API_FATAL_MEM_ALIGN							0xFFFF8008
/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_MP3DEC_CONFIG_NONFATAL_MP3_PCM_ADJUST_16				0x00000800
#define IA_MP3DEC_CONFIG_NONFATAL_MP3_PCM_ADJUST_24				0x00000801
/* Fatal Errors */
/*****************************************************************************/
/* Class 2: Execution Errors                                                 */
/*****************************************************************************/
/* Non Fatal Errors */
#define IA_MP3DEC_EXECUTE_NONFATAL_NEED_MORE					0x00001000
#define IA_MP3DEC_EXECUTE_NONFATAL_CANNOT_REWIND				0x00001001
#define IA_MP3DEC_EXECUTE_NONFATAL_CHANGED_CHANNELS				0x00001002
#define IA_MP3DEC_EXECUTE_NONFATAL_CHANGED_LAYER				0x00001003
#define IA_MP3DEC_EXECUTE_NONFATAL_CHANGED_SAMP_FREQ			0x00001004
#define IA_MP3DEC_EXECUTE_NONFATAL_NEXT_SYNC_NOT_FOUND			0x00001005
#define IA_MP3DEC_EXECUTE_NONFATAL_CRC_CHECK_FAILED 			0x00001006
#define IA_MP3DEC_EXECUTE_NONFATAL_INVALID_BIT_STREAM_ELEMENT   0x00001007
/* Fatal Errors */
#define IA_MP3DEC_EXECUTE_FATAL_MPEG1_NOT_SUPPORTED				0xFFFF9000
#define IA_MP3DEC_EXECUTE_FATAL_UNSUPPORTED_LAYER				0xFFFF9001
#define IA_MP3DEC_EXECUTE_FATAL_OVERLOADED_IN_BUF				0xFFFF9002
#define IA_MP3DEC_EXECUTE_FATAL_STREAM_ERROR					0xFFFF9003
#define IA_MP3DEC_EXECUTE_FATAL_OUTPUT_BUFFER_INSUFFICIENT		0xFFFF9004

#endif /* __IA_MP3DEC_ERROR_CODES_H__ */
