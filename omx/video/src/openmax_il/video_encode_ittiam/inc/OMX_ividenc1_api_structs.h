/*****************************************************************************/
/*                                                                           */
/*                              SPLIT ENCODER                                */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2008                             */
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
/*  File Name         : OMX_ividenc1_api_structs.h                           */
/*                                                                           */
/*  Description       : This file contains structure definitions for IVIDENC1*/
/*                      interface                                            */
/*                                                                           */
/*  List of Functions : <List the functions defined in this file>            */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         30 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
#ifndef _OMX_IVIDENC1_API_STRUCTS_H_
#define _OMX_IVIDENC1_API_STRUCTS_H_

/*****************************************************************************/
/* File Includes															 */
/*****************************************************************************/

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/
typedef enum
{
    PROCESSOR_TYPE_GPP   = 0,
    PROCESSOR_TYPE_DSP   = 1
}PROCESSOR_TYPE_T;

/*****************************************************************************/
/* Structure                                                                 */
/*****************************************************************************/
typedef struct
{
    IVIDENC1_Obj    s_obj;
    IVIDENC1_Handle ps_codec_hdl;
}instance_ctxt_t;

/*****************************************************************************/
/* Extern Variable Declarations                                              */
/*****************************************************************************/

/*****************************************************************************/
/* Extern Function Declarations                                              */
/*****************************************************************************/

#endif /* _OMX_IVIDENC1_API_STRUCTS_H_ */
