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
/*  File Name         : OMX_VideoDec_HandleCommands.c                        */
/*                                                                           */
/*  Description       : This file contains functions that handle commands    */
/*						issued by the OMX-IL client                          */
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
#define LOG_TAG "720p_VideoDec_HandleCommands"
#include <utils/Log.h>


/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSet                             */
/*  Description   : Handle State set commands from the client                */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSet (OMX_HANDLETYPE phandle,
											OMX_U32		   nParam1)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate =
								(VIDDEC_COMPONENT_PRIVATE *)phandle;
    OMX_ERRORTYPE 			  eError = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS("+++ENTERING\n");
    LOG_STATUS_OMX_CMDS("pComponentPrivate 0x%p phandle 0x%lx\n",
			pComponentPrivate, nParam1);

    switch (nParam1)
    {
        case OMX_StateIdle:
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandStateSetToIdle");
            eError = VIDDEC_HandleCommandStateSetToIdle(phandle);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }
        break;

        case OMX_StateExecuting:
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandStateSetToExecute");
            eError = VIDDEC_HandleCommandStateSetToExecute(phandle);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }
        break;
        case OMX_StateLoaded:
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandStateSetToLoaded");
            eError = VIDDEC_HandleCommandStateSetToLoaded(phandle);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }
        break;

        case OMX_StatePause:
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandStateSetToPause");
            eError = VIDDEC_HandleCommandStateSetToPause(phandle);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }
        break;
        case OMX_StateInvalid:
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandStateSetToInvalid");
            eError = VIDDEC_HandleCommandStateSetToInvalid(phandle);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }
        break;
        case OMX_StateWaitForResources:
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandStateSetToWaitForResources");
            eError = VIDDEC_HandleCommandStateSetToWaitForResources(phandle);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }
        break;

        case OMX_StateMax:
        {
            LOG_STATUS_OMX_CMDS("Transitioning to OMX_StateMax C 0x%x N 0x%lx\n",
								 pComponentPrivate->eState, nParam1);

            eError = OMX_ErrorIncorrectStateTransition;

            LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError because OMX_ErrorIncorrectStateTransition\n");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventError,
							   OMX_ErrorIncorrectStateTransition,
							   OMX_TI_ErrorMinor,
							   NULL);
        }
        break;
        default:
        {
            LOG_STATUS_OMX_CMDS("Transitioning to default C 0x%x N 0x%lx\n",
					pComponentPrivate->eState, nParam1);

            eError = OMX_ErrorIncorrectStateTransition;

            LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError because OMX_ErrorIncorrectStateTransition\n");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventError,
							   OMX_ErrorIncorrectStateTransition,
							   OMX_TI_ErrorMinor,
							   NULL);
        }
        break;
    } /* End of Switch */

EXIT:
    /* ASSUMPTION: The state transition command issued from the IL client    */
    /* will not be multiple commands at a time. i.e., after a state          */
    /* transition command is issued to the OMX IL component, the IL client   */
    /*  will do GetState to verify the completion of the state transition,   */
    /* although it may not wait until EventHandler callback from the OMX IL  */
    /* component.															 */
    if(pthread_mutex_lock(&pComponentPrivate->mutexStateChangeRequest))
    {
	   LOG_STATUS_OMX_CMDS( "---EXITING MUTEX LOCK FAILED\n");
       return OMX_ErrorUndefined;
    }

	/* Set state change request reference count to 0 */
    pComponentPrivate->nPendingStateChangeRequests = 0;

    /* Signal the thread waiting on this*/
    pthread_cond_signal(&(pComponentPrivate->StateChangeCondition));

    if(pthread_mutex_unlock(&pComponentPrivate->mutexStateChangeRequest))
    {
	   LOG_STATUS_OMX_CMDS( "---EXITING MUTEX UNLOCK FAILED\n");
       return OMX_ErrorUndefined;
    }

    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_EnablePort			                             */
/*  Description   : This Function Enables ports                              */
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
OMX_ERRORTYPE VIDDEC_EnablePort (VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								 OMX_U32                  nParam1)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    LOG_STATUS_OMX_CMDS("+++ENTERING\n");
    LOG_STATUS_OMX_CMDS("pComponentPrivate 0x%p nParam1 0x%lx\n",
								pComponentPrivate, nParam1);

    if (nParam1 == VIDDEC_INPUT_PORT)
    {
        LOG_STATUS_OMX_CMDS("Populated VIDDEC_INPUT_PORT IN 0x%x\n",
					pComponentPrivate->pInPortDef->bPopulated);

        if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
			!pComponentPrivate->pInPortDef->bPopulated) ||
            pComponentPrivate->sInSemaphore.bSignaled)
        {
			/* If the component is not in the loaded state, it waits here */
			/* until all input buffers are allocated                      */
            VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sInSemaphore);
        }

        if(pComponentPrivate->eLCMLState == VidDec_LCML_State_Unload &&
           pComponentPrivate->bDynamicConfigurationInProgress == OMX_FALSE &&
           pComponentPrivate->pInPortDef->bEnabled == OMX_TRUE &&
           pComponentPrivate->pOutPortDef->bEnabled == OMX_TRUE)
        {
            LOG_STATUS_OMX_CMDS( "DSC VIDDEC_INPUT_PORT\n");
            eError = VIDDEC_LoadCodec(pComponentPrivate);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }

        /* Sending notification to the IL client that port enable command is */
        /* complete                                                          */
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
        pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
								pComponentPrivate->pHandle->pApplicationPrivate,
								OMX_EventCmdComplete,
								OMX_CommandPortEnable,
								VIDDEC_INPUT_PORT,
								NULL);
    }
    else if (nParam1 == VIDDEC_OUTPUT_PORT)
    {
        LOG_STATUS_OMX_CMDS("Populated VIDDEC_OUTPUT_PORT OUT 0x%x\n",
				pComponentPrivate->pOutPortDef->bPopulated);

        if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
			!pComponentPrivate->pOutPortDef->bPopulated) ||
            pComponentPrivate->sOutSemaphore.bSignaled)
        {
			/* If the component is not in the loaded state, it waits here */
			/* until all output buffers are allocated                     */
            VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sOutSemaphore);
        }

        if(pComponentPrivate->eLCMLState == VidDec_LCML_State_Unload &&
		   pComponentPrivate->bDynamicConfigurationInProgress == OMX_FALSE &&
		   pComponentPrivate->pInPortDef->bEnabled == OMX_TRUE &&
		   pComponentPrivate->pOutPortDef->bEnabled == OMX_TRUE)
        {
            LOG_STATUS_OMX_CMDS( "BSC VIDDEC_OUTPUT_PORT\n");
            eError = VIDDEC_LoadCodec(pComponentPrivate);
            if(eError != OMX_ErrorNone)
            {
                goto EXIT;
            }
        }

        /* Sending notification to the IL client that port enable command is */
        /* complete                                                          */
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
        pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
								pComponentPrivate->pHandle->pApplicationPrivate,
								OMX_EventCmdComplete,
								OMX_CommandPortEnable,
								VIDDEC_OUTPUT_PORT,
								NULL);
    }
    else if (nParam1 == OMX_ALL)
    {
        if(pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
        {
			/* Enters here only in the case of tunneled components */
            LOG_STATUS_OMX_CMDS("Populated VIDDEC_INPUT_PORT IN 0x%x\n",
				pComponentPrivate->pInPortDef->bPopulated);

            if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
				!pComponentPrivate->pInPortDef->bPopulated) ||
                pComponentPrivate->sInSemaphore.bSignaled)
            {
				/* If the component is not in the loaded state, it waits here */
				/* until all input buffers are allocated                      */
                VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sInSemaphore);
            }

            if(pComponentPrivate->eLCMLState == VidDec_LCML_State_Unload)
            {
                eError = VIDDEC_LoadCodec(pComponentPrivate);
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }

			/* Sending notification to the IL client that port enable command */
			/* is complete                                                    */
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
            pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortEnable,
							VIDDEC_INPUT_PORT,
							NULL);
        }
        else
        {
            if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
				!pComponentPrivate->pInPortDef->bPopulated) ||
                pComponentPrivate->sInSemaphore.bSignaled)
            {
				/* If the component is not in the loaded state, it waits here */
				/* until all input buffers are allocated                      */
                VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sInSemaphore);
            }
            if(pComponentPrivate->eLCMLState == VidDec_LCML_State_Unload)
            {
                LOG_STATUS_OMX_CMDS( "BSC OMX_ALL\n");
                eError = VIDDEC_LoadCodec(pComponentPrivate);
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }

            LOG_STATUS_OMX_CMDS("Populated VIDDEC_INPUT_PORT IN 0x%x\n",
						pComponentPrivate->pInPortDef->bPopulated);

			/* Sending notification to the IL client that port enable command */
			/*  is complete                                                   */
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
            pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortEnable,
							VIDDEC_INPUT_PORT,
							NULL);

            if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
					!pComponentPrivate->pOutPortDef->bPopulated) ||
                pComponentPrivate->sOutSemaphore.bSignaled)
            {
				/* If the component is not in the loaded state, it waits here */
				/* until all output buffers are allocated                     */
                VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sOutSemaphore);
            }

            LOG_STATUS_OMX_CMDS("Populated VIDDEC_INPUT_PORT IN 0x%x\n",
							pComponentPrivate->pInPortDef->bPopulated);

			/* Sending notification to the IL client that port enable command */
			/*  is complete                                                   */
	        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
            pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortEnable,
							VIDDEC_OUTPUT_PORT,
							NULL);
        }
    }

EXIT:
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}



/*****************************************************************************/
/*  Function Name : VIDDEC_DisablePort			                             */
/*  Description   : This Function Disable ports.                             */
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
OMX_ERRORTYPE VIDDEC_DisablePort (VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								  OMX_U32 					nParam1)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS("+++ENTERING\n");
    LOG_STATUS_OMX_CMDS( "pComponentPrivate 0x%p nParam1 0x%lx\n",
						pComponentPrivate, nParam1);

    /* Flush is performed before Unload Codec to avoid issues during free */
    /* buffer as LCML would already be destroyed 						  */
    eError = VIDDEC_HandleCommandFlush(pComponentPrivate, nParam1, OMX_FALSE);
	if (eError != OMX_ErrorNone)
	{
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError Flush Failed\n");
		pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						eError,
						OMX_TI_ErrorMinor,
						"VIDDEC_UnloadCodec");
		goto EXIT;
	}

    /* Unloading codec */
    eError = VIDDEC_UnloadCodec(pComponentPrivate);
    if (eError != OMX_ErrorNone)
    {
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError VIDDEC_UnloadCodec failed\n");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							eError,
							OMX_TI_ErrorMinor,
							"VIDDEC_UnloadCodec");
        goto EXIT;
    }

    if (nParam1 == VIDDEC_INPUT_PORT)
    {
        if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
			  pComponentPrivate->pInPortDef->bPopulated) ||
              pComponentPrivate->sInSemaphore.bSignaled)
        {
            VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sInSemaphore);
        }
        LOG_STATUS_OMX_CMDS("Unpopulated VIDDEC_INPUT_PORT IN 0x%x\n",
				pComponentPrivate->pInPortDef->bPopulated);

        pComponentPrivate->bInPortSettingsChanged = OMX_FALSE;

        LOG_STATUS_OMX_CMDS("bInPortSettingsChanged = OMX_FALSE;\n");

		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
        pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortDisable,
							VIDDEC_INPUT_PORT,
							NULL);
    }
    else if (nParam1 == VIDDEC_OUTPUT_PORT)
    {
        if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
			  pComponentPrivate->pOutPortDef->bPopulated) ||
                pComponentPrivate->sOutSemaphore.bSignaled)
        {
            VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sOutSemaphore);
        }

        LOG_STATUS_OMX_CMDS("Unpopulated VIDDEC_OUTPUT_PORT OUT 0x%x\n",
						pComponentPrivate->pOutPortDef->bPopulated);
        LOG_STATUS_OMX_CMDS("bOutPortSettingsChanged = OMX_FALSE;\n");

        pComponentPrivate->bOutPortSettingsChanged = OMX_FALSE;

		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
        pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortDisable,
							VIDDEC_OUTPUT_PORT,
							NULL);
    }
    else if (nParam1 == OMX_ALL)
    {
        if(pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
        {
            if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
				  pComponentPrivate->pInPortDef->bPopulated) ||
                pComponentPrivate->sInSemaphore.bSignaled)
            {
                VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sInSemaphore);
            }
            LOG_STATUS_OMX_CMDS("Unpopulated VIDDEC_INPUT_PORT IN 0x%x\n",
					pComponentPrivate->pInPortDef->bPopulated);

            pComponentPrivate->bInPortSettingsChanged = OMX_FALSE;

            LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
            pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortDisable,
							VIDDEC_INPUT_PORT,
							NULL);
        }
        else
        {
            if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
				  pComponentPrivate->pInPortDef->bPopulated) ||
                pComponentPrivate->sInSemaphore.bSignaled)
            {
                VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sInSemaphore);
            }

            LOG_STATUS_OMX_CMDS("Populated VIDDEC_INPUT_PORT IN 0x%x\n",
						pComponentPrivate->pInPortDef->bPopulated);

            pComponentPrivate->bInPortSettingsChanged = OMX_FALSE;

            LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
            pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortDisable,
							VIDDEC_INPUT_PORT,
							NULL);

            if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
				  pComponentPrivate->pOutPortDef->bPopulated) ||
                pComponentPrivate->sOutSemaphore.bSignaled)
            {
                VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->sOutSemaphore);
            }

            LOG_STATUS_OMX_CMDS("Populated VIDDEC_OUTPUT_PORT IN 0x%x\n",
					pComponentPrivate->pInPortDef->bPopulated);

            LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete\n");
            pComponentPrivate->bOutPortSettingsChanged = OMX_FALSE;
            pComponentPrivate->cbInfo.EventHandler (pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandPortDisable,
							VIDDEC_OUTPUT_PORT,
							NULL);
        }
    }

EXIT:
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandFlush                                */
/*  Description   : This Functions Handles Flush request by Client           */
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
OMX_ERRORTYPE VIDDEC_HandleCommandFlush(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								OMX_U32 				 nParam1,
								OMX_BOOL 				 bPass)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BOOL 	  codecReset = OMX_FALSE;

    LOG_STATUS_OMX_CMDS("+++ENTERING");
    LOG_STATUS_OMX_CMDS("pComponentPrivate 0x%p nParam1 0x%lx",
										pComponentPrivate, nParam1);

    if (nParam1 == VIDDEC_INPUT_PORT || nParam1 == OMX_ALL)
    {

        /* MediaFrameworktests execution was causing the IM buffers generated */
        /* not to be unmapped.This would happen when explicit flush command   */
        /* is not sent to the OMX IL component. Now, the OMX IL thread will   */
        /* synchronise with the ARM thread before completing Flush command    */
        /* execution.Hence, the if(bPass) is commented                        */


        {
			/* This is to make sure that all the output buffers are with the  */
			/* DSP so that it can process and send back the IM buffers needed */
			/* by the ASC thread                                              */
            OMX_VidDec_HandlePipes(pComponentPrivate);

            /* bPass will be OMX_TRUE if it is an external command           */
            /* If this to handle the external command flush then we need to  */
            /* reset the codec, so that the reference frames would be        */
            /* discarded                                                     */
            if(OMX_TRUE == bPass)
            {
                codecReset = OMX_TRUE;
                pComponentPrivate->isOpBufFlushComplete = OMX_FALSE;
            }

			LOG_STATUS_OMX_CMDS("Issuing flush to ASC thread");
			/* Issue Flush to ASC thread to return all input buffers */
            IssueflushToASCThread(pComponentPrivate, codecReset);
        }

		LOG_STATUS_OMX_CMDS("Calling LCML flush on VIDDEC_INPUT_PORT");
		/* Calling LCML flush to return all IM buffers */
        eError = VIDDEC_LCML_Flush(pComponentPrivate, VIDDEC_INPUT_PORT);
        if (eError != OMX_ErrorNone)
        {
            goto EXIT;
        }

		LOG_STATUS_OMX_CMDS("Calling Circular buffer flush");
		/* Doing circular buffer flush to flush the timestamp queue */
        VIDDEC_CircBuf_Flush(pComponentPrivate, VIDDEC_CBUFFER_TIMESTAMP,
							VIDDEC_INPUT_PORT);

		LOG_STATUS_OMX_CMDS("Calling OMX_VidDec_HandlePipes and VIDDEC_ReturnBuffers");
        /* Clear the pipes */
        OMX_VidDec_HandlePipes(pComponentPrivate);

        /* Return buffers will return all input buffers back to the IL Client */
        VIDDEC_ReturnBuffers(pComponentPrivate, VIDDEC_INPUT_PORT, OMX_TRUE);

        if(bPass)
        {
            pComponentPrivate->isLevelCheckDone = 0;
            pComponentPrivate->isRefFrameCheckDone = 0;	
			/* If flush has been issued by the IL client send command   */
			/* complete notification                                    */
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete for VIDDEC_INPUT_PORT\n");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventCmdComplete,
							   OMX_CommandFlush,
							   VIDDEC_INPUT_PORT,
							   NULL);
        }
    }

    if(nParam1 == VIDDEC_OUTPUT_PORT || nParam1 == OMX_ALL)
    {
        if(bPass)
        {
			/* Queue all the output buffers that have been received */
            OMX_VidDec_HandlePipes(pComponentPrivate);
        }

		LOG_STATUS_OMX_CMDS("Calling LCML flush on VIDDEC_OUTPUT_PORT");
		/* Flush the DSP side to get back all the queued output buffers */
        eError = VIDDEC_LCML_Flush(pComponentPrivate, VIDDEC_OUTPUT_PORT);
        if (eError != OMX_ErrorNone)
        {
            goto EXIT;
        }

		LOG_STATUS_OMX_CMDS("Calling OMX_VidDec_HandlePipes on VIDDEC_ReturnBuffers on VIDDEC_OUTPUT_PORT");
		/* Handle all the output buffers received from the DSP and clear the */
		/* pipes                                                             */
        OMX_VidDec_Return_except_Freeoutbuf(pComponentPrivate);

        /* Returns all output buffers back to the IL Client */
        VIDDEC_ReturnBuffers(pComponentPrivate, VIDDEC_OUTPUT_PORT, OMX_TRUE);
        
		
		/* Added resetting EOS related flags to play after EOS */
		
		pComponentPrivate->bPlayCompleted = 0;
		pComponentPrivate->iEndofInputSent = 0;
		pComponentPrivate->iEndofInput = 0;
		pComponentPrivate->bUseFlaggedEos      = OMX_TRUE;
		pComponentPrivate->bBuffFound = 0;
		pComponentPrivate->EOSHasBeenSentToAPP = OMX_FALSE;
		
		if(bPass)
        {
            pComponentPrivate->isLevelCheckDone = 0;
            pComponentPrivate->isRefFrameCheckDone = 0;	

			/* If flush has been issued by the IL client send command   */
			/* complete notification                                    */
			LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete for VIDDEC_OUTPUT_PORT\n");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandFlush,
							VIDDEC_OUTPUT_PORT,
							NULL);
        }
    }

EXIT:
    LOG_STATUS_OMX_CMDS("---EXITING(0x%x)\n",eError);
    return eError;
}


/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSetToIdle                       */
/*  Description   : This Function Handles State Change request to idle.      */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToIdle(OMX_HANDLETYPE phandle)
{
    OMX_U32                      message[4];
    VIDDEC_COMPONENT_PRIVATE     *pComponentPrivate =
									(VIDDEC_COMPONENT_PRIVATE *)phandle;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefIn  = pComponentPrivate->pInPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefOut = pComponentPrivate->pOutPortDef;
    OMX_ERRORTYPE 				 eError       = OMX_ErrorNone;
    void				         *p 		  = NULL;

    LOG_STATUS_OMX_CMDS("+++ENTERING\n");

    message[0] = 0x400;
    message[1] = 100;
    message[2] = 0;
    p = (void *)&message;

    LOG_STATUS_OMX_CMDS("Transitioning to OMX_StateIdle Current State = 0x%x N\n",
						pComponentPrivate->eState);

    if (pComponentPrivate->eState == OMX_StateIdle)
    {
		/* If the current state is also idle return error */
        eError = OMX_ErrorSameState;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorSameState\n");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventError,
						   OMX_ErrorSameState,
						   OMX_TI_ErrorMinor,
						   "Same state");
    }
    else if ((pComponentPrivate->eState == OMX_StateLoaded) ||
              pComponentPrivate->eState == OMX_StateWaitForResources)
    {
#ifdef __PERF_INSTRUMENTATION__
		PERF_Boundary(pComponentPrivate->pPERFcomp,
					  PERF_BoundaryStart | PERF_BoundarySetup);
#endif
		/* This is during initialization */
        if ((pPortDefIn->bEnabled == OMX_TRUE &&
			 pPortDefOut->bEnabled == OMX_TRUE) ||
            (pPortDefIn->bEnabled == OMX_TRUE &&
             pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL))
        {
			/* ENters here only when the OUTPUT port is tunneled */
            LOG_STATUS_OMX_CMDS("Before pPortDefIn->bEnabled 0x%x pPortDefOut->bEnabled 0x%x\n",
								 pPortDefIn->bEnabled, pPortDefOut->bEnabled);

            if(pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
            {
                if((!pComponentPrivate->pInPortDef->bPopulated) ||
					 pComponentPrivate->sInSemaphore.bSignaled)
                {
					/* Wait till all Input buffers are allocated */
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sInSemaphore);

                    LOG_STATUS_OMX_CMDS("tunneling pPortDefIn->bEnabled 0x%x pPortDefOut->bEnabled 0x%x\n",
                        pPortDefIn->bEnabled, pPortDefOut->bEnabled);
                }
            }
            else
            {
                if((!pComponentPrivate->pOutPortDef->bPopulated) ||
					 pComponentPrivate->sOutSemaphore.bSignaled)
                {
					/* Wait till all I/O buffers are allocated */
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sInSemaphore);
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sOutSemaphore);

                    LOG_STATUS_OMX_CMDS("Standalone pPortDefIn->bEnabled 0x%x pPortDefOut->bEnabled 0x%x\n",
                        pPortDefIn->bEnabled, pPortDefOut->bEnabled);
                }
            }

            LOG_STATUS_OMX_CMDS("After pPortDefIn->bEnabled 0x%x pPortDefOut->bEnabled 0x%x\n",
								 pPortDefIn->bEnabled, pPortDefOut->bEnabled);
        }
        else
        {
            if(pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
            {
                if(pComponentPrivate->sInSemaphore.bSignaled)
                {
					/* Wait till all Input buffers are allocated */
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
															sInSemaphore);
                }
            }
            else
            {
				/* Non-tunneled Case */
                if(pComponentPrivate->sInSemaphore.bSignaled)
                {
					/* Wait till all Input buffers are allocated */
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
													sInSemaphore);
                }
                if(pComponentPrivate->sOutSemaphore.bSignaled)
                {
					/* Wait till all Output buffers are allocated */
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
													sOutSemaphore);
                }
            }
        }

#ifdef RESOURCE_MANAGER_ENABLED
	eError = VIDDEC_RM_LoadedToIdle(pComponentPrivate);

	if (eError != OMX_ErrorNone)
		goto EXIT;
#endif

        LOG_STATUS_OMX_CMDS( "Calling VIDDEC_LoadCodec\n");

        eError = VIDDEC_LoadCodec(pComponentPrivate);
        if (eError != OMX_ErrorNone)
        {
            goto EXIT;
        }

        pComponentPrivate->eState = OMX_StateIdle;

        /* Sending Notification back to IL Client that transition to idle is */
        /* complete                         								 */
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventCmdComplete,
							   OMX_CommandStateSet,
							   pComponentPrivate->eState,
							   NULL);
    }
    else if (pComponentPrivate->eState == OMX_StateExecuting ||
			 pComponentPrivate->eState == OMX_StatePause)
    {
		/* This is during de-init path */
		LOG_STATUS_OMX_CMDS( "Moving to Idle 0x%x\n",
				pComponentPrivate->eExecuteToIdle);

		pComponentPrivate->iEndofInput = OMX_FALSE;
		pComponentPrivate->iEndofInputSent = OMX_FALSE;

		/* Clear the pipes */
		OMX_VidDec_HandlePipes (pComponentPrivate);

		/* Destroy the ASC thread */
		LOG_STATUS_OMX_CMDS("Calling stop_asc_thread");
		stop_asc_thread(pComponentPrivate);

		/* Flush both input and output ports */
		LOG_STATUS_OMX_CMDS("Calling VIDDEC_HandleCommandFlush");
		VIDDEC_HandleCommandFlush(pComponentPrivate, -1, OMX_FALSE);

		/* Stop the DSP side codec */
		LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Stop");
		VIDDEC_LCML_Stop(pComponentPrivate);

		LOG_STATUS_OMX_CMDS("Calling OMX_VidDec_HandlePipes and VIDDEC_ReturnBuffers");
		/* Return all the I/O buffers back to the IL-Client/Appln */
		OMX_VidDec_HandlePipes (pComponentPrivate);
		VIDDEC_ReturnBuffers(pComponentPrivate, -1, OMX_TRUE);

#ifdef RESOURCE_MANAGER_ENABLED
		LOG_STATUS_OMX_CMDS("Calling VIDDEC_RM_ExecutingToIdle");
		eError = VIDDEC_RM_ExecutingToIdle(pComponentPrivate);
#endif
		/* Setting the state as idle */
		pComponentPrivate->eState = OMX_StateIdle;

		/* Send a notification to the IL Client that state transition */
		/* is complete                                                */
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete");
		pComponentPrivate->cbInfo.EventHandler(
					pComponentPrivate->pHandle,
					pComponentPrivate->pHandle->pApplicationPrivate,
					OMX_EventCmdComplete,
					OMX_CommandStateSet,
					pComponentPrivate->eState,
					NULL);

		eError = OMX_ErrorNone;

		pComponentPrivate->eExecuteToIdle = OMX_StateInvalid;
    }
    else
    {
        eError = OMX_ErrorIncorrectStateTransition;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorIncorrectStateTransition");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventError,
							   OMX_ErrorIncorrectStateTransition,
							   OMX_TI_ErrorMinor,
							   NULL);
    }

    pComponentPrivate->eExecuteToIdle = OMX_StateInvalid;

EXIT:
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);

#if ARM_PROCESSING_PROFILE
	LOG_STATUS_OMX_CMDS("Calling dump_arm_profile_info");
    dump_arm_profile_info();
#endif

#if DUMP_DSP_PROCESS_TIME
	LOG_STATUS_OMX_CMDS( "Calling dump_dsp_profile_info");
    dump_dsp_profile_info();
#endif

    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSetToExecute                    */
/*  Description   : This Function Handles State Change request to Execute    */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToExecute(OMX_HANDLETYPE phandle)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate =
									(VIDDEC_COMPONENT_PRIVATE*)phandle;
    OMX_ERRORTYPE 			 eError = OMX_ErrorNone;
    OMX_U32 			 	 tempCompressionFormat = pComponentPrivate->
								pInPortDef->format.video.eCompressionFormat;

    LOG_STATUS_OMX_CMDS("+++ENTERING\n");

#ifdef __PERF_INSTRUMENTATION__
        pComponentPrivate->lcml_nCntOpReceived = 0;
#endif

    LOG_STATUS_OMX_CMDS("Transitioning to OMX_StateExecuting C 0x%x N\n",
						pComponentPrivate->eState);

    if (pComponentPrivate->eState == OMX_StateExecuting)
    {
        eError = OMX_ErrorSameState;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError because OMX_ErrorSameState\n");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							OMX_ErrorSameState,
							OMX_TI_ErrorMinor,
							"Invalid State");
    }
    else if (pComponentPrivate->eState == OMX_StateIdle ||
			 pComponentPrivate->eState == OMX_StatePause)
    {
        LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Start\n");
        VIDDEC_LCML_Start(pComponentPrivate);
        if(pComponentPrivate->eState == OMX_StateIdle)
        {
            /* Call the control call on the arm side of the codec */
            LOG_STATUS_OMX_CMDS("Calling asc_set_params_control_call\n");
            asc_set_params_control_call(pComponentPrivate);
        }

        if ((tempCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
             tempCompressionFormat == OMX_VIDEO_CodingAVC ||
             tempCompressionFormat == OMX_VIDEO_CodingWMV) &&
            pComponentPrivate->eState == OMX_StateIdle)
        {
			LOG_STATUS_OMX_CMDS("Calling create_asc_thread\n");
            create_asc_thread(pComponentPrivate);
        }

#ifdef RESOURCE_MANAGER_ENABLED
		LOG_STATUS_OMX_CMDS("Calling VIDDEC_RM_StateSetToExecute\n");
		eError = VIDDEC_RM_StateSetToExecute(pComponentPrivate);

		if (eError != OMX_ErrorNone)
			goto EXIT;
#endif

        eError                             = OMX_ErrorNone;
        pComponentPrivate->iEndofInput     = 0;
        pComponentPrivate->iEndofInputSent = 0;
        pComponentPrivate->eState          = OMX_StateExecuting;

        /* Callback to the app that commandStateSet is complete */
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventCmdComplete,
						   OMX_CommandStateSet,
						   pComponentPrivate->eState,
						   NULL);
    }
    else
    {
        eError = OMX_ErrorIncorrectStateTransition;

        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError because OMX_ErrorIncorrectStateTransition");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorIncorrectStateTransition,
						OMX_TI_ErrorMinor,
						"Incorrect State Transition");
    }

EXIT:
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSetToLoaded                     */
/*  Description   : This Function Handles State Change request to Loaded     */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToLoaded(OMX_HANDLETYPE phandle)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate =
										(VIDDEC_COMPONENT_PRIVATE *)phandle;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefIn  = pComponentPrivate->pInPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefOut = pComponentPrivate->pOutPortDef;
    OMX_ERRORTYPE eError					  = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS( "+++ENTERING\n");

#ifdef __PERF_INSTRUMENTATION__
        pComponentPrivate->lcml_nCntOpReceived = 0;
#endif

    LOG_STATUS_OMX_CMDS("Transitioning to OMX_StateLoaded Present state = 0x%x N\n",
						 pComponentPrivate->eState);

    if (pComponentPrivate->eState == OMX_StateLoaded)
    {
        eError = OMX_ErrorSameState;

		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError because OMX_ErrorSameState\n");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
			pComponentPrivate->pHandle->pApplicationPrivate,
			OMX_EventError,
			OMX_ErrorSameState,
			OMX_TI_ErrorMinor,
			"Same State");
    }
    else if (pComponentPrivate->eState == OMX_StateIdle)
    {
#ifdef __PERF_INSTRUMENTATION__
        PERF_Boundary(pComponentPrivate->pPERFcomp,
                      PERF_BoundaryStart | PERF_BoundaryCleanup);
#endif
		/* Return all I/O buffers if any are left in the pipes */
		/* Ideally nothing should be present at this stage     */
        OMX_VidDec_HandlePipes(pComponentPrivate);

		/* Delete the ARM  side component by deleting the codec instance and */
		/* freeing all the memory allocated  to it                           */
		LOG_STATUS_OMX_CMDS("Calling delete_split_arm_side_component");
		delete_split_arm_side_component(pComponentPrivate);

        pComponentPrivate->eIdleToLoad = OMX_StateLoaded;

#ifdef RESOURCE_MANAGER_ENABLED
		LOG_STATUS_OMX_CMDS("Calling VIDDEC_RM_free_resources");
		eError = VIDDEC_RM_free_resources(pComponentPrivate);

		if (eError != OMX_ErrorNone)
			goto EXIT;
#endif

		LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Destroy");
		VIDDEC_LCML_Destroy(pComponentPrivate);

		LOG_STATUS_OMX_CMDS("Waiting for unpopulate ports IN 0x%x OUT 0x%x\n",
							 pPortDefIn->bEnabled,pPortDefOut->bEnabled);

		if ((pPortDefIn->bEnabled == OMX_TRUE &&
			 pPortDefOut->bEnabled == OMX_TRUE) ||
			(pPortDefIn->bEnabled == OMX_TRUE &&
			 pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL))
        {
            if(pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
            {
				/* Won't enter this unless the component is tunneled */
                if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
					  pComponentPrivate->pInPortDef->bPopulated) ||
                    pComponentPrivate->sInSemaphore.bSignaled)
                {
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sInSemaphore);
                }
                LOG_STATUS_OMX_CMDS("Tunneling unpopulated ports IN 0x%x TUNNEL 0x%p\n",
						pPortDefIn->bEnabled,
						pComponentPrivate->pCompPort[1]->hTunnelComponent);

                eError = OMX_ErrorNone;
                pComponentPrivate->eState = OMX_StateLoaded;

                LOG_STATUS_OMX_CMDS("EventHandler OMX_CommandStateSet");
                pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandStateSet,
							pComponentPrivate->eState,
							NULL);

				LOG_STATUS_OMX_CMDS("Calling VIDDEC_Load_Defaults!!!");
                VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_VARS);
                pComponentPrivate->eIdleToLoad = OMX_StateInvalid;
            }
            else
            {
                if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
					  pComponentPrivate->pInPortDef->bPopulated) ||
                      pComponentPrivate->sInSemaphore.bSignaled)
                {
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sInSemaphore);
                }
                if((!(pComponentPrivate->eState == OMX_StateLoaded) &&
					  pComponentPrivate->pOutPortDef->bPopulated) ||
                    pComponentPrivate->sOutSemaphore.bSignaled)
                {
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sOutSemaphore);
                }

                LOG_STATUS_OMX_CMDS("Standalone unpopulated ports IN 0x%x OUT 0x%x\n",
									pPortDefIn->bEnabled,pPortDefOut->bEnabled);

                eError = OMX_ErrorNone;
                pComponentPrivate->eState = OMX_StateLoaded;

                LOG_STATUS_OMX_CMDS("EventHandler OMX_CommandStateSet");
                pComponentPrivate->cbInfo.EventHandler(
							pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandStateSet,
							pComponentPrivate->eState,
							NULL);

				LOG_STATUS_OMX_CMDS("Calling VIDDEC_Load_Defaults!!!");
                VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_VARS);
                pComponentPrivate->eIdleToLoad = OMX_StateInvalid;

            }
#ifdef __PERF_INSTRUMENTATION__
            PERF_Boundary(pComponentPrivate->pPERFcomp,
                          PERF_BoundaryComplete | PERF_BoundaryCleanup);
#endif

        }
        else
        {
            if(pComponentPrivate->pCompPort[1]->hTunnelComponent != NULL)
            {
                if(pComponentPrivate->sInSemaphore.bSignaled)
                {
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sInSemaphore);
                }
            }
            else
            {
                if(pComponentPrivate->sInSemaphore.bSignaled)
                {
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sInSemaphore);
                }
                if(pComponentPrivate->sOutSemaphore.bSignaled)
                {
                    VIDDEC_PTHREAD_SEMAPHORE_WAIT(pComponentPrivate->
																sOutSemaphore);
                }
            }
            pComponentPrivate->eState = OMX_StateLoaded;

            LOG_STATUS_OMX_CMDS("EventHandler OMX_CommandStateSet");
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventCmdComplete,
						   OMX_CommandStateSet,
						   pComponentPrivate->eState,
						   NULL);

            pComponentPrivate->eIdleToLoad = OMX_StateInvalid;
            LOG_STATUS_OMX_CMDS("Calling VIDDEC_Load_Defaults!!!");
            VIDDEC_Load_Defaults(pComponentPrivate, VIDDEC_INIT_VARS);
        }
    }
    else if (pComponentPrivate->eState == OMX_StateWaitForResources)
    {
        pComponentPrivate->eState    = OMX_StateLoaded;

        LOG_STATUS_OMX_CMDS("EventHandler OMX_CommandStateSet");
        pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventCmdComplete,
						OMX_CommandStateSet,
						pComponentPrivate->eState,
						NULL);
    }
    else
    {
        eError = OMX_ErrorIncorrectStateTransition;

        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorIncorrectStateTransition");
        pComponentPrivate->cbInfo.EventHandler(
						pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorIncorrectStateTransition,
						OMX_TI_ErrorMinor,
						NULL);
    }
EXIT:
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSetToPause                      */
/*  Description   : This Function Handles State Change request to pause.     */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToPause(OMX_HANDLETYPE phandle)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate =
									(VIDDEC_COMPONENT_PRIVATE *)phandle;
    OMX_ERRORTYPE             eError = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS( "+++ENTERING\n");

#ifdef __PERF_INSTRUMENTATION__
        pComponentPrivate->lcml_nCntOpReceived = 0;
#endif

    LOG_STATUS_OMX_CMDS("Transitioning to OMX_StatePause C 0x%x \n",
						 pComponentPrivate->eState);

    if (pComponentPrivate->eState == OMX_StatePause)
    {
        eError = OMX_ErrorSameState;

        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorSameState");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventError,
						   OMX_ErrorSameState,
						   OMX_TI_ErrorMinor,
						   NULL);
    }
    else if (pComponentPrivate->eState == OMX_StateExecuting)
    {
    #ifdef __PERF_INSTRUMENTATION__
        pComponentPrivate->lcml_nCntOpReceived = 0;
        PERF_Boundary(pComponentPrivate->pPERFcomp,
                      PERF_BoundaryComplete | PERF_BoundarySteadyState);
    #endif

        VIDDEC_PTHREAD_MUTEX_LOCK(pComponentPrivate->sMutex);

		/* Clear the pipes  */
        OMX_VidDec_HandlePipes(pComponentPrivate);

		/* Pause the codec */
		LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Pause");
        VIDDEC_LCML_Pause(pComponentPrivate);

        pComponentPrivate->eLCMLState = VidDec_LCML_State_Pause;
        eError = OMX_ErrorNone;
        pComponentPrivate->eState = OMX_StatePause;

        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventCmdComplete,
						   OMX_CommandStateSet,
						   pComponentPrivate->eState,
						   NULL);
    }
    else if (pComponentPrivate->eState == OMX_StateIdle)
    {

		LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Pause");
        VIDDEC_LCML_Pause(pComponentPrivate);

        eError = OMX_ErrorNone;
        pComponentPrivate->eState = OMX_StatePause;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventCmdComplete,
							   OMX_CommandStateSet,
							   pComponentPrivate->eState,
							   NULL);
    }
    else
    {
        eError = OMX_ErrorIncorrectStateTransition;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorIncorrectStateTransition");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						   pComponentPrivate->pHandle->pApplicationPrivate,
						   OMX_EventError,
						   OMX_ErrorIncorrectStateTransition,
						   OMX_TI_ErrorMinor,
						   "Incorrect State Transition");
    }

    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSetToInvalid                    */
/*  Description   : This Function Handles State Change request to Invalid.   */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToInvalid(OMX_HANDLETYPE phandle)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate =
									(VIDDEC_COMPONENT_PRIVATE*)phandle;
    OMX_ERRORTYPE		     eError = OMX_ErrorNone;
    OMX_U32                  iCount = 0;

    LOG_STATUS_OMX_CMDS( "+++ENTERING\n");
    LOG_STATUS_OMX_CMDS( "pComponentPrivate 0x%p phandle \n",
									pComponentPrivate);

#ifdef __PERF_INSTRUMENTATION__
        pComponentPrivate->lcml_nCntOpReceived = 0;
#endif

    LOG_STATUS_OMX_CMDS("Transitioning to OMX_StateInvalid C 0x%x N \n",
			pComponentPrivate->eState);

    if (pComponentPrivate->eState == OMX_StateInvalid)
    {
        eError = OMX_ErrorSameState;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorSameState");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorSameState,
						OMX_TI_ErrorCritical,
						"Same State");
    }
    else if (pComponentPrivate->eState == OMX_StateIdle ||
			 pComponentPrivate->eState == OMX_StateExecuting)
    {
        if (pComponentPrivate->eState == OMX_StateExecuting)
        {
			LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Stop");
            VIDDEC_LCML_Stop(pComponentPrivate);
        }
        LOG_STATUS_OMX_CMDS("Calling VIDDEC_LCML_Destroy");
        VIDDEC_LCML_Destroy(pComponentPrivate);

        for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
        {
			VIDDEC_BUFFER_PRIVATE *tmpBufPrivate = pComponentPrivate->
					pCompPort[VIDDEC_INPUT_PORT]->pBufferPrivate[iCount];

            if((tmpBufPrivate->bAllocByComponent == OMX_TRUE) &&
               (tmpBufPrivate->pBufferHdr != NULL))
            {
				OMX_BUFFERHEADERTYPE *pBuffHead = NULL;
				OMX_U8               *pTemp     = NULL;

				pBuffHead = tmpBufPrivate->pBufferHdr;

				if(pBuffHead->pBuffer != NULL)
				{
					pTemp              = (OMX_U8 *)(pBuffHead->pBuffer);
					pBuffHead->pBuffer = (OMX_U8 *)pTemp;
#if !defined(DSPALIGN_128BYTE)
					/* The aligned free macro needs the structure type.   */
					/* This is deprecated after inc 2.5 onwards           */
					/* delete this after migration to inc 2.5 is complete */
					OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer,
													OMX_BUFFERHEADERTYPE);
#else
					OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer, NULL);
#endif
					pBuffHead->pBuffer = NULL;
					pTemp = NULL;
				}
            }
        }

        for (iCount = 0; iCount < MAX_PRIVATE_BUFFERS; iCount++)
        {
			VIDDEC_BUFFER_PRIVATE *tmpBufPrivate = pComponentPrivate->
						pCompPort[VIDDEC_OUTPUT_PORT]->pBufferPrivate[iCount];

            if((tmpBufPrivate->bAllocByComponent == OMX_TRUE) &&
			   (tmpBufPrivate->pBufferHdr != NULL))
            {
				OMX_BUFFERHEADERTYPE *pBuffHead = NULL;
				OMX_U8     			 *pTemp = NULL;

				pBuffHead = tmpBufPrivate->pBufferHdr;

				if(pBuffHead->pBuffer != NULL)
				{
					pTemp = (OMX_U8 *)(pBuffHead->pBuffer);
					pBuffHead->pBuffer = (OMX_U8 *)pTemp;

#if !defined(DSPALIGN_128BYTE)
					/* The aligned free macro needs the structure type.   */
					/* This is deprecated after inc 2.5 onwards           */
					/* delete this after migration to inc 2.5 is complete */
					OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer,
												OMX_BUFFERHEADERTYPE);
#else
					OMX_MEMFREE_STRUCT_DSPALIGN(pBuffHead->pBuffer, NULL);
#endif
					pBuffHead->pBuffer = NULL;
					pTemp = NULL;
				}
            }
        }
#ifdef RESOURCE_MANAGER_ENABLED
    LOG_STATUS_OMX_CMDS("Calling VIDDEC_RM_free_resources");
	eError = VIDDEC_RM_free_resources(pComponentPrivate);

	if (eError != OMX_ErrorNone)
		goto EXIT;
#endif
        eError = OMX_ErrorInvalidState;
        pComponentPrivate->eState = OMX_StateInvalid;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorInvalidState");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							   pComponentPrivate->pHandle->pApplicationPrivate,
							   OMX_EventError,
							   OMX_ErrorInvalidState,
							   OMX_TI_ErrorCritical,
							   "Invalid State");
    }
    else
    {
        eError = OMX_ErrorIncorrectStateTransition;
        pComponentPrivate->eState = OMX_StateInvalid;
        LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorInvalidState");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
					pComponentPrivate->pHandle->pApplicationPrivate,
					OMX_EventError,
					OMX_ErrorInvalidState,
					OMX_TI_ErrorCritical,
					"Incorrect State Transition");
    }

EXIT:
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandStateSetToWaitForResources           */
/*  Description   : This Function Handles State Change request to            */
/*					Wait For Resources										 */
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
OMX_ERRORTYPE VIDDEC_HandleCommandStateSetToWaitForResources(
														OMX_HANDLETYPE phandle)
{
    VIDDEC_COMPONENT_PRIVATE *pComponentPrivate =
									(VIDDEC_COMPONENT_PRIVATE*)phandle;
    OMX_ERRORTYPE 			 eError = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS( "+++ENTERING\n");

#ifdef __PERF_INSTRUMENTATION__
        pComponentPrivate->lcml_nCntOpReceived = 0;
#endif

    LOG_STATUS_OMX_CMDS("Transitioning to OMX_StateWaitForResources C 0x%x N \n",
						 pComponentPrivate->eState);

    if (pComponentPrivate->eState == OMX_StateWaitForResources)
    {
        eError = OMX_ErrorSameState;

		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorSameState");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
						pComponentPrivate->pHandle->pApplicationPrivate,
						OMX_EventError,
						OMX_ErrorSameState,
						OMX_TI_ErrorMinor,
						NULL);
    }
    else if (pComponentPrivate->eState == OMX_StateLoaded)
    {
        eError                       = OMX_ErrorNone;
        pComponentPrivate->eState    = OMX_StateWaitForResources;

        /* Nothing to do for wait for resources*/
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventCmdComplete");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventCmdComplete,
							OMX_CommandStateSet,
							pComponentPrivate->eState,
							NULL);
    }
    else
    {
        eError = OMX_ErrorIncorrectStateTransition;
		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorIncorrectStateTransition");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
				pComponentPrivate->pHandle->pApplicationPrivate,
				OMX_EventError,
				OMX_ErrorIncorrectStateTransition,
				OMX_TI_ErrorMinor,
				NULL);
    }

    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}


/*****************************************************************************/
/*  Function Name : VIDDEC_Handle_InvalidState                               */
/*  Description   : This Function Handles State Change request to Invalid.   */
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
OMX_ERRORTYPE VIDDEC_Handle_InvalidState (
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS( "+++ENTERING\n");
    LOG_STATUS_OMX_CMDS( "pComponentPrivate 0x%p\n",
												(int*)pComponentPrivate);

    if(pComponentPrivate->eState != OMX_StateInvalid)
    {
        pComponentPrivate->eState = OMX_StateInvalid;

		LOG_STATUS_OMX_CMDS("EventHandler OMX_EventError OMX_ErrorInvalidState");
        pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
							pComponentPrivate->pHandle->pApplicationPrivate,
							OMX_EventError,
							OMX_ErrorInvalidState,
							OMX_TI_ErrorCritical,
							"Transitioning to Invalid State");
        eError = OMX_ErrorNone;
    }
    else
    {
        eError = OMX_ErrorHardware;
    }
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);
    return eError;
}

/*****************************************************************************/
/*  Function Name : VIDDEC_HandleCommandMarkBuffer                           */
/*  Description   : This Function Mark Buffer for Tunnelled action.          */
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
OMX_ERRORTYPE VIDDEC_HandleCommandMarkBuffer(
								VIDDEC_COMPONENT_PRIVATE *pComponentPrivate,
								OMX_U32					 nParam1,
								OMX_PTR 				 pCmdData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    LOG_STATUS_OMX_CMDS( "+++ENTERING\n");
    LOG_STATUS_OMX_CMDS("pComponentPrivate 0x%p nParam1 0x%lx pCmdData 0x%p\n",
						 pComponentPrivate, nParam1, pCmdData);
    LOG_STATUS_OMX_CMDS( "---EXITING(0x%x)\n",eError);

    return eError;

}

