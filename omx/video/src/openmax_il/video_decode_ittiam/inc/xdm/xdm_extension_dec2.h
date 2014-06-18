/*****************************************************************************/
/*                                                                           */
/*                      Split Codec Sample Application                       */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2005                             */
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
/*  File Name         : xdm_extension_dec2.h                                 */
/*                                                                           */
/*  Description       : This file contains all the structures that are       */
/*                      extended over viddec2 structures			         */
/*                                                                           */
/*  List of Functions :											             */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         22 04 2008   Prashanth       Draft                                */
/*                                                                           */
/*****************************************************************************/

#ifndef ITT_IMOD_DEC2_H
#define ITT_IMOD_DEC2_H



/*****************************************************************************/
/* Structure                                                                 */
/*****************************************************************************/

/* IMOD_Fxns for the viddec2 api*/
typedef struct ITTVIDDEC2_IMOD_Obj *ITTVIDDEC2_IMOD_Handle;

typedef struct ITTVIDDEC2_IMOD_Obj {
    struct ITTVIDDEC2_IMOD_Fxns  *fxns;

} ITTVIDDEC2_IMOD_Obj;

typedef struct ITTVIDDEC2_IMOD_Fxns {
    IVIDDEC2_Fxns s_ividdec2_fxns;

} ITTVIDDEC2_IMOD_Fxns_t;

/* IMOD_Fxns for the viddec2 api of the front end component*/
typedef struct ITTVIDDEC2FRONT_IMOD_Obj *ITTVIDDEC2FRONT_IMOD_Handle;

typedef struct ITTVIDDEC2FRONT_IMOD_Obj {
    struct ITTVIDDEC2FRONT_IMOD_Fxns  *fxns;

} ITTVIDDEC2FRONT_IMOD_Obj;

typedef struct ITTVIDDEC2FRONT_IMOD_Fxns {
    IVIDDEC2FRONT_Fxns s_ividdec2_fxns;

} ITTVIDDEC2FRONT_IMOD_Fxns_t;

/* IMOD_Fxns for the viddec2 api of the back end component*/
typedef struct ITTVIDDEC2BACK_IMOD_Obj *ITTVIDDEC2BACK_IMOD_Handle;

typedef struct ITTVIDDEC2BACK_IMOD_Obj {
    struct ITTVIDDEC2BACK_IMOD_Fxns  *fxns;

} ITTVIDDEC2BACK_IMOD_Obj;

typedef struct ITTVIDDEC2BACK_IMOD_Fxns {
    IVIDDEC2BACK_Fxns s_ividdec2_fxns;

} ITTVIDDEC2BACK_IMOD_Fxns_t;

/*IMOD_Params */
typedef struct ITTVIDDEC2_IMOD_Params {
    IVIDDEC2_Params              s_ividdec2_params;
    WORD32						 post_proc_deblocking;
	WORD32						 i4_profile;
} ITTVIDDEC2_IMOD_Params_t;

/*IMOD_DynParams */
typedef struct ITTVIDDEC2_IMOD_DynamicParams {
    IVIDDEC2_DynamicParams       s_ividdec2_dyn_params;
} ITTVIDDEC2_IMOD_DynamicParams_t;

/* IMOD In Args */
typedef struct ITTVIDDEC2_IMOD_InArgs
{
    IVIDDEC2_InArgs  s_ividdec2_inargs;
} ITTVIDDEC2_IMOD_InArgs_t;

/*Pixel Aspect ratio required by the codecs */
typedef struct pixel_aspect_ratio2
{
	WORD32 numerator;
	WORD32 denominator;
}pixel_aspect_ratio2_t;

/* IMOD_FullStatus */
typedef struct ITTVIDDEC2_IMOD_Status {
    IVIDDEC2_Status              s_ividdec2_status;
} ITTVIDDEC2_IMOD_Status_t;

/*IMOD_Front status */
typedef struct ITTVIDDEC2FRONT_IMOD_Status {
    IVIDDEC2FRONT_Status         s_ividdec2_front_status;
	pixel_aspect_ratio2_t        s_pxl_aspect_ratio;
} ITTVIDDEC2FRONT_IMOD_Status_t;

/* IMOD Out Args */
typedef struct ITTVIDDEC2_IMOD_OutArgs
{
    IVIDDEC2_OutArgs	s_ividdec2_outargs;
	UWORD32				ResChangeFlag;

} ITTVIDDEC2_IMOD_OutArgs_t;

/* IMOD Front Out Args */
typedef struct ITTVIDDEC2FRONT_IMOD_OutArgs
{
    IVIDDEC2FRONT_OutArgs	s_ividdec2_front_outargs;
	WORD32					i4_imbufs_inuseflag;
} ITTVIDDEC2FRONT_IMOD_OutArgs_t;

#endif /* ITT_IMOD_DEC2_H */

