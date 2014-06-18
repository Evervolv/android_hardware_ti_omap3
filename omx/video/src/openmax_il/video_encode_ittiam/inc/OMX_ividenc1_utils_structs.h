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
/*  File Name         : OMX_ividenc1_utils_structs.h                         */
/*                                                                           */
/*  Description       : This file contains structure definitions for common  */
/*                      utils functions                                      */
/*                                                                           */
/*  List of Functions : <List the functions defined in this file>            */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         27 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
#ifndef _OMX_IVIDENC1_UTILS_STRUCTS_H_
#define _OMX_IVIDENC1_UTILS_STRUCTS_H_

/*****************************************************************************/
/* File Includes															 */
/*****************************************************************************/

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define ALIGN_POW2(ptr, align)  (((WORD32)ptr + align - 1) & ~(align -1))
#define IS_POW2(align)          (((align) & (align -1)) == 0)
#define MIN_ALIGN               8

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Structure                                                                 */
/*****************************************************************************/

/*****************************************************************************/
/* Extern Variable Declarations                                              */
/*****************************************************************************/

/*****************************************************************************/
/* Extern Function Declarations                                              */
/*****************************************************************************/

#endif /* _OMX_IVIDENC1_UTILS_STRUCTS_H_ */
