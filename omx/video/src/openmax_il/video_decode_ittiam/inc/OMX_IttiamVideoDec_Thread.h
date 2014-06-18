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
/*  File Name         : OMX_IttiamVideoDec_Thread.h                          */
/*                                                                           */
/*  Description       : Contains Declarations of functions present in 	     */
/*						OMX_IttiamVideoDec_Thread.c						     */
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

#ifndef OMX_VIDDEC_THREAD__H
#define OMX_VIDDEC_THREAD__H

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
/* The main function which receives the commands, I/O buffers and calls the */
/* appropriate functions to handle them                                     */
void* OMX_VidDec_Thread(void* pThreadData);

/* Empties the I/O buffer pipes by calling the appropriate functions */
void OMX_VidDec_HandlePipes (void* pThreadData);

void OMX_VidDec_Return_except_Freeoutbuf (void* pThreadData);

#endif /* OMX_VIDDEC_THREAD__H */
