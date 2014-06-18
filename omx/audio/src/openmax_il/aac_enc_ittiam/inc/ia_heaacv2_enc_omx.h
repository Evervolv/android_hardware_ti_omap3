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

#ifndef IA_HEAACV2_ENC_OMX_H
#define IA_HEAACV2_ENC_OMX_H

#include "ia_type_def.h"

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


/*
// ===========================================================================
// Status Structure
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct ia_heaacv2_enc_status {
    UWORD32      size;
    WORD32      extendedError;   /**< @extendedErrorField */
    WORD32      lfeFlag;        /**< Flag indicating whether LFE channel data
                                 *   is present or not in the input.*/
    WORD32      bitRate;        /**< Average bit rate, in bits per second.
                                 */
    WORD32      sampleRate;     /**< Sampling frequency in Hz.
                                 */
    WORD32      channelMode;    /**< Input Channel Mode.*/
    WORD32      encMode;         /**< Encoding mode.*/

} ia_heaacv2_enc_status;

/*
// ===========================================================================
// Params Structure
// This structure defines the creation parameters for all aac objects
*/
typedef struct ia_heaacv2_enc_params_t {
    UWORD32      size;
    WORD32      sampleRate;
    WORD32      bitRate;
    WORD32      channelMode;
    WORD32      dataEndianness;
    WORD32      encMode;
    WORD32      inputFormat;
    WORD32      inputBitsPerSample;
    WORD32      maxBitRate;
    WORD32      dualMonoMode;
    WORD32      crcFlag;
    WORD32      ancFlag;
    WORD32      lfeFlag;


    WORD32      noChannels;
    WORD32      aacClassic;
    WORD32      psEnable;
    WORD32      dualMono;
    WORD32      downmix;
    WORD32      useSpeechConfig;
    WORD32      fNoStereoPreprocessing;
    WORD32      invQuant;
    WORD32      useTns;
    WORD32      use_ADTS;
    WORD32      use_ADIF;
    WORD32      full_bandwidth;
    WORD32      i_channels_mask;
    WORD32      i_num_coupling_chan;
    WORD32      write_program_config_element;

} ia_heaacv2_enc_params_t;


/*
 *  input arguments
 *  This structure defines the runtime input arguments for process
 *  function.
 */
typedef struct ia_heaacv2_enc_input_args_t {
    UWORD32 size;            /**< @sizeField */
    WORD32  numInSamples;        /**< Size of input data in bytes,
                                 */
}ia_heaacv2_enc_input_args_t;

/*
 *   outargs
 *  This structure defines the run time output arguments for process
 *  function.
 */
typedef struct ia_heaacv2_enc_output_args_t
{
    UWORD32      size;            /**< @sizeField */

    WORD32   bytesGenerated;  /**< Number of bytes generated during the
                                 *   process() call.
                                 */
    WORD32   numZeroesPadded; /**< Number of zeroes padded to input.
                                 */
    WORD32   numInSamples;    /**< Number of input samples per channel consumed by the encoder
                                */
    //UWORD32        channels;
    //UWORD32        sampling_rate;
    //WORD32         bytes_consumed;
    //WORD32       i_init_done;
    WORD32       i_exec_done;
    WORD32       i_ittiam_err_code ;

}ia_heaacv2_enc_output_args_t;

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
WORD32 ia_heaacv2_enc_ocp_init(VOID **handle, ia_heaacv2_enc_params_t *heaacv2_dec_params);
WORD32 ia_heaacv2_enc_ocp_control(VOID *handle, WORD32 cmd, void *param);

WORD32 ia_heaacv2_enc_ocp_process(VOID *handle,
                         ia_heaacv2_enc_input_args_t *input_args,
                         WORD8 *input_buffer,
                         WORD32 input_buff_size,
                         ia_heaacv2_enc_output_args_t *output_args,
                         WORD8 *output_buffer,
                         WORD32 output_buff_size);

WORD32 ia_heaacv2_enc_ocp_deinit(VOID *handle);
#endif
