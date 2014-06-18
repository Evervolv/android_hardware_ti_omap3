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
/*  File Name         : OMX_ividenc1_api.c                                   */
/*                                                                           */
/*  Description       : This file contains implementation of IVIDENC1 API    */
/*                      calls for CTPI (Controlling module to Processing     */
/*                      unit Interface)                                      */
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

#include <utils/Log.h>
#define LOG_TAG "TI_720P_OMX_VideoEnc"

#include "ittiam_datatypes.h"
#include "std.h"
#include "ialg.h"
#include "xdas.h"
#include "xdm.h"
#include "ivideo.h"
#include "ividenc1.h"

#include "iv_enc1_api_consts.h"
#include "iv_enc1_extension_structs.h"
#include "iv_enc1_api_structs.h"

/* System include files */
#include "OMX_ividenc1_utils.h"

/* User include files */
#include "OMX_ividenc1_api.h"
#include "OMX_ividenc1_api_structs.h"

#include "iv_enc_export.h"

/*****************************************************************************/
/*Extern Variable Declarations                                               */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variable Definitions                                               */
/*****************************************************************************/
void    *g_pv_engine_handle = NULL;
/*****************************************************************************/
/* Static Global Variable Definitions                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Static function Definitions                                               */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_IVIDENC1_create                                      */
/*                                                                           */
/*  Description   : This function initializes a codec                        */
/*  Inputs        : <What inputs does the function take?>                    */
/*  Globals       : <Does it use any global variables?>                      */
/*  Processing    : <Describe how the function operates - include algorithm  */
/*                  description>                                             */
/*  Outputs       : <What does the function produce?>                        */
/*  Returns       : <What does the function return?>                         */
/*                                                                           */
/*  Issues        : <List any issues or problems with this function>         */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         22 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
#if 0
IVIDENC1_Handle OMX_IVIDENC1_create(Engine_Handle e, String name,
                                    IVIDENC1_Params *params)
#else
IVIDENC1_Handle OMX_IVIDENC1_create(Engine_Handle e, String name,
                                    ITTVIDENC1_IMOD_Params_t *params)
#endif
{
    instance_ctxt_t *ps_hdl;
    IVIDENC1_Handle ps_codec_hdl;
    IVIDENC1_Fxns   *ps_fxn;

    IALG_Params     *ps_ialg_params;
    IALG_MemRec     *ps_mem_tab;

    WORD32          i4_num_mem_tabs;
    WORD32          i4_status;

//ALOGD("%s %d DSP_ENCODING_PERCENTAGE = %d",__FUNCTION__,__LINE__,DSP_ENCODING_PERCENTAGE );
    /* Derive local variables */
    ps_ialg_params = (IALG_Params *)params;

    {
        /* ----------------------- Select Fxn Table ------------------------ */
        if(0 == strcmp(name, "OMX_VIDEO_CodingAVC"))
        {
            //ALOGD("Select Fxn Table : OMX_VIDEO_CodingAVC ");
            ps_fxn  = (IVIDENC1_Fxns *)(&IVIDENC1_H264_ENC_SPLIT_FXNS);
	    //ALOGD("ps_fxn IS ASSIGNED IVIDENC1_H264_ENC_SPLIT_FXNS ");
        }
        else if(0 == strcmp(name, "OMX_VIDEO_CodingMPEG4"))
        {
            //ALOGD("Select Fxn Table : OMX_VIDEO_CodingMPEG4 ");
            ps_fxn       = (IVIDENC1_Fxns *)(&IVIDENC1_MPEG4_ENC_SPLIT_FXNS);
        }

        /* ----------------- Get number of memory records ------------------ */
        i4_num_mem_tabs = ps_fxn->ialg.algNumAlloc();
	//ALOGD("i4_num_mem_tabs=%d",i4_num_mem_tabs);
        /* ------------------ Allocate memory for memTabs ------------------ */
        ps_mem_tab
            = OMX_ividenc1_utils_malloc(i4_num_mem_tabs * sizeof(IALG_MemRec), 128);

    	if(NULL ==  ps_mem_tab)
       {
		ALOGD("NULL ==  ps_mem_tab");
    	      utils_assert(0);
            return(NULL);
	    }
        /* -------------------- Get memory requirement --------------------- */
        i4_num_mem_tabs
            = ps_fxn->ialg.algAlloc(ps_ialg_params, NULL, ps_mem_tab);
//ALOGD("Get memory requirement=%d",i4_num_mem_tabs);
        utils_assert(0 < i4_num_mem_tabs);

        /* ------------------------ Allocate memory ------------------------ */
        i4_status = OMX_ividenc1_utils_alloc_mem(ps_mem_tab, i4_num_mem_tabs);
        if(0 != i4_status)
        {
//ALOGD("Allocate memory failed");
            utils_assert(0);
            return(NULL);
        }

        /* ---------- Store IALG Fxn table pointer inside handle ----------- */
        ps_codec_hdl        = ps_mem_tab[0].base;
        ps_codec_hdl->fxns  = ps_fxn;

        /* ----------------------- Initialize Codec ------------------------ */
        i4_status
            = ps_fxn->ialg.algInit((IALG_Handle)ps_codec_hdl, ps_mem_tab, NULL,
                                   ps_ialg_params);
        if(IALG_EOK != i4_status)
        {

            ALOGE("algInit FAILED \n");
            utils_assert(0);
            return((IVIDENC1_Handle)NULL);
        }

        /* --------------- Free memory acquired for memTabs ---------------- */
        OMX_ividenc1_utils_free(ps_mem_tab);
    }/* if(PROCESSOR_TYPE_GPP == i4_proc_type) */

    /* ------------------ Create Handle for the instance ------------------- */
    ps_hdl  = OMX_ividenc1_utils_malloc(sizeof(instance_ctxt_t), 128);
    utils_assert(NULL != ps_hdl);

    ps_hdl->s_obj.fxns   = NULL;
    ps_hdl->ps_codec_hdl = ps_codec_hdl;


    return((IVIDENC1_Handle)ps_hdl);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_IVIDENC1_delete                                      */
/*                                                                           */
/*  Description   : This function delete codec instance                      */
/*  Inputs        : <What inputs does the function take?>                    */
/*  Globals       : <Does it use any global variables?>                      */
/*  Processing    : <Describe how the function operates - include algorithm  */
/*                  description>                                             */
/*  Outputs       : <What does the function produce?>                        */
/*  Returns       : <What does the function return?>                         */
/*                                                                           */
/*  Issues        : <List any issues or problems with this function>         */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         30 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
Void OMX_IVIDENC1_delete(IVIDENC1_Handle handle)
{

    instance_ctxt_t     *ps_hdl;
    IVIDENC1_Handle     ps_codec_hdl;

    IALG_MemRec *ps_mem_tab;

    WORD32  i4_num_mem_tabs;

    /* Derive local variables */
    ps_hdl       = (instance_ctxt_t *)handle;
    ps_codec_hdl = ps_hdl->ps_codec_hdl;

    {
        /* ----------------- Get number of memory records ------------------ */
        i4_num_mem_tabs = ps_codec_hdl->fxns->ialg.algNumAlloc();

        /* ------------------ Allocate memory for memTabs ------------------ */
        ps_mem_tab
            = OMX_ividenc1_utils_malloc((i4_num_mem_tabs * sizeof(IALG_MemRec)),
                                    128);
        utils_assert(NULL != ps_mem_tab);

        /* ---------------------- Get memory records ----------------------- */
        i4_num_mem_tabs
            = ps_codec_hdl->fxns->ialg.algFree((IALG_Handle)ps_codec_hdl,
                                                ps_mem_tab);

        /* --------------- Free up the memory given to codec --------------- */
        OMX_ividenc1_utils_free_mem(ps_mem_tab, i4_num_mem_tabs);

        /* --------------- Free memory acquired for memTabs ---------------- */
        OMX_ividenc1_utils_free(ps_mem_tab);
    }

    /* ------------------ Delete handle for the instance ------------------- */
    OMX_ividenc1_utils_free(ps_hdl);

    return;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_IVIDENC1_process                                     */
/*                                                                           */
/*  Description   : This function makes a call to process a frame            */
/*  Inputs        : <What inputs does the function take?>                    */
/*  Globals       : <Does it use any global variables?>                      */
/*  Processing    : <Describe how the function operates - include algorithm  */
/*                  description>                                             */
/*  Outputs       : <What does the function produce?>                        */
/*  Returns       : <What does the function return?>                         */
/*                                                                           */
/*  Issues        : <List any issues or problems with this function>         */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         30 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
Int OMX_IVIDENC1_process(IVIDENC1_Handle handle, IVIDEO1_BufDescIn *inBufs,
                         XDM_BufDesc *outBufs, IVIDENC1_InArgs *inArgs,
                         IVIDENC1_OutArgs *outArgs)
{
    instance_ctxt_t *ps_hdl;
    IVIDENC1_Handle ps_codec_hdl;

    WORD32  i4_status;

    /* Derive local variables */
    ps_hdl          = (instance_ctxt_t *)handle;
    ps_codec_hdl    = ps_hdl->ps_codec_hdl;

    {
        /* ---------------------- Activate algorithm ----------------------- */
        if(NULL != ps_codec_hdl->fxns->ialg.algActivate) {
            ps_codec_hdl->fxns->ialg.algActivate((IALG_Handle)ps_codec_hdl);
        }

        /* ------------------------- Call Process -------------------------- */
        i4_status
            = ps_codec_hdl->fxns->process(ps_codec_hdl, inBufs, outBufs,
                                          inArgs, outArgs);
        utils_assert(IVIDENC1_EOK == i4_status);

        /* --------------------- Deactivate algorithm ---------------------- */
        if(NULL != ps_codec_hdl->fxns->ialg.algDeactivate) {
            ps_codec_hdl->fxns->ialg.algDeactivate((IALG_Handle)ps_codec_hdl);
        }
    }


    return(i4_status);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_IVIDENC1_control                                     */
/*                                                                           */
/*  Description   : This function makes a control call to algorithm          */
/*  Inputs        : <What inputs does the function take?>                    */
/*  Globals       : <Does it use any global variables?>                      */
/*  Processing    : <Describe how the function operates - include algorithm  */
/*                  description>                                             */
/*  Outputs       : <What does the function produce?>                        */
/*  Returns       : <What does the function return?>                         */
/*                                                                           */
/*  Issues        : <List any issues or problems with this function>         */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         30 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
Int OMX_IVIDENC1_control(IVIDENC1_Handle handle, IVIDENC1_Cmd id,
                         IVIDENC1_DynamicParams *params, IVIDENC1_Status *status)
{
    instance_ctxt_t *ps_hdl;
    IVIDENC1_Handle ps_codec_hdl;

    WORD32  i4_status;

    /* Derive local variables */
    ps_hdl          = (instance_ctxt_t *)handle;
    ps_codec_hdl    = ps_hdl->ps_codec_hdl;

    {
        /* ---------------------- Activate algorithm ----------------------- */
        if(NULL != ps_codec_hdl->fxns->ialg.algActivate) {
            ps_codec_hdl->fxns->ialg.algActivate((IALG_Handle)ps_codec_hdl);
        }

        /* ---------------------- Make a control call ---------------------- */
        i4_status
            = ps_codec_hdl->fxns->control(ps_codec_hdl, id, params, status);

        utils_assert(IVIDENC1_EOK == i4_status);

        /* --------------------- Deactivate algorithm ---------------------- */
        if(NULL != ps_codec_hdl->fxns->ialg.algDeactivate) {
            ps_codec_hdl->fxns->ialg.algDeactivate((IALG_Handle)ps_codec_hdl);
        }
    }



    return(i4_status);
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : OMX_IVIDENC1_process_wait                                */
/*                                                                           */
/*  Description   : This function waits for processing to complete if        */
/*                  processing is happenning on differenct thread of         */
/*                  processor                                                */
/*  Inputs        : <What inputs does the function take?>                    */
/*  Globals       : <Does it use any global variables?>                      */
/*  Processing    : <Describe how the function operates - include algorithm  */
/*                  description>                                             */
/*  Outputs       : <What does the function produce?>                        */
/*  Returns       : <What does the function return?>                         */
/*                                                                           */
/*  Issues        : Currently not used                                       */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         18 08 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
Int OMX_IVIDENC1_process_wait(IVIDENC1_Handle handle)
{
    instance_ctxt_t *ps_hdl;
    IVIDENC1_Handle ps_codec_hdl;
    WORD32  i4_status;

    /* Derive local variables */
    ps_hdl       = (instance_ctxt_t *)handle;
    ps_codec_hdl = ps_hdl->ps_codec_hdl;

    /* Running on DSP */
    i4_status = IVIDENC1_EOK;

    return(i4_status);
}
