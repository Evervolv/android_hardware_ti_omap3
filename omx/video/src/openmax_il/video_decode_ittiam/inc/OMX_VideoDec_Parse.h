/*****************************************************************************/
/*                                                                           */
/*					  Ittiam Video Decoder OMX-IL Component  	             */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2010                             */
/*                                                                           */
/*  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and */
/*  is protected under Indian  Copyright Law as an unpublished work. Its use */
/*  and  disclosure  is  limited by  the terms  and  conditions of a license */
/*  agreement. It may not be copied or otherwise  reproduced or disclosed to */
/*  persons outside the licensee's organization except in accordance with the*/
/*  terms  and  conditions   of  such  an  agreement.  All  copies  and      */
/*  reproductions shall be the property of Ittiam Systems Private Limited and*/
/*  must bear this notice in its entirety.                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : OMX_VideoDec_Parse.h                                 */
/*                                                                           */
/*  Description       : Contains structures and macros used in header 	     */
/*						parsing												 */
/*                                                                           */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          01 12 2010   Ittiam          Draft                               */
/*                                                                           */
/*****************************************************************************/

#ifndef OMX_VIDDEC_PARSE__H
#define OMX_VIDDEC_PARSE__H

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#ifdef PARSE_HEADER
#define VIDDEC_MULT16MASK 0xFFFFFFF0
#define VIDDEC_MULT16 15
#define VIDDEC_MULTIPLE16(x) ((x + VIDDEC_MULT16) & VIDDEC_MULT16MASK)
#define MULTIPLE_32(x) ((x + 31) & ~(31))
#endif

/*****************************************************************************/
/*  Function Name : OMX_PARSER_CHECKLIMIT()                                  */
/*  Description   : 	Sumukh!!!									         */
/*  Inputs        : 									                     */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         02 12 2010   Ittiam          Draft                                */
/*****************************************************************************/
#define OMX_PARSER_CHECKLIMIT(_total, _actual, _step) /*  \
    if(((_actual + _step) >> 3) >= _total){                \
    printf("_total %d _actual %d\n",_total,((_actual + _step)>>3)); \
        eError = OMX_ErrorStreamCorrupt;                \
        goto EXIT;                                      \
    }*/

/*****************************************************************************/
/* Structures                                                                 */
/*****************************************************************************/
typedef struct VIDDEC_Point
{
    OMX_S32 nX;
    OMX_S32 nY;
}VIDDEC_Point;

typedef struct VIDDEC_Rect
{
    VIDDEC_Point sTl;
    VIDDEC_Point sBr;
}VIDDEC_Rect;

typedef struct VIDDEC_Size
{
    OMX_S32 nWidth;
    OMX_S32 nHeight;
}VIDDEC_Size;

typedef struct VIDDEC_VideoPictureHeader
{
    OMX_U32 nOptions;
    OMX_S32 nProfile;
    OMX_S32 nLevel;
    OMX_S32 nVersion;
    OMX_U8* cnOptional;
    VIDDEC_Size sSizeInMemory;
    VIDDEC_Rect sDisplayedRect;
    OMX_TICKS nPresentationTimestamp;
    OMX_TICKS nDecodingTimestamp;
    OMX_U32 nPreDecoderBufferSize;
    OMX_U32 nPostDecoderBufferSize;
    OMX_U32 nPictureNumber;
    OMX_U32 nPictureLayer;
    OMX_U8* pcSupplementalData;
    OMX_BOOL bIsRandomAccessible;
    OMX_S32 nRandomAccessBufferingPeriod;
    OMX_U32 nRandomAccessBufferOccupancy;
}VIDDEC_VideoPictureHeader;

typedef struct VIDDEC_MPEG4VisualVbvParams
{
    OMX_TICKS nBitRate;
    OMX_U32 nVbvBufferSize;
    OMX_U32 nVbvOccupancy;
}VIDDEC_MPEG4VisualVbvParams;

typedef struct VIDDEC_MPEG4VisualVOLHeader
{
    OMX_U32 nVideoObjectLayerId;
    OMX_BOOL bShortVideoHeader;
    OMX_BOOL bRandomAccessibleVOL;
    OMX_U32 nVideoObjectTypeIndication;
    OMX_U32 nVideoObjectLayerVerId;
    OMX_U32 nVideoObjectLayerPriority;
    OMX_U32 nAspectRatioNum;
    OMX_U32 nAspectRatioDenom;
    VIDDEC_MPEG4VisualVbvParams sVbvParams;
    OMX_U16 nVOPTimeIncrementResolution;
    OMX_BOOL bnFnXedVopRate;
    OMX_U16 nFnXedVOPTimeIncrement;
    OMX_BOOL bDataPartitioning;
    OMX_BOOL bReversibleVLC;
    OMX_U8* pcUserData;
}VIDDEC_MPEG4VisualVOLHeader ;

typedef struct VIDDEC_MPEG4YuvConversionMatrix{
    OMX_S32 iPostOffset;
    OMX_S32 iMatrix;
    OMX_S32 iPreOffset;
}VIDDEC_MPEG4YuvConversionMatrix;

typedef struct VIDDEC_MPEG4YuvFormat
{
    OMX_U32 iCoefficients;
    OMX_U32 iPattern;
    OMX_U32 iDataLayout;
    VIDDEC_MPEG4YuvConversionMatrix * iYuv2RgbMatrix;
    VIDDEC_MPEG4YuvConversionMatrix * iRgb2YuvMatrix;
    OMX_U32 iAspectRatioNum;
    OMX_U32 iAspectRatioDenom;
}VIDDEC_MPEG4YuvFormat;

typedef struct VIDDEC_MPEG4UncompressedVideoFormat
{
    OMX_U32 iDataFormat;
    VIDDEC_MPEG4YuvFormat iYuvFormat;
    OMX_U32 iRgbFormat;
}VIDDEC_MPEG4UncompressedVideoFormat;

typedef struct VIDDEC_AVC_ParserParam
{
    OMX_U32 nBitPosTemp;
    OMX_U32 nForbiddenZeroBit;
    OMX_U32 nNalRefIdc;
    OMX_U32 nProfileIdc;
    OMX_U32 nConstraintSet0Flag;
    OMX_U32 nConstraintSet1Flag;
    OMX_U32 nConstraintSet2Flag;
    OMX_U32 nReservedZero5bits;
    OMX_U32 nLevelIdc;
    OMX_U32 nSeqParameterSetId;
    OMX_U32 nLog2MaxFrameNumMinus4;
    OMX_U32 nPicOrderCntType;
    OMX_U32 nLog2MaxPicOrderCntLsbMinus4;
    OMX_S32 nOffsetForNonRefPic;
    OMX_S32 nOffsetForTopToBottomField;
    OMX_U32 nNumRefFramesInPicOrderCntCycle;
    OMX_U32 nNumRefFrames;
    OMX_S32 nGapsInFrameNumValueAllowedFlag;
    OMX_U32 nPicWidthInMbsMinus1;
    OMX_U32 nPicHeightInMapUnitsMinus1;
    OMX_U32 nFrameMbsOnlyFlag;
    OMX_S32 nMBAdaptiveFrameFieldFlag ;
    OMX_U32 nDirect8x8InferenceFlag;
    OMX_U32 nFrameCroppingFlag;
    OMX_U32 nFrameCropLeftOffset;
    OMX_U32 nFrameCropRightOffset;
    OMX_U32 nFrameCropTopOffset;
    OMX_U32 nFrameCropBottomOffset;
}VIDDEC_AVC_ParserParam;

typedef struct VIDDEC_MPEG4_ParserParam
{
    OMX_U32 nIsVisualObjectIdentifier;
    OMX_U32 nVisualObjectType;
    OMX_U32 nVideoSignalType;
    OMX_U32 nVideoFormat;
    OMX_U32 nVideoRange;
    OMX_U32 nColorDescription;
    OMX_U32 NBitZero;
    OMX_U32 nVideoObjectTypeIndication;
    OMX_U32 nIsVisualObjectLayerIdentifier;
    OMX_U32 nLayerVerId;
    OMX_U32 nLayerPriority;
    OMX_U32 nAspectRadio;
    OMX_U32 nParWidth;
    OMX_U32 nParHeight;
    OMX_U32 nControlParameters;
    OMX_U32 nChromaFormat;
    OMX_U32 nLowDelay;
    OMX_U32 nVbvParameters;
    OMX_U32 nBitRate;
    OMX_U32 nFirstHalfVbvBufferSize;
    OMX_U32 nLatterHalfVbvBufferSize;
    OMX_U32 nFirstHalfVbvOccupancy;
    OMX_U32 nLatterHalfVbvOccupancy;
    OMX_U32 nLayerShape;
    OMX_U32 nTimeIncrementResolution;
    OMX_U32 nFnXedVopRate;
    OMX_U32 nNum_bits;
    OMX_U32 nInterlaced;
    OMX_U32 nObmc;
    OMX_U32 nUFEP;
    OMX_U32 NSpriteNotSupported;
    OMX_U32 nNot8Bit;
    OMX_U32 nQuantPrecision;
    OMX_U32 nBitsPerPnXel;
    OMX_U32 nIsInverseQuantMethodFirst;
    OMX_U32 nComplexityEstimationDisable;
    OMX_U32 nIsResyncMarkerDisabled;
    OMX_U32 nIsDataPartitioned;
    OMX_U32 nRvlc;
    OMX_U32 nScalability;
    OMX_S32 nSourceFormat;
    OMX_BOOL nOutputFormat;
    OMX_U32 nCPM;
    OMX_U32 nPWI;
    OMX_U32 nPHI;
}VIDDEC_MPEG4_ParserParam;

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
#ifdef PARSE_HEADER
OMX_ERRORTYPE VIDDEC_ParseVideo_WMV9_VC1(OMX_U32		      *nWidth,
										 OMX_U32			  *nHeight,
										 OMX_BUFFERHEADERTYPE *pBuffHead);

OMX_ERRORTYPE VIDDEC_ParseVideo_WMV9_RCV(OMX_U32 			  *nWidth,
										 OMX_U32 			  *nHeight,
										 OMX_BUFFERHEADERTYPE *pBuffHead);

OMX_ERRORTYPE VIDDEC_ParseHeader(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								 OMX_BUFFERHEADERTYPE 	  *pBuffHead);

OMX_ERRORTYPE VIDDEC_ParseVideo_MPEG4(OMX_U32 *nWidth, OMX_U32 *nHeight,
									  OMX_BUFFERHEADERTYPE *pBuffHead);

OMX_ERRORTYPE VIDDEC_ParseVideo_H264(
						VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
                        OMX_BUFFERHEADERTYPE     *pBuffHead,
                        OMX_U32					 *nWidth,
                        OMX_U32					 *nHeight,
                        OMX_U32				     *nCropWidth,
                        OMX_U32					 *nCropHeight,
                        OMX_U32 				 nType);

OMX_U32 VIDDEC_GetBits(OMX_U32  *nPosition,
					   OMX_U8   nBits,
					   OMX_U8   *pBuffer,
					   OMX_BOOL bIcreasePosition);

OMX_S32 VIDDEC_UVLC_dec(OMX_U32 *nPosition, OMX_U8* pBuffer);
#endif

#endif /* OMX_VIDDEC_PARSE__H */
