typedef enum {
	/*************************************************************************************/
	/*									SUCCESS/NO ERROR								 */
	/*************************************************************************************/
	MPEG4_ERROR_NONE_START								= 0x0000,
	MPEG4_ERROR_NONE									= 0x0000,
	MPEG4_ERROR_NONE_END								= 0x0001,

	/*************************************************************************************/
	/*								CORRUPT DATA ERROR									 */
	/*************************************************************************************/
	MPEG4_CORRUPT_DATA_ERROR_START						= 0x0802,
	VOP_TYPE_NOT_IPB 									= 0x0802,	 /* Frame type is not I,P or B*/
	INVALID_VOP_QUANT									= 0x0803,	 /* Bitstream error */
	FRM_DEC_VOP_START_CODE_NOT_FOUND					= 0x0807,    /* Bitstream error */
	INVALID_MOT_COMP_MODE								= 0x0808,	 /* Bitstream error */
	MARKER_BIT_NOT_FOUND 								= 0x0809,    /* Bitstream error */
    MARKER_BIT_NOT_FOUND_RVLC 							= 0x080A,    /* Bitstream error */
    ZERO_BIT_NOT_FOUND_SVH 								= 0x080B,    /* Bitstream error */
	MARKER_BIT_NOT_FOUND_SEQUENCE_HEADER 				= 0x080C,    /* VLD error */
    NUM_VLD_COEF_EXCEED_64 								= 0x080D,    /* Bitstream error */
    INVALID_MV_TABLE_ACCESS 							= 0x080E,    /* Bitstream error */
    INVALID_DC_VALUE_SVH 								= 0x080F,    /* Bitstream error */
    FOUR_RESERVED_ZERO_BITS_NOT_FOUND 					= 0x0810,    /* Bitstream error */
    INVALID_PRED_TYPE 									= 0x0811,    /* Bitstream error */
    BVOP_WITHOUT_REF_FRAME 								= 0x0812,    /* B frame without a valid reference */
    INVALID_MOTION_MARKER 								= 0x0813,	 /* Bitstream error */
    INVALID_FCODE 										= 0x0814,	 /* Bitstream error */
    INVALID_GOB 										= 0x0815,    /* Bitstream error */
    INVALID_PACKET 										= 0x0817,    /* Bitstream error */
    INVALID_HUFFMAN_CODE 								= 0x0818,    /* Bitstream error */
    INVALID_RVLC_CODE 									= 0x0819,    /* Bitstream error */
    INVALID_HUFFMAN_CODE_DC_COEFF 						= 0x081A,    /* Bitstream error */
    INVALID_HUFFMAN_CODE_DCT_COEFF 						= 0x081B,    /* Bitstream error */
    ERROR_ESC_CODE_SEQ_TYPE3 							= 0x081C,    /* Bitstream error */
    INVALID_LEVEL_SHORT_VIDEO_HEADER 					= 0x081D,    /* Bitstream error */
    INVALID_HUFFMAN_CODE_FOR_RVLC 						= 0x081E,    /* Bitstream error */
    UNEXPECTED_END_OF_STREAM 							= 0x081F,    /* Unexpected end of stream */
    INVALID_CBPY 										= 0x0820,    /* Bitstream error */
    NON_ZERO_STUFFING_BIT 								= 0x0821,    /* Bitstream error */
    UNSPECIFIED_MB 										= 0x0822,    /* Bitstream error */
    RESYNC_MARKER_NOT_FOUND 							= 0x0823,    /* Bitstream error */
	ESCAPE_FOLLOWS_ESCAPE 								= 0x0824,    /* Bitstream error */
	VOL_WD_HT_NOT_SUPPORTED_SVH 						= 0x0825,    /* Width/Heigth of the SVH header invalid */
	MARKER_BIT_NOT_FOUND_FRAME_HEADER 					= 0x0826,    /* Bitstream error */
	ITTM4D_ERRONEOUS_FRAME								= 0x0828,    /* Bitstream error */
	ITTM4D_KEY_FRAME_NOT_FOUND							= 0x0829,    /* Bitstream error */
	ERROR_BITSTREAM_T									= 0x082A,    /* Bitstream error */
	ERROR_EOB_NEXTBITS_T								= 0x082B,    /* Bitstream error */
	MPEG4_HEADER_DECODE_FAILED							= 0x082C,    /* Sequence header decode failed */
	MPEG4_FRAME_NOT_FOUND								= 0x082D,    /* Bitstream error */
	ITTM4D_BVOP_WITHOUT_REF_FRAME						= 0x082E,    /* B frame without a valid reference */
	SVH_VOP_NOT_IP										= 0x082F,	 /* Bitstream error */
	SVH_SOURCE_FORMAT_INVALID							= 0x0830,    /* Frame type is not I,P for SVH*/
	FEATURE_NOT_SUPPORTED_IN_VOP_HEADER					= 0x0831,    /* Some unsupported toolset is enable in the header */
	INTER_BLOCK_WITHOUT_IFRAME 							= 0x0832,    /* Bitstream error */
    INVALID_HUFFMAN_CODE                    			= 0x0878,    /* Bitstream error */
    END_OF_STREAM			                			= 0x087f,    /* Unexpected end of stream */
	VLD_ERROR			                    			= 0x0888     /* VLD decode error */
	MPEG4_CORRUPT_DATA_ERROR_END						= 0x08C7,

	/*************************************************************************************/
	/*										FATAL ERROR								     */
	/*************************************************************************************/
	MPEG4_FATAL_DATA_ERROR_START						= 0x80C8,
	NEW_PRED_ENABLE_NOT_SUPPORTED 						= 0x80C9,    /* New Pred Enable toolset not supported */
	SVOP_NOT_SUPPORTED 									= 0x80CA,    /* SVOP(GMC) toolset not supported */
	FEATURE_NOT_SUPPORTED_IN_SEQUENCE_HEADER 			= 0x80CB,    /* Some unsupported toolset is enable in the header */
	VOL_WD_HT_NOT_SUPPORTED 							= 0x80CC,    /* Width/Heigth in the header invalid */
	STATIC_SVOP_NOT_SUPPORTED 							= 0x80CD,    /* SVOP(GMC) toolset not supported */
	GMC_SVOP_NOT_SUPPORTED 								= 0x80CE,    /* SVOP(GMC) toolset not supported */
	GRAYSCALE_VOP_SHAPE_NOT_SUPPORTED 					= 0x80CF,    /* Grayscale VOP toolset not supported */
	CHANGE_OF_STREAM_PROPERTIES_NOT_SUPPORTED			= 0x80D0,    /* Change of stream properties not supported */
	ITTM4D_INVALID_INPUT_FORMAT							= 0x80D1,    /* Invalid input source format */

	/*************************************************************************************/
	/*		FATAL ERROR	BECAUSE THE DECODER IS A EVAL OF LIBMASKED VERSION			     */
	/*************************************************************************************/
	ITTM4D_HIGH_DEFINITON_STREAM						= 0x80F0,
	MPEG4_FATAL_DATA_ERROR_END							= 0x80FF,
} MPEG4_ERROR_CODE_T;