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
/*  File Name         : OMX_ividenc1_utils.h                                 */
/*                                                                           */
/*  Description       : This file contains function prototype of common utils*/
/*                      functions                                            */
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
#ifndef _OMX_IVIDENC1_UTILS_H_
#define _OMX_IVIDENC1_UTILS_H_

/*****************************************************************************/
/* File Includes															 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ittiam_datatypes.h"
#include "std.h"
#include "ialg.h"
#include "xdas.h"
#include "xdm.h"
#include "ivideo.h"
#include "ividenc1.h"

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#define utils_assert    assert
#define utils_strcmp    strcmp
#define utils_printf    printf
#define utils_memcpy    memcpy
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

void *OMX_ividenc1_utils_malloc(WORD32 i4_size, WORD32 i4_align);
void OMX_ividenc1_utils_free(void *ptr);

/*
 *  Return Type: Status
 *      0 : Succesfull memory allocation
 *      1 : Memory can not be allocated
 */
WORD32 OMX_ividenc1_utils_alloc_mem(IALG_MemRec *ps_mem_tab, 
                               WORD32 i4_num_mem_tabs);

void OMX_ividenc1_utils_free_mem(IALG_MemRec *ps_wkg_mem, 
                                 WORD32 i4_num_mem_tabs);

#endif /* _OMX_IVIDENC1_UTILS_H_ */
