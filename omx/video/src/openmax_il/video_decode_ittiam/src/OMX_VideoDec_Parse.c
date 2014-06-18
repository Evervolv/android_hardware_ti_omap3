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
/*  File Name         : OMX_VideoDec_Parse.c                                 */
/*                                                                           */
/*  Description       : This file Parse Different Video Format      		 */
/*																			 */
/*                                                                           */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          17 11 2009   ITTAIM          Draft                               */
/*                                                                           */
/*****************************************************************************/

/*******************************************************************************
*  INCLUDE FILES
*******************************************************************************/
/* User include files */
#include <OMX_Core.h>
#include <OMX_TI_Debug.h>
#include "OMX_TI_Common.h"
//#include "overlay_common.h"
#include "LCML_DspCodec.h"
#include "LCML_Types.h"
#include "LCML_CodecInterface.h"
#include "OMX_IttiamVideoDec_Utils.h"
#include "h264_dsp_api.h"
#include "mpeg4_dsp_api.h"
#include "wmv9_dsp_api.h"
#include "OMX_IttiamVideoDec_DSP.h"
#include "OMX_IttiamVideoDec_Thread.h"
#include "OMX_VideoDec_Parse.h"
#include "OMX_IttiamVideoDec_Logger.h"
#include "split_dsp_dec.h"
#include "xdas.h"
#include "xdm.h"
#include "ivideo.h"
#include "ividdec2.h"
#include "split/ividdec2.h"
#include "ittiam_datatypes.h"
#include "xdm_extension_dec2.h"
#include "split_dec.h"

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_OMX_VidDec_Parse"
#include <utils/Log.h>

#ifdef PARSE_HEADER

/*****************************************************************************/
/*  Function Name : GET_NUM_BIT_REQ                                          */
/*  Description   : Checks for the first one set from LSB.                   */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_U32 GET_NUM_BIT_REQ(OMX_U32 num)
{
    OMX_U8 i = 0;

    if (num)
    {
       num--;
       for (i = 32; i != 0; i--)
       {
          if (num & (0x1 << (i-1)))
			break;
       }
    }
    else
    {
       printf("%d: Error in GET_NUM_BIT_REQ arg can't be zero\n", __LINE__);
    }
    return (i);
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ParseVideo_WMV9_VC1                               */
/*  Description   : This file Parse WMV9 File Format VC!                     */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_ParseVideo_WMV9_VC1(OMX_U32			  *nWidth,
										 OMX_U32			  *nHeight,
										 OMX_BUFFERHEADERTYPE *pBuffHead)
{
    OMX_ERRORTYPE eError 			= OMX_ErrorUndefined;
    OMX_U32       nTempValue 		= 0;
    OMX_U32       nProfile 			= 0;
    OMX_U32       nLevel 			= 0;
    OMX_U32       nBitPosition 		= 0;
    OMX_U8*       pHeaderStream 	= (OMX_U8*)pBuffHead->pBuffer;
    OMX_BOOL      nStartFlag 		= OMX_FALSE;
    OMX_U32       nInBytePosition 	= 0;
    OMX_U32       nTotalInBytes 	= 0;
    OMX_U32       nNalUnitType 		= 0;

    nTotalInBytes = pBuffHead->nFilledLen;
	LOG_STATUS_UTILS("+++ENTERING\n");
    do
    {
        for (; (!nStartFlag) && (nInBytePosition < nTotalInBytes - 3); )
        {
           if (VIDDEC_GetBits(&nBitPosition, 24, pHeaderStream, OMX_FALSE) != 0x000001)
           {
                nBitPosition += 8;
                nInBytePosition++;
           }
           else
           {
               nStartFlag = OMX_TRUE;
               nBitPosition += 24;
               nInBytePosition += 3;
           }
        }
        if (!nStartFlag)
        {
            eError = OMX_ErrorStreamCorrupt;
            goto EXIT;
        }
        nNalUnitType = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        nInBytePosition++;
        if (nNalUnitType != 0x0f && nNalUnitType != 0x0e)
        {
            nStartFlag = OMX_FALSE;
        }
    }while (nNalUnitType != 0x0f && nNalUnitType != 0x0e);

    if (nNalUnitType == 0x0f || nNalUnitType == 0x0e)
    {
        nProfile   = VIDDEC_GetBits(&nBitPosition, 2, pHeaderStream, OMX_TRUE);
        nLevel     = VIDDEC_GetBits(&nBitPosition, 3, pHeaderStream, OMX_TRUE);
        nTempValue = VIDDEC_GetBits(&nBitPosition, 11, pHeaderStream, OMX_TRUE);
        nTempValue = VIDDEC_GetBits(&nBitPosition, 12, pHeaderStream, OMX_TRUE);
        (*nWidth)  = (nTempValue * 2) + 2;
        nTempValue = VIDDEC_GetBits(&nBitPosition, 12, pHeaderStream, OMX_TRUE);
        (*nHeight) = (nTempValue * 2) + 2;













        eError     = OMX_ErrorNone;
    }

EXIT:
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ParseVideo_WMV9_RCV                               */
/*  Description   : This file Parse WMV9 RCV.                                */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_ParseVideo_WMV9_RCV(OMX_U32    		  *nWidth,
										 OMX_U32   		      *nHeight,
										 OMX_BUFFERHEADERTYPE *pBuffHead)
{
    OMX_ERRORTYPE eError 		= OMX_ErrorUndefined;
    OMX_U32       nTempValue 	= 0;
    OMX_U8*       pTempValue 	= 0;
    OMX_U32       Profile		= 0;
    OMX_U32       nBitPosition 	= 0;
    OMX_U8*       pHeaderStream = (OMX_U8*)pBuffHead->pBuffer;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if (pBuffHead->nFilledLen >= 20)
    {
        nTempValue = VIDDEC_GetBits(&nBitPosition, 32, pHeaderStream, OMX_TRUE);
        nTempValue = VIDDEC_GetBits(&nBitPosition, 32, pHeaderStream, OMX_TRUE);
        Profile    = VIDDEC_GetBits(&nBitPosition, 4, pHeaderStream, OMX_TRUE);
        nTempValue = VIDDEC_GetBits(&nBitPosition, 28, pHeaderStream, OMX_TRUE);

        pTempValue    = (OMX_U8*)&nTempValue;
        pTempValue[0] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        pTempValue[1] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        pTempValue[2] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        pTempValue[3] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        (*nHeight)    = nTempValue;

        pTempValue[0] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        pTempValue[1] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        pTempValue[2] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        pTempValue[3] = VIDDEC_GetBits(&nBitPosition, 8, pHeaderStream, OMX_TRUE);
        (*nWidth)     = nTempValue;
        eError = OMX_ErrorNone;
    }
    else
    {
        (*nWidth) = 0;
        (*nHeight) = 0;
        eError = OMX_ErrorUndefined;
    }
	LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ParseVideo_MPEG4                                  */
/*  Description   : This file Parse MPEG4 File Format.                       */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_ParseVideo_MPEG4(OMX_U32* nWidth, OMX_U32* nHeight,
									  OMX_BUFFERHEADERTYPE *pBuffHead)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_U32       nSartCode = 0;
    OMX_U32       nBitPosition = 0;
    OMX_BOOL      bHeaderParseCompleted = OMX_FALSE;
    OMX_BOOL      bFillHeaderInfo = OMX_FALSE;
    OMX_U8*       pHeaderStream = (OMX_U8*)pBuffHead->pBuffer;

    VIDDEC_MPEG4_ParserParam            MPEG4_Param;
    VIDDEC_MPEG4UncompressedVideoFormat iOutputFormat;
    VIDDEC_MPEG4_ParserParam            *sMPEG4_Param = &MPEG4_Param;
    VIDDEC_VideoPictureHeader           sPictHeaderDummy;
    VIDDEC_MPEG4VisualVOLHeader         sVolHeaderDummy;
    VIDDEC_VideoPictureHeader           *pPictHeaderPtr = &sPictHeaderDummy;
    VIDDEC_MPEG4VisualVOLHeader         *sVolHeaderPtr = &sVolHeaderDummy;

    pPictHeaderPtr->cnOptional = (OMX_U8*)malloc(
									sizeof(VIDDEC_MPEG4VisualVOLHeader));
	LOG_STATUS_UTILS("+++ENTERING\n");
    while (!bHeaderParseCompleted)
    {
        nSartCode = VIDDEC_GetBits(&nBitPosition, 32, pHeaderStream, OMX_TRUE);
        if (nSartCode == 0x1B0)
        {
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
            pPictHeaderPtr->nProfile = VIDDEC_GetBits(&nBitPosition, 4,
													pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
            pPictHeaderPtr->nLevel = VIDDEC_GetBits(&nBitPosition, 4,
													pHeaderStream, OMX_TRUE);
        }
        else if (nSartCode == 0x1B5)
        {
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nIsVisualObjectIdentifier = VIDDEC_GetBits(
									&nBitPosition, 1, pHeaderStream, OMX_TRUE);

            if (sMPEG4_Param->nIsVisualObjectIdentifier)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 7);

                /* DISCARD THIS INFO (7 bits) */
                (void)VIDDEC_GetBits(&nBitPosition, 7, pHeaderStream, OMX_TRUE);
            }
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
            sMPEG4_Param->nVisualObjectType = VIDDEC_GetBits(&nBitPosition, 4,
										      pHeaderStream, OMX_TRUE);

            if((sMPEG4_Param->nVisualObjectType== 1) ||
			   (sMPEG4_Param->nVisualObjectType== 2))
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                sMPEG4_Param->nVideoSignalType = VIDDEC_GetBits(&nBitPosition,
													1, pHeaderStream, OMX_TRUE);
                if (sMPEG4_Param->nVideoSignalType)
                {
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 3);

                    sMPEG4_Param->nVideoFormat = VIDDEC_GetBits(
								&nBitPosition, 3, pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

                    sMPEG4_Param->nVideoRange = VIDDEC_GetBits(
							&nBitPosition, 1, pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

                    sMPEG4_Param->nColorDescription = VIDDEC_GetBits(
							&nBitPosition, 1, pHeaderStream, OMX_TRUE);
                    if (sMPEG4_Param->nColorDescription)
                    {
                        /*Discard this info*/
                        OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 24);
                        (void)VIDDEC_GetBits(&nBitPosition, 24, pHeaderStream,
											 OMX_TRUE);
                    }
                }
            }
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->NBitZero = VIDDEC_GetBits(&nBitPosition, 1,
													pHeaderStream, OMX_TRUE);
            while ((nBitPosition%8)!= 0)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

                /* discard align bits */
                (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            }
        }
        else if ((nSartCode >= 0x100)&&(nSartCode <= 0x11F))

        {
            /*Do nothing*/
        }
        else if (nSartCode == 0x1B3) /*GOV*/
        {
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 20);
            (void)VIDDEC_GetBits(&nBitPosition, 20, pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->NBitZero = VIDDEC_GetBits(&nBitPosition, 1,
												pHeaderStream, OMX_TRUE);
            while ((nBitPosition%8)!= 0)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

                /* discard align bits */
                (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            }
        }
        else if (nSartCode == 0x1B2) /* user data */
        {
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 24);
            while (VIDDEC_GetBits(&nBitPosition, 24, pHeaderStream,
														OMX_TRUE)!= 0x1)
                nBitPosition-=16; /* discard only 8 bits and try againg until */
                                  /* the next start code is found 			  */
            nBitPosition -=24;    /* prepare to read the entire start code    */
        }
        else if ((nSartCode >= 0x120) && (nSartCode <= 0x12F))
        {
            sVolHeaderPtr->nVideoObjectLayerId = nSartCode&0x0000000f;
            sVolHeaderPtr->bShortVideoHeader = 0;
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            pPictHeaderPtr->bIsRandomAccessible = VIDDEC_GetBits(&nBitPosition,
													1, pHeaderStream, OMX_TRUE);

            sVolHeaderPtr->bRandomAccessibleVOL =
								pPictHeaderPtr->bIsRandomAccessible;

            if (pPictHeaderPtr->bIsRandomAccessible)
            {
                /* it seems this never happens */
            }
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 8);
            sMPEG4_Param->nVideoObjectTypeIndication = VIDDEC_GetBits(
							&nBitPosition, 8, pHeaderStream, OMX_TRUE);

            sVolHeaderPtr->nVideoObjectTypeIndication =
							sMPEG4_Param->nVideoObjectTypeIndication;
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

            sMPEG4_Param->nIsVisualObjectLayerIdentifier = VIDDEC_GetBits(
							&nBitPosition, 1, pHeaderStream, OMX_TRUE);

            sVolHeaderPtr->nVideoObjectLayerId =
							sMPEG4_Param->nIsVisualObjectLayerIdentifier;
            sMPEG4_Param->nLayerVerId = 0;

            if (sMPEG4_Param->nIsVisualObjectLayerIdentifier)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
                sMPEG4_Param->nLayerVerId = VIDDEC_GetBits(&nBitPosition,
												4, pHeaderStream, OMX_TRUE);

                sVolHeaderPtr->nVideoObjectLayerVerId =
											sMPEG4_Param->nLayerVerId;
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 3);

                sMPEG4_Param->nLayerPriority = VIDDEC_GetBits(
								&nBitPosition, 3, pHeaderStream, OMX_TRUE);
                sVolHeaderPtr->nVideoObjectLayerPriority =
											sMPEG4_Param->nLayerPriority;
            }

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
            sMPEG4_Param->nAspectRadio = VIDDEC_GetBits(&nBitPosition, 4,
										 pHeaderStream, OMX_TRUE);

            if (sMPEG4_Param->nAspectRadio == 0xf)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 8);
                sMPEG4_Param->nParWidth = VIDDEC_GetBits(&nBitPosition, 8,
										pHeaderStream, OMX_TRUE);
                sVolHeaderPtr->nAspectRatioNum = sMPEG4_Param->nParWidth;
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 8);
                sMPEG4_Param->nParHeight = VIDDEC_GetBits(&nBitPosition, 8,
										pHeaderStream, OMX_TRUE);
                sVolHeaderPtr->nAspectRatioDenom = sMPEG4_Param->nParHeight;
            }
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nControlParameters = VIDDEC_GetBits(&nBitPosition, 1,
									pHeaderStream, OMX_TRUE);

            if (sMPEG4_Param->nControlParameters)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 2);
                sMPEG4_Param->nChromaFormat = VIDDEC_GetBits(&nBitPosition, 2,
							pHeaderStream, OMX_TRUE);

                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                sMPEG4_Param->nLowDelay = VIDDEC_GetBits(&nBitPosition, 1,
							pHeaderStream, OMX_TRUE);

                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                sMPEG4_Param->nVbvParameters = VIDDEC_GetBits(&nBitPosition, 1,
							pHeaderStream, OMX_TRUE);

                if (sMPEG4_Param->nVbvParameters)
                {
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 15);
                    sMPEG4_Param->nBitRate = VIDDEC_GetBits(&nBitPosition, 15,
							pHeaderStream, OMX_TRUE)<<15;
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                    (void)VIDDEC_GetBits(&nBitPosition, 1,
								pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 15);
                    sMPEG4_Param->nBitRate |= VIDDEC_GetBits(&nBitPosition, 15,
												pHeaderStream, OMX_TRUE);
                    sVolHeaderPtr->sVbvParams.nBitRate = sMPEG4_Param->nBitRate;
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                    (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream,
														OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 15);
                    sMPEG4_Param->nFirstHalfVbvBufferSize =
                    VIDDEC_GetBits(&nBitPosition, 15, pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                    (void)VIDDEC_GetBits(&nBitPosition, 1,
												pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 3);
                    sMPEG4_Param->nLatterHalfVbvBufferSize =
					VIDDEC_GetBits(&nBitPosition, 3, pHeaderStream, OMX_TRUE);

                    sVolHeaderPtr->sVbvParams.nVbvBufferSize =
                        (((sMPEG4_Param->nFirstHalfVbvBufferSize) << 3) +
                           sMPEG4_Param->nLatterHalfVbvBufferSize) * 2048;
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 11);

                    sMPEG4_Param->nFirstHalfVbvOccupancy =
                    VIDDEC_GetBits(&nBitPosition, 11, pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

                    (void)VIDDEC_GetBits(&nBitPosition, 1,
													pHeaderStream, OMX_TRUE);
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 15);

                    sMPEG4_Param->nLatterHalfVbvOccupancy =
                    VIDDEC_GetBits(&nBitPosition, 15, pHeaderStream, OMX_TRUE);
                    sVolHeaderPtr->sVbvParams.nVbvOccupancy =
                        (((sMPEG4_Param->nFirstHalfVbvOccupancy) << 15) +
                           sMPEG4_Param->nLatterHalfVbvOccupancy) * 2048;
                    OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

                    (void)VIDDEC_GetBits(&nBitPosition, 1,
											pHeaderStream, OMX_TRUE);
                }
                else
                {
                    sMPEG4_Param->nBitRate = 0;
                }
            }
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 2);
            sMPEG4_Param->nLayerShape = VIDDEC_GetBits(&nBitPosition, 2,
													pHeaderStream, OMX_TRUE);

            /* skip one marker_bit */
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 16);
            sMPEG4_Param->nTimeIncrementResolution =
					VIDDEC_GetBits(&nBitPosition, 16, pHeaderStream, OMX_TRUE);
            sVolHeaderPtr->nVOPTimeIncrementResolution =
										sMPEG4_Param->nTimeIncrementResolution;

            /* skip one market bit */
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nFnXedVopRate =
				VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            sVolHeaderPtr->bnFnXedVopRate = sMPEG4_Param->nFnXedVopRate;
            if (sMPEG4_Param->nFnXedVopRate)
            {
                sMPEG4_Param->nNum_bits =
					GET_NUM_BIT_REQ (sMPEG4_Param->nTimeIncrementResolution);
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition,
										sMPEG4_Param->nNum_bits);
                sVolHeaderPtr->nFnXedVOPTimeIncrement =
						VIDDEC_GetBits (&nBitPosition, sMPEG4_Param->nNum_bits,
										pHeaderStream, OMX_TRUE);
            }

            /* skip one market bit */
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 13);
            (*nWidth) = VIDDEC_GetBits(&nBitPosition, 13, pHeaderStream,
									   OMX_TRUE);

            /* skip one market bit */
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 13);
            (*nHeight) = VIDDEC_GetBits(&nBitPosition, 13, pHeaderStream,
										OMX_TRUE);

            /* skip one market bit */
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);                                /*1 bit*/
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

            sMPEG4_Param->nInterlaced = VIDDEC_GetBits(&nBitPosition, 1,
										        pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);

            sMPEG4_Param->nObmc = VIDDEC_GetBits(&nBitPosition, 1,
										pHeaderStream, OMX_TRUE);
            if (sMPEG4_Param->nLayerVerId)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                sMPEG4_Param->NSpriteNotSupported =
					VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
                if (sMPEG4_Param->NSpriteNotSupported)
                {
                }
            }
            else
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 2);
                sMPEG4_Param->NSpriteNotSupported =
					VIDDEC_GetBits(&nBitPosition, 2, pHeaderStream, OMX_TRUE);
                if (sMPEG4_Param->NSpriteNotSupported)
                {
                }
            }
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nNot8Bit = VIDDEC_GetBits(&nBitPosition, 1,
									pHeaderStream, OMX_TRUE);
            sMPEG4_Param->nQuantPrecision = 5;
            sMPEG4_Param->nBitsPerPnXel = 8;

            if (sMPEG4_Param->nNot8Bit)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
                sMPEG4_Param->nQuantPrecision =
                VIDDEC_GetBits(&nBitPosition,4, pHeaderStream, OMX_TRUE);
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 4);
				sMPEG4_Param->nBitsPerPnXel =
					VIDDEC_GetBits(&nBitPosition,4, pHeaderStream, OMX_TRUE);
            }

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nIsInverseQuantMethodFirst =
				VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);

            if (sMPEG4_Param->nLayerVerId !=1)
            {
                /* does not support quater sample */
                /* kip one market bit	          */
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                (void)VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            }

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nComplexityEstimationDisable =
					VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nIsResyncMarkerDisabled =
					VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nIsDataPartitioned =
					VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
            sVolHeaderPtr->bDataPartitioning = sMPEG4_Param->nIsDataPartitioned;
            if (sMPEG4_Param->nIsDataPartitioned)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
                sMPEG4_Param->nRvlc =
                VIDDEC_GetBits(&nBitPosition, 1, pHeaderStream, OMX_TRUE);
                sVolHeaderPtr->bReversibleVLC = sMPEG4_Param->nRvlc;
                if (sMPEG4_Param->nRvlc)
                {
                }
            }

            if (sMPEG4_Param->nLayerVerId !=1)
            {
                OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 2);
                (void)VIDDEC_GetBits(&nBitPosition, 2, pHeaderStream, OMX_TRUE);
            }

            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 1);
            sMPEG4_Param->nScalability = VIDDEC_GetBits(&nBitPosition, 1,
												pHeaderStream, OMX_TRUE);

            if (iOutputFormat.iYuvFormat.iPattern == 0x00000001)
            {
                pPictHeaderPtr->nPostDecoderBufferSize =
						(*nHeight) * (*nWidth) * 3 / 2;    /* YUV 420 Planar */
			}
            else if (iOutputFormat.iYuvFormat.iPattern == 0x00000008)
            {
                pPictHeaderPtr->nPostDecoderBufferSize =
						(*nHeight) * (*nWidth) * 2;    /* YUV 422 Interleaved */
			}

            pPictHeaderPtr->nOptions |= 0x00000008;

            if(bFillHeaderInfo)
            {
                ;
            }
            bHeaderParseCompleted = OMX_TRUE;
            eError = OMX_ErrorNone;
        }
        else if ( (nSartCode&0xfffffc00) == 0x00008000 )
        {
            sVolHeaderPtr->bShortVideoHeader = 1;
            /* discard 3 bits for split_screen_indicator, 				  */
            /* document_camera_indicator and full_picture_freeze_release  */
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 3);
            (void)VIDDEC_GetBits(&nBitPosition, 3, pHeaderStream, OMX_TRUE);
            OMX_PARSER_CHECKLIMIT(nTotalInBytes, nBitPosition, 3);
            sMPEG4_Param->nSourceFormat = VIDDEC_GetBits(&nBitPosition, 3,
													pHeaderStream, OMX_TRUE);
            if (sMPEG4_Param->nSourceFormat == 0x1)
            {
                (*nWidth) = 128;
                (*nHeight) = 96;
            }
            else if (sMPEG4_Param->nSourceFormat == 0x2)
            {
                (*nWidth) = 176;
                (*nHeight) = 144;
            }
            else if (sMPEG4_Param->nSourceFormat == 0x3)
            {
                (*nWidth) = 352;
                (*nHeight) = 288;
            }
            else if (sMPEG4_Param->nSourceFormat == 0x4)
            {
                (*nWidth) = 704;
                (*nHeight) = 576;
            }
            else if (sMPEG4_Param->nSourceFormat == 0x5)
            {
                (*nWidth) = 1408;
                (*nHeight) = 1152;
            }
            else if (sMPEG4_Param->nSourceFormat == 0x7)
            {
                sMPEG4_Param->nUFEP = VIDDEC_GetBits(&nBitPosition, 3,
												pHeaderStream, OMX_TRUE);
                if(sMPEG4_Param->nUFEP == 1)
                {
                    sMPEG4_Param->nSourceFormat = VIDDEC_GetBits(&nBitPosition,
									3, pHeaderStream, OMX_TRUE);

                    if (sMPEG4_Param->nSourceFormat == 0x1)
                    {
                        (*nWidth) = 128;
                        (*nHeight) = 96;
                    }
                    else if (sMPEG4_Param->nSourceFormat == 0x2)
                    {
                        (*nWidth) = 176;
                        (*nHeight) = 144;
                    }
                    else if (sMPEG4_Param->nSourceFormat == 0x3)
                    {
                        (*nWidth) = 352;
                        (*nHeight) = 288;
                    }
                    else if (sMPEG4_Param->nSourceFormat == 0x4)
                    {

                        (*nWidth) = 704;
                        (*nHeight) = 576;
                    }
                    else if (sMPEG4_Param->nSourceFormat == 0x5)
                    {
                        (*nWidth) = 1408;
                        (*nHeight) = 1152;
                    }
                    else if (sMPEG4_Param->nSourceFormat == 0x6)
                    {
                        (void)VIDDEC_GetBits(&nBitPosition, 24,
												pHeaderStream, OMX_TRUE);

                        sMPEG4_Param->nCPM = VIDDEC_GetBits(&nBitPosition, 1,
												pHeaderStream, OMX_TRUE);

                        if(sMPEG4_Param->nCPM)
                            (void)VIDDEC_GetBits(&nBitPosition, 2,
												pHeaderStream, OMX_TRUE);

                        (void)VIDDEC_GetBits(&nBitPosition, 4,
												pHeaderStream, OMX_TRUE);

                        sMPEG4_Param->nPWI = VIDDEC_GetBits(&nBitPosition, 9,
													pHeaderStream, OMX_TRUE);
                        (*nWidth) = (sMPEG4_Param->nPWI + 1)*4;

                        (void)VIDDEC_GetBits(&nBitPosition, 1,
													pHeaderStream, OMX_TRUE);

                        sMPEG4_Param->nPHI = VIDDEC_GetBits(&nBitPosition, 9,
												pHeaderStream, OMX_TRUE);
                        (*nHeight) = sMPEG4_Param->nPHI*4;

                    }
                    else if (sMPEG4_Param->nSourceFormat == 0x7)
                    {
                        sMPEG4_Param->nSourceFormat =
								VIDDEC_GetBits(&nBitPosition, 3,
											pHeaderStream, OMX_TRUE);

                        (*nWidth) = 1408;
                        (*nHeight) = 1152;
                    }
                    else
                    {
                        eError = OMX_ErrorUnsupportedSetting;
                        goto EXIT;
                    }
                }
            }
            else
            {
                eError = OMX_ErrorUnsupportedSetting;
                goto EXIT;
            }
            bHeaderParseCompleted = OMX_TRUE;
            eError = OMX_ErrorNone;
        }
        else
        {
            eError = OMX_ErrorUnsupportedSetting;
            goto EXIT;
        }
    }

EXIT:
    if(pPictHeaderPtr->cnOptional != NULL)
    {
            free( pPictHeaderPtr->cnOptional);
            pPictHeaderPtr->cnOptional = NULL;
    }
    LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ScanConfigBufferAVC                               */
/*  Description   : Use to scan buffer for certain patter. Used to know      */
/*					if ConfigBuffers are together                            */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
static OMX_U32 VIDDEC_ScanConfigBufferAVC(OMX_BUFFERHEADERTYPE *pBuffHead,
										  OMX_U32              pattern)
{
    OMX_U32 nBitPosition = 0;
    OMX_U32 nInBytePosition = 0;
    OMX_U32 nPatternCounter = 0;
    OMX_U32 nTotalInBytes = pBuffHead->nFilledLen;
    OMX_U8  *nBitStream = (OMX_U8*)pBuffHead->pBuffer;
	LOG_STATUS_UTILS("+++ENTERING\n");
    while (nInBytePosition < nTotalInBytes - 3)
    {
         if (VIDDEC_GetBits(&nBitPosition, 24, nBitStream, OMX_FALSE) != pattern)
         {
              nBitPosition += 8;
              nInBytePosition++;
         }
         else
         {
             /* Pattern found; add count */
             nPatternCounter++;
             nBitPosition += 24;
             nInBytePosition += 3;
         }
    }
    LOG_STATUS_UTILS("---EXITING\n");
    return nPatternCounter;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ParseVideo_H264                                   */
/*  Description   : This file Parse H264.                                    */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_ParseVideo_H264(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                     OMX_BUFFERHEADERTYPE     *pBuffHead,
                                     OMX_U32                  *nWidth,
                                     OMX_U32                  *nHeight,
                                     OMX_U32                  *nCropWidth,
                                     OMX_U32                  *nCropHeight,
                                     OMX_U32 	              nType)
{
    OMX_ERRORTYPE          eError = OMX_ErrorBadParameter;
    OMX_U32                i = 0;
    OMX_BOOL               nStartFlag = OMX_FALSE;
    OMX_U32                nBitPosition = 0;
    OMX_U32 			   nRbspPosition = 0;
    OMX_U32 			   nTotalInBytes = 0;
    OMX_U32 		       nInBytePosition = 0;
    OMX_U32				   nInPositionTemp = 0;
    OMX_U32 			   nNumOfBytesInRbsp = 0;
    OMX_U32 			   nNumBytesInNALunit = 0;
    OMX_U8				   *nBitStream = 0;
    OMX_U32 			   nNalUnitType = 0;
    OMX_U8			       *nRbspByte = NULL;
    OMX_U8 				   *pDataBuf;
    VIDDEC_AVC_ParserParam *sParserParam = NULL;
    static OMX_U32         nConfigBufferCounter; /* counter used for          */
                                                 /* fragmentation of Config   */
                                                 /* Buffer Code               */

    nTotalInBytes = pBuffHead->nFilledLen;
    nBitStream = (OMX_U8*)pBuffHead->pBuffer;
    nRbspByte = (OMX_U8*)malloc(nTotalInBytes);
	LOG_STATUS_UTILS("+++ENTERING\n");
    if (nRbspByte == NULL)
    {
        eError =  OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    memset(nRbspByte, 0x0, nTotalInBytes);
    sParserParam = (VIDDEC_AVC_ParserParam *)malloc(sizeof(VIDDEC_AVC_ParserParam));
    if (sParserParam == NULL)
    {
        eError =  OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    if (nType == 0)
    {
        /* Start of Handle fragmentation of Config Buffer  Code          */
        /* Scan for 2 "0x000001", requiered on buffer to parser properly */
        nConfigBufferCounter += VIDDEC_ScanConfigBufferAVC(pBuffHead, 0x000001);
        if(nConfigBufferCounter < 2)
        {
			/* If less of 2 we need to store the data internally to later */
			/* assembly the complete ConfigBuffer                         */
            /* Set flag to False, the Config Buffer is not complete       */
            LOG_STATUS_UTILS(
							"Setting bConfigBufferCompleteAVC = OMX_FALSE");

            pComponentPrivate->bConfigBufferCompleteAVC = OMX_FALSE;

            /* Malloc Buffer if is not created yet, use Port  buffer size */
            if (pComponentPrivate->pInternalConfigBufferAVC != NULL)
            {
                free(pComponentPrivate->pInternalConfigBufferAVC);
                pComponentPrivate->pInternalConfigBufferAVC = NULL;
            }
            if (pComponentPrivate->pInternalConfigBufferAVC == NULL)
            {
                pComponentPrivate->pInternalConfigBufferAVC =
						malloc(pComponentPrivate->pInPortDef->nBufferSize);
                if(pComponentPrivate->pInternalConfigBufferAVC == NULL)
                {
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
            }
            /* Check if memcpy is safe*/
            if(pComponentPrivate->pInPortDef->nBufferSize >=
					(pComponentPrivate->nInternalConfigBufferFilledAVC +
					 pBuffHead->nFilledLen))
            {
                /* Append current buffer data to Internal Config Buffer */
                if(memcpy((OMX_U8*)pComponentPrivate->pInternalConfigBufferAVC +
						pComponentPrivate->nInternalConfigBufferFilledAVC,
                        pBuffHead->pBuffer,
                        pBuffHead->nFilledLen) == NULL)
                {
					eError = OMX_ErrorInsufficientResources;
					goto EXIT;
                }
            }
            else
            {
                eError =OMX_ErrorInsufficientResources;
                goto EXIT;
            }
            /* Update Filled length of Internal Buffer */
            pComponentPrivate->nInternalConfigBufferFilledAVC += pBuffHead->nFilledLen;
            /* Exit with out error */
            eError = OMX_ErrorNone;
            goto EXIT;
        }
        else
        {
			/* We have all the requiered data*/
            LOG_STATUS_UTILS("Setting bConfigBufferCompleteAVC = OMX_TRUE");
            pComponentPrivate->bConfigBufferCompleteAVC = OMX_TRUE;
            /* If we have already Config data of previous buffer, we assembly */
            /* the final ConfigBuffer										  */
            if(pComponentPrivate->pInternalConfigBufferAVC != NULL)
            {
				/* Check if memcpy is safe */
				if(pComponentPrivate->pInPortDef->nBufferSize >=
					(pComponentPrivate->nInternalConfigBufferFilledAVC +
					pBuffHead->nFilledLen))
				{
					/* The current data of the Buffer has to be placed at the */
					/* end of buffer										  */
					if(memmove((OMX_U8 *)(pBuffHead->pBuffer +
						pComponentPrivate->nInternalConfigBufferFilledAVC),
						pBuffHead->pBuffer,
						pBuffHead->nFilledLen) == NULL)
					{
						eError = OMX_ErrorInsufficientResources;
						goto EXIT;
					}
					/*  The data internally stored has to be put at the   */
					/*  beginning of the buffer						      */
					if(memcpy(pBuffHead->pBuffer,
					pComponentPrivate->pInternalConfigBufferAVC,
					pComponentPrivate->nInternalConfigBufferFilledAVC) == NULL)
					{
						eError = OMX_ErrorInsufficientResources;
						goto EXIT;
					}
				}
				else
				{
					eError = OMX_ErrorInsufficientResources;
					goto EXIT;
				}

				/* Update filled length of current buffer */

				pBuffHead->nFilledLen =
						pComponentPrivate->nInternalConfigBufferFilledAVC +
						pBuffHead->nFilledLen;

				/* Free Internal Buffer used to temporarly hold the data */
				if (pComponentPrivate->pInternalConfigBufferAVC != NULL)
				{
					free(pComponentPrivate->pInternalConfigBufferAVC);
				}
				/* Reset Internal Variables */
				pComponentPrivate->pInternalConfigBufferAVC = NULL;
				pComponentPrivate->nInternalConfigBufferFilledAVC = 0;
				nConfigBufferCounter = 0;

				/* Update Buffer Variables before parsing */
				nTotalInBytes = pBuffHead->nFilledLen;
				if(nRbspByte != NULL)
				{
					free(nRbspByte);
				}

				nRbspByte = (OMX_U8*)malloc(nTotalInBytes);
				if(nRbspByte == NULL)
				{
					eError = OMX_ErrorInsufficientResources;
					goto EXIT;
				}
				memset(nRbspByte, 0x0, nTotalInBytes);
				/*Buffer ready to be parse */
            }
        }
         /* End of Handle fragmentation Config Buffer Code*/
        do{
            for (; (!nStartFlag) && (nInBytePosition < nTotalInBytes - 3); )
            {
               if (VIDDEC_GetBits(&nBitPosition, 24, nBitStream, OMX_FALSE) !=
								0x000001)
               {
                    nBitPosition += 8;
                    nInBytePosition++;
               }
               else
               {
                   /* Start Code found */
                   nStartFlag = OMX_TRUE;
                   nBitPosition += 24;
                   nInBytePosition += 3;
               }
            }

            nStartFlag = OMX_FALSE;
            /* offset to NumBytesInNALunit*/
            nNumBytesInNALunit = nInBytePosition;
            sParserParam->nBitPosTemp = nBitPosition;

            for (;(!nStartFlag)&&(nNumBytesInNALunit < nTotalInBytes-3); )
            {
                if (VIDDEC_GetBits(&sParserParam->nBitPosTemp, 24,
									nBitStream, OMX_FALSE) != 0x000001)
                /* find start code */
                {
                    sParserParam->nBitPosTemp += 8;
                    nNumBytesInNALunit++;
                }
                else
                {
                   /* Start Code found */
                    nStartFlag = OMX_TRUE;
                    sParserParam->nBitPosTemp += 24;
                    nNumBytesInNALunit += 3;
                }
            }

            if (!nStartFlag)
            {
                eError = OMX_ErrorStreamCorrupt;
                goto EXIT;
            }
            /* forbidden_zero_bit */
            sParserParam->nForbiddenZeroBit = VIDDEC_GetBits(&nBitPosition,
													1, nBitStream, OMX_TRUE);
            /* nal_ref_idc */
            sParserParam->nNalRefIdc = VIDDEC_GetBits(&nBitPosition, 2,
														nBitStream, OMX_TRUE);
            /* nal_unit_type */
            nNalUnitType = VIDDEC_GetBits(&nBitPosition, 5,
														nBitStream, OMX_TRUE);
            nInBytePosition++;

            /* This code is to ensure we will get parameter info */
            if (nNalUnitType != 7)
            {
                LOG_STATUS_UTILS("nal_unit_type does not specify parameter information need to look for next startcode\n");
                nStartFlag = OMX_FALSE;
            }
        }while (nNalUnitType != 7);
    }
    else
    {
		pDataBuf = (OMX_U8*)nBitStream;
		do
		{
			if (pComponentPrivate->H264BitStreamFormat == 1 ||
				pComponentPrivate->H264BitStreamFormat == 2 ||
				pComponentPrivate->H264BitStreamFormat == 4)
			{
				/* We have all the requiered data*/
				LOG_STATUS_UTILS("Setting bConfigBufferCompleteAVC = OMX_TRUE");
				pComponentPrivate->bConfigBufferCompleteAVC = OMX_TRUE;
			}
			/* iOMXComponentUsesNALStartCodes is set to OMX_FALSE on opencore */
			if (pComponentPrivate->H264BitStreamFormat == 1)
			{
				if (pComponentPrivate->bIsNALBigEndian)
				{
					nNumBytesInNALunit = (OMX_U32)pDataBuf[nInBytePosition];
				}
				else
				{
					nNumBytesInNALunit = (OMX_U32)pDataBuf[nInBytePosition];
				}
			}
			else if (pComponentPrivate->H264BitStreamFormat == 2)
			{
				if (pComponentPrivate->bIsNALBigEndian)
				{
					nNumBytesInNALunit =
							(OMX_U32)pDataBuf[nInBytePosition] << 8 |
												pDataBuf[nInBytePosition+1];
				}
				else
				{
					nNumBytesInNALunit =
						(OMX_U32)pDataBuf[nInBytePosition] << 0 |
										pDataBuf[nInBytePosition+1] << 8 ;
				}
			}
			else if (pComponentPrivate->H264BitStreamFormat == 4)
			{
				if (pComponentPrivate->bIsNALBigEndian)
				{
					nNumBytesInNALunit =
						(OMX_U32)pDataBuf[nInBytePosition]<<24 |
							 pDataBuf[nInBytePosition+1] << 16 |
							  pDataBuf[nInBytePosition+2] << 8 |
							  pDataBuf[nInBytePosition+3];
				}
				else
				{
					nNumBytesInNALunit = (OMX_U32)pDataBuf[nInBytePosition]<<0 |
						pDataBuf[nInBytePosition+1] << 8 |
						pDataBuf[nInBytePosition+2] << 16 |
						pDataBuf[nInBytePosition+3]<< 24;
				}
			}
			else
			{
				eError = OMX_ErrorBadParameter;
				goto EXIT;
			}
			nBitPosition     = (nInPositionTemp + nType) * 8;
			nInBytePosition  = nInPositionTemp + nType;
			nInPositionTemp += nNumBytesInNALunit + nType;
			if (nInBytePosition > nTotalInBytes)
			{
				eError = OMX_ErrorBadParameter;
				goto EXIT;
			}
			/* forbidden_zero_bit */
			sParserParam->nForbiddenZeroBit = VIDDEC_GetBits(&nBitPosition,
												1, nBitStream, OMX_TRUE);
			/* nal_ref_idc */
			sParserParam->nNalRefIdc = VIDDEC_GetBits(&nBitPosition, 2,
													nBitStream, OMX_TRUE);
			/* nal_unit_type */
			nNalUnitType = VIDDEC_GetBits(&nBitPosition, 5, nBitStream,
																OMX_TRUE);
			nInBytePosition++;
			/* This code is to ensure we will get parameter info */
			if (nNalUnitType != 7)
			{
				nBitPosition = (nInPositionTemp) * 8;
				nInBytePosition = (nInPositionTemp);

			}
        } while (nNalUnitType != 7);
        nNumBytesInNALunit += 8;/* sum to keep the code flow */

        /* the buffer must had enough space to enter this number */
        nNumBytesInNALunit += nInBytePosition;
    }
    for (i=0; nInBytePosition < (nNumBytesInNALunit - 3); )
    {

        if (((nInBytePosition + 2) < (nNumBytesInNALunit - 3)) &&
            (VIDDEC_GetBits(&nBitPosition, 24, nBitStream, OMX_FALSE) == 0x000003))
        {
            LOG_STATUS_UTILS("discard emulation prev byte\n");
            nRbspByte[i++] = nBitStream[nInBytePosition++];
            nRbspByte[i++] = nBitStream[nInBytePosition++];
            nNumOfBytesInRbsp += 2;
            /* discard emulation prev byte */
            nInBytePosition++;
            nBitPosition += 24;
        }
        else
        {
            nRbspByte[i++] = nBitStream[nInBytePosition++];
            nNumOfBytesInRbsp++;
            nBitPosition += 8;
        }
        if (nInBytePosition >= (nNumBytesInNALunit - 3)) {
            break;
        }
    }

    /* Parse RBSP sequence */
    /*  profile_idc u(8)   */
    sParserParam->nProfileIdc = VIDDEC_GetBits(&nRbspPosition, 8, nRbspByte,
																	OMX_TRUE);
    pComponentPrivate->AVCProfileType = sParserParam->nProfileIdc;
    ALOGD("%s:%d %d", __FUNCTION__, __LINE__, sParserParam->nProfileIdc);

    /* constraint_set0_flag u(1)*/
    sParserParam->nConstraintSet0Flag = VIDDEC_GetBits(&nRbspPosition, 1,
													nRbspByte, OMX_TRUE);
    /* constraint_set1_flag u(1) */
    sParserParam->nConstraintSet1Flag = VIDDEC_GetBits(&nRbspPosition, 1,
													nRbspByte, OMX_TRUE);
    /* constraint_set2_flag u(1) */
    sParserParam->nConstraintSet2Flag = VIDDEC_GetBits(&nRbspPosition, 1,
													nRbspByte, OMX_TRUE);
    /* reserved_zero_5bits u(5) */
    sParserParam->nReservedZero5bits = VIDDEC_GetBits(&nRbspPosition, 5,
													nRbspByte, OMX_TRUE);
    /* level_idc*/
    sParserParam->nLevelIdc = VIDDEC_GetBits(&nRbspPosition, 8,
													nRbspByte, OMX_TRUE);
    pComponentPrivate->nLevelIdc = sParserParam->nLevelIdc;

    sParserParam->nSeqParameterSetId = VIDDEC_UVLC_dec(&nRbspPosition,
																nRbspByte);

	if(100 == sParserParam->nProfileIdc)
	{
		OMX_U32 chroma_format_idc;
		OMX_U32 temp;

	    chroma_format_idc = VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
		if(3 == chroma_format_idc)
		{
			temp = VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
		}
		
		temp = VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
		temp = VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
		temp = VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
		temp = VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
		if(1 == temp)
		{
			for(i = 0; i < 8; i++)
			{
				temp = VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
				if(temp)
				{
					if(i < 6)
						h264_scaling_list(16, &nRbspPosition, nRbspByte);
					else
						h264_scaling_list(64, &nRbspPosition, nRbspByte);
				}
			}
		}
		
	}
	
    sParserParam->nLog2MaxFrameNumMinus4 = VIDDEC_UVLC_dec(&nRbspPosition,
																nRbspByte);
    sParserParam->nPicOrderCntType = VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);

    if(sParserParam->nPicOrderCntType == 0)
    {
        sParserParam->nLog2MaxPicOrderCntLsbMinus4 =
								VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
    }
    else if(sParserParam->nPicOrderCntType == 1)
    {
        /* delta_pic_order_always_zero_flag */
        VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
        sParserParam->nOffsetForNonRefPic = VIDDEC_UVLC_dec(&nRbspPosition,
																	nRbspByte);

        if (sParserParam->nOffsetForNonRefPic > 1)
        {
              sParserParam->nOffsetForNonRefPic =
              sParserParam->nOffsetForNonRefPic & 0x1 ?
					sParserParam->nOffsetForNonRefPic >> 1 :
				  -(sParserParam->nOffsetForNonRefPic >> 1);
		}

        sParserParam->nOffsetForTopToBottomField =
							VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);

        sParserParam->nNumRefFramesInPicOrderCntCycle =
							VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);

        for(i = 0; i < sParserParam->nNumRefFramesInPicOrderCntCycle; i++ )
            VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
    }

    sParserParam->nNumRefFrames = VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
    pComponentPrivate->nNumRefFrames = sParserParam->nNumRefFrames;

    sParserParam->nGapsInFrameNumValueAllowedFlag =
				VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
    sParserParam->nPicWidthInMbsMinus1 =
				VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
    (*nWidth) = (sParserParam->nPicWidthInMbsMinus1 + 1) * 16;
    sParserParam->nPicHeightInMapUnitsMinus1 =
				VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
    (*nHeight) = (sParserParam->nPicHeightInMapUnitsMinus1 + 1) * 16;

    /* Checking for cropping in picture saze */
    /* getting frame_mbs_only_flag */
    sParserParam->nFrameMbsOnlyFlag =
				VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
    if (!sParserParam->nFrameMbsOnlyFlag)
    {
        sParserParam->nMBAdaptiveFrameFieldFlag =
				VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
    }

    /* getting direct_8x8_inference_flag and frame_cropping_flag */
    sParserParam->nDirect8x8InferenceFlag =
				VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
    sParserParam->nFrameCroppingFlag =
				VIDDEC_GetBits(&nRbspPosition, 1, nRbspByte, OMX_TRUE);
    /*getting the crop values if exist*/
    if (sParserParam->nFrameCroppingFlag)
    {
        sParserParam->nFrameCropLeftOffset =
				VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
        sParserParam->nFrameCropRightOffset =
				VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
        sParserParam->nFrameCropTopOffset =
				VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);
        sParserParam->nFrameCropBottomOffset =
				VIDDEC_UVLC_dec(&nRbspPosition, nRbspByte);

        /* Update framesize taking into account the cropping values */
        (*nCropWidth) = (2 * sParserParam->nFrameCropLeftOffset +
						 2 * sParserParam->nFrameCropRightOffset);
        (*nCropHeight) = (2 * sParserParam->nFrameCropTopOffset +
						  2 * sParserParam->nFrameCropBottomOffset);
    }
    eError = OMX_ErrorNone;

EXIT:
    if (nRbspByte != NULL)
    {
        free( nRbspByte);
    }
    if (sParserParam != NULL)
    {
        free( sParserParam);
    }
    LOG_STATUS_UTILS("---EXITING\n");
    return eError;
}

/*****************************************************************************/
/*  Function Name : app_alloc_mem_tables                                     */
/*  Description   : Gets aBits number of bits from position aPosition of one */
/* 					buffer and returns the value in a TUint value.  		 */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_U32 VIDDEC_GetBits(OMX_U32  *nPosition,
					   OMX_U8   nBits,
					   OMX_U8   *pBuffer,
					   OMX_BOOL bIcreasePosition)
{
    OMX_U32 nOutput;
    OMX_U32 nNumBitsRead = 0;
    OMX_U32 nBytePosition = 0;
    OMX_U8  nBitPosition =  0;

    nBytePosition = *nPosition / 8;
    nBitPosition =  *nPosition % 8;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if (bIcreasePosition)
        *nPosition += nBits;
    nOutput = ((OMX_U32)pBuffer[nBytePosition] << (24+nBitPosition) );
    nNumBitsRead = nNumBitsRead + (8 - nBitPosition);

    if (nNumBitsRead < nBits)
    {
        nOutput = nOutput | ( pBuffer[nBytePosition + 1] << (16+nBitPosition));
        nNumBitsRead = nNumBitsRead + 8;
    }
    if (nNumBitsRead < nBits)
    {
        nOutput = nOutput | ( pBuffer[nBytePosition + 2] << (8+nBitPosition));
        nNumBitsRead = nNumBitsRead + 8;
    }
    if (nNumBitsRead < nBits)
    {
        nOutput = nOutput | ( pBuffer[nBytePosition + 3] << (nBitPosition));
        nNumBitsRead = nNumBitsRead + 8;
    }
    nOutput = nOutput >> (32 - nBits) ;
    LOG_STATUS_UTILS("---EXITING\n");
    return nOutput;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_UVLC_dec                                          */
/*  Description   :                                                          */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_S32 VIDDEC_UVLC_dec(OMX_U32 *nPosition, OMX_U8* pBuffer)
{

    OMX_U32 nBytePosition = (*nPosition) / 8;
    OMX_U8 cBitPosition =  (*nPosition) % 8;
    OMX_U32 nLen = 1;
    OMX_U32 nCtrBit = 0;
    OMX_U32 nVal = 1;
    OMX_U32 nInfoBit=0;

    nCtrBit = pBuffer[nBytePosition] & (0x1 << (7-cBitPosition));
    LOG_STATUS_UTILS("+++ENTERING\n");
    while (nCtrBit==0)
    {
        nLen++;
        cBitPosition++;
        (*nPosition)++;
        if (!(cBitPosition%8))
        {
            cBitPosition=0;
            nBytePosition++;
        }
        nCtrBit = pBuffer[nBytePosition] & (0x1<<(7-cBitPosition));
    }
    for(nInfoBit=0; (nInfoBit<(nLen-1)); nInfoBit++)
    {
        cBitPosition++;
        (*nPosition)++;

        if (!(cBitPosition%8))
        {
            cBitPosition=0;
            nBytePosition++;
        }
        nVal=(nVal << 1);
        if(pBuffer[nBytePosition] & (0x01 << (7 - cBitPosition)))
            nVal |= 1;
    }
    (*nPosition)++;
    nVal -= 1;
    LOG_STATUS_UTILS("---EXITING\n");
    return nVal;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_ParseHeader                                       */
/*  Description   : Parse the input buffer to get the correct width and      */
/*					height                                                   */
/*  Inputs        :                                                          */
/*  Globals       :                                                          */
/*  Processing    :                                                          */
/*  Outputs       :                                                          */
/*  Returns       :                                                          */
/*  Issues        :                                                          */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2010   Ittiam          Draft                                */
/*****************************************************************************/
OMX_ERRORTYPE VIDDEC_ParseHeader(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								 OMX_BUFFERHEADERTYPE     *pBuffHead)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 	  nWidth = 0;
    OMX_U32 	  nHeight = 0;
    OMX_U32 	  nPadWidth = 0;
    OMX_U32       nPadHeight = 0;
    OMX_U32       nCropWidth = 0;
    OMX_U32       nCropHeight = 0;
    OMX_U32 	  tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;
	LOG_STATUS_UTILS("+++ENTERING\n");
    if(!pComponentPrivate)
    {
        goto EXIT;
    }
    LOG_STATUS_UTILS("Entering");

    LOG_STATUS_UTILS(
			"pBuffHead %p, Original resolution IN %ldx%ld : OUT %ldx%ld\n",
            pBuffHead,
            pComponentPrivate->pInPortDef->format.video.nFrameWidth,
            pComponentPrivate->pInPortDef->format.video.nFrameHeight,
            pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
            pComponentPrivate->pOutPortDef->format.video.nFrameHeight);

        if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
        {
            eError = VIDDEC_ParseVideo_H264( pComponentPrivate, pBuffHead,
					 &nWidth, &nHeight, &nCropWidth, &nCropHeight,
					 pComponentPrivate->H264BitStreamFormat);

            pComponentPrivate->nCropWidth = nCropWidth;
			pComponentPrivate->nCropHeight = nCropHeight;
            ALOGD("%s : %d Crop width from header = %d, Crop height from header = %d", __func__, __LINE__, pComponentPrivate->nCropWidth, pComponentPrivate->nCropHeight);

			/* Start Code to handle fragmentation of ConfigBuffer for AVC */
            if((OMX_FALSE == pComponentPrivate->bConfigBufferCompleteAVC) &&
               (0 == pComponentPrivate->ProcessMode) &&
               pComponentPrivate->H264BitStreamFormat == 0)
            {
                /* We have received some part of the config Buffer */
                /* Send EmptyThisBuffer of the buffer we have just */
                /* received to Client                              */
                pComponentPrivate->cbInfo.EmptyBufferDone(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						pBuffHead);

				/* Decrease the input buffer from APP count after doing EBD */
				pthread_mutex_lock(&pComponentPrivate->InputBCountAppMutex);
				pComponentPrivate->bInputBCountApp--;
				pthread_mutex_unlock(&pComponentPrivate->InputBCountAppMutex);

                /* Exit with out error to avoid sending again EmptyBufferDone */
                /* in upper function 										  */
                eError = OMX_ErrorNone;
                goto EXIT;

            }
            /* End Code to handle fragmentation of ConfigBuffer for AVC */
        }
        else if(tempCompressionFormat == OMX_VIDEO_CodingMPEG4  ||
                tempCompressionFormat == OMX_VIDEO_CodingH263)
        {
            VIDDEC_ParseVideo_MPEG4( &nWidth, &nHeight, pBuffHead);
        }
        else if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
        {
            if (pComponentPrivate->nWMVFileType == VIDDEC_WMV_ELEMSTREAM)
            {
                eError = VIDDEC_ParseVideo_WMV9_VC1(&nWidth, &nHeight,
																pBuffHead);
            }
            else
            {
                eError = VIDDEC_ParseVideo_WMV9_RCV(&nWidth, &nHeight,
															pBuffHead);
            }
        }

        /* Use default values when zero resolution is parsed */
        if ((eError != OMX_ErrorNone) || (nWidth == 0) || (nHeight == 0))
        {
            if ((nWidth == 0) || (nHeight == 0))
            {
                eError = OMX_ErrorStreamCorrupt;
                pBuffHead->nFlags |= OMX_BUFFERFLAG_DATACORRUPT;
            }
            nWidth  = pComponentPrivate->pInPortDef->format.video.nFrameWidth;
            nHeight =  pComponentPrivate->pInPortDef->format.video.nFrameHeight;
            LOG_STATUS_UTILS("Error while parsering: %x, %ldx%ld",
						eError, nWidth, nHeight);
            eError = OMX_ErrorNone;
            goto EXIT;
        }

        /* For MPeg4 WVGA SN requires that used resolutions be 854		  */
        /* if parser read 864 it needs to be changed to 854               */
        /* nHeigth is changed in order to change portrait resolutions too */
        if (tempCompressionFormat == OMX_VIDEO_CodingMPEG4  ||
            tempCompressionFormat == OMX_VIDEO_CodingH263)
        {
            if ((nWidth == 864) || (nHeight == 864))
            {
                nPadWidth = nWidth;
                nPadHeight = nHeight;
            }
            else
            {
                nPadWidth = VIDDEC_MULTIPLE16 ( nWidth);
                nPadHeight = VIDDEC_MULTIPLE16 ( nHeight);
                if ((nPadWidth == 864))
                {
                    nWidth = 854;
                    nPadWidth = 854;
                }
                if ((nPadHeight == 864))
                {
                    nHeight = 854;
                    nPadHeight = 854;
                }
            }
        }
        else
        {
            nPadWidth = VIDDEC_MULTIPLE16(nWidth);
            nPadHeight = VIDDEC_MULTIPLE16(nHeight);
        }

       	ALOGD("%s : %d Parsed Width = %d, Parsed Height = %d, Crop Width = %d, Crop Height = %d, OutPort WIDTH = %d , OutPort Height %d\n",
	        __func__,
		    __LINE__,
		    nWidth,
		    nHeight,
		    nCropWidth,
		    nCropHeight,
		    pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
        	pComponentPrivate->pOutPortDef->format.video.nFrameHeight);

		/* TODO: Get minimum INPUT buffer size & verify if the actual size */
        /* is enough. Verify correct values in the initial setup           */
        /*	Verify if actual width & height parameters are correct		   */
        /*if (pComponentPrivate->pInPortDef->format.video.nFrameWidth != nWidth ||
            pComponentPrivate->pInPortDef->format.video.nFrameHeight != nHeight)
        {
            LOG_STATUS_UTILS("The Resolution from Parser and the bitstream are different");
	    
       	    ALOGD("%s : %d Parsed Width = %d, Parsed Height = %d, Crop Width = %d, Crop Height = %d, InPort WIDTH %ld : InPort Height %ld\n",
	        __func__,
		    __LINE__,
		    nWidth,
		    nHeight,
		    nCropWidth,
		    nCropHeight,
		    pComponentPrivate->pInPortDef->format.video.nFrameWidth,
        	pComponentPrivate->pInPortDef->format.video.nFrameHeight);

			// Set the input port width, height
			pComponentPrivate->pInPortDef->format.video.nFrameWidth = nWidth;
			pComponentPrivate->pInPortDef->format.video.nFrameHeight = nHeight;

			// Set the output port width, height and buffer size
			pComponentPrivate->pOutPortDef->format.video.nFrameWidth = nWidth;
			pComponentPrivate->pOutPortDef->format.video.nFrameHeight = nHeight;
			pComponentPrivate->pOutPortDef->nBufferSize = 
			pComponentPrivate->pOutPortDef->format.video.nFrameWidth *
		        pComponentPrivate->pOutPortDef->format.video.nFrameHeight *
			((pComponentPrivate->pOutPortFormat->eColorFormat == VIDDEC_COLORFORMAT420) ? 
			VIDDEC_FACTORFORMAT420 : VIDDEC_FACTORFORMAT422);

       	    ALOGD("%s : %d Parsed Width = %d, Parsed Height = %d, Crop Width = %d, Crop Height = %d, InPort WIDTH %ld : InPort Height %ld\n",
	        __func__,
		    __LINE__,
		    nWidth,
		    nHeight,
		    nCropWidth,
		    nCropHeight,
		    pComponentPrivate->pInPortDef->format.video.nFrameWidth,
        	pComponentPrivate->pInPortDef->format.video.nFrameHeight);

			pComponentPrivate->controlCallNeeded = OMX_TRUE;

			// Make callback for indexparamportdefinition for output port
			pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
		                                          pComponentPrivate->pHandle->pApplicationPrivate,
		                                          OMX_EventPortSettingsChanged,
		                                          VIDDEC_OUTPUT_PORT,
		                                          OMX_IndexParamPortDefinition,
		                                          NULL);
        }*/

       	 /* If input port width is not a multiple of 32 and the output port width is ceil_32(input port width), then, make the event handler callback.*/
		if( ((pComponentPrivate->pInPortDef->format.video.nFrameWidth != MULTIPLE_32(pComponentPrivate->pInPortDef->format.video.nFrameWidth)) && 
	    (MULTIPLE_32(pComponentPrivate->pInPortDef->format.video.nFrameWidth) == pComponentPrivate->pOutPortDef->format.video.nFrameWidth)) )
        {
       		ALOGD("%s : %d Parsed Width = %d, Parsed Height = %d, Crop Width = %d, Crop Height = %d, OutPort WIDTH %ld : OutPort Height %ld\n",
	        __func__,
		    __LINE__,
		    nWidth,
		    nHeight,
		    nCropWidth,
		    nCropHeight,
		    pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
        	pComponentPrivate->pOutPortDef->format.video.nFrameHeight);

      		ALOGD("%s : %d Parsed Width = %d, Parsed Height = %d, Crop Width = %d, Crop Height = %d, OutPort WIDTH %ld, OutPort Height %ld\n",
	        __func__,
		    __LINE__,
		    nWidth,
		    nHeight,
		    nCropWidth,
		    nCropHeight,
		    pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
        	pComponentPrivate->pOutPortDef->format.video.nFrameHeight);
	
		// Set the flag to notify ASC thread that additional control call is needed
		pComponentPrivate->controlCallNeeded = OMX_TRUE;

		// Make callback for cropping only
		pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
	                                          pComponentPrivate->pHandle->pApplicationPrivate,
	                                          OMX_EventPortSettingsChanged,
	                                          VIDDEC_OUTPUT_PORT,
	                                          OMX_IndexConfigCommonOutputCrop,
	                                          NULL);		
	}

 	/* If either nCropWidth or nCropHeight parsed from header is non-zero, cropping needs to be done */
	if(nCropWidth != 0 || nCropHeight != 0)
        {
       		ALOGD("%s : %d Parsed Width = %d, Parsed Height = %d, Crop Width = %d, Crop Height = %d, OutPort WIDTH %ld,  OutPort Height %ld\n",
	        __func__,
		    __LINE__,
		    nWidth,
		    nHeight,
		    nCropWidth,
		    nCropHeight,
		    pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
        	pComponentPrivate->pOutPortDef->format.video.nFrameHeight);

		// Set the input port width, height
		pComponentPrivate->pInPortDef->format.video.nFrameWidth = nWidth;
		pComponentPrivate->pInPortDef->format.video.nFrameHeight = nHeight;

			pComponentPrivate->controlCallNeeded = OMX_TRUE;

			// Make callback for cropping only
			pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
		                                          pComponentPrivate->pHandle->pApplicationPrivate,
		                                          OMX_EventPortSettingsChanged,
		                                          VIDDEC_OUTPUT_PORT,
		                                          OMX_IndexConfigCommonOutputCrop,
		                                          NULL);		
		}

EXIT:
    if (pComponentPrivate)
    {
	LOG_STATUS_UTILS("---EXITING\n");
    }
    return eError;
}
#endif /* PARSE_HEADER */	

void h264_scaling_list(OMX_U32 size, OMX_U32 *nRbspPosition, OMX_U8 *nRbspByte)
{
	int i;
	OMX_U32 next_scale = 8;
	OMX_U32 last_scale = 8;
	OMX_S32 delta_scale;

	for(i = 0; i < size; i++)
	{
		if(0 != next_scale)
		{
			delta_scale = VIDDEC_UVLC_dec(nRbspPosition, nRbspByte);
			next_scale = (last_scale + delta_scale + 256) % 256;
		}
		last_scale = (0 == next_scale) ? last_scale : next_scale;
	}

	return;
}
