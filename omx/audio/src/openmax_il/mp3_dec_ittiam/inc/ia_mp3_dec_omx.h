/* struct_def.h */

#ifndef __IA_MP3_DEC_OMX_H__
#define __IA_MP3_DEC_OMX_H__
#define OCP_GET_IOBUF_INFO     0x10000000
#define OCP_FLUSH              0x20000000
#define OCP_RESET              0x30000000
#define OCP_GET_STATUS		   0x40000000
#define OCP_GET_PARAM_BASE     0x50000000
#define OCP_SET_PARAM_BASE     0x60000000

#define OCP_EOK            0x00000000
#define OCP_EFAIL          0x80001000
#define ERR_INVALID_HANDLE 0x80001001

#define CODEC_NOT_SUPPORTED -3

#define BUFF_COUNT_MIN 1
#define BUFF_COUNT_ACTUAL 1

#define WORD8   OMX_S8
#define WORD32  OMX_S32
#define UWORD32 OMX_U32
#define VOID    void
/*
// ===========================================================================
// Status Structure
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct ia_mp3_dec_status {
    UWORD32      size;
    UWORD32      i_channel_mask;
    UWORD32      i_channel_mode;
	UWORD32      i_num_channels;
} ia_mp3_dec_status;

/*
// ===========================================================================
// Params Structure
// This structure defines the creation parameters for all aac objects
*/
typedef struct ia_mp3_dec_params_t {
    UWORD32      size;
    UWORD32      i_output_pcm_fmt;
	UWORD32      pcmFormat;
	UWORD32		 data_endianness;   /* Endianness of output data */
    UWORD32      ui_fsynch;
} ia_mp3_dec_params_t;


/*
 *  input arguments
 *  This structure defines the runtime input arguments for process
 *  function.
 */
typedef struct ia_mp3_dec_input_args_t {
	UWORD32 size;            /**< @sizeField */
    WORD32 numBytes;        /**< Size of input data in bytes,
                                 */
    WORD32 desiredChannelMode; /**< Desired Channel Configuration.*/
    WORD32 lfeFlag;         /**< Flag indicating whether LFE channel data
								  is desired in the output */

}ia_mp3_dec_input_args_t;

/*
 *   outargs
 *  This structure defines the run time output arguments for process
 *  function.
 */
typedef struct ia_mp3_dec_output_args_t
{
	UWORD32		 size;            /**< @sizeField */
	WORD32		 channels;
	WORD32		 sampling_rate;
	WORD32		 bytes_consumed;
	WORD32		 out_bytes;
	WORD32       i_init_done;
	WORD32       i_exec_done;
	WORD32       i_ittiam_err_code ;
	WORD32       i_output_bits_per_sample;
    WORD32       i_channel_mask;
	WORD32		 numSamples;
}ia_mp3_dec_output_args_t;

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
WORD32 ia_mp3_dec_ocp_init(VOID **handle, ia_mp3_dec_params_t *mp3_dec_params);
WORD32 ia_mp3_dec_ocp_control(VOID *handle, WORD32 cmd, void *param);

WORD32 ia_mp3_dec_ocp_process(VOID *handle,
						 ia_mp3_dec_input_args_t *input_args,
						 WORD8 *input_buffer,
						 WORD32 input_buff_size,
						 ia_mp3_dec_output_args_t *output_args,
						 WORD8 *output_buffer,
						 WORD32 output_buff_size);

WORD32 ia_mp3_dec_ocp_deinit(VOID *handle);

#endif /* __IA_MP3_DEC_OMX__ */
