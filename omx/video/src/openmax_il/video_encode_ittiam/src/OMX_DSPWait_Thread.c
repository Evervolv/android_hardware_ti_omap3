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

int totalNoOfDSPWaits;

/*----------------------------------------------------------------------------*/
/**
  * handleCommand()
  *
  * Called by OMX_VIDENC_Thread function.
  *
  * @param pComponentPrivate
  *        threadExit
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE processFrame(VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                           FRAME_ENCODE_BUFFER* pFrmEncSpecifics)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    instance_ctxt_t *ps_hdl;
    IVIDENC1_Handle ps_codec_hdl;
    struct timespec timeout;
    Int nStatus;
    int status = -1;
    int fdmax = -1;
    fd_set rfds;
    sigset_t set;
    int eCmd;
    int nRet = -1;

    ps_hdl          = (instance_ctxt_t *)pComponentPrivate->pCodecHandle;
    ps_codec_hdl    = ps_hdl->ps_codec_hdl;

    //ALOGD("ARM Process Index %d", pFrmEncSpecifics->index);
    /* Complete the ARM process for the frame */
    nStatus = pComponentPrivate->pf_arm_process(ps_codec_hdl, pFrmEncSpecifics->index);
    if(IVIDENC1_EOK != nStatus)
    {
        eError = OMX_ErrorUndefined;
        OMX_ERROR4(pComponentPrivate->dbg,"failed spltenc_atci_arm_process");
        goto OMX_CONF_CMD_BAIL;
    }

    totalNoOfDSPWaits++;
    /* Wait on the DSP to complete the the frame encoding */
    nStatus = pComponentPrivate->pf_dsp_wait(ps_codec_hdl,
                                             pFrmEncSpecifics->index);

    if(IVIDENC1_EOK != nStatus)
    {
        ALOGD("DSP Wait Index %d", pFrmEncSpecifics->index);
        ALOGD("Total no of DSP Waits %d", totalNoOfDSPWaits);
        //eError = OMX_ErrorUndefined;
        OMX_ERROR4(pComponentPrivate->dbg,"failed spltenc_atci_dsp_wait");

        OMX_VIDENC_FatalErrorRecover(pComponentPrivate);
        eError = OMX_ErrorUndefined;
        goto OMX_CONF_CMD_BAIL;
    }

    //ALOGD("A DSP Wait");
    /* Fill the encoded frame details - filledLen of the buffer and type of   */
    /* encoded frame                                                          */
    pFrmEncSpecifics->pOutputBuffer->nFilledLen = pComponentPrivate->sOutArgs[pFrmEncSpecifics->index].bytesGenerated;
    if(IVIDEO_I_FRAME == pComponentPrivate->sOutArgs[pFrmEncSpecifics->index].encodedFrameType)
    {
        pFrmEncSpecifics->pOutputBuffer->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
    }

    /* Decrement the buffers triggered counter inorder to keep track of the   */
    /* number of buffers that has already been triggered to the DSP. This     */
    /* variable is mutex protected                                            */

    if (pthread_mutex_lock(&(pComponentPrivate->mbuffersTriggeredMutex)) != 0)
    {
        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorHardware,
                               pComponentPrivate->dbg, OMX_TRACE4,
                               "pthread_mutex_lock() failed.\n");
    }

    pComponentPrivate->buffersTriggered--;

    if (pthread_mutex_unlock(&(pComponentPrivate->mbuffersTriggeredMutex)) != 0)
    {
        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorHardware,
                               pComponentPrivate->dbg, OMX_TRACE4,
                               "pthread_mutex_unlock() failed.\n");
    }

    /* Check if the Video encoder thread requires an ack, if so send the ACK */
    /* before we start the arm processing. The timeout value will be zero in */
    /* this case since we are not waiting for this event rather checking if  */
    /* there is an existing event                                            */

    fdmax = pComponentPrivate->nVidenc_DSP_wait_comm_thread[0];
    FD_ZERO (&rfds);
    FD_SET (pComponentPrivate->nVidenc_DSP_wait_comm_thread[0], &rfds);

    /* Set the timeout values to zero - no need to wait */
    timeout.tv_sec  = 0;         /* seconds     */
    timeout.tv_nsec = 0;         /* nanoseconds */

    sigemptyset(&set);
    sigaddset(&set,SIGALRM);
    status = pselect(fdmax+1, &rfds, NULL, NULL, &timeout, &set);

    if (0 == status)
    {
        /* Time out occured and no file descriptor was set. No need to send */
        /* ACK to calling thread. Continue with ARM processing.             */
        //ALOGD("Nothing present in the dsp wait thread event pipe");

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
        nRet = read(pComponentPrivate->nVidenc_DSP_wait_comm_thread[0], &eCmd, sizeof(pFrmEncSpecifics));
        if (nRet == -1)
        {
            OMX_TRACE4(pComponentPrivate->dbg, "Error while reading from the input pipe\n");
            OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
        }

        if(SEND_DSP_ACK == eCmd)
        {
            //ALOGD("Sending the ACK back to the video encoder thread");
            /* Send the ack back to the Video Encoder thread */
            eCmd = DSP_ACK;

            nRet = write(pComponentPrivate->nDSP_wait_videnc_comm_thread[1], &eCmd, sizeof(eCmd));
            if (nRet == -1)
            {
                OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                       pComponentPrivate->dbg, OMX_PRCOMM4,
                                       "Failed to write to nCmdPipe.\n");
            }
        }
        else
        {
            ALOGD("Unknown command received in the DSP Wait thread (processFrame)!!");
        }
    }

    /* Send the buffers to the callback thread. The callback thread will in  */
    /* turn call the necessary callbacks on the input and output buffers.    */
    nRet = write(pComponentPrivate->nCallback_thread_data_pipe[1], &(pFrmEncSpecifics), sizeof(pFrmEncSpecifics));
    if (nRet == -1)
    {
        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                               pComponentPrivate->dbg, OMX_PRCOMM4,
                               "Failed to write to nCallback_thread_data_pipe.\n");
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMXIL_DSPWait_Thread()
  *
  * Called by VIDENC_Start_ComponentThread function.
  *
  * @param pThreadData
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
void* OMXIL_DSPWait_Thread (void* pThreadData)
{
    int status = -1;
    int fdmax = -1;
    int fdmaxCbDSPWait = -1;
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

    /* Wait for data buffers from the main video encoder component thread or  */
    /* events from the video encoder thread. If it is a data buffer then      */
    /* complete the frame encoding by calling the arm process and the DSP wait*/
    /* calls for the frame. The events that are expected here are an EXIT     */
    /* command or a FLUSH command. In case of the EXIT command forward the    */
    /* command to the Callback thread and exit the thread. In case of FLUSH   */
    /* command complete all the data processing present in the data pipes,    */
    /* then pass the FLUSH command to the callback thread. Wait untill the    */
    /* callback thread completes the FLUSH command and then report back to    */
    /* Video encoder component thread that the FLUSH command has been handled */

    /** Looking for highest number of file descriptor
        for pipes inorder to put in select loop */

    fdmax = pComponentPrivate->nDsp_wait_thread_event_pipe[0];

    if (pComponentPrivate->nDsp_wait_thread_data_pipe[0] > fdmax)
    {
        fdmax = pComponentPrivate->nDsp_wait_thread_data_pipe[0];
    }

    while (1)
    {
        /* Wait for commands or input buffers to come into the OMX component   */
        FD_ZERO (&rfds);
        FD_SET (pComponentPrivate->nDsp_wait_thread_event_pipe[0], &rfds);
        FD_SET (pComponentPrivate->nDsp_wait_thread_data_pipe[0], &rfds);

        sigemptyset(&set);
        sigaddset(&set,SIGALRM);

        if(OMX_TRUE == pComponentPrivate->bDSPWaitThreadFlushing)
        {
            /* Set the timeout values to zero - no need to wait */
            timeout.tv_sec  = 0;         /* seconds     */
            timeout.tv_nsec = 0;         /* nanoseconds */

            status = pselect(fdmax+1, &rfds, NULL, NULL, &timeout, &set);
        }
        else
        {
            //ALOGD("B Waiting in DSP Wait thread for data and events");
            status = pselect(fdmax+1, &rfds, NULL, NULL, NULL, &set);
            //ALOGD("A Waiting in DSP Wait thread for data and events");
        }

        if (0 == status)
        {
            if(OMX_TRUE == pComponentPrivate->bDSPWaitThreadFlushing)
            {
                eCmd = FLUSH_PIPES;
                /* Send the flush command to the callback thread */
                nRet = write(pComponentPrivate->nCallback_thread_event_pipe[1], &eCmd, sizeof(eCmd));
                if (nRet == -1)
                {
                    OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                           pComponentPrivate->dbg, OMX_PRCOMM4,
                                           "Failed to write to nCmdPipe.\n");
                }

                {
                    /* Wait on the callback thread to complete the flush command */
                    fdmaxCbDSPWait = pComponentPrivate->nCallback_DSPWait_comm_thread[0];

                    FD_ZERO (&rfds);
                    FD_SET (pComponentPrivate->nCallback_DSPWait_comm_thread[0], &rfds);

                    sigemptyset(&set);
                    sigaddset(&set,SIGALRM);
                    status = pselect(fdmaxCbDSPWait+1, &rfds, NULL, NULL, NULL, &set);

                    if (0 == status)
                    {
                        ALOGD("The timeout value is specified as NULL should never come here");
                        sched_yield();
                    }
                    else if (-1 == status)
                    {
                        ALOGE("pselect returned an error");
                        OMX_VIDENC_SET_ERROR_BAIL(eError, OMX_ErrorHardware, pComponentPrivate);
                    }
                    else
                    {
                        if (FD_ISSET(pComponentPrivate->nCallback_DSPWait_comm_thread[0], &rfds))
                        {
                            nRet = read(pComponentPrivate->nCallback_DSPWait_comm_thread[0], &eCmd, sizeof(eCmd));
                            if (nRet == -1)
                            {
                                OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from nDSP_wait_videnc_comm_thread\n");
                                OMX_VIDENC_SET_ERROR_BAIL(eError,
                                                          OMX_ErrorHardware,
                                                          pComponentPrivate);
                            }

                            if(FLUSH_PIPES_COMPLETED != eCmd)
                            {
                                ALOGE("Unknown command received from the nCallback_DSPWait_comm_thread");
                            }
                        }
                    }
                }

                /* Signal the Video Encoder component thread that this       */
                /* thread completed the flush                                */
                eCmd = FLUSH_PIPES_COMPLETED;

                nRet = write(pComponentPrivate->nDSP_wait_videnc_comm_thread[1], &eCmd, sizeof(eCmd));
                if (nRet == -1)
                {
                    OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                           pComponentPrivate->dbg, OMX_PRCOMM4,
                                           "Failed to write to nCmdPipe.\n");
                }

                pComponentPrivate->bDSPWaitThreadFlushing = OMX_FALSE;
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
            if (FD_ISSET(pComponentPrivate->nDsp_wait_thread_data_pipe[0], &rfds))
            {
                /* Wait for the DSP to complete encoding of the video frame  */
                /* then issue an ARM process for the same video frame        */
                nRet = read(pComponentPrivate->nDsp_wait_thread_data_pipe[0], &(pFrmEncSpecifics), sizeof(pFrmEncSpecifics));
                if (nRet == -1)
                {
                    OMX_TRACE4(pComponentPrivate->dbg, "Error while reading from the input pipe\n");
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
                }

                eError = processFrame(pComponentPrivate, pFrmEncSpecifics);
                if(OMX_ErrorNone != eError)
                {
                    eCmd = -1;

                    OMX_ERROR4(pComponentPrivate->dbg,"failed processFrame Triggering close down of Callback thread %s:%d", __FUNCTION__, __LINE__);
                    nRet = write(pComponentPrivate->nCallback_thread_event_pipe[1], &eCmd, sizeof(eCmd));
                    goto OMX_CONF_CMD_BAIL;
                }
            }

            if(FD_ISSET(pComponentPrivate->nDsp_wait_thread_event_pipe[0], &rfds))
            {
                /* The events that are expected here are 1) EXIT 2) FLUSH */
                nRet = read(pComponentPrivate->nDsp_wait_thread_event_pipe[0], &eCmd, sizeof(eCmd));
                if (nRet == -1)
                {
                    OMX_TRACE4(pComponentPrivate->dbg, "Error while reading from the input pipe\n");
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
                }

                if(-1 == (OMX_S32)eCmd)
                {
                    /* Exit command send the command to the callback thread  */
                    /* and bail out of this thread                           */
                    nRet = write(pComponentPrivate->nCallback_thread_event_pipe[1], &eCmd, sizeof(eCmd));
                    if (nRet == -1)
                    {
                        OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                               pComponentPrivate->dbg, OMX_PRCOMM4,
                                               "Failed to write to nCmdPipe.\n");
                    }

                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorNone);
                }
                else if(FLUSH_PIPES == eCmd)
                {
                    /* In case of the FLUSH command set a timeout value of    */
                    /* and empty all the data and when the timeout occurs     */
                    /* forward the FLUSH command to the callback thread       */
                    pComponentPrivate->bDSPWaitThreadFlushing = OMX_TRUE;
                }
                else
                {
                    ALOGE("Unknown command received in the main DSP Wait thread event loop ");
                }
            }
        }
    }/* end - while(1) */

OMX_CONF_CMD_BAIL:

    OMX_PRINT4(pComponentPrivate->dbg, "OMXIL_DSPWait_Thread Exits\n");
    return (void*)eError;

}
