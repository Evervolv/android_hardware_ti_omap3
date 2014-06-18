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
/*  File Name         : split_dsp_dec.h	                                     */
/*                                                                           */
/*  Description       : 												     */
/*																			 */
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

#ifndef SPLIT_DSP_DEC__H
#define SPLIT_DSP_DEC__H

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
/* Called to initialize the DSP side */
OMX_ERRORTYPE VIDDEC_InitDSPDec (VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

/* Called to load the LCML library */
OMX_ERRORTYPE VIDDEC_LoadLCMLLibrary(OMX_HANDLETYPE phandle);

/* Called to load the DSP side codec */
OMX_ERRORTYPE VIDDEC_LoadCodec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

/* Called to unload the dsp side codec */
OMX_ERRORTYPE VIDDEC_UnloadCodec(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

/* Called to start the DSP side codec instance */
OMX_ERRORTYPE VIDDEC_LCML_Start(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Called to stop the DSP side codec instance */
OMX_ERRORTYPE VIDDEC_LCML_Stop(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Called when state transition to pause happens to pause the codec */
OMX_ERRORTYPE VIDDEC_LCML_Pause(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Called to flush the DSP side */
OMX_ERRORTYPE VIDDEC_LCML_Flush(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								OMX_U32                   portID);

/* Called to Destroy the codec instance */
OMX_ERRORTYPE VIDDEC_LCML_Destroy(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Asynchronous callback from DSP side to ARM side */
OMX_ERRORTYPE VIDDEC_LCML_Callback (TUsnCodecEvent event, void *argsCb [10]);

/* Called by VIDDEC_LCML_Callback when output buffer is produced */
OMX_ERRORTYPE VIDDEC_LCML_Callback_OutputBufferProduced(void *argsCb[10]);

/* Called by VIDDEC_LCML_Callback when output buffer is produced */
OMX_ERRORTYPE VIDDEC_LCML_Callback_InputBufferReleased(void *argsCb[10]);

/* This Function calls the LCML_QueueBuffer function on the free output buffer*/
OMX_ERRORTYPE VIDDEC_LCML_QueueOutputBuffer(
				VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
				OMX_BUFFERHEADERTYPE* pBuffHead, 
				OMX_PTR *pBuffer);

/* allocate auxilliary information for the buffer for extending socket node's */
/* communication structure this is a codec-type dependent allocation          */
OMX_ERRORTYPE VIDDEC_allocate_SN_InputArgument(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							VIDDEC_BUFFER_PRIVATE    *pBufferPrivate);

/* Free auxilliary information structure allocated for the buffer */
OMX_ERRORTYPE VIDDEC_free_SN_InputArgument(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							VIDDEC_BUFFER_PRIVATE    *pBufferPrivate);

#endif
