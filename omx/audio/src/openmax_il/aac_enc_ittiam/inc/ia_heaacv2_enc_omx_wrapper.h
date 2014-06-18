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

#ifndef IA_HEAACV2_ENC_OMX_WRAPPER_H
#define IA_HEAACV2_ENC_OMX_WRAPPER_H


typedef struct ia_heaacv2_enc_state_t
{
    WORD32 size;
    ITTIAM_EAACPLUSENC_Handle handle; /* self codec handle */
    ITTIAM_EAACPLUSENC_Params  params;
    ITTIAM_EAACPLUSENC_InArgs inArgs;
    ITTIAM_EAACPLUSENC_OutArgs outArgs;
    ITTIAM_EAACPLUSENC_DynamicParams dynParams;
    ITTIAM_EAACPLUSENC_Status   status;
    XDM1_BufDesc inBuf;
    XDM1_BufDesc outBuf;

} ia_heaacv2_enc_state_t;


#endif /* IA_HEAACV2_ENC_OMX_WRAPPER_H */

