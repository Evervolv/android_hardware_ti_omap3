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
/*  File Name         : OMX_ividenc1_utils.c                                 */
/*                                                                           */
/*  Description       : This file contains implementation of certain utils   */
/*                      functions for general purpose                        */
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
/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/

/* System include files */
#include "OMX_ividenc1_utils.h"

/* User include files */
#include "OMX_ividenc1_utils_structs.h"

/*****************************************************************************/
/*Extern Variable Declarations												 */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variable Definitions                                               */
/*****************************************************************************/

/*****************************************************************************/
/* Static Global Variable Definitions                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Static function Definitions                                               */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ividenc1_utils_get_align_ptr                         */
/*                                                                           */
/*  Description   : Aligns a pointer to a specific byte boundary             */
/*                                                                           */
/*  Inputs        :                                                          */
/*  ptr           : Ptr that needs to be aligned                             */
/*  i4_size       :                                                          */
/*  i4_align      : Alignment requirements                                   */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : Same as description                                      */
/*                                                                           */
/*  Outputs       : None                                                     */
/*                                                                           */
/*  Returns       : Aligned pointer                                          */
/*                                                                           */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         13 07 2002   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void *OMX_ividenc1_utils_get_align_ptr(UWORD8 *ptr, WORD32 i4_size, 
                                       WORD32 i4_align)
{
    UWORD8 *align_ptr;
    UWORD8 **orig_ptr_loc;
    WORD32 *size_loc;

    align_ptr = (UWORD8 *)ALIGN_POW2(ptr, i4_align);

    if(align_ptr == ptr)
        align_ptr = ptr + i4_align;

    /* return the align pointer and store the original ptr in the hole */
    /* created in the memory buffer                                    */
    orig_ptr_loc = (UWORD8 **)(align_ptr - 4);
    size_loc     = (WORD32 *)(align_ptr - 8);

    *orig_ptr_loc = ptr;
    *size_loc     = i4_size;

    return align_ptr;
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ividenc1_utils_malloc                                */
/*                                                                           */
/*  Description   : Function used to allocate memory for all application     */
/*                  specific memory requests                                 */
/*  Inputs        : Size of memory and alignment                             */
/*  Globals       : <Does it use any global variables?>                      */
/*  Processing    : <Describe how the function operates - include algorithm  */
/*                  description>                                             */
/*  Outputs       : <What does the function produce?>                        */
/*  Returns       : Ptr to the allocated buffer                              */
/*                                                                           */
/*  Issues        : <List any issues or problems with this function>         */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         27 06 2008   ITTIAM          Creation                             */
/*                                                                           */
/*****************************************************************************/
void *OMX_ividenc1_utils_malloc(WORD32 i4_size, WORD32 i4_align)
{
    UWORD8 *ptr, *align_ptr;

    if(!IS_POW2(i4_align))
    {
        utils_assert(0);
        return NULL;
    }

    if(i4_align < MIN_ALIGN)
    {
        i4_align = MIN_ALIGN;
    }

    /* Allocate the memory taking into account the worst case hole in memory */
    ptr = malloc(i4_size + i4_align);
    assert(NULL != ptr);

    if(NULL == ptr)
        return NULL;

    align_ptr 
        = OMX_ividenc1_utils_get_align_ptr(ptr, i4_size + i4_align, i4_align);

    return align_ptr;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ividenc1_utils_get_orig_ptr                          */
/*                                                                           */
/*  Description   : Gets the original pointer location given the aligned ptr */
/*                  location.                                                */
/*                                                                           */
/*  Inputs        :                                                          */
/*  align_ptr     : Aligned pointer                                          */
/*  pi4_size      :                                                          */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : Same as description                                      */
/*                                                                           */
/*  Outputs       : None                                                     */
/*                                                                           */
/*  Returns       : Returns the original pointer                             */
/*                                                                           */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         13 07 2002   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/

void *OMX_ividenc1_utils_get_orig_ptr(UWORD8 *align_ptr, WORD32 *pi4_size)
{
    UWORD8 **orig_ptr_loc;

    /* The original pointer is stored jsut before the address where the align*/
    /* pointer refers to                                                     */
    orig_ptr_loc = (UWORD8 **)(align_ptr - 4);

    if(NULL != pi4_size)
    {
        WORD32 *size_loc     = (WORD32 *)(align_ptr - 8);

        *pi4_size    = *size_loc;
    }

    return *orig_ptr_loc;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ividenc1_utils_free                                  */
/*                                                                           */
/*  Description   : Frees the buffer that was allocated by tb_sapp_malloc()  */
/*                                                                           */
/*  Inputs        :                                                          */
/*  ptr           : Ptr to the memory region to be freed                     */
/*                                                                           */
/*  Globals       :                                                          */
/*                                                                           */
/*  Processing    :                                                          */
/*                                                                           */
/*  Outputs       : None                                                     */
/*                                                                           */
/*  Returns       : None                                                     */
/*                                                                           */
/*  Issues        :                                                          */
/*                                                                           */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         13 07 2002   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void OMX_ividenc1_utils_free(void *ptr)
{
    ptr = OMX_ividenc1_utils_get_orig_ptr(ptr, NULL);
    free(ptr);
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ividenc1_utils_alloc_mem                             */
/*                                                                           */
/*  Description   : This function allocates memory for all the memtabs for   */
/*                  a given algorithm                                        */
/*                                                                           */
/*  Inputs        :                                                          */
/*  Globals       : None                                                     */
/*  Processing    :                                                          */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*  Issues        :                                                          */
/*                                                                           */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         27 08 2006   ITTIAM          Creation                             */
/*                                                                           */
/*****************************************************************************/
WORD32 OMX_ividenc1_utils_alloc_mem(IALG_MemRec *ps_mem_tab, 
                               WORD32 i4_num_mem_tabs)
{
    WORD32 i4_i;

    for(i4_i = 0; i4_i < i4_num_mem_tabs; i4_i++)
    {
        WORD32  i4_size;
        WORD32  i4_attrs;
        WORD32  i4_alignment;
        void    *pv_base;

        i4_size      = ps_mem_tab[i4_i].size;
        i4_attrs     = ps_mem_tab[i4_i].attrs;
        i4_alignment = ps_mem_tab[i4_i].alignment;

        /* Ignore the memtabs with size 0 */
        if((0 == i4_size) || (IALG_WRITEONCE == i4_attrs))
        {
            continue;
        }

        if(IALG_SCRATCH == i4_attrs)
        {
            pv_base = OMX_ividenc1_utils_malloc(i4_size, i4_alignment);
        }
        else
        {
            pv_base = OMX_ividenc1_utils_malloc(i4_size, i4_alignment);
        }

        if(NULL == pv_base) {
            return 1;
        }

        ps_mem_tab[i4_i].base   = pv_base;
    }

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_ividenc1_utils_free_mem                              */
/*                                                                           */
/*  Description   : This function frees the memory for all the memtabs for   */
/*                  a given algorithm                                        */
/*                                                                           */
/*  Inputs        :                                                          */
/*  Globals       : None                                                     */
/*  Processing    :                                                          */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*                                                                           */
/*  Issues        :                                                          */
/*                                                                           */
/*  Revision History:                                                        */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         27 08 2006   ITTIAM          Creation                             */
/*                                                                           */
/*****************************************************************************/
void OMX_ividenc1_utils_free_mem(IALG_MemRec *ps_wkg_mem, WORD32 i4_num_mem_tabs)
{
    WORD32 i4_i;

    for(i4_i = 0; i4_i < i4_num_mem_tabs; i4_i++)
    {
        IALG_MemRec *ps_mem_tab;

        ps_mem_tab = &ps_wkg_mem[i4_i];

        /* Ignore the memtabs with size 0 */
        if((0 == ps_mem_tab->size) || (IALG_WRITEONCE == ps_mem_tab->attrs))
            continue;

        if(IALG_SCRATCH == ps_mem_tab->attrs) {
            OMX_ividenc1_utils_free(ps_mem_tab->base);
        } else {
            OMX_ividenc1_utils_free(ps_mem_tab->base);
        }
    }
}
