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
/*  File Name         : mpeg4_asc_api.h                                      */
/*                                                                           */
/*  Description       : This file contains the structure definitions	     */
/*						for the toolset configuration						 */
/*                                                                           */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          17 11 2009   A.D.Almeida     Draft                               */
/*                                                                           */
/*****************************************************************************/

#ifndef MPEG4_ASC_API_H
#define MPEG4_ASC_API_H

/*****************************************************************************/
/*  Constant Macros		                                                     */
/*****************************************************************************/
#define MAX_CONTROL_CALLS 	16
#define MAX_BITSTREAM_SIZE  (1024 * 1024)

/*****************************************************************************/
/* Enums				                                                     */
/*****************************************************************************/
typedef enum
{
    MEMTAB0,
    MEMTAB1,
    ASC_MEMTABS
} MEMTABS_ENUM;

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/
typedef struct Info_Per_ControlCall
{
	IVIDDEC2_Cmd 			id;
    IVIDDEC2_DynamicParams 	s_dyn_params;
}Info_Per_ControlCall;

typedef struct ControlCallInfo
{
	XDAS_Int32 				numControlCalls;
	Info_Per_ControlCall 	s_Info_Per_ControlCall[MAX_CONTROL_CALLS];
    UWORD8                  u1_dummy[124];
}ControlCallInfo;

typedef struct dec_ctxt
{
	IVIDDEC2FRONT_Handle 	handle;
	ControlCallInfo    		*ps_ControlCallInfo;
	WORD32					i4_create_width;
	WORD32					i4_create_height;
	WORD32					i4_chroma_format;
    WORD32                  i4_num_process_after_flush;
    WORD32                  i4_got_flush;
}dec_ctxt;

typedef struct ITTAlg_context
{
    XDAS_Int32 size;
    XDAS_Int32 numBytes;
    XDAS_Int32 inputID;
	XDAS_Int32 bytesConsumed;
	XDAS_Int32 chromaFormat;
	XDAS_Int32 display_width;
	XDAS_Int32 decoded_frame_ht;
    XDAS_Int32 decoded_frame_wt;
    XDAS_Int32 extendedError;
    XDAS_Int32 vop_coding_type;
} ITTAlg_context;

#endif /* MPEG4_ASC_API_H */
