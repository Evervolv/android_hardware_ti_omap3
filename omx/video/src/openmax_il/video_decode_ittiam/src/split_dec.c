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
/*  File Name         : split_dec.c                           	             */
/*                                                                           */
/*  Description       : This file contains functions that create arm	     */
/*						side component										 */
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

#ifdef __PERF_INSTRUMENTATION__
	#include "perf.h"
#endif

#ifdef RESOURCE_MANAGER_ENABLED
    #include <ResourceManagerProxyAPI.h>
#endif

#undef LOG_TAG
#define LOG_TAG "720p_split_dec"
#include <utils/Log.h>

/*****************************************************************************/
/* Variables Used for profiling                                              */
/*****************************************************************************/
#if ARM_PROCESSING_PROFILE
	unsigned long long armProcessingBefore[MAX_PROFILE];
	int                armProcessingBeforeIndex = 1;
	unsigned long long armProcessingAfter[MAX_PROFILE];
	unsigned long long armProcessTime[MAX_PROFILE];
	int                armProcessingAfterIndex = 2;
	FILE               *gProfileFP = NULL;
	struct timeval   startTime;
	struct timeval   endTime;
#endif
#if DUMP_INP_TO_ARM
	FILE *gDumpFp = NULL;
	FILE *gDumpFileSize = NULL;
#endif

#if DSP_START_PROCESSING_PROFILE
	unsigned long long dspStartProcessing[MAX_PROFILE];
	int                dspStartProcessingIndex = 1;
	FILE               *gDspStartProcProfileFP = NULL;
#endif

/*****************************************************************************/
/* Static Function Declarations                                              */
/*****************************************************************************/
static OMX_ERRORTYPE pass_context_to_dsp(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							XDM_Context              *ps_context,
							OMX_BUFFERHEADERTYPE     *pInputBuffHead);

static void *populate_context(OMX_VIDEO_PARAM_SPLITTYPE *pSplit,
                              VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate);

static void *asc_thread(void *pv_handle);

/*****************************************************************************/
/*  Function Name : allocate_split_component                                 */
/*  Description   : This Function Allocates Spilt component.                 */
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
OMX_ERRORTYPE allocate_split_component(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE             eError = OMX_ErrorNone;
    OMX_VIDEO_PARAM_SPLITTYPE *split = NULL;

    OMX_MALLOC_STRUCT(pComponentPrivate->pSplit, OMX_VIDEO_PARAM_SPLITTYPE,
					  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

    split = (OMX_VIDEO_PARAM_SPLITTYPE *)pComponentPrivate->pSplit;
    split->m_bCodecReset  = OMX_FALSE;
    split->isResetPending = OMX_FALSE;

EXIT:
    return eError;
}

/*****************************************************************************/
/*  Function Name : create_asc_thread                                        */
/*  Description   : This Function Creates ASC Thread.                        */
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
void create_asc_thread(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_S32  				  retval;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;

    pSplit->bStop               = OMX_FALSE;
    pSplit->bEosReceivedAtInput = OMX_FALSE;
    pSplit->m_isThreadStopped   = OMX_TRUE;

    pthread_mutex_init(&pSplit->m_isStopped_mutex, NULL);
    pthread_cond_init (&pSplit->m_isStopped_threshold, NULL);

    pthread_mutex_init(&pSplit->m_isFlushed_mutex, NULL);
    pthread_cond_init (&pSplit->m_flushCond, NULL);

    LOG_STATUS_ARM_API("Split: Calling pthread create\n");
    retval = pthread_create (&pSplit->arm_thread_handle,
                            NULL,
                            asc_thread,
                            pComponentPrivate);

    if(retval)
    {
        LOG_STATUS_ARM_API("Could not start the asc thread  %d\n",retval);
    }
    else
    {
        LOG_STATUS_ARM_API("Split: Pthread done\n");
    }
}

/*****************************************************************************/
/*  Function Name : write_to_asc_pipe                                        */
/*  Description   : This Function writes into ASC pipe from input pipe.      */
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
OMX_ERRORTYPE write_to_asc_pipe(OMX_VIDEO_PARAM_SPLITTYPE *pSplit,
                                OMX_BUFFERHEADERTYPE*     pBuffHead )
{
	OMX_S32 ret = 0;
	OMX_ERRORTYPE eError = OMX_ErrorNone;

    ret = write(pSplit->AscFilledInputPipe[1], &(pBuffHead), sizeof(pBuffHead));
    if (ret == -1)
    {
        eError = OMX_ErrorHardware;
    }

    return eError;
}

/*****************************************************************************/
/*  Function Name : create_split_pipes                                       */
/*  Description   : This Function create split pipes.                        */
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
OMX_ERRORTYPE create_split_pipes(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
    OMX_ERRORTYPE             eError  = OMX_ErrorNone;

    LOG_STATUS_ARM_API("Openining up pipes\n");

    SPLIT_OPEN_PIPE(AscFilledInputPipe);
    SPLIT_OPEN_PIPE(free_context_pipe);

    LOG_STATUS_ARM_API("Openining up pipes done\n");
EXIT:
    return eError;
}

/*****************************************************************************/
/*  Function Name : stop_asc_thread                                          */
/*  Description   : This Function Stops ASC Thread.                          */
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
void stop_asc_thread(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    void                      *ptr;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;

    /* We will wait upon the thread to come and join here */
    pSplit->bStop = OMX_TRUE;

    pthread_mutex_lock(&pSplit->m_isStopped_mutex);
    pthread_cond_wait(&pSplit->m_isStopped_threshold,
											&pSplit->m_isStopped_mutex);
    pthread_mutex_unlock(&pSplit->m_isStopped_mutex);

    pthread_mutex_destroy(&pSplit->m_isStopped_mutex);
    pthread_cond_destroy(&pSplit->m_isStopped_threshold);

    pthread_mutex_destroy(&pSplit->m_isFlushed_mutex);
    pthread_cond_destroy(&pSplit->m_flushCond);

    LOG_STATUS_ARM_API("Calling pthread join\n");
    pthread_join(pSplit->arm_thread_handle, (void **)&ptr);
    LOG_STATUS_ARM_API("pthread join done\n");
}

/*****************************************************************************/
/*  Function Name : close_split_pipes                                        */
/*  Description   : This Function Closes split pipes.                        */
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
OMX_ERRORTYPE close_split_pipes(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_S32					  err;
    OMX_ERRORTYPE			  eError = OMX_ErrorNone;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;

    SPLIT_CLOSE_PIPE(AscFilledInputPipe);
    SPLIT_CLOSE_PIPE(free_context_pipe);

    return eError;
}

/*****************************************************************************/
/*  Function Name : delete_split_arm_side_component                          */
/*  Description   : This Function delete split arm side component.           */
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
OMX_ERRORTYPE delete_split_arm_side_component(
								VIDDEC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE             eError = OMX_ErrorNone;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
    IALG_MemRec               *ps_mem_recs;
    int                       i4_num_mem_recs;
    int                       i4_free_count, i4_i;
    unsigned char             *pu1_ptr;

    LOG_STATUS_ARM_API("Entering");

    i4_num_mem_recs = pSplit->h->fxns->s_ividdec2_fxns.ialg.algNumAlloc();
    ps_mem_recs     = codec_malloc_aligned(i4_num_mem_recs *
												sizeof(IALG_MemRec),128);

    pSplit->h->fxns->s_ividdec2_fxns.ialg.algFree((IALG_Handle)pSplit->h,
												   ps_mem_recs);

    app_free_mem_tables(ps_mem_recs, i4_num_mem_recs);

    codec_free_aligned(ps_mem_recs);
    codec_free_aligned(pSplit->ps_dynamic_params);
    codec_free_aligned(pSplit->ps_inargs);
    codec_free_aligned(pSplit->ps_prms);
    codec_free_aligned(pSplit->ps_front_status);
    codec_free_aligned(pSplit->ps_front_outargs);
    codec_free_aligned(pSplit->ps_inbufs);
    i4_free_count = pSplit->i4_free_count;

    /* Free up the IM buffers in the free IM queue */
    for(i4_i = 0; i4_i < i4_free_count; i4_i++)
    {
        pu1_ptr = (unsigned char *)pSplit->ps_im_bufs[i4_i]->descs[0].buf;
        if(NULL != pu1_ptr)
        {
            codec_free_aligned(pu1_ptr);
        }

        pu1_ptr = (unsigned char *)pSplit->ps_im_bufs[i4_i];
        if(NULL != pu1_ptr)
        {
            free(pu1_ptr);
        }
    }
    if(pSplit->ps_im_bufs)
    {
        free(pSplit->ps_im_bufs);
        pSplit->ps_im_bufs = NULL;
    }

    /* Free the context pointers in the free context queue */
    for(i4_i = 0; i4_i < i4_free_count; i4_i++)
    {
        pu1_ptr = (unsigned char *)pSplit->ps_context[i4_i]->algContext.buf;
        if(NULL != pu1_ptr)
        {
            codec_free_aligned(pu1_ptr);
        }
        pu1_ptr = (unsigned char *)pSplit->ps_context[i4_i];
        if(NULL != pu1_ptr)
        {
            free(pu1_ptr);
        }

        pu1_ptr = (unsigned char *)pSplit->pInpParam[i4_i];
        if(NULL != pu1_ptr)
        {
            /*  free(pu1_ptr);*/
            OMX_MEMFREE_STRUCT_DSPALIGN(pu1_ptr, ITTVDEC_UALGInputParam_t);
        }

        pu1_ptr = (unsigned char *)pSplit->pBufferHdr[i4_i];
        if(NULL != pu1_ptr)
        {
            free(pu1_ptr);
        }
    }

    if(pSplit->ps_context)
    {
        free(pSplit->ps_context);
        pSplit->ps_context= NULL;
    }

    if(pSplit->pInpParam)
    {
        free(pSplit->pInpParam);
        pSplit->pInpParam = NULL;
    }

    if(pSplit->pBufferHdr)
    {
        free(pSplit->pBufferHdr);
        pSplit->pBufferHdr = NULL;
    }

    LOG_STATUS_ARM_API("Exiting");

    return eError;
}

/*****************************************************************************/
/*  Function Name : create_split_arm_side_component                          */
/*  Description   : This Function create splt arm side component.            */
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
OMX_ERRORTYPE create_split_arm_side_component(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
	OMX_VIDEO_PARAM_SPLITTYPE   *pSplit = pComponentPrivate->pSplit;
	IALG_MemRec   			    *ps_mem_recs;
	int                         i4_num_mem_recs;
	int                         i4_num_recs;
	int                         i4_status;
	OMX_ERRORTYPE               eError = OMX_ErrorNone;
	int                         i4_free_count = NUM_IMBUFS;
	unsigned char               *buff_ptr;
	int                         i,j,i4_num_im_bufs = 0;
	int                         total_im_buffer_size;
	int                         remainder;
	XDM_AlgBufInfo              *ps_buf_info;
	XDM_ContextInfo             *ps_ctxt_info;
	ITTVIDDEC2FRONT_IMOD_Fxns_t *ps_front_fxns;
	OMX_U32 			 	    tempCompressionFormat = pComponentPrivate->
									pInPortDef->format.video.eCompressionFormat;
	OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef = pComponentPrivate->pInPortDef;
	OMX_PARAM_PORTDEFINITIONTYPE *pOutPortDef = pComponentPrivate->pOutPortDef;

	if((tempCompressionFormat == OMX_VIDEO_CodingMPEG4) ||
	  (tempCompressionFormat == OMX_VIDEO_CodingWMV))
	{
	   i4_free_count = 4;
	}
	if(pComponentPrivate->pInPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC &&
        pComponentPrivate->pInPortDef->format.video.nFrameHeight <= 480 &&
        pComponentPrivate->pInPortDef->format.video.nFrameWidth <= 720)
        {
           i4_free_count = 2;
        }
	pSplit->i4_free_count       = i4_free_count;

	/***********************************************************************/
	/* Allocate memory for the various structures                          */
	/***********************************************************************/
	pSplit->ps_prms = codec_malloc_aligned(sizeof(ITTVIDDEC2_IMOD_Params_t),128);
	CHECK_ALLOCATION_AND_RETURN_ERROR(pSplit->ps_prms);

	pSplit->ps_dynamic_params =
	codec_malloc_aligned(sizeof(ITTVIDDEC2_IMOD_DynamicParams_t),128);
	CHECK_ALLOCATION_AND_RETURN_ERROR(pSplit->ps_dynamic_params);

	pSplit->ps_front_status =
	codec_malloc_aligned(sizeof(ITTVIDDEC2FRONT_IMOD_Status_t),128);
	CHECK_ALLOCATION_AND_RETURN_ERROR(pSplit->ps_front_status);

	pSplit->ps_inargs =
	codec_malloc_aligned(sizeof(ITTVIDDEC2_IMOD_InArgs_t),128);
	CHECK_ALLOCATION_AND_RETURN_ERROR(pSplit->ps_inargs);

	pSplit->ps_front_outargs =
	codec_malloc_aligned(sizeof(ITTVIDDEC2FRONT_IMOD_OutArgs_t),128);
	CHECK_ALLOCATION_AND_RETURN_ERROR(pSplit->ps_front_outargs);

	pSplit->ps_inbufs = codec_malloc_aligned(sizeof(XDM1_BufDesc),128);
	CHECK_ALLOCATION_AND_RETURN_ERROR(pSplit->ps_inbufs);


	/***********************************************************************/
	/* Copy the contents of the creation param structure into component's  */
	/* structure.Fill up the size element of the structures.               */
	/***********************************************************************/
	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.size
								= sizeof(ITTVIDDEC2_IMOD_DynamicParams_t);
	pSplit->ps_inargs->s_ividdec2_inargs.size
								= sizeof(ITTVIDDEC2_IMOD_InArgs_t);
	pSplit->ps_front_outargs->s_ividdec2_front_outargs.size
								= sizeof(ITTVIDDEC2FRONT_IMOD_OutArgs_t);
	pSplit->ps_prms->s_ividdec2_params.size
								= sizeof(ITTVIDDEC2_IMOD_Params_t);
	/* Why not other sizes? */
	pSplit->ps_front_status->
			s_ividdec2_front_status.fullStatus.data.bufSize = 0;

	pSplit->ps_front_status->
			s_ividdec2_front_status.fullStatus.data.buf     = NULL;

	/***********************************************************************/
	/* Set the init params before the comonent instance create             */
	/***********************************************************************/
	pSplit->ps_prms->s_ividdec2_params.maxHeight =
					pComponentPrivate->pOutPortDef->format.video.nFrameHeight;

	pSplit->ps_prms->s_ividdec2_params.maxWidth  =
					pComponentPrivate->pOutPortDef->format.video.nFrameWidth;

	if(pComponentPrivate->pOutPortDef->format.video.eColorFormat
													== VIDDEC_COLORFORMAT422)
	{
		pSplit->ps_prms->s_ividdec2_params.forceChromaFormat = XDM_YUV_422ILE;
	}
	else if(pComponentPrivate->pOutPortDef->format.video.eColorFormat
													== VIDDEC_COLORFORMAT420)
	{
		pSplit->ps_prms->s_ividdec2_params.forceChromaFormat = XDM_YUV_420P;
	}
	else
	{
		pSplit->ps_prms->s_ividdec2_params.forceChromaFormat = XDM_YUV_422ILE;
	}

	pSplit->ps_prms->s_ividdec2_params.maxBitRate        = 10000000;
	pSplit->ps_prms->s_ividdec2_params.maxFrameRate      = 30000;
	pSplit->ps_prms->s_ividdec2_params.dataEndianness    = XDM_BYTE;
	pSplit->ps_prms->post_proc_deblocking                = 0;
	/***********************************************************************/
	/* Set the dynamic paramters to default values                         */
	/***********************************************************************/
	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.decodeHeader  =
																XDM_DECODE_AU;
	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.displayWidth  = 0;
	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.frameSkipMode =
																IVIDEO_NO_SKIP;
	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.frameOrder    =
													IVIDDEC2_FRAMEORDER_DEFAULT;

	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.newFrameFlag  = XDAS_FALSE;
	pSplit->ps_dynamic_params->s_ividdec2_dyn_params.mbDataFlag    = XDAS_FALSE;

	if(tempCompressionFormat == OMX_VIDEO_CodingMPEG4)
	{
		ps_front_fxns = (ITTVIDDEC2FRONT_IMOD_Fxns_t *) &MPEG4VDEC_ARM_COMPONENT_FXNS;
	}
	else if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
	{
		ps_front_fxns = (ITTVIDDEC2FRONT_IMOD_Fxns_t *) &H264VDEC_ARM_COMPONENT_FXNS;
	}
	else if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
	{
		if(pComponentPrivate->nWMVFileType == VIDDEC_WMV_ELEMSTREAM)
		{
			pSplit->ps_prms->i4_profile = 1;
		}
		else if(pComponentPrivate->nWMVFileType == VIDDEC_WMV_RCVSTREAM)
		{
			pSplit->ps_prms->i4_profile = 0;
		}
		else
		{
			eError = OMX_ErrorBadParameter;
			goto EXIT;
		}

		ps_front_fxns =
				(ITTVIDDEC2FRONT_IMOD_Fxns_t *)&WMV9VDEC_ARM_COMPONENT_FXNS;
	}
	else
	{
		LOG_STATUS_ARM_API("ASC: No Function Table is available for this format\n");
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}

	i4_num_mem_recs = ps_front_fxns->s_ividdec2_fxns.ialg.algNumAlloc();
	ps_mem_recs     =
			codec_malloc_aligned(i4_num_mem_recs * sizeof(IALG_MemRec),128);

	i4_num_recs     = ps_front_fxns->s_ividdec2_fxns.ialg.algAlloc(
					  (IALG_Params *)pSplit->ps_prms,
					  NULL, ps_mem_recs);

	if(0 != app_alloc_mem_tables(ps_mem_recs, i4_num_mem_recs))
	{
		LOG_STATUS_ARM_API(
							"allocation of memtabs for asc failed \n");
		goto EXIT;
	}

	pSplit->h        = ps_mem_recs[0].base;
	pSplit->h->fxns  = ps_front_fxns;

	i4_status = pSplit->h->fxns->s_ividdec2_fxns.ialg.algInit(
	            (IALG_Handle)pSplit->h,
				ps_mem_recs,
				NULL,
				(IALG_Params *)pSplit->ps_prms);

	CHECK_API_AND_RETURN_ERROR(i4_status);
	codec_free_aligned(ps_mem_recs);

	
	/*******************************************************************/
	/*Control call for the getting Input and output buffer information */
	/*******************************************************************/
	i4_status   = pSplit->h->fxns->s_ividdec2_fxns.control(
				   (IVIDDEC2FRONT_Handle)pSplit->h,
				   XDM_GETBUFINFO,
				   (IVIDDEC2_DynamicParams *) pSplit->ps_dynamic_params,
				   NULL,
				   (IVIDDEC2FRONT_Status *) pSplit->ps_front_status);
	CHECK_API_AND_RETURN_ERROR(i4_status);

        /* Set the stride to the decoder */
	if(pOutPortDef->format.video.nFrameWidth != pInPortDef->format.video.nFrameWidth)
	{
//		ALOGD("%s : %d Setting stride", __func__, __LINE__);
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.displayWidth  = pOutPortDef->format.video.nFrameWidth;
	}

	ps_buf_info = &pSplit->ps_front_status->
						s_ividdec2_front_status.fullStatus.bufInfo;

	/*********************************************************************/
	/*Get the number of input buffers required from the status structure */
	/*********************************************************************/
	pSplit->ps_inbufs->numBufs = ps_buf_info->minNumInBufs;

	/*Get the input buffer size from the status structure */
	for(i = 0; i < pSplit->ps_inbufs->numBufs; i++)
	{
			pSplit->ps_inbufs->descs[i].bufSize = ps_buf_info->minInBufSize[i];
	}
	/*******************************************************************/
	/*Control call for the Context info (Intermediate buffers)         */
	/*******************************************************************/
	i4_status = pSplit->h->fxns->s_ividdec2_fxns.control(
					(IVIDDEC2FRONT_Handle) pSplit->h,
					XDM_GETCONTEXTINFO,
					(IVIDDEC2_DynamicParams *)pSplit->ps_dynamic_params,
					NULL,
					(IVIDDEC2FRONT_Status *)pSplit->ps_front_status);
	CHECK_API_AND_RETURN_ERROR(i4_status);

	OMX_MALLOC_STRUCT_SIZED(pSplit->ps_im_bufs, XDM1_BufDesc*,
					i4_free_count*sizeof(void *),
					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);
	total_im_buffer_size = 0;

	/*******************************************************************/
	/* Control call to cap the ARM codec, currently applicable only to */
	/* H.264 Codec                                                     */
	/*******************************************************************/
#ifdef CONFIG_3630
	if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
	{
		/**********************************************************************/
		/*	No of Cycles per frame allowed is calculated in the following way */
		/*	Cycles per frame = Clock / FPS									  */
		/*																	  */
		/*	For BP clips													  */
		/*	Clock @ 240 Mhz                                                   */
		/*	FPS   @ 30 FPS                                                    */
		/*	cycles per frame = 240000000 / 30 = 8000000                       */
		/*                                                                    */
		/*	For MP/HP clips                                                   */
		/*	Clock @ 540 Mhz                                                   */
		/*	FPS   @ 30 FPS                                                    */
		/*	cycles per frame = 540000000 / 30 = 18000000                      */
		/**********************************************************************/

		int displayWidthTemp = pSplit->ps_dynamic_params->
									s_ividdec2_dyn_params.displayWidth;

		int frameSkipMode =	pSplit->ps_dynamic_params->
									s_ividdec2_dyn_params.frameSkipMode;

		int frameOrder = pSplit->ps_dynamic_params->
									s_ividdec2_dyn_params.frameOrder;

		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.decodeHeader = 18000000;
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.displayWidth = 8000000;

		/* ASC clock */
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.frameSkipMode = 26666666;

		/* DSC clock */
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.frameOrder = 26666666;

		i4_status = pSplit->h->fxns->s_ividdec2_fxns.control(
				(IVIDDEC2FRONT_Handle) pSplit->h,
				LIMIT_CORTEX_LOAD,
				(IVIDDEC2_DynamicParams *)pSplit->ps_dynamic_params,
				NULL,
				(IVIDDEC2FRONT_Status *)pSplit->ps_front_status);
		CHECK_API_AND_RETURN_ERROR(i4_status);

		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.decodeHeader
															= XDM_DECODE_AU;
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.displayWidth
															= displayWidthTemp;
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.frameSkipMode
															=  frameSkipMode;
		pSplit->ps_dynamic_params->s_ividdec2_dyn_params.frameOrder
															=  frameOrder;
	}
#endif /* CONFIG_3630 */

	ps_ctxt_info = &pSplit->ps_front_status->s_ividdec2_front_status.contextInfo;
	for(i = 0; (i < XDM_MAX_IO_BUFFERS) &&
					(ps_ctxt_info->minIntermediateBufSizes[i] != 0); i++)
	{
		remainder = ps_ctxt_info->minIntermediateBufSizes[i] % 128;
		if(remainder != 0)
		{
			ps_ctxt_info->minIntermediateBufSizes[i] =
				ps_ctxt_info->minIntermediateBufSizes[i] + (128 - remainder);
		}
	}

	for(i = 0; (i < XDM_MAX_IO_BUFFERS) &&
						(ps_ctxt_info->minIntermediateBufSizes[i] != 0) ; i++)
	{
		total_im_buffer_size += ps_ctxt_info->minIntermediateBufSizes[i];
		i4_num_im_bufs++;
	}

	/* The codec fills the sizes of intermediate buffers with 0 terminating */
	for(j = 0; j < i4_free_count; j++)
	{
		/* Do not use the malloc macro to allocate IM buffers.           */
		/* The malloc macro has a memset to zero, this will increase the */
		/* time taken for the clip to start playing, as the size of IM   */
		/* buffers is quite large around 3MB and there are 4 in number   */
		pSplit->ps_im_bufs[j] = (XDM1_BufDesc *)malloc(sizeof(XDM1_BufDesc));
		pSplit->ps_im_bufs[j]->numBufs  = i4_num_im_bufs;
		buff_ptr = codec_malloc_aligned(total_im_buffer_size, 128);

		for( i = 0 ; i < i4_num_im_bufs; i++)
		{
			pSplit->ps_im_bufs[j]->descs[i].buf     = (char *)buff_ptr;
			pSplit->ps_im_bufs[j]->descs[i].bufSize =
									ps_ctxt_info->minIntermediateBufSizes[i];
			buff_ptr += ps_ctxt_info->minIntermediateBufSizes[i];
		}
	}

	/* Get the Alg context scalar buffer size */
	pSplit->s_alg_context.bufSize = ps_ctxt_info->minContextSize;

	OMX_MALLOC_STRUCT_SIZED(pSplit->ps_context, XDM_Context*,
					i4_free_count * sizeof(XDM_Context *),
					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

	OMX_MALLOC_STRUCT_SIZED(pSplit->pBufferHdr, OMX_BUFFERHEADERTYPE*,
						i4_free_count * sizeof(OMX_BUFFERHEADERTYPE *),
						pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

	OMX_MALLOC_STRUCT_SIZED(pSplit->pInpParam,
						  ITTVDEC_UALGInputParam_t *,
						  i4_free_count * sizeof(ITTVDEC_UALGInputParam_t *),
						  pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

	/* Allocate memory for the context pointers and push them into the free */
	/* context queue 														*/
	for(j = 0; j < i4_free_count; j++)
	{
		/* buff_ptr = codec_malloc_aligned(sizeof(XDM_Context),128); */
		OMX_MALLOC_STRUCT(buff_ptr, XDM_Context,
					pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

		OMX_MALLOC_STRUCT_DSPALIGN(pSplit->pInpParam[j],
								   ITTVDEC_UALGInputParam_t *,
								   sizeof(ITTVDEC_UALGInputParam_t));

		pSplit->ps_context[j] = (XDM_Context *)buff_ptr;

		for(i = 0; i < i4_num_im_bufs; i++)
		{
			/* Update the context structure with the IM buffers info */
			pSplit->ps_context[j]->intermediateBufs[i].buf     =
									pSplit->ps_im_bufs[j]->descs[i].buf;
			pSplit->ps_context[j]->intermediateBufs[i].bufSize =
									pSplit->ps_im_bufs[j]->descs[i].bufSize;
		}

		pSplit->ps_context[j]->algContext.buf     =
						codec_malloc_aligned(ps_ctxt_info->minContextSize,128);
		pSplit->ps_context[j]->algContext.bufSize =
						ps_ctxt_info->minContextSize;

		/* Set the last one to 0 */
		pSplit->ps_context[j]->intermediateBufs[i].buf     = 0;
		pSplit->ps_context[j]->intermediateBufs[i].bufSize = 0;
		pSplit->ps_context[j]->numInOutBufs     = 0;
		pSplit->ps_context[j]->inOutBufs[0].buf = 0;

		OMX_MALLOC_STRUCT(pSplit->pBufferHdr[j], OMX_BUFFERHEADERTYPE,
						pComponentPrivate->nMemUsage[VIDDDEC_Enum_MemLevel0]);

		memset(pSplit->pBufferHdr[j], 0, sizeof(OMX_BUFFERHEADERTYPE));

		/* Anything else that needs to be populated into the buffer header? */
		memset(pSplit->pBufferHdr[j], 0, sizeof(OMX_BUFFERHEADERTYPE));

		pSplit->pBufferHdr[j]->nAllocLen          = total_im_buffer_size;
		pSplit->pBufferHdr[j]->nFilledLen         = total_im_buffer_size;
		pSplit->pBufferHdr[j]->pBuffer            = (unsigned char *)pSplit->
												    ps_im_bufs[j]->descs[0].buf;
		pSplit->pBufferHdr[j]->nSize              = sizeof(OMX_BUFFERHEADERTYPE);
		pSplit->pBufferHdr[j]->nInputPortIndex    = VIDDEC_INPUT_PORT;
		pSplit->pBufferHdr[j]->nOutputPortIndex   = VIDDEC_NOPORT;
		pSplit->pBufferHdr[j]->pOutputPortPrivate = NULL;

		write(pSplit->free_context_pipe[1], &buff_ptr, sizeof(XDM_Context *));
	}

	return eError;

EXIT:

	LOG_STATUS_ARM_API("Come to exit there has been an error\n");
	CHECK_ALLOCATION_AND_FREE(ps_mem_recs);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_prms);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_dynamic_params);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_front_status);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_inargs);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_front_outargs);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_context);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_inbufs);
	CHECK_ALLOCATION_AND_FREE(pSplit->ps_im_bufs);

	ALOGE("Error in creation/initialization of arm split decoder");

	return eError;
}

/*****************************************************************************/
/*  Function Name : populate_context                                         */
/*  Description   : Checks continuously to see if a free context             */
/*					has been returned by the DSP and acquires it so that     */
/*                  the ARM side process call can fill it up again           */
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
static void *populate_context(OMX_VIDEO_PARAM_SPLITTYPE *pSplit,
							  VIDDEC_COMPONENT_PRIVATE  *pComponentPrivate)
{
    int          i4_i;
    XDM1_BufDesc *ps_inbufs;
    XDM_Context  *ps_context;
    int          fd_max;
    fd_set       rfds;
    int          status;
    sigset_t     set;
    struct       timespec tv;

    ps_inbufs = pSplit->ps_inbufs;
    fd_max    = pSplit->free_context_pipe[0];

    while(1)
    {
        FD_ZERO (&rfds);
        FD_SET(pSplit->free_context_pipe[0], &rfds);

        /* Change the timeout value to 0 in case of flush. For seek perf   */
        /* During stop, the timeout has been made 0 to decrease the   	   */
        /* time it takes to exit                                           */
        {
            OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;

            if((OMX_TRUE == pSplit->m_isFlushing) ||
                (pSplit->bStop == OMX_TRUE))
            {
                tv.tv_sec = 0;
                tv.tv_nsec = 30 * 1000 * 1000;
            }
            else if(pSplit->i4_free_count == pComponentPrivate->bInputBCountDsp)
            {
                tv.tv_sec = 0;
                tv.tv_nsec = 30 * 1000 * 1000;
            }
            else
            {
                tv.tv_sec = 1;
                tv.tv_nsec = 0;
            }
        }
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fd_max+1, &rfds, NULL, NULL, &tv, &set);
        sigdelset (&set, SIGALRM);
        if (0 == status)
        {
            if(OMX_TRUE == pSplit->bStop || OMX_TRUE == pSplit->m_isFlushing)
            {
                ps_context = NULL;
                goto EXIT;
            }
        }
        else if (-1 == status)
        {
            if(pSplit->bStop == OMX_TRUE)
            {
                ps_context = NULL;
                goto EXIT;
            }
        }
        else if(FD_ISSET(pSplit->free_context_pipe[0], &rfds))
        {
             break;
        }
        else if(pSplit->bStop == OMX_TRUE)
        {
            ps_context = NULL;
            goto EXIT;
        }
    }
    read(pSplit->free_context_pipe[0], &ps_context, sizeof(XDM_Context*));

    /* Get the input buffer size from the status structure */
    for(i4_i = 0; i4_i < ps_inbufs->numBufs; i4_i++)
    {
        ps_context->inBufs[i4_i].buf     = ps_inbufs->descs[i4_i].buf;
        ps_context->inBufs[i4_i].bufSize = ps_inbufs->descs[i4_i].bufSize;
    }

    /* Get the number of output buffers required from the status structure */
    ps_context->inBufs[i4_i].buf = 0;
    ps_context->numInBufs        = ps_inbufs->numBufs;

EXIT:
    return((void *)ps_context);
}

/*****************************************************************************/
/*  Function Name : IssueflushToASCThread                                    */
/*  Description   : Sets the isFlushing flag to true, resulting in ASC flush */
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
void IssueflushToASCThread(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
						   OMX_BOOL                 bCodecReset)
{
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;

    if(OMX_TRUE == pSplit->m_isThreadStopped)
    {
        LOG_STATUS_ARM_API("OMX_TRUE == pSplit->m_isThreadStopped\n");
        goto EXIT;
    }
    else
    {
        LOG_STATUS_ARM_API("OMX_FALSE == pSplit->m_isThreadStopped\n");
    }

    pSplit->m_isFlushing  = OMX_TRUE;
    pSplit->m_bCodecReset = bCodecReset;

    {
        pthread_mutex_lock(&pSplit->m_isFlushed_mutex);
        pthread_cond_wait(&pSplit->m_flushCond, &pSplit->m_isFlushed_mutex);
        pthread_mutex_unlock(&pSplit->m_isFlushed_mutex);
    }

    pSplit->m_isFlushing  = OMX_FALSE;
    pSplit->m_bCodecReset = OMX_FALSE;

EXIT:
    return;
}

/*****************************************************************************/
/*  Function Name : resetTheCodec	                                         */
/*  Description   : This function resets the codec.                          */
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
int resetTheCodec(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    int 					  i4_status;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
    OMX_U32 			      tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;

    /* Currently (3rd May 2010) the XDM_RESET is supported only by the H264   */
    /* codec.                                                                 */

    if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        i4_status = pSplit->h->fxns->s_ividdec2_fxns.control(
					(IVIDDEC2FRONT_Handle) pSplit->h,
					XDM_RESET,
					(IVIDDEC2_DynamicParams *)pSplit->ps_dynamic_params,
					NULL,
					(IVIDDEC2FRONT_Status *) pSplit->ps_front_status);
        if(i4_status != 0)
        {
            LOG_STATUS_ARM_API("Error while making the control call XDM_RESET to the codec");
        }
    }

    return i4_status;
}

UWORD32 get_max_ref_frms(OMX_U32 wd, OMX_U32 ht)
{
    OMX_U32 i4_size;
    OMX_U32 wd_mbs = wd / 16;
    OMX_U32 ht_mbs = ht / 16;
    OMX_U32 num_frms;

    /* Allocation is done assuming a level  of 3.1 */
    i4_size = 6912000;

    num_frms = i4_size / (wd_mbs * ht_mbs);
    num_frms = num_frms / 384; 
    if(num_frms > 16)
        num_frms = 16;
    
    return(num_frms);
}

/*****************************************************************************/
/*  Function Name : asc_thread                                               */
/*  Description   : ARM side decoding takes place here. An input frame is    */
/*                  consumed and an intermediate buffer is produced.         */
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
void *asc_thread(void *pv_handle)
{
    VIDDEC_COMPONENT_PRIVATE       *pComponentPrivate = pv_handle;
    OMX_VIDEO_PARAM_SPLITTYPE      *pSplit = pComponentPrivate->pSplit;
    XDM1_BufDesc                   *ps_inbufs;
    ITTVIDDEC2_IMOD_InArgs_t       *ps_inargs;
    XDM_Context                    *ps_context;
    ITTVIDDEC2FRONT_IMOD_OutArgs_t *ps_front_outargs;
    int                            i4_status;
    int                            fd_max;
    fd_set                         rfds;
    int                            status;
    OMX_BUFFERHEADERTYPE*          pBuffHead = NULL;
    sigset_t                       set;
    struct                         timespec tv;
    OMX_ERRORTYPE                  eError = OMX_ErrorNone;
    bool						   zeroByteBuffer = false;
	OMX_U32 			  		   tempCompressionFormat = pComponentPrivate->
								   pInPortDef->format.video.eCompressionFormat;

    static int count = 0;

    LOG_STATUS_ARM_API("Inside ASC thread\n");

    ps_inbufs                            = pSplit->ps_inbufs;
    ps_inargs                            = pSplit->ps_inargs;
    ps_inargs->s_ividdec2_inargs.inputID = 0;
    ps_front_outargs                     = pSplit->ps_front_outargs;

    fd_max = pSplit->AscFilledInputPipe[0];

    pSplit->m_isThreadStopped = OMX_FALSE;
    while(1)
    {
START:
        FD_ZERO(&rfds);
        FD_SET(pSplit->AscFilledInputPipe[0], &rfds);

        /* Reduce the timeout value in case of flush. For seek performance */
        /* During stop also the timeout has been reduced to decrease the   */
        /* time it takes to exit                                           */
        {
            if((OMX_TRUE == pSplit->m_isFlushing) ||
               ((OMX_TRUE == pSplit->bStop) &&
                (OMX_FALSE == pSplit->bEosReceivedAtInput)))
            {
                tv.tv_sec = 0;
                tv.tv_nsec = 0;
            }
            else
            {
                tv.tv_sec = 1;
                tv.tv_nsec = 0;
            }
        }
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fd_max+1, &rfds, NULL, NULL, &tv, &set);
        sigdelset (&set, SIGALRM);

        if (0 == status)
        {
            if(OMX_TRUE == pSplit->m_isFlushing)
            {
                LOG_STATUS_ARM_API("OMX_TRUE == pSplit->m_isFlushing.\n");

                if(OMX_TRUE == pSplit->m_bCodecReset)
                {
					pSplit->isResetPending = OMX_TRUE;
                }

                pthread_mutex_lock(&pSplit->m_isFlushed_mutex);
                pthread_cond_signal(&pSplit->m_flushCond);
                pthread_mutex_unlock(&pSplit->m_isFlushed_mutex);
            }
            else
            {
                LOG_STATUS_ARM_API("OMX_FALSE == pSplit->m_isFlushing.\n");
            }

            if(pSplit->bStop == OMX_TRUE)
            {
				/* This will close the ASC thread */
				 LOG_STATUS_ARM_API("Breaking Out of ASC thread\n");
                break;
            }
        }
        else if (-1 == status)
        {
            /* This is an error situation */
            LOG_STATUS_ARM_API("ASC_Thread - Breaking out of loop due to error in reading pipe.\n");
            break;
        }
        else if(FD_ISSET(pSplit->AscFilledInputPipe[0], &rfds))
        {
            int          ret;
            int          handle_incomplete_input_buff = 1;
            OMX_U32      bdoNotProcess = 0;

            ret = read(pSplit->AscFilledInputPipe[0], &(pBuffHead),
									sizeof(pBuffHead));

            /* Function to handle Header parsing and timestamp queuing */
            eError = VIDDEC_HandleDataBuf_FromApp (pComponentPrivate, pBuffHead,
													&bdoNotProcess);

			if (bdoNotProcess == 1)
			{
				LOG_STATUS_ARM_API("ASC_Thread - Continue\n");
				continue;
			}

			if((0 == pComponentPrivate->isLevelCheckDone) && \
				(0 == pComponentPrivate->isRefFrameCheckDone) && \
			   (pComponentPrivate->pInPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC))
			{
				UWORD32 max_ref_frames;
				
				max_ref_frames = get_max_ref_frms(
				    pComponentPrivate->pOutPortDef->format.video.nFrameWidth,
			        pComponentPrivate->pOutPortDef->format.video.nFrameHeight);

				ALOGD("%s:%d isRefFrameCheckDone flag set", __FUNCTION__, __LINE__);
				ALOGD("%s:%d isLevelCheckDone flag set", __FUNCTION__, __LINE__);
				pComponentPrivate->isRefFrameCheckDone = 1;
				pComponentPrivate->isLevelCheckDone = 1;
				
				ALOGD("*** nNumRefFrames = %d ***", pComponentPrivate->nNumRefFrames);
				ALOGD("*** nLevelIdc = %d ***", pComponentPrivate->nLevelIdc);
				ALOGD("*** max_ref_frames = %d ***", max_ref_frames);
				
            		/* CSR-OMAPS00285998 kirti.badkundri@sasken.com fix - Unable to Play 720p mkv files - Unsupported file Type [START] */

				//if((pComponentPrivate->nNumRefFrames > max_ref_frames) || (pComponentPrivate->nLevelIdc > 31))
				if((pComponentPrivate->nNumRefFrames > max_ref_frames) && (pComponentPrivate->nLevelIdc > 31))

                        /* CSR-OMAPS00285998 kirti.badkundri@sasken.com fix - Unable to Play 720p mkv files - Unsupported file Type [END] */
				{

					/* Return error event handler */
					ALOGD("%s:%d Sending: OMX_EventError->OMX_ErrorUnsupportedSetting", __FUNCTION__, __LINE__);

					eError = OMX_ErrorUnsupportedSetting;

					pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
									pComponentPrivate->pHandle->pApplicationPrivate,
									OMX_EventError,
									eError,
									OMX_ErrorUnsupportedSetting,
					"eError: OMX_ErrorUnsupportedSetting, set it as OMX_TI_ErrorCritical");

					eError = OMX_ErrorNone;
				}
			}
		
			/* When stop is done in the middle of playback or flush is done  */
			/* during seek, we want to exit ASAP                             */
			if(((OMX_TRUE == pSplit->bStop) &&
				(OMX_FALSE == pSplit->bEosReceivedAtInput))||
				(OMX_TRUE == pSplit->m_isFlushing))
			{
				write(pComponentPrivate->free_inpBuf_Q[1], &(pBuffHead),
					  sizeof(pBuffHead));
				continue;
			}

			if(OMX_TRUE == pSplit->isResetPending)
			{
				int i4_status = 0;

				i4_status = resetTheCodec(pComponentPrivate);
				pSplit->isResetPending = OMX_FALSE;
			}
            pComponentPrivate->frameCount++;

			/* This case is to handle codec config data in WMV */
            if((tempCompressionFormat == OMX_VIDEO_CodingWMV) &&
               (pBuffHead->nFlags & OMX_BUFFERFLAG_CODECCONFIG) &&
               (pBuffHead->nFilledLen) != 0)
            {
                OMX_S32              bytes_remaining;
                OMX_BUFFERHEADERTYPE pTempBuffHead;

                bytes_remaining             = pBuffHead->nFilledLen - 51;
                ps_inbufs->descs[0].buf     = (char *) pBuffHead->pBuffer + 51;
                ps_inbufs->descs[0].bufSize = pBuffHead->nFilledLen - 51;
                ps_inargs->s_ividdec2_inargs.numBytes
                                            = pBuffHead->nFilledLen - 51;

                memcpy(&(pTempBuffHead), pBuffHead, sizeof(pTempBuffHead));

                while(handle_incomplete_input_buff == 1)
                {
                    ps_inbufs->numBufs = 1;
                    ps_inargs->s_ividdec2_inargs.inputID++;
                    zeroByteBuffer = false;

                    ps_context = populate_context(pSplit, pComponentPrivate);
                    if(NULL == ps_context)
                    {
                        ps_inargs->s_ividdec2_inargs.inputID--;
                        write(pComponentPrivate->free_inpBuf_Q[1], &(pBuffHead),
														sizeof(pBuffHead));
                        goto START;
                    }
                    LOG_STATUS_ARM_API("Calling process pBuffHead->pBuffer %x InputID %d NumBytes %d\n",
								(int)pBuffHead->pBuffer,
								(int)ps_inargs->s_ividdec2_inargs.inputID,
								(int)pBuffHead->nFilledLen);

                    pSplit->arm_process_running = 1;
                    {

                        i4_status = pSplit->h->fxns->s_ividdec2_fxns.process(
									(IVIDDEC2FRONT_Handle)pSplit->h,
									(IVIDDEC2_InArgs *) ps_inargs,
									ps_context,
									(IVIDDEC2FRONT_OutArgs *)ps_front_outargs);
                    }
                    pSplit->arm_process_running = 0;

                    pComponentPrivate->sendEOSToDSP = OMX_FALSE;
                    pTempBuffHead.nFilledLen = ps_front_outargs->
										s_ividdec2_front_outargs.bytesConsumed;
                    pTempBuffHead.pBuffer = (void *)ps_context->inBufs[0].buf;
                    pTempBuffHead.nAllocLen = pTempBuffHead.nFilledLen;

                    /* Pass the Im buffer to DSP */
                    pass_context_to_dsp(pComponentPrivate, ps_context,
										&pTempBuffHead);

                    bytes_remaining -= ps_front_outargs->
									   s_ividdec2_front_outargs.bytesConsumed;

                    if((0 == bytes_remaining) ||
					   (pComponentPrivate->nWMVFileType == VIDDEC_WMV_RCVSTREAM))
                    {
                        handle_incomplete_input_buff = 0;
                    }
                    else
                    {
                        ps_inbufs->descs[0].buf += ps_front_outargs->
								   s_ividdec2_front_outargs.bytesConsumed;

                        ps_inbufs->descs[0].bufSize -= ps_front_outargs->
                                   s_ividdec2_front_outargs.bytesConsumed;

                        ps_inargs->s_ividdec2_inargs.numBytes -=
                        ps_front_outargs->s_ividdec2_front_outargs.bytesConsumed;

                        ps_inargs->s_ividdec2_inargs.inputID++;
                    }

                }
            }
            else
            {
				/* This is the case where normal processing takes place */
                ps_inbufs->numBufs = 1;
                ps_inargs->s_ividdec2_inargs.inputID++;

                if(0 == pBuffHead->nFilledLen)
                {
                    OMX_S32 i4_status = 0;

                    i4_status = pSplit->h->fxns->s_ividdec2_fxns.control(
						(IVIDDEC2FRONT_Handle) pSplit->h,
						 XDM_FLUSH,
						(IVIDDEC2_DynamicParams *)pSplit->ps_dynamic_params,
						 NULL,
						(IVIDDEC2FRONT_Status *) pSplit->ps_front_status);

                    if(i4_status != 0)
                        LOG_STATUS_ARM_API("Error while flushing the component");

                    ps_inargs->s_ividdec2_inargs.numBytes = 0;
                    zeroByteBuffer                        = true;
                }
                else
                {
                    zeroByteBuffer    = false;
                    ps_inargs->s_ividdec2_inargs.numBytes =
											pBuffHead->nFilledLen;
                }
                ps_inbufs->descs[0].buf     = (char *) pBuffHead->pBuffer;
                ps_inbufs->descs[0].bufSize = pBuffHead->nFilledLen;

				/* Acquire a free IM buffer */
                ps_context = populate_context(pSplit, pComponentPrivate);

                if(NULL == ps_context)
                {
//--[[ GB Patch START : junghyun.you@lge.com [2012.05.31]
                	//LGE_CHANGE_S [hj.eum@lge.com]  2011_05_13, for preventing this thread to be dead, when flush
//			                   if(true == zeroByteBuffer)
					if(true == zeroByteBuffer && OMX_FALSE == pSplit->m_isFlushing)

                	//LGE_CHANGE_S [hj.eum@lge.com]  2011_05_13, for preventing this thread to be dead, when flush
//--]] GB Patch END
                    {
                        write(pComponentPrivate->free_inpBuf_Q[1], &(pBuffHead),
							  sizeof(pBuffHead));
                        break;
                    }
                    else
                    {
                        ps_inargs->s_ividdec2_inargs.inputID--;
                        write(pComponentPrivate->free_inpBuf_Q[1], &(pBuffHead),
							  sizeof(pBuffHead));

                        goto START;
                    }
                }


#if DUMP_INP_TO_ARM
            dump_input_to_arm(pBuffHead);
#endif

#if ARM_PROCESSING_PROFILE
                gettimeofday(&startTime, NULL);
#endif

                pSplit->arm_process_running = 1;

		if(pComponentPrivate->controlCallNeeded)
		{
			/* Make a XDM_SETPARAMS control call to ASC */
//			ALOGD("%s : %d Calling Set Params Control Call", __func__, __LINE__);
			// Set params control call to the ARM side decoder
			pSplit->ps_dynamic_params->s_ividdec2_dyn_params.displayWidth  = pComponentPrivate->pOutPortDef->format.video.nFrameWidth;
				i4_status = pSplit->h->fxns->s_ividdec2_fxns.control(
		       				(IVIDDEC2FRONT_Handle) pSplit->h,
				   		XDM_SETPARAMS,
		       				(IVIDDEC2_DynamicParams *)pSplit->ps_dynamic_params,  NULL,
		       				(IVIDDEC2FRONT_Status *) pSplit->ps_front_status);
			//pComponentPrivate->controlCallNeeded = OMX_FALSE;
		}

		/* This is the ARM Process call */
                i4_status = pSplit->h->fxns->s_ividdec2_fxns.process(
								(IVIDDEC2FRONT_Handle)pSplit->h,
								(IVIDDEC2_InArgs *) ps_inargs,
								ps_context,
								(IVIDDEC2FRONT_OutArgs *)ps_front_outargs);

                pSplit->arm_process_running = 0;

//		ALOGD("%s ::: %d process() called. count = %d", __func__, __LINE__, ++count);
                LOG_STATUS_ARM_API("ARM_INP_PARAM Number of bytes = %d",
									ps_inargs->s_ividdec2_inargs.numBytes);
                LOG_STATUS_ARM_API("ARM_OUT_PARAM extended error = 0x%x",
					ps_front_outargs->s_ividdec2_front_outargs.extendedError);
                LOG_STATUS_ARM_API("ARM_OUT_PARAM bytes consumed = %d",
					ps_front_outargs->s_ividdec2_front_outargs.bytesConsumed);
                LOG_STATUS_ARM_API("first 4 bytes- 0x%x 0x%x 0x%x 0x%x \n",
											ps_context->inBufs[0].buf[0],
											ps_context->inBufs[0].buf[1],
											ps_context->inBufs[0].buf[2],
											ps_context->inBufs[0].buf[3]);

#if ARM_PROCESSING_PROFILE
				arm_processin_profile();
#endif

                /* Setting nFilledLen again here because the value gets reset */
                /* during the LCML callback for input buffer                  */
                pBuffHead->nFilledLen = pBuffHead->nAllocLen;

                if(true == zeroByteBuffer)
                {
                    pComponentPrivate->sendEOSToDSP = OMX_TRUE;
                }
                else
                {
                    pComponentPrivate->sendEOSToDSP = OMX_FALSE;
                }

                /* Send the IM buffer to DSP */
                pass_context_to_dsp(pComponentPrivate, ps_context, pBuffHead);
            }

            LOG_STATUS_ARM_API("Writing input buffer %p to free q \n", pBuffHead);

            write(pComponentPrivate->free_inpBuf_Q[1], &(pBuffHead),
															sizeof(pBuffHead));
        }
        else
        {
            LOG_STATUS_ARM_API("Unknown Pipe activity\n");
        }
    }/* end while */

    pSplit->m_isThreadStopped = OMX_TRUE;

    {
        pthread_mutex_lock(&pSplit->m_isStopped_mutex);
        pthread_cond_signal(&pSplit->m_isStopped_threshold);
        pthread_mutex_unlock(&pSplit->m_isStopped_mutex);
    }
    LOG_STATUS_ARM_API("Returning %d \n",(int)OMX_ErrorNone);

    return (void *) pComponentPrivate;
}

/*****************************************************************************/
/*  Function Name : put_context_into_free_pipe                               */
/*  Description   : When an IM buf is returned from the DSP, it is written   */
/*                  into the free context pipe so that it can be used by     */
/*                  the ARM side process again                               */
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
int put_context_into_free_pipe(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							   OMX_BUFFERHEADERTYPE     *pBuffHead)
{
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
    XDM_Context               *ps_context;
    int                       ret;
    int                       i;

    for( i = 0 ; i < pSplit->i4_free_count ; i++ )
    {
        if(pSplit->ps_context[i]->intermediateBufs[0].buf ==
		   (char *)pBuffHead->pBuffer)
            break;
    }

    ps_context = pSplit->ps_context[i];
    LOG_STATUS_ARM_API(
					"Got IMBuf = %x back from dsp Context = %x \n",
						(int)pBuffHead->pBuffer, (int) ps_context);

    /* Write the IM buffer that was received from DSP into the free pipe so */
    /* that it can be used by the ARM side again                            */
    ret = write(pSplit->free_context_pipe[1], &ps_context,
													sizeof(XDM_Context *));

    {
        int i;
        for(i = 0; i < MAX_DSP_INP_BUFS; i++)
        {
            if(pBuffHead->pBuffer == pSplit->dsp_inp_buf_status[i].buf)
            {
                pSplit->dsp_inp_buf_status[i].buf_status = BUF_RELEASED;
                break;
            }
        }
    }

    return ret;
}

/*****************************************************************************/
/*  Function Name : pass_context_to_dsp                                      */
/*  Description   : This is run time: Pass a buffer from ARM processing to   */
/*                  DSP and then you are done with it on the ASC. The im     */
/*                  buffer will be returned by the dsp when it is done and   */
/*                  that has to be given back to the free context queue. The */
/*                  input buffer is actually ready to be free. It does not   */
/*                  matter what we put into it here.                         */
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

static OMX_ERRORTYPE pass_context_to_dsp(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
							XDM_Context              *ps_context,
							OMX_BUFFERHEADERTYPE     *pInputBuffHead)
{
    OMX_ERRORTYPE             eError = OMX_ErrorNone;
    LCML_DSP_INTERFACE        *pLcmlHandle = pComponentPrivate->pLCML;
    OMX_VIDEO_PARAM_SPLITTYPE *pSplit = pComponentPrivate->pSplit;
    OMX_U8                    *pSrc, *pDst;
    OMX_S32                   i,j;
    OMX_S32                   nAllocLen, nFilledLen, in_param_size;
    OMX_U8                    *pBuffer;
    void                      *pUAlgInBufParam;
    OMX_BUFFERHEADERTYPE      *pBufferHeader;
    VIDDEC_BUFFER_PRIVATE     *pBufferPrivate ;
    TMMCodecBufferType        bufType;
    OMX_U32                   tempCompressionFormat = pComponentPrivate->
									pInPortDef->format.video.eCompressionFormat;

    pBufferPrivate = pInputBuffHead->pInputPortPrivate;
    for(i = 0; i < pSplit->i4_free_count; i++)
    {
        if(ps_context->intermediateBufs[0].buf ==
		   (char *)pSplit->pBufferHdr[i]->pBuffer)
            break;
    }

    pBuffer         = pSplit->pBufferHdr[i]->pBuffer;
    nAllocLen       = pSplit->pBufferHdr[i]->nAllocLen;
    nFilledLen      = pSplit->pBufferHdr[i]->nFilledLen;
    pBufferHeader   = pSplit->pBufferHdr[i];
    pUAlgInBufParam = pSplit->pInpParam[i];
    pSrc            = (OMX_U8*)(ps_context->algContext.buf);

    if(tempCompressionFormat == OMX_VIDEO_CodingWMV)
    {
        pDst = (OMX_U8*)(&(((WMV9VDEC_UALGInputParam*)pUAlgInBufParam)->
															sIttAlgContext));
        memcpy(pDst, pSrc, sizeof(WMV9Alg_context));
        in_param_size = sizeof(WMV9VDEC_UALGInputParam);
    }
    else if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        pDst = (OMX_U8*)(&(((H264VDEC_UALGInputParam*)pUAlgInBufParam)->
															sIttAlgContext));
        memcpy(pDst, pSrc, sizeof(H264Alg_context));
        in_param_size = sizeof(H264VDEC_UALGInputParam);
    }
    else
    {
        pDst = (OMX_U8*)(&(((MPEG4VDEC_UALGInputParam*)pUAlgInBufParam)->
															sIttAlgContext));
        memcpy(pDst, pSrc, sizeof(Mpeg4Alg_context));
        in_param_size = sizeof(MPEG4VDEC_UALGInputParam);
    }

    if(pInputBuffHead != NULL)
    {
        pBufferHeader->pInputPortPrivate = pInputBuffHead->pInputPortPrivate;
        pBufferHeader->nFlags = pInputBuffHead->nFlags;

        if(OMX_TRUE == pComponentPrivate->sendEOSToDSP)
        {
            pComponentPrivate->iEndofInput = 0x1;

            ((ITTVDEC_UALGInputParam*)pUAlgInBufParam)->bLastBuffer = TRUE;
            LOG_STATUS_ARM_API("This is the last buffer: pBuffer %x \n",(int)pBuffer);
        }
        else
        {
            ((ITTVDEC_UALGInputParam*)pUAlgInBufParam)->bLastBuffer = FALSE;
        }
    }

    for(j = 0; j < pSplit->ps_im_bufs[i]->numBufs; j++)
    {
        ((ITTVDEC_UALGInputParam *)pUAlgInBufParam)->ai4_im_buf_size[j] =
								  pSplit->ps_im_bufs[i]->descs[j].bufSize;
    }

    ((ITTVDEC_UALGInputParam *)pUAlgInBufParam)->i4_num_im_bufs =
											pSplit->ps_im_bufs[i]->numBufs;
    ((ITTVDEC_UALGInputParam *)pUAlgInBufParam)->i4_profile_enable =
											DSP_PROCESSING_PROFILE;

#if CHECK_SUM_ENABLE
    for(i = 0; i < 3; i++)
    {
        checksum(ps_context->intermediateBufs[i].buf,
				ps_context->intermediateBufs[i].bufSize, pComponentPrivate);
	}
#endif

    pBufferPrivate->eBufferOwner = VIDDEC_BUFFER_WITH_DSP;

    pthread_mutex_lock(&pComponentPrivate->InputBCountDspMutex);
    pComponentPrivate->bInputBCountDsp++;
    pthread_mutex_unlock(&pComponentPrivate->InputBCountDspMutex);

    if( pInputBuffHead->nFilledLen != 0 )
    {
#if DSP_START_PROCESSING_PROFILE
    dsp_start_processing();
#endif

        nFilledLen  = pSplit->pBufferHdr[i]->nFilledLen;

#ifndef DISABLE_MAP_REUSE
        bufType = EMMCodecInputBufferMapReuse;
        if((tempCompressionFormat == OMX_VIDEO_CodingAVC) ||
           (tempCompressionFormat == OMX_VIDEO_CodingMPEG4))
        {
            UWORD32 i4_mapped = 0;
            UWORD32 i;

            for(i = 0; i < MAX_DSP_INP_BUFS; i++)
            {
                if((int)pSplit->dsp_inp_buf_status[i].buf == (int)pBuffer)
                {
                    i4_mapped = 1;
                    break;
                }
                else if(pSplit->dsp_inp_buf_status[i].buf == NULL)
                {
                    i4_mapped = 0;
                    break;
                }
            }

            if(i4_mapped)
            {

                UWORD32 		   total_bufsize = 0, num_imbufs = 0;
                LCML_DSP_INTERFACE *phandle;
                OMX_HANDLETYPE     hComponent;
                int		           status;
                WORD32             imbufsizes[4];

                hComponent = ((LCML_DSP_INTERFACE*)pLcmlHandle)->
											pCodecinterfacehandle;
                phandle = (LCML_DSP_INTERFACE *)(
								((LCML_CODEC_INTERFACE *)hComponent)->pCodec);

                H264Alg_context *algbuf_ctxt =
								(H264Alg_context*)(ps_context->algContext.buf);

                if(tempCompressionFormat == OMX_VIDEO_CodingAVC)
                {
                    for(i = 0; i < 4; i++)
                    {
                        total_bufsize += algbuf_ctxt->au2_size_imbuf[i] << 7;
                        imbufsizes[i] = algbuf_ctxt->au2_size_imbuf[i] << 7;
                    }
                    num_imbufs = 4;
                }
                else
                {
                    imbufsizes[0] = 1024;
                    imbufsizes[1] = pSplit->ps_inargs->
											s_ividdec2_inargs.numBytes;
                    total_bufsize = imbufsizes[0] + imbufsizes[1];
                    num_imbufs	  = 2;
                }

                if(total_bufsize < (INVALIDATE_TRESHOLD >> 1))
                {
                    for(i = 0; i < num_imbufs; i++)
                    {
                        status = DSPProcessor_FlushMemory(
									phandle->dspCodec->hProc,
                                    ps_context->intermediateBufs[i].buf,
                                    imbufsizes[i],
                                    DSPMSG_WRBK_MEM);
                    }
                }
                else
                {
                    status = DSPProcessor_FlushMemory(phandle->dspCodec->hProc,
								pBuffer, total_bufsize,
								DSPMSG_WRBK_INV_ALL);	

                }
                nFilledLen = 0;
            }
        }

#else
        bufType = EMMCodecInputBufferMapBufLen;
#endif
        for(i = 0; i < MAX_DSP_INP_BUFS; i++)
        {
            if((int)pBuffer == (int)pSplit->dsp_inp_buf_status[i].buf)
            {
                pSplit->dsp_inp_buf_status[i].buf_status = BUF_QUEUED;
                break;
            }
            else if(NULL == pSplit->dsp_inp_buf_status[i].buf)
            {
                pSplit->dsp_inp_buf_status[i].buf = pBuffer;
                pSplit->dsp_inp_buf_status[i].buf_status = BUF_QUEUED;
                break;
            }
        }

        eError = LCML_QueueBuffer(
					((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
					bufType,
					pBuffer, /* Pointer to Large IM buffer */
					nAllocLen,
					nFilledLen, /* Set it to 0. */
					(OMX_U8 *) pUAlgInBufParam, /* contains the algContext */
					in_param_size,
					(OMX_U8*)pBufferHeader);

    }
    else
    {
        /* Give back the context. It does not have to go to dsp */
        write(pSplit->free_context_pipe[1], &ps_context, sizeof(XDM_Context *));

        /* Instead a zero size buffer needs to go */
        LOG_STATUS_ARM_API("Sending zero buf to DSP: pBuffer %x nAllocLen %d nFilledLen %d pUAlgInBufParam %x pBufferHeader %x \n",
			(int)pBuffer, (int)nAllocLen, (int)nFilledLen,  (int)pUAlgInBufParam,
			(int)pBufferHeader);

        ((ITTVDEC_UALGInputParam *)pUAlgInBufParam)->bLastBuffer = TRUE;

        pBufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
        eError = LCML_QueueBuffer(
				 ((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
				  EMMCodecInputBufferMapReuse,
				  pBuffer, /* Pointer to Large IM buffer */
				  0,
				  0,
				  (OMX_U8 *) pUAlgInBufParam, /* contains the algContext */
				  in_param_size,
				  (OMX_U8*)pBufferHeader);
    }

    return eError;
}

/*****************************************************************************/
/*  Function Name : asc_set_params_control_call                              */
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
void asc_set_params_control_call(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
   OMX_VIDEO_PARAM_SPLITTYPE *pSplit   = pComponentPrivate->pSplit;
   int 						 i4_status = 0;

	LOG_STATUS_ARM_API("Calling Set Params Control Call");
	/* Set params control call to the ARM side decoder */
	i4_status = pSplit->h->fxns->s_ividdec2_fxns.control(
               (IVIDDEC2FRONT_Handle) pSplit->h,
			   XDM_SETPARAMS,
               (IVIDDEC2_DynamicParams *)pSplit->ps_dynamic_params,  NULL,
               (IVIDDEC2FRONT_Status *) pSplit->ps_front_status);
}

/*****************************************************************************/
/*  Function Name : dsp_start_processing                                     */
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
#if DSP_START_PROCESSING_PROFILE
void dsp_start_processing()
{
		struct timeval  	      startTime;
		gettimeofday(&startTime, NULL);
        startTime.tv_usec += startTime.tv_sec * 1000 * 1000;
		if(dspStartProcessingIndex < MAX_PROFILE)
        {
            dspStartProcessing[dspStartProcessingIndex++] = startTime.tv_usec;
        }
        else if(MAX_PROFILE == dspStartProcessingIndex)
        {
            char *prefixStr = "Issuing IM buffer to DSP ";
            int index = 0;
            gDspStartProcProfileFP = fopen("/tmp/dsp_start_processing.txt", "w");

            for(index = 0; index < dspStartProcessingIndex; index++)
                    fprintf(gDspStartProcProfileFP, "\n %s is %llu", prefixStr,
												dspStartProcessing[index]);

            fclose(gDspStartProcProfileFP);

            dspStartProcessingIndex++;
        }
}
#endif
/*****************************************************************************/
/*  Function Name : dump_input_to_arm                                        */
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
#if DUMP_INP_TO_ARM
void dump_input_to_arm(OMX_BUFFERHEADERTYPE *pBuffHead)
{
		gDumpFp = fopen("/tmp/arm_dump", "ab");
		fwrite(pBuffHead->pBuffer, 1, pBuffHead->nFilledLen, gDumpFp);
		fclose(gDumpFp);
		gDumpFileSize = fopen("/tmp/arm_inp_size","a");
		fprintf(gDumpFileSize,"Inp Size %d\n", pBuffHead->nFilledLen);
		fclose(gDumpFileSize);
}
#endif
/*****************************************************************************/
/*  Function Name : arm_processin_profile                                    */
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
#if ARM_PROCESSING_PROFILE
void arm_processin_profile()
{
	gettimeofday(&endTime, NULL);
	startTime.tv_usec += startTime.tv_sec * 1000 * 1000;
	endTime.tv_usec += endTime.tv_sec * 1000 * 1000;

	ALOGD("%d : %s", __LINE__, __FUNCTION__);
	if(armProcessingAfterIndex < MAX_PROFILE)
	{
		armProcessTime[armProcessingBeforeIndex] =
						endTime.tv_usec - startTime.tv_usec;

		/* over 200 micro seconds => spurious peak */
		if(armProcessTime[armProcessingBeforeIndex] > (100000 ))
		{
			if(armProcessingBeforeIndex > 0)
				armProcessTime[armProcessingBeforeIndex] =
					armProcessTime[armProcessingBeforeIndex - 1];
			else
				armProcessTime[armProcessingBeforeIndex] = 0;
		}

		armProcessingBefore[armProcessingBeforeIndex++] =
												startTime.tv_usec;
		armProcessingAfter[armProcessingAfterIndex++] =
												endTime.tv_usec;
	}
}
#endif
/*****************************************************************************/
/*  Function Name : dump_arm_profile_info                                    */
/*  Description   : If profiling is enabled, this function dumps the ARM     */
/*					profiling info                                           */
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
#if ARM_PROCESSING_PROFILE
void dump_arm_profile_info()
{
    int                index = 0;
    unsigned long long armPeak4WindowTimeInUS = 0;
    unsigned long long armPeakTimeInUS = 0;
    unsigned long long armAvgTimeInUS = 0;

    LOG_STATUS_ARM_API("Dumping ARM profile info to /tmp/arm_process_time.txt\n");
    gProfileFP = fopen("/tmp/arm_process_time.txt", "w");

    for(index = 0; index < armProcessingBeforeIndex; index++)
    {
        if(index == 0)
        {
            fprintf(gProfileFP, "ArmStartTimeDiff %d ProcessTime %llu\n",
                    0,
                    armProcessTime[index]);
        }
        else
        {
            fprintf(gProfileFP, "ArmStartTimeDiff %llu ProcessTime %llu\n",
                    armProcessingBefore[index] - armProcessingBefore[index - 1],
                    armProcessTime[index]);
        }

        armAvgTimeInUS += armProcessTime[index];
        if(index < armProcessingBeforeIndex - 4)
        {
            unsigned long long tmp;
            unsigned int       i;
            tmp = 0;
            for(i = 0; i < 3;i++)
            {
                tmp += armProcessTime[index + i];
            }
            tmp /= 4;
            if(tmp > armPeak4WindowTimeInUS)
            {
                armPeak4WindowTimeInUS = tmp;
            }
        }

        if(armProcessTime[index] > armPeakTimeInUS)
        {
            armPeakTimeInUS = armProcessTime[index];
        }
    }

    armAvgTimeInUS /= armProcessingBeforeIndex;
    fprintf(gProfileFP, "\n armAvgTimeInUS         = %llu", armAvgTimeInUS);
    fprintf(gProfileFP, "\n armPeak4WindowTimeInUS = %llu",
													armPeak4WindowTimeInUS);
    fprintf(gProfileFP, "\n armPeakTimeInUS        = %llu", armPeakTimeInUS);

    fclose(gProfileFP);
    ALOGD("armAvgTimeInUS         = %llu", armAvgTimeInUS);
    ALOGD("armPeak4WindowTimeInUS = %llu", armPeak4WindowTimeInUS);
    ALOGD("armPeakTimeInUS        = %llu", armPeakTimeInUS);

    armProcessingAfterIndex++;
}
#endif
