#ifndef IA_WMA_DEC_H
#define IA_WMA_DEC_H



#define OCP_GET_IOBUF_INFO     0x10000000
#define OCP_FLUSH              0x20000000
#define OCP_RESET              0x30000000


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

/* Structure declarations for WMA decoder */
typedef struct ia_wma_dec_params_t
{
    UWORD32 size;             /* size of structure */
    WORD32 output_pcm_width;  /* Number of bits per output PCM Sample */
    WORD32 pcm_format;        /* Output PCM Format Block/Interleaved */
    WORD32 data_endianness;   /* Endianness of output data */
	WORD32 i_constant_output; /* Constant output size even in case of Packet loss */
	WORD32 i_use_full_output_buff;   /* Use full output buffer even for mono files */
} ia_wma_dec_params_t;

typedef struct ia_wma_input_args_t
{
	WORD32 size;
	WORD32 input_bytes;
	WORD32 output_channel_mode;
} ia_wma_input_args_t;

typedef struct ia_wma_output_args_t
{
	WORD32 size;
	UWORD32 channels;
	UWORD32 sampling_rate;
	UWORD32 bytes_consumed;
	UWORD32 out_bytes;
	WORD32 i_exec_done;
    /* codec specific parameters which can change after 1st frame decode */
} ia_wma_output_args_t;


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


/* Fuction declarations for WMA decoder */
WORD32 ia_wma_dec_ocp_init(VOID **handle, ia_wma_dec_params_t *wma_dec_params);
WORD32 ia_wma_dec_ocp_control(VOID *handle, WORD32 cmd, void *param);

WORD32 ia_wma_dec_ocp_process(VOID *handle,
						 ia_wma_input_args_t *input_args,
						 WORD8 *input_buffer,
						 WORD32 input_buff_size,
						 ia_wma_output_args_t *output_args,
						 WORD8 *output_buffer,
						 WORD32 output_buff_size);

WORD32 ia_wma_dec_ocp_deinit(VOID *handle);

#endif /* #ifndef IA_WMA_DEC_H */
