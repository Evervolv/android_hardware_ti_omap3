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
/*  File Name         : iv_enc1_extension_structs.h                          */
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

#ifndef _IV_ENC1_EXTENSION_STRUCTS_H_
#define _IV_ENC1_EXTENSION_STRUCTS_H_

/****************************************************************************/
/* ivea - Stands for: Ittiam Video Encoder Application                      */
/****************************************************************************/

/****************************************************************************/
/* Extensions to init(static) params                                        */
/****************************************************************************/
typedef struct ivea_ext_params
{
    WORD32  i4_size;
/****************************************************************************/
/* Pre-processing tools. Bits have predefined significance                  */
/* Set unused bits to 0                                                     */
/* i4_dsp_processing1:                                                      */
/*      bits 2 - 0:     3:2 pulldown presets.                               */
/*      bits 5 - 3:     deinterlace presets.                                */
/*      bits 31 - 6:    reserved. Application SHOULD set these to 0         */
/* i4_dsp_processing2:                                                      */
/*      bits 31 - 0:    reserved. Application SHOULD set these to 0         */  
/****************************************************************************/
    WORD32  i4_dsp_processing1;     /* Pre-processing tools                 */
    WORD32  i4_dsp_processing2;     /* Reserved for future extensions       */
/****************************************************************************/
/* Encoder coding tools. Bits have predefined significance                  */
/* Set unused bits to 0                                                     */
/****************************************************************************/
    WORD32  i4_coding_tools1;       /* Codec specific tools                 */
    WORD32  i4_coding_tools2;       /* Reserved for future extensions       */
/****************************************************************************/
/* Packed parameter indicating target width and height. most significant    */
/* 16 bits indicate width and least significant 16 bits indicate height     */
/****************************************************************************/
    WORD32  i4_min_tgt_dimensions_xy;
/****************************************************************************/
/* Packed parameter indicating the maximum search range value in pixels     */
/* most significant 16 bits indicate the search range [-msb16, +msb16]      */
/* in x direction and least significant 16 bits indicate the search range   */
/* [-lsb16, +lsb16] in y direction                                          */
/****************************************************************************/
    WORD32  i4_max_search_range_xy;
} ivea_ext_params_t;

/****************************************************************************/
/* Extensions to inArgs                                                     */
/****************************************************************************/
typedef struct ivea_ext_inargs
{
    WORD32  i4_size;
    WORD32  i4_delta_time_stamp;
} ivea_ext_inargs_t;


/****************************************************************************/
/* Extensions to dynamic params                                             */
/****************************************************************************/

/****************************************************************************/
/* Single element                                                           */
/****************************************************************************/
typedef union ivea_dyn_prm_elem
{
   WORD32 i4_param;
   void * pv_param;
}ivea_dyn_prm_elem_t;

/****************************************************************************/
/* Structure ivea_ext_dyn_prms_t                                            */
/* The extended parameters that go to the codec on a dyn call.              */
/* No control command can recieve more that ITT_MAX_DYN_PARAMS_TO_ENC       */
/* values in a single control call. Should suffice, eh?                     */
/****************************************************************************/
typedef struct ivea_ext_dyn_prms
{
    ivea_dyn_prm_elem_t i4_dyn_param[IVEA_MAX_DYN_PARAMS_TO_ENC];
} ivea_ext_dyn_prms_t;


/****************************************************************************/
/* One Time dynamic Parameters                                              */
/****************************************************************************/

/****************************************************************************/
/* Rate control : IVEA_SET_RC_OTP_PRMS                                      */
/****************************************************************************/

/****************************************************************************/
/*  Element             *   Description                                     */
/****************************************************************************/
/*                      *   Initial QP scale to be used for I pictures. If  */
/*  i4_init_i_qp        *   rate control is disabled this QP scale will be  */
/*                      *   used for I pictures.                            */
/****************************************************************************/
/*                      *                                                   */
/*  i4_init_p_qp        *   Initial QP scale to be used for P pictures.     */
/*                      *   If rate control is disabled this QP scale will  */
/*                      *   be used for P pictures.                         */
/****************************************************************************/
/*                      *                                                   */
/*  i4_init_b_qp        *   Initial QP scale to be used for B pictures. If  */
/*                      *   rate control is disabled this QP scale will be  */
/*                      *   used for B pictures.                            */
/****************************************************************************/
/* i4_stuffing_disabled *   1: Never add stuffing bits                      */
/*                      *   0: Add stuffing bits                            */
/****************************************************************************/
/* i4_buffer_delay      *   if (low_delay == 1) signifies the maximum       */
/*                      *   buffering delay if (low_delay == 0) signifies   */
/*                      *   the vbv delay                                   */
/*                      *   given in milli sec.                             */
/****************************************************************************/
/*  i4_limit_vbv_to...  *   if (low_delay == 0)                             */
/*                      *   Ensure (bit-rate*i4_buffer_delay <=MAX_VBV_SIZE)*/
/****************************************************************************/
typedef struct ivea_ext_rc_otp
{
    WORD32 i4_size;
    WORD32 i4_init_i_qp;
    WORD32 i4_init_p_qp;
    WORD32 i4_init_b_qp;
    WORD32 i4_stuffing_disabled;
    WORD32 i4_buffer_delay;
    WORD32 i4_limit_vbv_to_std_def_buf_size;
}ivea_ext_rc_otp_t;

/****************************************************************************/
/* Rate control : IVEA_OTP_SET_VUI_PARAMS                                      */
/****************************************************************************/

/****************************************************************************/
/*  Element             *   Description                                     */
/****************************************************************************/
/*                       */
/*  i4_enable_sei_vui_params */
/*                       */
/****************************************************************************/
/*                       */
/*  i4_aspect_ratio_x   . */
/*                       */
/*                       */
/****************************************************************************/
/*                      */
/*  i4_aspect_ratio_y   */
/*                      */
/*                      */
/****************************************************************************/
/* i4_pixel_range           */
/*                          */
/****************************************************************************/
typedef struct ivea_ext_vui_otp
{
    WORD32 i4_size;
    WORD32 i4_enable_sei_vui_params;
    WORD32 i4_aspect_ratio_x;
    WORD32 i4_aspect_ratio_y;
    WORD32 i4_pixel_range;
}ivea_ext_vui_otp_t;

/****************************************************************************/
/* Random access points : IVEA_SET_RND_ACCESS_PTS_OTP                       */
/****************************************************************************/
typedef struct ivea_ext_rnd_access_pt
{
    WORD32 i4_size;
    WORD32 i4_en_random_access_pts;
}ivea_ext_rnd_access_pt_t;

/****************************************************************************/
/* Slice level encoding prms :IVEA_SET_SLICE_ENC_API_OTP_PRMS               */
/****************************************************************************/
typedef struct ivea_ext_slice_api_prms
{
    WORD32 i4_size;
    WORD32 i4_mb_rows_per_process_api;
}ivea_ext_slice_api_prms_t;

/****************************************************************************/
/* Slice level encoding prms :IVEA_SET_SLICE_OTP_PRMS                       */
/****************************************************************************/
typedef struct ivea_ext_slice_prms
{
  WORD32 i4_size;
  WORD32 i4_slice_mode; /* 0: disabled 1: Number of bytes 2: Number of MBs */
  WORD32 i4_value;      /* 1: Value corresponding to the above mode */
}ivea_ext_slice_prms_t;

/****************************************************************************/
/* Quant matrix */
/****************************************************************************/
typedef struct ivea_quant_mat_ext_t
{
    WORD32  i4_size;
    UWORD8  u1_quant_mat[64];
}ivea_quant_mat_ext_t;

/****************************************************************************/
/* Truly Dynamic Parameters                                                 */
/****************************************************************************/

/****************************************************************************/
/* Air refresh period : IVEA_SET_AIR_REFRESH_PERIOD                         */
/****************************************************************************/

typedef struct ivea_ext_air_params
{
    WORD32  i4_size;
    WORD32  i4_air_refersh_period; /* AIR period in milli seconds */
} ivea_ext_air_params_t;


/****************************************************************************/
/* No rc, quant params : IVEA_SET_QUANT_PARAMS                              */
/****************************************************************************/

/****************************************************************************/
/*   Element        *   Description                                         */
/****************************************************************************/
/*                  *   Initial QP scale to be used for I pictures. If rate */
/*  i4_quant_i      *   control is disabled this QP scale will be used for I*/
/*                  *   pictures.                                           */
/****************************************************************************/
/*                  *   Initial QP scale to be used for P pictures. If rate */
/*  i4_quant_p      *   control is disabled this QP scale will be used for P*/
/*                  *   pictures.                                           */
/****************************************************************************/
/*                  *   Initial QP scale to be used for B pictures. If rate */
/*  i4_quant_b      *   control is disabled this QP scale will be used for B*/
/*                  *   pictures.                                           */
/****************************************************************************/

typedef struct ivea_ext_quant_params
{
    WORD32 i4_size;
    WORD32 i4_quant_i;
    WORD32 i4_quant_p;
    WORD32 i4_quant_b;
} ivea_ext_quant_params_t;


/****************************************************************************/
/* CBR params : IVEA_SET_CBR_PARAMS                                         */
/****************************************************************************/

/****************************************************************************/
/* Element          *   Description                                         */
/****************************************************************************/
/* i4_min_I_qp      *   Minimum QP scale to use on an I picture             */
/****************************************************************************/
/* i4_max_I_qp      *   Maximum QP scale to use on an I picture             */
/****************************************************************************/
/* i4_min_P_qp      *   Minimum QP scale to use on an P picture             */
/****************************************************************************/
/* i4_max_P_qp      *   Maximum QP scale to use on an P picture             */
/****************************************************************************/
/* i4_min_B_qp      *   Minimum QP scale to use on an B picture             */
/****************************************************************************/
/* i4_max_B_qp      *   Maximum QP scale to use on an B picture             */
/****************************************************************************/

typedef struct ivea_ext_cbr_params
{
    WORD32 i4_size;
    WORD32 i4_min_I_qp;
    WORD32 i4_max_I_qp;
    WORD32 i4_min_P_qp;
    WORD32 i4_max_P_qp;
    WORD32 i4_min_B_qp;
    WORD32 i4_max_B_qp;
} ivea_ext_cbr_params_t;


/****************************************************************************/
/* VBR params : IVEA_SET_VBR_PARAMS                                        */
/****************************************************************************/

/****************************************************************************/
/* Element          *   Description                                         */
/****************************************************************************/
/* i4_min_I_qp      *   Minimum QP scale to use on an I picture             */
/****************************************************************************/
/* i4_max_I_qp      *   Maximum QP scale to use on an I picture             */
/****************************************************************************/
/* i4_min_P_qp      *   Minimum QP scale to use on an P picture             */
/****************************************************************************/
/* i4_max_P_qp      *   Maximum QP scale to use on an P picture             */
/****************************************************************************/
/* i4_min_B_qp      *   Minimum QP scale to use on an B picture             */
/****************************************************************************/
/* i4_max_B_qp      *   Maximum QP scale to use on an B picture             */
/****************************************************************************/
/* i4_avg_channel_  *   Average channel bandwidth always guaranteed by the  */
/* bandwidth        *   application                                         */
/****************************************************************************/
/* i4_peak_channel_ *   Peak channel bandwidth available to drain the I     */
/* bandwidth        *   picture bits                                        */
/****************************************************************************/

typedef struct ivea_vbr_params
{
    WORD32 i4_size;
    WORD32 i4_min_I_qp;
    WORD32 i4_max_I_qp;
    WORD32 i4_min_P_qp;
    WORD32 i4_max_P_qp;
    WORD32 i4_min_B_qp;
    WORD32 i4_max_B_qp;
    WORD32 i4_avg_channel_bandwidth;
    WORD32 i4_peak_channel_bandwidth;
} ivea_vbr_params_t;

/****************************************************************************/
/* Error Info params: IVEA_GET_ERR_INFO                                     */
/****************************************************************************/
/*    Return to the commad IVEA_GET_ERR_INFO will be in this format. Status */
/* data buffer has to be typecasted to this struct.                         */
/****************************************************************************/

/****************************************************************************/
/* Element          *   Description                                         */
/****************************************************************************/
/* ii1_error_string *   This array will contain the error details of the    */
/*                  *   previous call                                       */
/****************************************************************************/
typedef struct ivea_error_string
{
    WORD32  i4_size;
    WORD8   ai1_error_string[IVEA_MAX_ERROR_LENGTH];
}ivea_enc_ext_error_string_t;

/****************************************************************************/
/* Packetization : IVEA_SET_PKT_PRMS                                        */
/****************************************************************************/
typedef struct ivea_ext_packetization_prms
{
    WORD32 i4_size;
    WORD32 i4_packet_size;
    WORD32 i4_max_rtp_header_size;
}ivea_ext_packetization_prms_t;

/****************************************************************************/
/* Target dimensions : IVEA_SET_TGT_DIM                                     */
/****************************************************************************/

typedef struct ivea_tgt_dim
{
    WORD32 i4_size;
    WORD32 i4_target_width;    /* scaled output width in pixels */
    WORD32 i4_target_height;   /* scaled output height in pixels */
    WORD32 i4_recon_buffer_stride; /* Stride of recon buffers allocated by  */
                                /* the application  */
}ivea_tgt_dim_t;


/****************************************************************************/
/* Profile Level Parameters :IVEA_SET_PRFLVL_OTP_PRMS                       */
/****************************************************************************/

typedef struct ivea_ext_profile_level_prms
{
    WORD32 i4_size;
    WORD32 i4_profile;
    WORD32 i4_level;

}ivea_ext_profile_level_prms_t;

/****************************************************************************/
/* TLV Buffer structures                                                    */
/****************************************************************************/

/****************************************************************************/
/* Bitstream common                                                         */
/****************************************************************************/

typedef struct ivea_bit_stream_common 
{
    WORD32      i4_size;      /* Size of this container        */                    
    WORD32      i4_fill_size; /* Fill size                     */
    WORD32      i4_priority;  /* Priority indicator 0 - 15. 15 being most important*/
}ivea_bit_stream_common_t;

/****************************************************************************/
/* User - data                                                              */
/****************************************************************************/
typedef struct ivea_user_data_payload_header
{
    WORD32      i4_size;
    WORD32      i4_user_data_size;
}ivea_user_data_payload_header_t;

/*****************************************************************************/
/* DAR Params structure : Display Aspect Ratio                               */
/*****************************************************************************/
typedef struct ivea_ext_par_prms
{
    WORD32 i4_size;
    WORD32 i4_par_width;
    WORD32 i4_par_height;
}ivea_ext_par_prms_t;

/*****************************************************************************/
/* IIR Params structure : Filetring parameters                               */
/*****************************************************************************/
typedef struct ivea_ext_iir_prms
{
    WORD32 i4_size;
    WORD32 i4_luma_noise_var;
    WORD32 i4_chrm_noise_var_cb;
	WORD32 i4_chrm_noise_var_cr;
    WORD32 i4_alpha_0;
    WORD32 i4_alpha_1;
    WORD32 i4_alpha_2;    
}ivea_ext_iir_prms_t;


/*****************************************************************************/
/* Structure for setting or querying the TCs Assignment.                     */
/*****************************************************************************/
typedef struct ivea_tc_info
{
    WORD32 i4_size;    				/* Size of this structure */
    WORD32 i4_num_tcs_for_L2_wr;    /* Number of TCs that codec shall require to write into L1/L2 memories */
    WORD32 i4_num_tcs_for_non_L2_wr;/* No of TCs that codec shall require to write regions other than L1/L2 */
    WORD32 pi4_tcs_for_L2_wr[4];    /* Array of TCs used to write into L1/L2 memories */
    WORD32 pi4_tcs_for_non_L2_wr[4];/* Array of TCs used to write into regions other than L1/L2 memories */
} ivea_tc_info_t;

#endif /* _IV_ENC1_EXTENSION_STRUCTS_H_ */


