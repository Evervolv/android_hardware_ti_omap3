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

#if WIN32

#if     ENC_CAL_XDM_DLL_EXPORTS
#define ENC_CAL_DLL_API __declspec(dllexport)
#else
#define ENC_CAL_DLL_API __declspec(dllimport)
#endif

#else  /* #ifdef  WIN32 */
#define ENC_CAL_DLL_API extern
#endif /* #ifdef  WIN32 */

ENC_CAL_DLL_API IVIDENC1_Fxns IVIDENC1_MPEG4_ENC_SPLIT_FXNS;

ENC_CAL_DLL_API IVIDENC1_Fxns IMPEG4_ENC_API;

ENC_CAL_DLL_API XDAS_Int32 spltenc_atci_dsp_trig(IVIDENC1_Handle handle,
                                IVIDEO1_BufDescIn *inBufs,
                                XDM_BufDesc *outBufs,
                                IVIDENC1_InArgs *inArgs,
                                IVIDENC1_OutArgs *outArgs,
                                WORD32 i4_state);
/*{
    usleep(3000);
    return 0;
}*/

ENC_CAL_DLL_API XDAS_Int32 spltenc_atci_arm_process(IVIDENC1_Handle handle, WORD32 i4_state);
/*{
    usleep(20000);
    return 0;
}*/


ENC_CAL_DLL_API XDAS_Int32 spltenc_atci_dsp_wait(IVIDENC1_Handle handle, WORD32 i4_state);
/*{
    usleep(47000);
    return 0;
}*/

/* Following functions are exported by H264 encoder library */
ENC_CAL_DLL_API IVIDENC1_Fxns IVIDENC1_H264_ENC_SPLIT_FXNS;

ENC_CAL_DLL_API IVIDENC1_Fxns IH264_ENC_API;

ENC_CAL_DLL_API XDAS_Int32 h264enc_atci_dsp_trig(IVIDENC1_Handle handle,
                                IVIDEO1_BufDescIn *inBufs,
                                XDM_BufDesc *outBufs,
                                IVIDENC1_InArgs *inArgs,
                                IVIDENC1_OutArgs *outArgs,
                                WORD32 i4_state);

ENC_CAL_DLL_API XDAS_Int32 h264enc_atci_arm_process(IVIDENC1_Handle handle, WORD32 i4_state);

ENC_CAL_DLL_API XDAS_Int32 h264enc_atci_dsp_wait(IVIDENC1_Handle handle, WORD32 i4_state);