/*****************************************************************************/
/* Enum definitions - Error List present here                                */
/*****************************************************************************/
typedef enum
{
/*****************************************************************************/
/* FATAL ERROR CODES 						             */
/*****************************************************************************/
    WMV9DSC_FATAL_DATA_ERROR     = 0x8001,
    FATAL_MAIN_PROF_NO_SUPPORT_T = 0x8004,   // Main Profile Support not present
    FATAL_ADV_PROF_NO_SUPPORT_T  = 0x8005,   // Advanced Profile Support not present
    FATAL_DIMENSION_NO_SUPPORT_T = 0x8006,   // Dimensions not supported
    WMV9_PROFILE_ERROR           = 0x8007,   //Profile indicated in the bitstream is erroneous
    WMV9_SP_INVALID_SYNTAX_ELEMENT_ERROR = 0x8008,
    ERROR_HEADER_DECODE_FAIL_T   = 0x800F,   // Header Decode had failed

/*****************************************************************************/
/* CORRUPT DATA ERROR CODES  					             */
/*****************************************************************************/
	WMV9DSC_CORRUPT_DATA_ERROR   = 0x081F,    //Error returned by the decoder if the IMBUFFER is erroneous
    WMV9_PQINDEX_ERROR           = 0x0820,    //PQINDEX  syntax element error
    WMV9_MVRANGE_ERROR           = 0x0821,    //MVRANGE  syntax element error
    WMV9_BFRACTION_ERROR         = 0x0822,    //BFRACTION syntax element error
    WMV9_ALTPQUANT_ERROR         = 0x0823,    //ALTPQUANT value erroneous
    WMV9_PQUANT_ERROR            = 0x0824,    //PQUANT value erroneous
    WMV9_SYNCMARKER_PIC_ERROR    = 0x0825,    //Erroneous sync marker in the picture layer
    WMV9_MQUANT_ERROR            = 0x0826,    //MQUANT value erroneous
    WMV9_MVMODE_ERROR            = 0x0827,    //MVMODE value erroneous
    WMV9_PIC_BUF_NULL            = 0x0828,    //one of the picture buffer is null
    WMV9_IPIC_EXPECTED_ERROR     = 0x0829,    //If the first picture decoded is not an I picture
    ERROR_INV_SLICE_ADDR_T       = 0x082A,    //Slice address was invalid
    WMV9_INTERLACE_FRAME_ERROR   = 0x082B,    //Frame got to the decoder was an interlace frame
    WMV9_RND_CNTRL_ERROR         = 0x082C,
    WMV9_BITSTREAM_LENGTH_ERROR  = 0x082D,
    WMV9_SLICE_ERROR             = 0x082E,
    ERROR_EOB_GETBITS_T          = 0x0830,    // get_bits should not be more than 15
    ERROR_EOB_NEXTBITS_T         = 0x0831,    // next_bits should not be more than 32
    ERROR_EOB_GETBIT_T           = 0x0832,
    ERROR_BITSTREAM_T            = 0x0833,    // Error in the bitstream functions
    ERROR_DC_DIFFERENTIAL_T      = 0x0834,    // Error in bitstream while decoding DC diff
    ERROR_AC_COEFF_T             = 0x0835,    // Error in bitstream while decoding AC coeff
    ERROR_AC_LAST_FLAG_MISSING_T = 0x0836,    // Last Flag not found Error
    ERROR_BITPLANE_T             = 0x0837,    // Error in bitstream while decoding bitplane
    ERROR_CBP_T                  = 0x0838,    // Error in bitstream while decoding CBP
    ERROR_TTMB_T                 = 0x0839,    // Error in bitstream while decoding TTMB
    ERROR_MVDATA_T               = 0x083A,    // Error in bitstream while decoding MV data
    ERROR_INV_BFRACTION_T        = 0x083B,    // Invalid value of u1_bfraction
    ERROR_PIC_BUF_NOT_FREE_T     = 0x083C,    // wmv9_frame_buffer_not_free
    ERROR_PAST_BUF_INIT_FAIL_T   = 0x083D,    // wmv9_frame_buffer_not_free
    ERROR_FUT_BUF_INIT_FAIL_T    = 0x083E,    // wmv9_frame_buffer_not_free
    ERROR_GET_PICBUF_T           = 0x083F,
    ERROR_UNAVAIL_PICBUF_T       = 0x0840,
    ERROR_UNAVAIL_REFBUF_T       = 0x0841,
    ERROR_INV_RANGE_QP_T         = 0x0842,
    ERROR_END_OF_FRAME_EXPECTED_T= 0x0843,
    WMV9_CODED_WIDTH_ERROR       = 0x0844,
    WMV9_CODED_HEIGHT_ERROR      = 0x0845,
    WMV9_ENTRY_ERROR             = 0x0846,
    WMV9_BDU_CODE_ERROR          = 0x0847,
    WMV9_SKIP_PIC_ERROR 	     = 0x0848,
    WMV9_RANGEREDFRM_ERROR       = 0x0849,    //RANGEREDFRM syntax element error

} wmv9_dec_err_code_t;

