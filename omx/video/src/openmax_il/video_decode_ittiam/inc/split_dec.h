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
/*  File Name         : split_dec.h  	                                     */
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

#ifndef __split_dec_h__
#define __split_dec_h__

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/
#define NUM_IMBUFS              (6)
#define MAX_DSP_INP_BUFS 		(32)
#define MAX_DSP_OUT_BUFS        (32)
#define MAX_ARM_INP_BUFS        (32)
#define MAX_ARM_OUT_BUFS        (32)
#define BUF_QUEUED              (2)
#define BUF_RELEASED            (1)

#define MPEG4VDEC_ARM_COMPONENT_FXNS MPEG4HDFRONT_ITTIAMVIDEODECODER_IMPEG4HDFRONT_IALG
#define H264VDEC_ARM_COMPONENT_FXNS  H264DECARM_ITTIAM_H264DECARM
#define WMV9VDEC_ARM_COMPONENT_FXNS  WMV9APSPLITDECFRONT_ITTIAM_IWMV9APSPLITDECFRONT

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/
#define CHECK_ALLOCATION_AND_RETURN_ERROR(ptr)\
if( NULL == ptr ){\
 eError = OMX_ErrorInsufficientResources;\
 goto EXIT;\
}

#define CHECK_ALLOCATION_AND_FREE(ptr)\
if( ptr ) codec_free_aligned(ptr);

#define CHECK_API_AND_RETURN_ERROR(param)\
if( param == -1 ) goto EXIT;

#define SPLIT_OPEN_PIPE(pipeName)\
    eError = pipe(pSplit->pipeName);\
    if(eError) {\
    LOG_STATUS_ARM_API("Could not create "#pipeName);\
    eError = OMX_ErrorInsufficientResources;\
    goto EXIT;\
    }

#define SPLIT_CLOSE_PIPE(pipeName)\
    err = close (pSplit->pipeName[VIDDEC_PIPE_READ]);\
    if ( 0 != err) {\
        eError = OMX_ErrorHardware;\
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,\
                pComponentPrivate->pHandle->pApplicationPrivate,\
                OMX_EventError,\
                eError,\
                OMX_TI_ErrorMajor,\
                "Error closing "#pipeName" for read");\
    }\
    err = close (pSplit->pipeName[VIDDEC_PIPE_WRITE]);\
    if ( 0 != err) {\
        eError = OMX_ErrorHardware;\
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,\
                pComponentPrivate->pHandle->pApplicationPrivate,\
                OMX_EventError,\
                eError,\
                OMX_TI_ErrorMajor,\
                "Error closing "#pipeName" for write");\
    }

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/
typedef struct
{
	void *buf;
	int  buf_status;
}itt_buf_status_t;

typedef struct OMX_PARAM_SPLITTYPE
{
	/* Asc will use, input pipe, output free pipe and these three pipes */
	OMX_S32 AscFilledInputPipe[2];
	OMX_S32 free_context_pipe[2];
	OMX_U32 bStop;

	/* Flag to detect EOS in input buffer */
	OMX_U32 bEosReceivedAtInput;

	pthread_t                            arm_thread_handle;
	ITTVIDDEC2FRONT_IMOD_Handle          h;
	ITTVIDDEC2_IMOD_Params_t            *ps_prms;
	ITTVIDDEC2_IMOD_DynamicParams_t     *ps_dynamic_params;
	ITTVIDDEC2FRONT_IMOD_Status_t       *ps_front_status;
	ITTVIDDEC2_IMOD_InArgs_t            *ps_inargs;
	ITTVIDDEC2FRONT_IMOD_OutArgs_t      *ps_front_outargs;
	ITTVIDDEC2FRONT_IMOD_Fxns_t         *ps_front_fxns;
	XDM_Context                         **ps_context;

	/* IM buffer headers array.Each one has a buffer header corresponding */
	/* to the IM buffers allocated 										  */
	OMX_BUFFERHEADERTYPE                **pBufferHdr;
	/* Input params to be sent to the dsp, need as many as the context */
	XDM1_BufDesc                        *ps_inbufs;
	XDM_BufDesc                         *ps_outbufs;
	XDM1_BufDesc                        **ps_im_bufs;
	XDM_SingleBufDesc                   s_alg_context;
	int  								i4_free_count;

	/* Variable used during flusing/stopping of ASC */
	OMX_U32 							m_isFlushing;
	OMX_U32 							m_isThreadStopped;
	pthread_cond_t 						m_isStopped_threshold;
	pthread_mutex_t 					m_isStopped_mutex;
	pthread_cond_t 						m_flushCond;
	pthread_mutex_t					    m_isFlushed_mutex;
	OMX_BOOL 							m_bCodecReset;
	OMX_BOOL                            isResetPending;

	ITTVDEC_UALGInputParam  **pInpParam;
	itt_buf_status_t 		dsp_inp_buf_status[MAX_DSP_INP_BUFS];
	itt_buf_status_t	    dsp_out_buf_status[MAX_DSP_OUT_BUFS];
	int     				arm_process_running;
	int     				timeout_cnt;
	int     				timeout_reported;
}OMX_VIDEO_PARAM_SPLITTYPE;

/*****************************************************************************/
/* Function Declarations                                              		 */
/*****************************************************************************/
/* Puts the IM buffer received from LCML callback into the free pipe */
int  put_context_into_free_pipe(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								OMX_BUFFERHEADERTYPE     *pBuffHead);

/* Writes the input buffer to the asc pipe */
OMX_ERRORTYPE write_to_asc_pipe(OMX_VIDEO_PARAM_SPLITTYPE *pSplit,
								OMX_BUFFERHEADERTYPE      *pBuffHead);

/* Creates the ASC thread */
void create_asc_thread(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

/* Stops the ASC thread */
void stop_asc_thread(  VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Makes the control call to set params of the ARM side codec */
void asc_set_params_control_call(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Allocates the split component structure */
OMX_ERRORTYPE allocate_split_component(
							VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Creates the asc_pipe and the free_context pipe */
OMX_ERRORTYPE create_split_pipes(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

/* Closes the asc_pipe and the free context pipe */
OMX_ERRORTYPE close_split_pipes(VIDDEC_COMPONENT_PRIVATE* pComponentPrivate);

/* Creates the ASC component by setting the various parameters of the codec */
OMX_ERRORTYPE create_split_arm_side_component(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Deletes the ARM side component */
OMX_ERRORTYPE delete_split_arm_side_component(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate);

/* Issues flush to the input buffers in the ASC thread */
void IssueflushToASCThread(VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
						   OMX_BOOL 				bCodecReset);

/* dumps the time taken for each frame by the arm process */
void arm_processin_profile();

/* Function tables of MPEG4, H264 and WMV9 codecs */
extern IALG_Fxns MPEG4HDFRONT_ITTIAMVIDEODECODER_IMPEG4HDFRONT_IALG;
extern IALG_Fxns H264DECARM_ITTIAM_H264DECARM;
extern IALG_Fxns WMV9APSPLITDECFRONT_ITTIAM_IWMV9APSPLITDECFRONT;

#endif // __split_dec_h__

