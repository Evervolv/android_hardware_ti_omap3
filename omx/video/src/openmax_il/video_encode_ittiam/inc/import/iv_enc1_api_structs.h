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
/*  File Name         : iv_enc1_api_structs.h                                */
/*                                                                           */
/*  Description       : Describes the extensions to XDM1.0 structures        */
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

#ifndef _IV_ENC1_API_STRUCTS_H_
#define _IV_ENC1_API_STRUCTS_H_

/*****************************************************************************/
/* API Functions                                                             */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_Fxns
{
    IVIDENC1_Fxns  s_ividenc_fxns;

} ITTVIDENC1_IMOD_Fxns_t;


/*****************************************************************************/
/* Encoder Object                                                            */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_Obj
{
    struct ITTVIDENC1_IMOD_Fxns  *ps_fxns;

} ITTVIDENC1_IMOD_Obj;


/*****************************************************************************/
/* Handle to the Video encoder                                               */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_Obj *ITTVIDENC1_IMOD_Handle;


/*****************************************************************************/
/* Static params                                                             */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_Params
{
    IVIDENC1_Params              s_ividenc_params;
    ivea_ext_params_t            s_ext;

} ITTVIDENC1_IMOD_Params_t;


/*****************************************************************************/
/* Dynamic params                                                            */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_DynamicParams
{
    IVIDENC1_DynamicParams       s_ividenc_dyn_params;
    ivea_ext_dyn_prms_t          s_ext;

} ITTVIDENC1_IMOD_DynamicParams_t;


/*****************************************************************************/
/* Status                                                                    */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_Status
{
    IVIDENC1_Status              s_ividenc_status;

} ITTVIDENC1_IMOD_Status_t;


/*****************************************************************************/
/* InArgs                                                                    */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_InArgs
{
    IVIDENC1_InArgs              s_ividenc_inargs;
    ivea_ext_inargs_t            s_ext;

} ITTVIDENC1_IMOD_InArgs_t;


/*****************************************************************************/
/* OutArgs                                                                   */
/*****************************************************************************/
typedef struct ITTVIDENC1_IMOD_OutArgs
{
    IVIDENC1_OutArgs              s_ividenc_outargs;

} ITTVIDENC1_IMOD_OutArgs_t;


#endif /* _IV_ENC1_API_STRUCTS_H_ */


