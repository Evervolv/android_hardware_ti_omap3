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
/*  File Name         : mpeg4_asc_api.c                                      */
/*                                                                           */
/*  Description       : Functions which recieve the API call from user       */
/*                                                                           */
/*  List of Functions : <List the functions defined in this file>            */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         05 02 2009   Ittiam          Creation                             */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/
#include <string.h>
#include "ittiam_datatypes.h"
#include "std.h"
#include "ialg.h"
#include "xdas.h"
#include "xdm.h"
#include "ivideo.h"
#include "ividdec2.h"
#include "split/ividdec2.h"
#include "mpeg4_asc_api.h"

#undef LOG_TAG
#define LOG_TAG "720p_mpeg4_asc_api.c"
#include <utils/Log.h>

/*****************************************************************************/
/* Static Function Declarations                                              */
/*****************************************************************************/

static Int dec_num_alloc(Void);

static Int dec_alloc(const IALG_Params *algParams,
                     IALG_Fxns         **parent,
                     IALG_MemRec       *memTab);

static Int  dec_init(IALG_Handle 	   handle,
					 const IALG_MemRec *memTab,
					 IALG_Handle 	    dummy_handle,
					 const IALG_Params *algParams);

static XDAS_Int32 dec_process(IVIDDEC2FRONT_Handle  handle,
                              IVIDDEC2_InArgs       *inArgs,
                              XDM_Context           *context,
                              IVIDDEC2FRONT_OutArgs *outArgs);

static XDAS_Int32 dec_control(IVIDDEC2FRONT_Handle   handle,
                              IVIDDEC2_Cmd 	         id,
                              IVIDDEC2_DynamicParams *params,
                              XDM_Context 			 *context,
                              IVIDDEC2FRONT_Status   *status);

static Int dec_algFree(IALG_Handle handle,IALG_MemRec *memTab);

static void  dec_algMoved(IALG_Handle handle,
                          const IALG_MemRec *,
                          IALG_Handle,
                          const IALG_Params *);

static void  dec_algActivate(IALG_Handle handle);

static void  dec_algDeactivate(IALG_Handle handle);


IVIDDEC2FRONT_Fxns MPEG4HDFRONT_ITTIAMVIDEODECODER_IMPEG4HDFRONT_IALG =
{
    {
             NULL,              /* implementationId */
             dec_algActivate,   /* algActivate      */
             dec_alloc,         /* algAlloc         */
             NULL,              /* algControl       */
             dec_algDeactivate, /* algDeactivate    */
             dec_algFree,       /* algFree          */
             dec_init,          /* algInit          */
             dec_algMoved,      /* algMoved         */
             dec_num_alloc      /* algNumAlloc      */
    },
     dec_process,
     dec_control
};


/*****************************************************************************/
/*  Function Name : dec_num_alloc                                            */
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
static Int   dec_num_alloc(Void)
{
    return ASC_MEMTABS;
}

/*****************************************************************************/
/*  Function Name : dec_alloc                                                */
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
static Int   dec_alloc(const IALG_Params *algParams,
                   IALG_Fxns **parent,
                   IALG_MemRec *memTab)
{
    /* Request memory for MEMTAB0 */
    memTab[MEMTAB0].size      = sizeof(dec_ctxt);
    memTab[MEMTAB0].alignment = 16;
    memTab[MEMTAB0].space     = IALG_EXTERNAL;
    memTab[MEMTAB0].attrs     = IALG_PERSIST;

    /* Request memory for MEMTAB1 */
    memTab[MEMTAB1].size      = sizeof(ControlCallInfo);
    memTab[MEMTAB1].alignment = 16;
    memTab[MEMTAB1].space     = IALG_EXTERNAL;
    memTab[MEMTAB1].attrs     = IALG_PERSIST;

    return ASC_MEMTABS;
}

/*****************************************************************************/
/*  Function Name : dec_init                                                 */
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
static Int  dec_init(IALG_Handle 	   handle,
					 const IALG_MemRec *memTab,
					 IALG_Handle       dummy_handle,
					 const IALG_Params *algParams)

{

    IVIDDEC2_Params *xdm_params = (IVIDDEC2_Params *)algParams;

    /* Initialize the decoder context */
    dec_ctxt *ps_dec_ctxt = (dec_ctxt*)handle;
    ps_dec_ctxt->ps_ControlCallInfo = (ControlCallInfo*)memTab[MEMTAB1].base;

    memset(ps_dec_ctxt->ps_ControlCallInfo,
           0,
           sizeof(ControlCallInfo));

    ps_dec_ctxt->i4_create_width    = xdm_params->maxWidth;
    ps_dec_ctxt->i4_create_height   = xdm_params->maxHeight;
    ps_dec_ctxt->i4_chroma_format   = xdm_params->forceChromaFormat;

    ps_dec_ctxt->i4_num_process_after_flush = 0;
    ps_dec_ctxt->i4_got_flush = 0;

    return(IALG_EOK);
}

/*****************************************************************************/
/*  Function Name : dec_control                                              */
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
static XDAS_Int32 dec_control(IVIDDEC2FRONT_Handle handle,
                              IVIDDEC2_Cmd id,
                              IVIDDEC2_DynamicParams *params,
                              XDM_Context *context,
                              IVIDDEC2FRONT_Status *status)
{

    dec_ctxt            *ps_dec_ctxt        = (dec_ctxt*)handle;
    ControlCallInfo     *ps_ControlCallInfo = ps_dec_ctxt->ps_ControlCallInfo;
    WORD32              numControlCalls     = ps_ControlCallInfo->
											  numControlCalls;
    int 				i;

    switch ((XDM_CmdId)id)
    {
        case XDM_GETSTATUS:
        {
            ps_ControlCallInfo->
					s_Info_Per_ControlCall[numControlCalls].id = XDM_GETSTATUS;
            ps_ControlCallInfo->numControlCalls++;
        }
        break;

        case XDM_SETPARAMS:
        {
            ps_ControlCallInfo->
					s_Info_Per_ControlCall[numControlCalls].id = XDM_SETPARAMS;

            memcpy(&ps_ControlCallInfo->
						s_Info_Per_ControlCall[numControlCalls].s_dyn_params,
						params,
						sizeof(IVIDDEC2_DynamicParams));

			ps_ControlCallInfo->numControlCalls++;
        }
        break;

        case XDM_GETBUFINFO:
        {

            status->fullStatus.bufInfo.minNumInBufs     = 1;
            for(i = 0; i < status->fullStatus.bufInfo.minNumInBufs; i++)
            {
                status->fullStatus.bufInfo.minInBufSize[i] = MAX_BITSTREAM_SIZE;
            }

            status->fullStatus.bufInfo.minNumOutBufs    = 1;
            if(ps_dec_ctxt->i4_chroma_format == XDM_YUV_422ILE)
            {
                for(i = 0; i < status->fullStatus.bufInfo.minNumOutBufs; i++)
                {
					status->fullStatus.bufInfo.minOutBufSize[i] =
					(ps_dec_ctxt->i4_create_width *
					 ps_dec_ctxt->i4_create_height * 2);
                }
            }
            else
            {
                for(i = 0; i < status->fullStatus.bufInfo.minNumOutBufs; i++)
                {
					status->fullStatus.bufInfo.minOutBufSize[i] =
					((ps_dec_ctxt->i4_create_width *
					  ps_dec_ctxt->i4_create_height * 3) >> 1);
                }
            }

            memset(&status->contextInfo,0,sizeof(XDM_ContextInfo));
            status->contextInfo.minIntermediateBufSizes[0] =
													sizeof(ControlCallInfo);
            status->contextInfo.minIntermediateBufSizes[1] = MAX_BITSTREAM_SIZE;
        }
        break;

        case XDM_GETVERSION:
        {
            ps_ControlCallInfo->
					s_Info_Per_ControlCall[numControlCalls].id = XDM_GETVERSION;
            ps_ControlCallInfo->numControlCalls++;
        }
        break;

        case XDM_FLUSH:
        {
            ps_dec_ctxt->i4_got_flush = 1;
            ps_ControlCallInfo->
						s_Info_Per_ControlCall[numControlCalls].id = XDM_FLUSH;
            ps_ControlCallInfo->numControlCalls++;
        }
        break;

        case XDM_SETDEFAULT:
        {
            ps_ControlCallInfo->
					s_Info_Per_ControlCall[numControlCalls].id = XDM_SETDEFAULT;
            ps_ControlCallInfo->numControlCalls++;
        }
        break;

        case XDM_GETCONTEXTINFO:
        {
            memset(&status->contextInfo,0,sizeof(XDM_ContextInfo));
            status->contextInfo.minContextSize = sizeof(ITTAlg_context);
            status->contextInfo.minIntermediateBufSizes[0] =
											sizeof(ControlCallInfo);
            status->contextInfo.minIntermediateBufSizes[1] = MAX_BITSTREAM_SIZE;
        }
        break;

        case XDM_RESET:
        {
            ps_ControlCallInfo->
						s_Info_Per_ControlCall[numControlCalls].id = XDM_RESET;
            ps_ControlCallInfo->numControlCalls++;
        }
        break;

        default:
            break;
    }

    status->fullStatus.extendedError = 0;
    return(IALG_EOK) ;
}


/*****************************************************************************/
/*  Function Name : dec_process                                              */
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
static XDAS_Int32 dec_process(IVIDDEC2FRONT_Handle  handle,
                              IVIDDEC2_InArgs       *inArgs,
                              XDM_Context           *context,
                              IVIDDEC2FRONT_OutArgs *outArgs)

{
    ITTAlg_context 		*ps_imod_inargs;
    dec_ctxt            *ps_dec_ctxt = (dec_ctxt*)handle;
    ControlCallInfo     *ps_ControlCallInfo;

    ps_imod_inargs = (ITTAlg_context *)context->algContext.buf;

    if((handle == NULL) || (inArgs == NULL) || (context == NULL) ||
    (outArgs == NULL))
    {
		return(IALG_EFAIL);
    }

    ps_ControlCallInfo = ps_dec_ctxt->ps_ControlCallInfo;

    if(inArgs->numBytes > 0 && 1 == ps_dec_ctxt->i4_got_flush)
    {
		ps_dec_ctxt->i4_got_flush = 0;
    }

    if(1 == ps_dec_ctxt->i4_got_flush)
    {
		if(ps_dec_ctxt->i4_num_process_after_flush == 1)
		{
			return IALG_EFAIL;
		}

		ps_dec_ctxt->i4_num_process_after_flush++;
    }
    else
    {
		ps_dec_ctxt->i4_num_process_after_flush = 0;
    }

    /* Populate the outArgs*/
    outArgs->extendedError    = 0;
    outArgs->bytesConsumed    = 0;

    /* Populate the ITTAlg_context that should be passed to the DSP */
    ps_imod_inargs->size                = sizeof(ITTAlg_context);
    ps_imod_inargs->inputID             = inArgs->inputID;
    ps_imod_inargs->numBytes            = inArgs->numBytes;
    ps_imod_inargs->bytesConsumed       = inArgs->numBytes;
    ps_imod_inargs->chromaFormat 		= 1;
    ps_imod_inargs->display_width 		= 0;
    ps_imod_inargs->decoded_frame_wt 	= 176;
    ps_imod_inargs->decoded_frame_ht 	= 144;
    ps_imod_inargs->extendedError 		= 0;
    ps_imod_inargs->vop_coding_type 	= 0;


    /* copy the control calls into the imbuffer0 */
    memcpy(context->intermediateBufs[0].buf,
    ps_ControlCallInfo,
    sizeof(ControlCallInfo));

    /* Memset the control call information once the copy is done */
    memset(ps_ControlCallInfo,
    0,
    sizeof(ControlCallInfo));

    /* Copy the bitstream into IMBUF1 */
    memcpy(context->intermediateBufs[1].buf,
    context->inBufs[0].buf,
    inArgs->numBytes);

    /* Set the access modes for the various buffers */
    context->inBufs[0].accessMask           = 0;
    context->algContext.accessMask          = 0;
    context->intermediateBufs[0].accessMask = 0;
    context->intermediateBufs[1].accessMask = 0;

    XDM_SETACCESSMODE_READ(context->inBufs[0].accessMask);
    XDM_SETACCESSMODE_WRITE(context->algContext.accessMask);
    XDM_SETACCESSMODE_WRITE(context->intermediateBufs[0].accessMask);
    XDM_SETACCESSMODE_WRITE(context->intermediateBufs[1].accessMask);

    return IALG_EOK;
}

/*****************************************************************************/
/*  Function Name : dec_algFree                                              */
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
static Int dec_algFree(IALG_Handle handle,IALG_MemRec *memTab)
{
    dec_ctxt *ps_dec_ctxt = (dec_ctxt*)handle;

    /* Request memory for MEMTAB0 */
    memTab[MEMTAB0].base 	  = ps_dec_ctxt;
    memTab[MEMTAB0].size 	  = sizeof(dec_ctxt);
    memTab[MEMTAB0].alignment = 16;
    memTab[MEMTAB0].space 	  = IALG_EXTERNAL;
    memTab[MEMTAB0].attrs 	  = IALG_PERSIST;

    /* Request memory for MEMTAB1 */
    memTab[MEMTAB1].base 	  = ps_dec_ctxt->ps_ControlCallInfo;
    memTab[MEMTAB1].size 	  = sizeof(ControlCallInfo);
    memTab[MEMTAB1].alignment = 16;
    memTab[MEMTAB1].space 	  = IALG_EXTERNAL;
    memTab[MEMTAB1].attrs 	  = IALG_PERSIST;

    return IALG_EOK;
}


/*****************************************************************************/
/*  Function Name : dec_algMoved                                             */
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
static void  dec_algMoved(IALG_Handle handle, const IALG_MemRec *memTab,
                          IALG_Handle dummy_handle, const IALG_Params *params)
{
	return;
}

/*****************************************************************************/
/*  Function Name : dec_algActivate                                          */
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
static void  dec_algActivate(IALG_Handle handle)
{
	return;
}

/*****************************************************************************/
/*  Function Name : dec_algDeactivate                                        */
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
static void  dec_algDeactivate(IALG_Handle handle)
{
	return;
}


