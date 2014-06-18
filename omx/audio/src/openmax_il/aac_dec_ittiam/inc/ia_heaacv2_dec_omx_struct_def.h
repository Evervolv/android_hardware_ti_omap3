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
#ifndef IA_HEAACV2_DEC_OMX_H
#define IA_HEAACV2_DEC_OMX_H



#define OCP_GET_IOBUF_INFO     0x10000000
#define OCP_FLUSH              0x20000000
#define OCP_RESET              0x30000000
#define OCP_GET_STATUS         0x40000000
#define OCP_GET_PARAM_BASE     0x50000000
#define OCP_SET_PARAM_BASE     0x60000000

#define OCP_EOK            0x00000000
#define OCP_EFAIL          0x80001000
#define ERR_INVALID_HANDLE 0x80001001

#define CODEC_NOT_SUPPORTED -3
#define BUFF_COUNT_MIN 1
#define BUFF_COUNT_ACTUAL 1

#define WORD8 OMX_S8
#define UWORD32 OMX_U32
#define WORD32 OMX_S32
#define VOID void


/*
// ===========================================================================
// Status Structure
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct ia_heaacv2_dec_status {
    UWORD32      size;
    UWORD32      i_channel_mask;
    UWORD32      i_channel_mode;
    UWORD32      i_sbr_mode;
    UWORD32      i_num_channels;
} ia_heaacv2_dec_status;

/*
// ===========================================================================
// Params Structure
// This structure defines the creation parameters for all aac objects
*/
typedef struct ia_heaacv2_dec_params_t {
    UWORD32      size;
    UWORD32      i_output_pcm_fmt;
    UWORD32      pcmFormat;
    UWORD32      data_endianness;   /* Endianness of output data */
    UWORD32      i_max_channels;
    UWORD32      i_flag_08khz_out;
    UWORD32      i_flag_16khz_out;
    UWORD32      i_interleave;     /* to enable/disable to_stereo option */
    UWORD32      i_mp4_flag;
    UWORD32      i_disable_sync;
    UWORD32      i_auto_sbr_upsample;
    UWORD32      i_sampfreq;
    UWORD32      i_coupling_channel;
} ia_heaacv2_dec_params_t;


/*
 *  input arguments
 *  This structure defines the runtime input arguments for process
 *  function.
 */
typedef struct ia_heaacv2_input_args_t {
    UWORD32 size;            /**< @sizeField */
    WORD32 numBytes;        /**< Size of input data in bytes,
                                 */
    WORD32 desiredChannelMode; /**< Desired Channel Configuration.*/
    WORD32 lfeFlag;         /**< Flag indicating whether LFE channel data
                                  is desired in the output */
    //UWORD32 input_bytes;

}ia_heaacv2_input_args_t;

/*
 *   outargs
 *  This structure defines the run time output arguments for process
 *  function.
 */
typedef struct ia_heaacv2_output_args_t
{
    UWORD32      size;            /**< @sizeField */
    UWORD32      channels;
    UWORD32      sampling_rate;
    WORD32       bytes_consumed;
    WORD32       out_bytes;
    WORD32       i_init_done;
    WORD32      i_exec_done;
    WORD32      i_ittiam_err_code ;
    WORD32      i_extended_err_code ;
    WORD32      i_output_bits_per_sample;
    WORD32      i_num_channels;
    WORD32      i_channel_mask;
    WORD32       numSamples;
    WORD32       channelMode;
    WORD32       lfeFlag;
}ia_heaacv2_output_args_t;

typedef struct buf_info_t
{
    WORD32 num_bufs;   /* number of buffers */
    WORD32 buf_size;   /* buffer size */

} buf_info_t;

typedef struct io_buf_info_t
{
    WORD32 size;
    buf_info_t in_buf_info;
    buf_info_t out_buf_info;

} io_buf_info_t;



/* Fuction declarations for HEAAC v2 decoder */
WORD32 ia_heaacv2_dec_ocp_init(VOID **handle, ia_heaacv2_dec_params_t *heaacv2_dec_params);
WORD32 ia_heaacv2_dec_ocp_control(VOID *handle, WORD32 cmd, void *param);

WORD32 ia_heaacv2_dec_ocp_process(VOID *handle,
                         ia_heaacv2_input_args_t *input_args,
                         WORD8 *input_buffer,
                         WORD32 input_buff_size,
                         ia_heaacv2_output_args_t *output_args,
                         WORD8 *output_buffer,
                         WORD32 output_buff_size);

WORD32 ia_heaacv2_dec_ocp_deinit(VOID *handle);
#endif
