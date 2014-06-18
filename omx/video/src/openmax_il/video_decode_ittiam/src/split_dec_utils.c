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
/*  File Name         : split_dec_utils.c                                    */
/*                                                                           */
/*  Description       : This file contains the utility implementation	     */
/*						of arm side component 								 */
/*                                                                           */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          17 11 2009   ITTIAM          Draft                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/
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
#include "split_dec_utils.h"

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_split_dec_utils"
#include <utils/Log.h>

/*****************************************************************************/
/* Static variable declarations                                              */
/*****************************************************************************/
#if CHECK_SUM_ENABLE
	static int checkSumEnableCount = 1;
#endif

/*****************************************************************************/
/*  Function Name : app_alloc_mem_tables                                     */
/*  Description   : This function Allocate memory tables.                    */
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
int app_alloc_mem_tables(IALG_MemRec *ps_wkg_mem, int u4_num_mem_tabs)
{
    int i;
    int alignment;
    for(i = 0; i < u4_num_mem_tabs; i++)
    {
        IALG_MemRec *ps_mem_tab;
        ps_mem_tab = &ps_wkg_mem[i];
        alignment = ps_mem_tab->alignment;
        if(-1 == alignment)
        {
           alignment = MIN_ALIGN;
        }
        /* Ignore the memtabs with size 0 */
        if(0 == ps_mem_tab->size)
        {
            continue;
        }
        if(ps_mem_tab->attrs != IALG_WRITEONCE)
        {
            ps_mem_tab->base = iram_alloc(ps_mem_tab->size, alignment);
        }
        if(NULL == ps_mem_tab->base)
        {
            LOG_STATUS_ARM_API("Memtab alloc failed - size : %d ps_mem_tab->size alignment: %d\n",
                (int)ps_mem_tab->size, (int)alignment);
           return 1;
        }
    }

    return 0;
}

/*****************************************************************************/
/*  Function Name : iram_alloc                                               */
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
void* iram_alloc(int i4_size, int i4_align)
{
    unsigned char *ptr, *align_ptr;

    if(!IS_POW2(i4_align))
    {
        return NULL;
    }
    /* minimum alignment is 4*/
    if(i4_align < MIN_ALIGN)
    {
        i4_align = MIN_ALIGN;
    }

    /* Allocate the memory taking into account the worst case hole in memory */
    ptr = malloc(i4_size + i4_align);

    if(NULL == ptr)
    {
        return NULL;
    }
    align_ptr = get_align_ptr(ptr, i4_size + i4_align, i4_align);
    return align_ptr;
}

/*****************************************************************************/
/*  Function Name : get_align_ptr                                            */
/*  Description   : This function retrieves aligned pointer.                 */
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
void *get_align_ptr(unsigned char *ptr, int i4_size, int i4_align)
{
    unsigned char *align_ptr;
    unsigned char **orig_ptr_loc;
    int *size_loc;

    align_ptr = (unsigned char *)PTR_ALIGN_POW2(ptr, i4_align);

    if(align_ptr == ptr)
    {
            align_ptr = ptr + i4_align;
    }

    /* return the align pointer and store the original ptr in the hole       */
    orig_ptr_loc = (unsigned char **)(align_ptr - 4);
    size_loc     = (int *)(align_ptr - 8);

    *orig_ptr_loc = ptr;
    *size_loc     = i4_size;

    return align_ptr;
}

/*****************************************************************************/
/*  Function Name : get_orig_ptr                                             */
/*  Description   : This function orginal pointer.                           */
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
void *get_orig_ptr(unsigned char *align_ptr, int *pi4_size)
{
    unsigned char **orig_ptr_loc;
    orig_ptr_loc = (unsigned char **)(align_ptr - 4);

    if(NULL != pi4_size)
    {
            int *size_loc     = (int *)(align_ptr - 8);
            *pi4_size    = *size_loc;
    }
    return *orig_ptr_loc;
}

/*****************************************************************************/
/*  Function Name : codec_malloc_aligned                                     */
/*  Description   : Allocate the memory taking into account the worst case   */
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
void *codec_malloc_aligned(int i4_size, int i4_align)
{
    unsigned char *ptr, *align_ptr;

    if(!ALIGN_IS_POW2(i4_align))
    {
            return NULL;
    }
    if(i4_align < MIN_ALIGN)
    {
            i4_align = MIN_ALIGN;
    }
    /* Allocate the memory taking into account the worst case hole in memory */
    #define DEBUG_SAFE_PADDING 0
    ptr = malloc(i4_size + i4_align + DEBUG_SAFE_PADDING);
    if(NULL == ptr)
    {
            return NULL;
    }
    align_ptr = get_align_ptr(ptr, i4_size + i4_align, i4_align);
    return align_ptr;
}

/*****************************************************************************/
/*  Function Name : codec_free_aligned                                       */
/*  Description   : This function aligns the codec.                                                         */
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
void codec_free_aligned(void *ptr)
{
    void *ptr_actual;
    ptr_actual = get_orig_ptr(ptr, NULL);
    free(ptr_actual);
}

/*****************************************************************************/
/*  Function Name : app_free_mem_tables                                      */
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
void app_free_mem_tables(IALG_MemRec *ps_wkg_mem, int u4_num_mem_tabs)
{
    int i;
    for(i = 0; i < u4_num_mem_tabs; i++)
    {
        IALG_MemRec *ps_mem_tab;
        ps_mem_tab = &ps_wkg_mem[i];
        if(0 == ps_mem_tab->size)
        {
            continue;
        }
        if(ps_mem_tab->attrs != IALG_WRITEONCE)
        {
            ps_mem_tab->base = get_orig_ptr(ps_mem_tab->base, NULL);
            free(ps_mem_tab->base);
        }
    }
}

/*****************************************************************************/
/*  Function Name : checksum                                                 */
/*  Description   : This function Do Checksum.                               */
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
#if CHECK_SUM_ENABLE
void checksum(unsigned char            *ptr,
			  int 			           size,
			  VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    if(checkSumEnableCount%50 == 0)
    {
        int i;
        unsigned char *orig = ptr;
        unsigned int checksum = 0;
        unsigned int *bufPtr = (unsigned int *)ptr;
        size =(int) size / 4;

        for(i = 0; i < size; i++)
        {
            checksum ^= bufPtr[i];
            //bufPtr++;
        }
        LOG_STATUS_ARM_API("Checksum %d size %d \n",checksum, size);
        FILE *fp = fopen("/tmp/checksum.txt","a");

        fprintf(fp, "checksum is %d size is %d\n", checksum, size);
        fclose(fp);

        ptr = orig;

        LOG_STATUS_ARM_API("Checksum for 100 bytes %d size %d \n",checksum, size);
        checkSumEnableCount++;
    }
    else
    {
        checkSumEnableCount++;
    }
}
#endif


