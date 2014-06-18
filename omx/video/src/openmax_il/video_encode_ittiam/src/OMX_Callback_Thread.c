/*****************************************************************************/

/*                                                                           */
/*                             Standard Files                                */
/*                  ITTIAM SYSTEMS PVT LTD, BANGALORE                        */
/*                          COPYRIGHT(C) 2006                                */
/*                                                                           */
/*  This program  is  proprietary to  Ittiam  Systems  Private  Limited  and */
/*  is protected under Indian  Copyright Law as an unpublished work. Its use */
/*  and  disclosure  is  limited by  the terms  and  conditions of a license */
/*  agreement. It may not be copied or otherwise  reproduced or disclosed to */
/*  persons outside the licensee's organization except in accordance with the*/
/*  the  terms  and  conditions   of  such  an  agreement.  All  copies  and */
/*  reproductions shall be the property of Ittiam Systems Private Limited and*/
/*  must bear this notice in its entirety.                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name         : ittiam_datatypes.h                                   */
/*                                                                           */
/*  Description       : This file has the definitions of the data types used */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         06 04 2006   Malavika        Draft                                */
/*                                                                           */
/*****************************************************************************/


/* ------compilation control switches ----------------------------------------*/
/******************************************************************************
*  INCLUDE FILES
*******************************************************************************/
/* ----- system and platform files -------------------------------------------*/
#define _XOPEN_SOURCE 600
#include <wchar.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>

#include <dbapi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


#include "OMX_ividenc1_utils.h"
#include "OMX_ividenc1_api_structs.h"
/*------- Program Header Files -----------------------------------------------*/
#include "OMX_VideoEnc_Utils.h"
#include "OMX_VideoEncoder.h"
#include "iv_enc_export.h"

/*----------------------------------------------------------------------------*/
/**
  * returnInOutBuffers()
  *
  * Called by OMXIL_Callback_Thread function.
  *
  * @param pComponentPrivate
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/

OMX_ERRORTYPE returnInOutBuffers(VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                 FRAME_ENCODE_BUFFER* pFrmEncSpecifics)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    //ALOGD("Calling ETBD");
    /* Return the input buffer - EmptyThisBufferDone */
    eError = OMX_VIDENC_Process_FreeInBuf(pComponentPrivate, pFrmEncSpecifics->pInputBuffer);
    if (eError != OMX_ErrorNone)
    {
        OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_VIDENC_Process_FreeInBuf");
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorUndefined, 0, NULL);
        OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
    }

    //ALOGD("Calling FTBD");
    /* Return the output buffer - FillThisBufferDone */
    eError = OMX_VIDENC_Process_FilledOutBuf(pComponentPrivate, pFrmEncSpecifics->pOutputBuffer);
    if (eError != OMX_ErrorNone)
    {
            OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_VIDENC_Process_FilledOutBuf");
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
    }

    /* The structure variable was malloced when it was first written into the */
    /* pipe in the video encoder component thread. This variable will not be  */
    /* used further so free the memory that has been allocated to it          */
    VIDENC_FREE(pFrmEncSpecifics, pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMXIL_Callback_Thread()
  *
  * Called by VIDENC_Start_ComponentThread function.
  *
  * @param pThreadData
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
void* OMXIL_Callback_Thread (void* pThreadData)
{
    int status = -1;
    int fdmax = -1;
    fd_set rfds;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMMANDTYPE eCmd = -1;
    int nRet = -1;
    VIDENC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    sigset_t set;
    FRAME_ENCODE_BUFFER* pFrmEncSpecifics;
    struct timespec timeout;

    if (!pThreadData)
    {
        eError = OMX_ErrorBadParameter;
        goto OMX_CONF_CMD_BAIL;
    }

    pComponentPrivate = (VIDENC_COMPONENT_PRIVATE*)pThreadData;

    /* Wait infinitley for events or data from the DSP Wait thread. If data   */
    /* comes in then make the callbacks EBD and FBD on the input and output   */
    /* buffers. There are only 2 events that are expected here 1) Exit - do   */
    /* nothing, just bail out of the thread. 2) FLUSH - Empty the data pipes  */
    /* and inform the DSP Wait thread that the FLUSH has been handled.        */

    /** Looking for highest number of file descriptor
    for pipes inorder to put in select loop */

    fdmax = pComponentPrivate->nCallback_thread_event_pipe[0];

    if (pComponentPrivate->nCallback_thread_data_pipe[0] > fdmax)
    {
        fdmax = pComponentPrivate->nCallback_thread_data_pipe[0];
    }

    while (1)
    {
        /* Wait for commands or input buffers to come into the OMX component   */
        FD_ZERO (&rfds);
        FD_SET (pComponentPrivate->nCallback_thread_event_pipe[0], &rfds);
        FD_SET (pComponentPrivate->nCallback_thread_data_pipe[0], &rfds);

        sigemptyset(&set);
        sigaddset(&set,SIGALRM);

        if(OMX_TRUE == pComponentPrivate->bCallbackThreadFlushing)
        {
            /* Set the timeout values to zero - no need to wait */
            timeout.tv_sec  = 0;         /* seconds     */
            timeout.tv_nsec = 0;         /* nanoseconds */

            status = pselect(fdmax+1, &rfds, NULL, NULL, &timeout, &set);
        }
        else
        {
            //ALOGD("B Waiting in Callback thread for data and events");
            status = pselect(fdmax+1, &rfds, NULL, NULL, NULL, &set);
            //ALOGD("A Waiting in Callback thread for data and events");
        }

        if (0 == status)
        {
            if(OMX_TRUE == pComponentPrivate->bCallbackThreadFlushing)
            {
                /* Signal the DSP Wait thread that this thread completed     */
                /* the flush                                                 */
                {
                    eCmd = FLUSH_PIPES_COMPLETED;

                    nRet = write(pComponentPrivate->nCallback_DSPWait_comm_thread[1], &eCmd, sizeof(eCmd));
                    if (nRet == -1)
                    {
                        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                               pComponentPrivate->dbg, OMX_PRCOMM4,
                                               "Failed to write to nCmdPipe.\n");
                    }
                }

                pComponentPrivate->bCallbackThreadFlushing = OMX_FALSE;
            }
            else
            {
                ALOGD("The timeout value is specified as NULL should never come here");
                sched_yield();
            }
        }
        else if (-1 == status)
        {
            if (pComponentPrivate->eState != OMX_StateLoaded)
            {
                OMX_TRACE3(pComponentPrivate->dbg, "select() error.\n");
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorHardware, 0, NULL);
            }
            OMX_VIDENC_SET_ERROR_BAIL(eError, OMX_ErrorHardware, pComponentPrivate);
        }
        else
        {
            /* Only the exit command is expected in this thread, anything    */
            /* else is flagged as an error                                   */
            if (FD_ISSET(pComponentPrivate->nCallback_thread_event_pipe[0], &rfds))
            {
                nRet = read(pComponentPrivate->nCallback_thread_event_pipe[0], &eCmd, sizeof(eCmd));
                if (nRet == -1)
                {
                    OMX_TRACE4(pComponentPrivate->dbg, "Error while reading from the input pipe\n");
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
                }

                if(-1 == (OMX_S32)eCmd)
                {
                    /* Exit command received bail out of this thread         */
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorNone);
                }
                else if(FLUSH_PIPES == eCmd)
                {
                    /* In case of FLUSH set the timeout value to zero, empty  */
                    /* the data pipes, when the timeout occurs signal the DSP */
                    /* Wait thread that the FLUSH has been completed.         */
                    pComponentPrivate->bCallbackThreadFlushing = OMX_TRUE;
                }
                else
                {
                    ALOGE("Unknown command received in the callback thread");
                }
            }

            /* Return both the input and the output buffers back to           */
            /* the application by making the EmptyThisBufferDone and          */
            /* FillThisBufferDone callbacks.                                  */
            if (FD_ISSET(pComponentPrivate->nCallback_thread_data_pipe[0], &rfds))
            {
                /* The only event that can come here is EXIT and FLUSH command*/
                nRet = read(pComponentPrivate->nCallback_thread_data_pipe[0], &pFrmEncSpecifics, sizeof(pFrmEncSpecifics));
                if (nRet == -1)
                {
                    OMX_TRACE4(pComponentPrivate->dbg, "Error while reading from the input pipe\n");
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
                }

                eError = returnInOutBuffers(pComponentPrivate, pFrmEncSpecifics);
                if(OMX_ErrorNone != eError)
                {
                    OMX_ERROR4(pComponentPrivate->dbg,"failed returnInOutBuffers");
                    goto OMX_CONF_CMD_BAIL;
                }
            }
        }
    }/* end while(1) */

OMX_CONF_CMD_BAIL:

    OMX_PRINT4(pComponentPrivate->dbg, "OMXIL_Callback_Thread Exits\n");
    return (void*)eError;
}
