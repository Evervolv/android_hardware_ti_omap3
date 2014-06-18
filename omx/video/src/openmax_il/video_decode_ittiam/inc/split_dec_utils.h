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
/*  File Name         : split_dec_utils.h                                    */
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

#ifndef __split_dec_utils_h__
#define __split_dec_utils_h__

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#define MIN_ALIGN                  (8)
#define IS_POW2(align)             (((align) & (align -1)) == 0)
#define PTR_ALIGN_POW2(ptr, align) (((int)ptr + align - 1) & ~(align -1))
#define ALIGN_IS_POW2(align)       (((align) & (align -1)) == 0)

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
void *iram_alloc(int i4_size, int i4_align);

int  app_alloc_mem_tables(IALG_MemRec *ps_wkg_mem, int u4_num_mem_tabs);

void codec_free_aligned(void *ptr);

void app_free_mem_tables(IALG_MemRec *ps_wkg_mem, int u4_num_mem_tabs);

void *get_align_ptr(unsigned char *ptr, int i4_size, int i4_align);

void *get_orig_ptr( unsigned char *align_ptr, int *pi4_size);

void *codec_malloc_aligned(int i4_size, int i4_align);

#if CHECK_SUM_ENABLE
void checksum(unsigned char *ptr, int size, VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);
#endif

#endif /* __split_dec_utils_h__ */
