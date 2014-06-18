 /*	Error codes enumeration	*/
typedef enum{
  NO_ERROR =						0x0,
  ERROR_UNKNOWN_LEVEL = 			0x8001,          //"\nUnknown Level "
  ERROR_FEATURE_UNAVAIL = 			0x8002,    //"\nLevel not supported "
  ERROR_NOT_SUPP_RESOLUTION = 		0x8003,    //"\n Resolution not supported"
  ERROR_INV_SPS_PPS_T = 			0x8004,
  ERROR_MEM_ALLOC_ISRAM_T = 		0x8005,
  ERROR_MEM_ALLOC_SDRAM_T = 		0x8006,
  ERROR_MEM_ALLOC_PICBUF_T = 		0x8007,
  ERROR_DBP_MANAGER_T = 			0x0808,
  ERROR_GAPS_IN_FRM_NUM = 			0x0809,        //"Error in gaps in frame number "
  ERROR_UNKNOWN_NAL = 				0x080A,            //"Unknown NAL unit type"
  ERROR_INV_MB_SLC_GRP_T = 			0x080B,       //"Invalid mb_slice_group_map_type"
  ERROR_MULTIPLE_SLC_GRP_T = 		0x080C,     //"\nMultiple slice groups not supported"
  ERROR_INVALID_PIC_PARAM = 		0x080D,      //"\nInvalid Picture parametre"
  ERROR_INVALID_SEQ_PARAM = 		0x080E,      //"\nInvalid Sequence parametre"
  /* "\nExponential Golomb code exceeds the 32 bit limit" */
  ERROR_EGC_EXCEED_32_1_T = 		0x080F,      //FindOneinBitstream_h264
  ERROR_EGC_EXCEED_32_2_T = 		0x0810,      //FindOneinBitstreamlen
  ERROR_INV_RANGE_TEV_T = 			0x0811,        //"\nRange for te(v) < 0"
  ERROR_INV_SLC_TYPE_T = 			0x0812,         //"\nSlice type not recognized"
  ERROR_UNAVAIL_PICBUF_T = 			0x0813,       //"\nNo more picture buffers left"
  ERROR_UNAVAIL_REFBUF_T = 			0x0814,       //"\nCould not get a free Reference picture buffer"
  ERROR_UNAVAIL_DISPBUF_T = 		0x0815,
  ERROR_INV_POC_TYPE_T = 			0x0816,         //"\nError in POC type"
  ERROR_PIC1_NOT_FOUND_T = 			0x0817,       //"\nPIC1 not found in default list"
  ERROR_PIC0_NOT_FOUND_T = 			0x0818,       //"\nPIC0 not found in default list"
  ERROR_NUM_REF = 					0x0819,                // Number of reference Pic not Valid
  ERROR_REFIDX_ORDER_T = 			0x081A,         //"\nError in Ref_idx_reordering_h264"
  ERROR_EOB_FLUSHBITS_T = 			0x081B,        //"End of bitstream encountered"
  ERROR_EOB_GETBITS_T = 			0x081C,
  ERROR_EOB_GETBIT_T = 				0x081D,
  ERROR_EOB_BYPASS_T = 				0x081E,           //decodebypassbins
  ERROR_EOB_DECISION_T =	 		0x081F,         //decodedecisionbins
  ERROR_EOB_TERMINATE_T = 			0x0820,        //decodeterminate
  ERROR_EOB_READCOEFF4X4CAB_T = 	0x0821,  //"End of bitstream encountered"
  ERROR_INV_RANGE_QP_T = 			0x0822,         //"\nMbQpDelta value not in range"
  ERROR_END_OF_FRAME_EXPECTED_T = 	0x0823,
  ERROR_MB_TYPE = 					0x0824,                // "Error in decoding Mbtype"
  ERROR_SUB_MB_TYPE = 				0x0825,            // "Error in decoding subMbtype"
  ERROR_CBP = 						0x0826,                    // "Error in CBP"
  ERROR_REF_IDX = 					0x0827,                // "error in decoding Reference Index"
  ERROR_NUM_MV = 					0x0828,                 // "error in motion vector range for level
  ERROR_CHROMA_PRED_MODE = 			0x0829,       // "error in decoding chroma pred mode"
  ERROR_INTRAPRED = 				0x082A,              // "error in decoding intra pred 4x4 mode"
  ERROR_NEXT_MB_ADDRESS_T = 		0x082B,
  ERROR_MB_ADDRESS_T = 				0x082C,
  ERROR_MB_GROUP_ASSGN_T = 			0x082D,
  ERROR_CAVLC_NUM_COEFF_T = 		0x082E,
  ERROR_CAVLC_SCAN_POS_T = 			0x082F,
  ERROR_CABAC_RENORM_T = 			0x0830,
  ERROR_CABAC_SIG_COEFF1_T = 		0x0831,
  ERROR_CABAC_SIG_COEFF2_T = 		0x0832,
  ERROR_CABAC_ENCODE_COEFF_T = 		0x0833,
  ERROR_INV_SLICE_HDR_T =	 		0x0834,
  ERROR_IN_IMBUF_PARSING =			0x0835,
  ERROR_DMA_GLOBAL_REGISTERS =		0x0836,
  ERROR_INSUFFICIENT_PICTURE_BUFFERS = 0x0837,
  ERROR_INSUFFICIENT_DATA = 		0x0400



} H264DecoderErrorCode_t;

