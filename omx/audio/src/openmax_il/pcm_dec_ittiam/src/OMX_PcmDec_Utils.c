/*****************************************************************************/
/*                                                                           */
/*                        Enhanced pcmPlus Decoder                           */
/*                                                                           */
/*                   ITTIAM SYSTEMS PVT LTD, BANGALORE                       */
/*                          COPYRIGHT(C) 2010                                */
/*                                                                           */
/*  This program is proprietary to Ittiam Systems Pvt. Ltd. and is protected */
/*  under Indian Copyright Act as an unpublished work.Its use and disclosure */
/*  is  limited by  the terms and conditions of a license  agreement. It may */
/*  be copied or  otherwise reproduced or  disclosed  to persons outside the */
/*  licensee 's  organization  except  in  accordance  with  the  terms  and */
/*  conditions of  such an agreement. All  copies and reproductions shall be */
/*  the  property  of Ittiam Systems Pvt.  Ltd. and  must  bear  this notice */
/*  in its entirety.                                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  File Name        : OMX_PcmDec_Utils.c                                    */
/*                                                                           */
/*  Description      :                                                       */
/*                                                                           */
/*  List of Functions: None                                                  */
/*                                                                           */
/*  Issues / Problems: None                                                  */
/*                                                                           */
/*  Revision History :                                                       */
/*                                                                           */
/*****************************************************************************/


/* ------compilation control switches -------------------------*/
/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/

#include <wchar.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <malloc.h>
#include <memory.h>
#include <fcntl.h>

#include <dbapi.h>
#include <string.h>
#include <stdio.h>

/*------- Program Header Files -----------------------------------------------*/
#include "OMX_IttiamPcmDec_Utils.h"
#include <decode_common_ti.h>

#ifdef RESOURCE_MANAGER_ENABLED
#include <ResourceManagerProxyAPI.h>
#endif

#ifdef UNDER_CE
#define HASHINGENABLE 1
#endif

//#define MULTICHANNEL_ENABLE

/* ================================================================================= * */
/**
* @fn PcmDec_StartCompThread() starts the component thread. This is internal
* function of the component.
*
* @param pHandle This is component handle allocated by the OMX core.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful Inirialization of the component\n
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */
OMX_ERRORTYPE PcmDec_StartCompThread(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    PCMDEC_COMPONENT_PRIVATE *pComponentPrivate =
        (PCMDEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    int nRet = 0;
#ifdef UNDER_CE
    pthread_attr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.__inheritsched = PTHREAD_EXPLICIT_SCHED;
    attr.__schedparam.__sched_priority = OMX_AUDIO_DECODER_THREAD_PRIORITY;
#endif



    /* Ceating two separate pipes for Input and Output Buffers*/
    /* create the pipe used to send Input buffers to the thread */
    eError = pipe (pComponentPrivate->dataPipeInputBuf);
    if (eError)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  PcmDec_StartCompThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* create the pipe used to send Output buffers to the thread */
    eError = pipe (pComponentPrivate->dataPipeOutputBuf);
    if (eError)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  PcmDec_StartCompThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    nRet = pipe (pComponentPrivate->cmdPipe);
    if (0 != nRet)
    {
        PCMDEC_OMX_ERROR_EXIT(eError, OMX_ErrorInsufficientResources,"Pipe Creation Failed");
    }

    nRet = pipe (pComponentPrivate->cmdDataPipe);
    if (0 != nRet)
    {
        PCMDEC_OMX_ERROR_EXIT(eError, OMX_ErrorInsufficientResources,"Pipe Creation Failed");
    }

#ifdef UNDER_CE
    nRet = pthread_create (&(pComponentPrivate->ComponentThread), &attr, PCMDEC_ComponentThread, pComponentPrivate);
#else
    nRet = pthread_create (&(pComponentPrivate->ComponentThread), NULL, PCMDEC_ComponentThread, pComponentPrivate);
#endif
    if ((0 != nRet) || (!pComponentPrivate->ComponentThread)) {
        PCMDEC_OMX_ERROR_EXIT(eError, OMX_ErrorInsufficientResources,"Thread Creation Failed");
    }

    pComponentPrivate->bCompThreadStarted = 1;

 EXIT:

    return eError;
}


/* ================================================================================= * */
/**
* @fn PCMDEC_FreeCompResources() function frees the component resources.
*
* @param pComponent This is the component handle.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful Inirialization of the component\n
*               OMX_ErrorHardware = Hardware error has occured.
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_FreeCompResources(OMX_HANDLETYPE pComponent)
{
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    PCMDEC_COMPONENT_PRIVATE *pComponentPrivate = (PCMDEC_COMPONENT_PRIVATE *)
        pHandle->pComponentPrivate;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 nIpBuf=0, nOpBuf=0;
    int nRet=0;

    OMX_PRINT1(pComponentPrivate->dbg, "%d:::pComponentPrivate->bPortDefsAllocated = %ld\n", __LINE__,pComponentPrivate->bPortDefsAllocated);
    if (pComponentPrivate->bPortDefsAllocated)
    {
        nIpBuf = pComponentPrivate->pInputBufferList->numBuffers;
        nOpBuf = pComponentPrivate->pOutputBufferList->numBuffers;
    }
    OMX_PRDSP2(pComponentPrivate->dbg, "%d :: Closing pipes.....\n",__LINE__);

    nRet = close (pComponentPrivate->dataPipeInputBuf[0]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->dataPipeInputBuf[1]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->dataPipeOutputBuf[0]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->dataPipeOutputBuf[1]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->cmdPipe[0]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->cmdPipe[1]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->cmdDataPipe[0]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    nRet = close (pComponentPrivate->cmdDataPipe[1]);
    if (0 != nRet && OMX_ErrorNone == eError)
    {
        eError = OMX_ErrorHardware;
    }

    if (pComponentPrivate->bPortDefsAllocated)
    {
        PCMDEC_OMX_FREE(pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]);
        PCMDEC_OMX_FREE(pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]);
        PCMDEC_OMX_FREE(pComponentPrivate->aacParams);
        PCMDEC_OMX_FREE(pComponentPrivate->pcmParams);
        PCMDEC_OMX_FREE(pComponentPrivate->pCompPort[INPUT_PORT_PCMDEC]->pPortFormat);
        PCMDEC_OMX_FREE(pComponentPrivate->pCompPort[OUTPUT_PORT_PCMDEC]->pPortFormat);
        PCMDEC_OMX_FREE(pComponentPrivate->pCompPort[INPUT_PORT_PCMDEC]);
        PCMDEC_OMX_FREE(pComponentPrivate->pCompPort[OUTPUT_PORT_PCMDEC]);
        PCMDEC_OMX_FREE(pComponentPrivate->sPortParam);
        PCMDEC_OMX_FREE(pComponentPrivate->pPriorityMgmt);
        PCMDEC_OMX_FREE(pComponentPrivate->pInputBufferList);
        PCMDEC_OMX_FREE(pComponentPrivate->pOutputBufferList);
        PCMDEC_OMX_FREE(pComponentPrivate->componentRole);
    }


    pComponentPrivate->bPortDefsAllocated = 0;

#ifndef UNDER_CE
    OMX_PRDSP2(pComponentPrivate->dbg, "\n\n FreeCompResources: Destroying threads.\n\n");
    pthread_mutex_destroy(&pComponentPrivate->InLoaded_mutex);
    pthread_cond_destroy(&pComponentPrivate->InLoaded_threshold);

    pthread_mutex_destroy(&pComponentPrivate->InIdle_mutex);
    pthread_cond_destroy(&pComponentPrivate->InIdle_threshold);

    pthread_mutex_destroy(&pComponentPrivate->AlloBuf_mutex);
    pthread_cond_destroy(&pComponentPrivate->AlloBuf_threshold);

    pthread_mutex_destroy(&pComponentPrivate->codecStop_mutex);
    pthread_cond_destroy(&pComponentPrivate->codecStop_threshold);

    pthread_mutex_destroy(&pComponentPrivate->codecFlush_mutex);
    pthread_cond_destroy(&pComponentPrivate->codecFlush_threshold);
#else
    OMX_DestroyEvent(&(pComponentPrivate->InLoaded_event));
    OMX_DestroyEvent(&(pComponentPrivate->InIdle_event));
    OMX_DestroyEvent(&(pComponentPrivate->AlloBuf_event));
#endif

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_HandleDataBuf_FromApp() function handles the input and output buffers
* that come from the application. It is not direct function wich gets called by
* the application rather, it gets called eventually.
*
* @param *pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*               OMX_ErrorHardware = Hardware error has occured lile LCML failed
*               to do any said operartion.
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_HandleDataBuf_FromApp(
                                           PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_S32 ret = 0;

    OMX_BUFFERHEADERTYPE* pInputBufHeader;
    OMX_BUFFERHEADERTYPE* pOutputBufHeader;

    OMX_S32 status = 0;
    OMX_S32 extended_err_code = 0;
    OMX_S32 bytes_consumed = 0;
    OMX_S32 out_bytes;
    OMX_S32 resize_needed = 0;


    OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL: Entering PCMDEC_HandleDataBuf_FromApp - curState = %d\n",
        __LINE__,pComponentPrivate->curState);


    pInputBufHeader = pComponentPrivate->pInputBufHeader;
    pOutputBufHeader = NULL;

    if (pComponentPrivate->curState == OMX_StateExecuting)
    {
        OMX_S32 bytesToCopy=0;

        OMX_S32 max_frame_size = pComponentPrivate->pcmParams->nChannels * 768;

       {
            OMX_PRCOMM2(pComponentPrivate->dbg, "Need to read more input \n");
            if(pComponentPrivate->pInputBufHeader == NULL)
            {
                OMX_PRCOMM2(pComponentPrivate->dbg,"Dequeuing new i/p buffer from pipe \n");
                ret = read(pComponentPrivate->dataPipeInputBuf[0], &pInputBufHeader, sizeof(pInputBufHeader));
                if (ret == -1)
                {
                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Input Buffer pipe\n",__LINE__);
                    eError = OMX_ErrorHardware;
                    goto EXIT;
                }
                pComponentPrivate->pInputBufHeader = pInputBufHeader;
                OMX_PRCOMM2(pComponentPrivate->dbg, "Received Input buffer with %ld bytes \n",pInputBufHeader->nFilledLen);
                if( pInputBufHeader->nFlags & OMX_BUFFERFLAG_EOS)
                {
                    //If EOS has come, Stop decoding , but do it only when whole of input is consumed
                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: EOS came from the Application !!\n",__LINE__);
                    pComponentPrivate->iEOS = 1;
                }
				else
                {
                    pComponentPrivate->iEOS = 0;
                    pComponentPrivate->SendEOStoApp = 0;
                }
            }

            bytesToCopy = pInputBufHeader->nFilledLen;
            if((pComponentPrivate->first_buff == 1) &&
               (0 == (pInputBufHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG)))
            {
                pComponentPrivate->first_buff = 2;
                pComponentPrivate->first_TS = pInputBufHeader->nTimeStamp;

				ALOGD("%s %d First Buffer ts=%lld", __func__, __LINE__, pInputBufHeader->nTimeStamp);

                OMX_PRCOMM2(pComponentPrivate->dbg,"----- first_TS --- :%lld \n",(pComponentPrivate->first_TS));
                pComponentPrivate->m_numSamplesProduced = 0;
                pComponentPrivate->iPreviousTimeStamp = pInputBufHeader->nTimeStamp;
                pComponentPrivate->iLatestTimeStamp = pInputBufHeader->nTimeStamp;
            }
            else if(pComponentPrivate->first_buff == 1)
            {
				ALOGD("%s %d CodecConfig Flag is set; First Buffer ts=%lld",
				__func__, __LINE__, pInputBufHeader->nTimeStamp);
			}
            else
            {
                pComponentPrivate->iPreviousTimeStamp = pComponentPrivate->iLatestTimeStamp;
                pComponentPrivate->iLatestTimeStamp = pInputBufHeader->nTimeStamp;
                OMX_PRCOMM2(pComponentPrivate->dbg,"-----InputTimeStamp--- :%lld \n",(pInputBufHeader->nTimeStamp));
            }
            pInputBufHeader->nFilledLen -= bytesToCopy;
            pComponentPrivate->inputBufFilledLen += bytesToCopy ;

        }
        if(pComponentPrivate->bNewOutputBufRequired)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg,"Dequeuing new o/p buffer from pipe \n");
            /* Dequeue an Output Buffer */
            ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }
            pComponentPrivate->pOutputBufHeader = pOutputBufHeader;
            pComponentPrivate->bNewOutputBufRequired = 0;
            pOutputBufHeader->nFilledLen = 0;
        }
        else
        {
            pOutputBufHeader = pComponentPrivate->pOutputBufHeader;
        }

        memcpy(pComponentPrivate->pOutputBufHeader->pBuffer + pComponentPrivate->pOutputBufHeader->nOffset,
                pComponentPrivate->pInputBufHeader->pBuffer + pComponentPrivate->pInputBufHeader->nOffset,
                pComponentPrivate->inputBufFilledLen);

       {
            pComponentPrivate->iFrameCount++;
            /* First Frame has only config data */
            if( pComponentPrivate->bConfigData == 1)
            {
                pComponentPrivate->bConfigData = 0;
                OMX_PRCOMM2(pComponentPrivate->dbg, "Decoded Config data successfully: Config : %d!!\n", pComponentPrivate->bConfigData);
            }

      }

        bytes_consumed = pComponentPrivate->inputBufFilledLen;
        out_bytes   = pComponentPrivate->inputBufFilledLen;

        if ( out_bytes == 0 && pComponentPrivate->iEOS == 1)
            pComponentPrivate->SendEOStoApp = 1;

        OMX_PRCOMM2(pComponentPrivate->dbg, "-----------FrameCount:%ld----------- \n", pComponentPrivate->iFrameCount);

        if( out_bytes > 0  &&   pComponentPrivate->pcmParams->nChannels == 2)
        {
            if(pComponentPrivate->interleave_mode == 0 || pComponentPrivate->pcmParams->bInterleaved == OMX_FALSE )
            {
                OMX_S16* pTmpBuf = pOutputBufHeader->pBuffer + pOutputBufHeader->nOffset;
                OMX_S16* pOpBuf  = pOutputBufHeader->pBuffer + pOutputBufHeader->nFilledLen;
                OMX_S32 k = 0, numSamples_per_ch = out_bytes >> 2;

                OMX_PRCOMM2(pComponentPrivate->dbg, "De-Interleaving  !!!!!\n");
                for (k=0; k < numSamples_per_ch; k++)
                { //k = 2k, k+ numSamples_per_ch = 2k+1

                    pTmpBuf[k]                    = pOpBuf[2*k];
                    pTmpBuf[k+ numSamples_per_ch] = pOpBuf[2*k + 1];

                }
                memcpy(pOpBuf, pTmpBuf, sizeof(OMX_S16)* (out_bytes >> 1));
            }

        }
       pOutputBufHeader->nFilledLen += out_bytes;

       OMX_PRCOMM2(pComponentPrivate->dbg, "Bytes Consumed :: %ld \n",bytes_consumed);
       OMX_PRCOMM2(pComponentPrivate->dbg, "out_bytes      :: %ld \n",out_bytes);

           if(NULL != pInputBufHeader)
           {

               if ( 0 == pInputBufHeader->nFilledLen)
               {

                   OMX_PRCOMM2(pComponentPrivate->dbg, "Return Input Buffer\n");
                   pComponentPrivate->nUnhandledEmptyThisBuffers--;
                   pComponentPrivate->cbInfo.EmptyBufferDone (
                       pComponentPrivate->pHandle,
                       pComponentPrivate->pHandle->pApplicationPrivate,
                       pInputBufHeader
                       );
                   pComponentPrivate->pInputBufHeader = NULL;
                   pComponentPrivate->inputBufFilledLen = 0;
               }
           }
           if( pComponentPrivate->SendEOStoApp == 1) {
               OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Sending EOS Event to Application !!\n",__LINE__);
               pOutputBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;
               pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                   pComponentPrivate->pHandle->pApplicationPrivate,
                   OMX_EventBufferFlag,
                   PCMDEC_OUTPUT_PORT,
                   OMX_BUFFERFLAG_EOS, //pOutputBufHeader->nFlags,
                   NULL);
               pComponentPrivate->iEOS = 0;
			   //pComponentPrivate->SendEOStoApp = 0;
			   pComponentPrivate->first_buff = 1;	//change for rpt_cur_song

           }


           //Return the output buffer when no more space left
           //Dont wait for output buffer to be completely filled, if EOS has come.
           if(resize_needed == 0)
           {
#ifdef MULTICHANNEL_ENABLE
OMX_S32 max_output_per_frame = 24576;
#else
OMX_S32 max_output_per_frame = 8192;
#endif

           if(((pOutputBufHeader->nAllocLen - pOutputBufHeader->nFilledLen ) < max_output_per_frame )||
                   ( pComponentPrivate->SendEOStoApp == 1 ) || (pComponentPrivate->framemode ==1 ))
           {
                   OMX_PRCOMM2(pComponentPrivate->dbg, "Return Output Buffer\n");
               /*Calculate the time stamp*/

               {
               OMX_U64 timeStampUS;
               timeStampUS = (pComponentPrivate->m_numSamplesProduced * 1000000) / pComponentPrivate->pcmParams->nSamplingRate;
               pOutputBufHeader->nTimeStamp = pComponentPrivate->first_TS + timeStampUS; // In milli seconds
               pComponentPrivate->m_numSamplesProduced += (pOutputBufHeader->nFilledLen) / (pComponentPrivate->pcmParams->nChannels * pComponentPrivate->pcmParams->nBitPerSample / 8);
            OMX_PRCOMM2(pComponentPrivate->dbg,"----OutputTimeStamp---- :%lld \n",(pOutputBufHeader->nTimeStamp));
               }
               pComponentPrivate->nUnhandledFillThisBuffers--;
               pComponentPrivate->bNewOutputBufRequired = 1;
               pComponentPrivate->cbInfo.FillBufferDone (
                   pComponentPrivate->pHandle,
                   pComponentPrivate->pHandle->pApplicationPrivate,
                   pOutputBufHeader
                   );
               pComponentPrivate->pOutputBufHeader = NULL;
			   pComponentPrivate->SendEOStoApp = 0;		//change for rpt_cur_song


           }
           }


           if (resize_needed)
           {
               OMX_PRCOMM2(pComponentPrivate->dbg, "OMX_EventPortSettingsChanged: %d !!\n", __LINE__);
               pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                   pComponentPrivate->pHandle->pApplicationPrivate,
                   OMX_EventPortSettingsChanged,
                   PCMDEC_OUTPUT_PORT,
                   0,
                   NULL);

           }


    } /*if (pComponentPrivate->curState == OMX_StateExecuting) */


EXIT:
    return eError;
}




/* ================================================================================= * */
/**
* @fn PCMDEC_HandleCommand() function handles the command sent by the application.
* All the state transitions, except from nothing to loaded state, of the
* component are done by this function.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_U32 PCMDEC_HandleCommand (PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;
    OMX_COMMANDTYPE command;
    OMX_STATETYPE commandedState;
    OMX_S32 commandData;
    OMX_S32 ret = 0;

    OMX_PRSTATE1(pComponentPrivate->dbg, "%d :: PCMDEC: Entering PCMDECHandleCommand Function - curState = %d\n",
                __LINE__,pComponentPrivate->curState);


    ret = read (pComponentPrivate->cmdPipe[0], &command, sizeof (command));
    OMX_TRACE1(pComponentPrivate->dbg, "%d :: PCMDEC: Command pipe has been read = %ld \n",__LINE__,ret);

    if (ret == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Reading from the Data pipe\n", __LINE__);
        eError = OMX_ErrorHardware;
        goto EXIT;
    }

    ret = read (pComponentPrivate->cmdDataPipe[0], &commandData, sizeof (commandData));
    OMX_TRACE1(pComponentPrivate->dbg, "%d :: PCMDEC: Command data pipe has been read = %ld \n",__LINE__,ret);
    if (ret == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Reading from the Data pipe\n", __LINE__);
        eError = OMX_ErrorHardware;
        goto EXIT;
    }

    OMX_PRDSP2(pComponentPrivate->dbg, "%d :: PCMDEC_HandleCommand :: Command is = %d\n",__LINE__,command);

    if(command == OMX_CommandStateSet)
    {
        eError = PCMDEC_HandleCommandStateSet(pComponentPrivate, commandData);
        if(eError != OMX_ErrorNone)
        {
            goto EXIT;
        }
    }
    else if (command == OMX_CommandMarkBuffer)
    {

        if(!pComponentPrivate->pMarkBuf)
        {
            pComponentPrivate->pMarkBuf = (OMX_MARKTYPE *)(commandData);
        }
    }
    else if (command == OMX_CommandPortDisable)
    {
        eError = PCMDEC_HandleCommandPortDisable(pComponentPrivate, commandData);
        if(eError != OMX_ErrorNone)
        {
            goto EXIT;
        }
    }
    else if (command == OMX_CommandPortEnable)
    {
        eError = PCMDEC_HandleCommandPortEnable(pComponentPrivate, commandData);
        if(eError != OMX_ErrorNone)
        {
            goto EXIT;
        }
    }
    else if (command == OMX_CommandFlush)
    {

    OMX_PRBUFFER2(pComponentPrivate->dbg, "OMX_CommandFlush Received %d\n",__LINE__);

        if(commandData == 0x0 || commandData == -1)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "OMX_CommandFlush Received on INPUT port %d\n",__LINE__);

            ReturnInputBuffers(pComponentPrivate);

            OMX_PRBUFFER2(pComponentPrivate->dbg, "OMX_CommandFlush nUnhandledEmptyThisBuffers %d\n",
                                                    pComponentPrivate->nUnhandledEmptyThisBuffers);

            pComponentPrivate->InputPortFlushed = 1;

            pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                   pHandle->pApplicationPrivate,
                                                   OMX_EventCmdComplete,
                                                   OMX_CommandFlush,
                                                   OMX_DirInput,
                                                   NULL);
        }

        if(commandData == 0x1 || commandData == -1)
        {
        OMX_PRCOMM2(pComponentPrivate->dbg, "OMX_CommandFlush Received on OUTPUT port %d\n",__LINE__);
        OMX_PRBUFFER2(pComponentPrivate->dbg, "OMX_CommandFlush nUnhandledFillThisBuffers %d\n",
                                                    pComponentPrivate->nUnhandledFillThisBuffers);

            ReturnOutputBuffers(pComponentPrivate);

            OMX_PRBUFFER2(pComponentPrivate->dbg, "OMX_CommandFlush nUnhandledFillThisBuffers %d\n",
                                                    pComponentPrivate->nUnhandledFillThisBuffers);

            pComponentPrivate->OutputPortFlushed = 1;
            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                                   pComponentPrivate->pHandle->pApplicationPrivate,
                                                   OMX_EventCmdComplete,
                                                   OMX_CommandFlush,
                                                   OMX_DirOutput,
                                                   NULL);
        }
    }
 EXIT:

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_HandleCommandStateSet() function handles All the state transitions, except
* from nothing to loaded state, of the component are done by this function.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_HandleCommandStateSet(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                           OMX_S32                   commandData)
{

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;
    OMX_STATETYPE commandedState;


    commandedState = (OMX_STATETYPE)commandData;
    if (pComponentPrivate->curState == commandedState)
    {
        pComponentPrivate->cbInfo.EventHandler ( pHandle,
            pHandle->pApplicationPrivate,
            OMX_EventError,
            OMX_ErrorSameState,
            OMX_TI_ErrorMinor,
            NULL);
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Same State Given by \
                Application\n",__LINE__);
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: PCMDEC: State Given is: %d \n",
                __LINE__, commandedState);
    }
    else
    {
        switch(commandedState)
        {
        case OMX_StateIdle:
            {
                OMX_PRDSP2(pComponentPrivate->dbg, "%d: PCMDEC_HandleCommandStateSet: Cmd Idle \n",__LINE__);
                eError = PCMDEC_SetStateToIdle(pComponentPrivate);
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            break;

        case OMX_StateExecuting:
            {
                eError = PCMDEC_SetStateToExecuting(pComponentPrivate);
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            break;

        case OMX_StateLoaded:
            {
                eError = PCMDEC_SetStateToLoaded(pComponentPrivate);
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            break;

        case OMX_StatePause:
            {
                eError = PCMDEC_SetStateToPause(pComponentPrivate);
                if(eError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }

            break;

        case OMX_StateWaitForResources:
            {


                OMX_PRDSP2(pComponentPrivate->dbg, "%d: HandleCommand: Cmd : OMX_StateWaitForResources\n",__LINE__);
                if (pComponentPrivate->curState == OMX_StateLoaded)
                {
                    pComponentPrivate->curState = OMX_StateWaitForResources;

                    pComponentPrivate->cbInfo.EventHandler( pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pComponentPrivate->curState,
                        NULL);
                }
                else
                {
                    pComponentPrivate->cbInfo.EventHandler( pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventError,
                        OMX_ErrorIncorrectStateTransition,
                        OMX_TI_ErrorMinor,
                        NULL);
                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: state transition error\n",__LINE__);
                }
            }

            break;

        case OMX_StateInvalid:
            {


                OMX_PRDSP2(pComponentPrivate->dbg, "%d: HandleCommand: Cmd OMX_StateInvalid:\n",__LINE__);
                if (pComponentPrivate->curState != OMX_StateWaitForResources &&
                    pComponentPrivate->curState != OMX_StateInvalid &&
                    pComponentPrivate->curState != OMX_StateLoaded)
                {

                    /* EMMCodecControlDestroy, (void *)pArgs);*/
                    eError = PCMDEC_CloseTheComponent(pComponentPrivate);
                    if(OMX_ErrorNone != eError)
                    {
                        goto EXIT;
                    }
                }

                pComponentPrivate->curState = OMX_StateInvalid;
                pComponentPrivate->cbInfo.EventHandler( pHandle,
                    pHandle->pApplicationPrivate,
                    OMX_EventError,
                    OMX_ErrorInvalidState,
                    OMX_TI_ErrorSevere,
                    NULL);
            }
            break;

        case OMX_StateMax:
            OMX_PRDSP2(pComponentPrivate->dbg, "%d: HandleCommand: Cmd OMX_StateMax::\n",__LINE__);
            break;
        } /* End of Switch */
    }
EXIT:

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_SetStateToIdle() function sets state of the OMX Component to StateIdle.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_SetStateToIdle(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    int inputPortFlag = 0;
    int outputPortFlag = 0;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;



    if(pComponentPrivate->curState == OMX_StateLoaded ||
       pComponentPrivate->curState == OMX_StateWaitForResources)
    {


        if (pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bPopulated &&
            pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled)
        {
            inputPortFlag = 1;
        }

        if (!pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bPopulated &&
            !pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled)
        {
            inputPortFlag = 1;
        }

        if (pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bPopulated &&
            pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bEnabled)
            {
            outputPortFlag = 1;
        }

        if (!pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bPopulated &&
            !pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bEnabled)
        {
            outputPortFlag = 1;
        }

        if (!(inputPortFlag && outputPortFlag))
        {
            pComponentPrivate->InLoaded_readytoidle = 1;

            pthread_mutex_lock(&pComponentPrivate->InLoaded_mutex);
            pthread_cond_wait(&pComponentPrivate->InLoaded_threshold, &pComponentPrivate->InLoaded_mutex);
            pthread_mutex_unlock(&pComponentPrivate->InLoaded_mutex);
        }


            eError = PCMDEC_InitializeTheComponent(pComponentPrivate);
            if(OMX_ErrorNone != eError)
            {
                goto EXIT;
            }

        pComponentPrivate->curState = OMX_StateIdle;
        pComponentPrivate->cbInfo.EventHandler(pHandle,
                                               pHandle->pApplicationPrivate,
                                               OMX_EventCmdComplete,
                                               OMX_CommandStateSet,
                                               pComponentPrivate->curState,
                                               NULL);

    } //if (pComponentPrivate->curState == OMX_StateLoaded || pComponentPrivate->curState == OMX_StateWaitForResources)
    else if ((pComponentPrivate->curState == OMX_StateExecuting) || (pComponentPrivate->curState == OMX_StatePause))
    {


      //Shall return all buffers:
        eError = ReturnInputOutputBuffers(pComponentPrivate);
        if (eError != OMX_ErrorNone)
          {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Return I/O Buffers  %x\n",__LINE__, eError);
            goto EXIT;
          }

        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: After Returned all buffers\n", __LINE__);

        pComponentPrivate->curState = OMX_StateIdle;
        pComponentPrivate->cbInfo.EventHandler(pHandle,
                                               pHandle->pApplicationPrivate,
                                               OMX_EventCmdComplete,
                                               OMX_CommandStateSet,
                                               pComponentPrivate->curState,
                                               NULL);

    OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCM_DEC : Component state changed to Idle \n", __LINE__);
    }
    else
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d: Comp: Sending ErrorNotification: Invalid State\n",__LINE__);
        pComponentPrivate->cbInfo.EventHandler(pHandle,
                                               pHandle->pApplicationPrivate,
                                               OMX_EventError,
                                               OMX_ErrorIncorrectStateTransition,
                                               OMX_TI_ErrorMinor,
                                               "Invalid State Error");
    }

EXIT:

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_SetStateToExecuting() function sets state of the OMX Component to StateExecuting.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_SetStateToExecuting(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;

    if (pComponentPrivate->curState == OMX_StateIdle)
    {
    }
    else if (pComponentPrivate->curState == OMX_StatePause)
    {
    }
    else
    {
        pComponentPrivate->cbInfo.EventHandler ( pHandle,
                                                 pHandle->pApplicationPrivate,
                                                 OMX_EventError,
                                                 OMX_ErrorIncorrectStateTransition,
                                                 OMX_TI_ErrorMinor,
                                                 "Invalid State");
        goto EXIT;
    }

    pComponentPrivate->curState = OMX_StateExecuting;
    OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: Current State is changed to OMX_StateExecuting \n", __LINE__);

    pComponentPrivate->cbInfo.EventHandler( pHandle,
                                            pHandle->pApplicationPrivate,
                                            OMX_EventCmdComplete,
                                            OMX_CommandStateSet,
                                            pComponentPrivate->curState,
                                            NULL);
EXIT:

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_SetStateToLoaded() function sets state of the OMX Component to StateLoaded.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_SetStateToLoaded(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;



    if (pComponentPrivate->curState == OMX_StateWaitForResources )
    {
        OMX_PRSTATE2(pComponentPrivate->dbg, "****************** Component State Set to Loaded\n\n");
        pComponentPrivate->curState = OMX_StateLoaded;
        pComponentPrivate->cbInfo.EventHandler ( pHandle,
                                                 pHandle->pApplicationPrivate,
                                                 OMX_EventCmdComplete,
                                                 OMX_CommandStateSet,
                                                 pComponentPrivate->curState,
                                                 NULL);
        goto EXIT;
    }

    if (pComponentPrivate->curState != OMX_StateIdle)
    {
        pComponentPrivate->cbInfo.EventHandler ( pHandle,
                                                 pHandle->pApplicationPrivate,
                                                 OMX_EventError,
                                                 OMX_ErrorIncorrectStateTransition,
                                                 OMX_TI_ErrorMinor,
                                                 "Incorrect State Transition");
        goto EXIT;
    }

    if (pComponentPrivate->pInputBufferList->numBuffers || pComponentPrivate->pOutputBufferList->numBuffers)
    {

        pComponentPrivate->InIdle_goingtoloaded = 1;
        pthread_mutex_lock(&pComponentPrivate->InIdle_mutex);
        pthread_cond_wait(&pComponentPrivate->InIdle_threshold, &pComponentPrivate->InIdle_mutex);
        pthread_mutex_unlock(&pComponentPrivate->InIdle_mutex);
        pComponentPrivate->bLoadedCommandPending = OMX_FALSE;

    }

    /* EMMCodecControlDestroy,(void *)pArgs); */
        OMX_PRSTATE2(pComponentPrivate->dbg, "**Calling PCMDEC_CloseTheComponent \n\n");
        eError = PCMDEC_CloseTheComponent(pComponentPrivate);
        if(OMX_ErrorNone != eError)
        {
            goto EXIT;
        }

        pComponentPrivate->curState = OMX_StateLoaded;
        pComponentPrivate->cbInfo.EventHandler ( pHandle,
                                                 pHandle->pApplicationPrivate,
                                                 OMX_EventCmdComplete,
                                                 OMX_CommandStateSet,
                                                 pComponentPrivate->curState,
                                                 NULL);

    //eError = EXIT_COMPONENT_THRD;

    //Otherwise this would result in multiple callbacks, reporting command completion.
    // one here and the other one in component thread
    // This is applicable for all other Audio OMX Components

    pComponentPrivate->bInitParamsInitialized = 0;

EXIT:

    OMX_PRSTATE2(pComponentPrivate->dbg, "Component in State Loaded \n\n");
    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_SetStateToPause() function sets state of the OMX Component to StatePause.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_SetStateToPause(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;



    if ((pComponentPrivate->curState != OMX_StateExecuting) &&
        (pComponentPrivate->curState != OMX_StateIdle))
    {

        pComponentPrivate->cbInfo.EventHandler ( pHandle,
                                                 pHandle->pApplicationPrivate,
                                                 OMX_EventError,
                                                 OMX_ErrorIncorrectStateTransition,
                                                 OMX_TI_ErrorMinor,
                                                 "Incorrect State Transition");
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Invalid State Given by \
           Application\n",__LINE__);
        goto EXIT;
    }
    else
    {

        pComponentPrivate->curState = OMX_StatePause;
        pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                pHandle->pApplicationPrivate,
                                                OMX_EventCmdComplete,
                                                OMX_CommandStateSet,
                                                pComponentPrivate->curState,
                                                NULL);
    }

EXIT:

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_HandleCommandPortDisable() function disables the requested port/s .
*
* @param pComponentPrivate  This is component's private date structure.
*
* @param commandData This tells the port/s to be disabled
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_HandleCommandPortDisable(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;
    OMX_PRCOMM2(pComponentPrivate->dbg, "Entering PCMDEC_HandleCommandPortDisable \n\n");


    if (!pComponentPrivate->bDisableCommandPending)
    {
        OMX_PRCOMM2(pComponentPrivate->dbg, " Setting bEnabled to OMX_FALSE for Command data:%d \n", commandData);

        if(commandData == 0x0)
        {
            pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled = OMX_FALSE;
        }
        if(commandData == -1)
        {
            pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled = OMX_FALSE;
        }
        if(commandData == 0x1 || commandData == -1)
        {
            pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bEnabled = OMX_FALSE;
        }
    }



    if(commandData == 0x0)
    {
        if(!pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bPopulated)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "Disable Input port completed\n\n");
            pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventCmdComplete,
                                                    OMX_CommandPortDisable,
                                                    INPUT_PORT_PCMDEC,
                                                    NULL);
            pComponentPrivate->bDisableCommandPending = 0;

        }
        else
        {
            pComponentPrivate->bDisableCommandPending = 1;
            pComponentPrivate->bDisableCommandParam = commandData;
        }
    }

    if(commandData == 0x1)
    {
        if (!pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bPopulated)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "Disable Output port completed\n\n");
            pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                   pHandle->pApplicationPrivate,
                                                   OMX_EventCmdComplete,
                                                   OMX_CommandPortDisable,
                                                   OUTPUT_PORT_PCMDEC,
                                                   NULL);

            pComponentPrivate->bDisableCommandPending = 0;
        }
        else
        {
            pComponentPrivate->bDisableCommandPending = 1;
            pComponentPrivate->bDisableCommandParam = commandData;
        }
    }

    if(commandData == -1)
    {
        if (!pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bPopulated &&
            !pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bPopulated){

            pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventCmdComplete,
                                                    OMX_CommandPortDisable,
                                                    INPUT_PORT_PCMDEC,
                                                    NULL);

            pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventCmdComplete,
                                                    OMX_CommandPortDisable,
                                                    OUTPUT_PORT_PCMDEC,
                                                    NULL);
            pComponentPrivate->bDisableCommandPending = 0;
        }
        else
        {
            pComponentPrivate->bDisableCommandPending = 1;
            pComponentPrivate->bDisableCommandParam = commandData;
        }
        OMX_PRINT2(pComponentPrivate->dbg, "pComponentPrivate->bDisableCommandParam = %ld\n", pComponentPrivate->bDisableCommandParam);
    }

    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_HandleCommandPortDisable() function disables the requested port/s .
*
* @param pComponentPrivate  This is component's private date structure.
*
* @param commandData This tells the port/s to be disabled
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*
*  @see         None
*/
/* ================================================================================ * */
OMX_ERRORTYPE PCMDEC_HandleCommandPortEnable(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate,
                                              OMX_S32                  commandData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;



    if(!pComponentPrivate->bEnableCommandPending)
    {
        if(commandData == 0x0 || commandData == -1)
        {

            OMX_PRCOMM2(pComponentPrivate->dbg, "setting input port to enabled\n");

            pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled = OMX_TRUE;

            OMX_PRCOMM2(pComponentPrivate->dbg, "pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled = %d\n",
                          pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bEnabled);

            if(pComponentPrivate->AlloBuf_waitingsignal)
            {
                pComponentPrivate->AlloBuf_waitingsignal = 0;
            }
        }
        if(commandData == 0x1 || commandData == -1)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "setting output port to enabled\n");
            pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bEnabled = OMX_TRUE;
            OMX_PRCOMM2(pComponentPrivate->dbg, "pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bEnabled = %d\n",
                          pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bEnabled);
        }
    }

    if(commandData == 0x0)
    {


        if (pComponentPrivate->curState == OMX_StateLoaded ||
            pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bPopulated)
            {
                            pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventCmdComplete,
                            OMX_CommandPortEnable,
                            INPUT_PORT_PCMDEC,
                            NULL);
            pComponentPrivate->bEnableCommandPending = 0;

            if(pComponentPrivate->AlloBuf_waitingsignal)
            {
                pComponentPrivate->AlloBuf_waitingsignal = 0;
                pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
                pthread_cond_signal(&pComponentPrivate->AlloBuf_threshold);
                pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);
            }


        }
    }
    else if(commandData == 0x1)
    {


        if ((pComponentPrivate->curState == OMX_StateLoaded) ||
            (pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bPopulated))
           {
                pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                        pHandle->pApplicationPrivate,
                                                        OMX_EventCmdComplete,
                                                        OMX_CommandPortEnable,
                                                        OUTPUT_PORT_PCMDEC,
                                                        NULL);
               if(pComponentPrivate->AlloBuf_waitingsignal)
               {
                    pComponentPrivate->AlloBuf_waitingsignal = 0;
                    pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
                    pthread_cond_signal(&pComponentPrivate->AlloBuf_threshold);
                    pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);
                }


                pComponentPrivate->bEnableCommandPending = 0;

            }
            else
            {
                pComponentPrivate->bEnableCommandPending = 1;
                pComponentPrivate->bEnableCommandParam = commandData;
            }
      }
      else if(commandData == -1)
      {

            if (pComponentPrivate->curState == OMX_StateLoaded ||
                (pComponentPrivate->pPortDef[INPUT_PORT_PCMDEC]->bPopulated &&
                 pComponentPrivate->pPortDef[OUTPUT_PORT_PCMDEC]->bPopulated))
              {
                     pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                            pHandle->pApplicationPrivate,
                                                            OMX_EventCmdComplete,
                                                            OMX_CommandPortEnable,
                                                            INPUT_PORT_PCMDEC,
                                                            NULL);

                     pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                            pHandle->pApplicationPrivate,
                                                            OMX_EventCmdComplete,
                                                            OMX_CommandPortEnable,
                                                            OUTPUT_PORT_PCMDEC,
                                                            NULL);

                 if(pComponentPrivate->AlloBuf_waitingsignal)
                 {
                     pComponentPrivate->AlloBuf_waitingsignal = 0;
                     pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
                     pthread_cond_signal(&pComponentPrivate->AlloBuf_threshold);
                     pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);
                 }

                 pComponentPrivate->bEnableCommandPending = 0;
             }
             else
             {
                 pComponentPrivate->bEnableCommandPending = 1;
                 pComponentPrivate->bEnableCommandParam = commandData;
             }
         }

         pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
         pthread_cond_signal(&pComponentPrivate->AlloBuf_threshold);
         pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);


    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_InitializeTheComponent() function initializes the ARM PCM decoder component.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_InitializeTheComponent(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    pComponentPrivate->bNewOutputBufRequired = 1;

    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Component Created Successfully \n",__LINE__);


EXIT:
    return eError;
}

/* ================================================================================= * */
/**
* @fn PCMDEC_CloseTheComponent() function de-initializes the ARM PCM decoder component.
*
* @param pComponentPrivate  This is component's private date structure.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Successful processing.
*
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE PCMDEC_CloseTheComponent(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

EXIT:

    OMX_PRSTATE2(pComponentPrivate->dbg, "Exiting PCMDEC_CloseTheComponent  \n\n");
    return eError;
}

OMX_ERRORTYPE ReturnInputOutputBuffers(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_S32 ret = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pInputBufHeader;
    OMX_BUFFERHEADERTYPE *pOutputBufHeader;

    pComponentPrivate->first_TS = 0;
    pComponentPrivate->first_buff = 1;

    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Remaining InputBuff [%d], Remaining OutputBuf [%d] !!!!! \n",__LINE__,pComponentPrivate->nUnhandledEmptyThisBuffers, pComponentPrivate->nUnhandledFillThisBuffers );

    if(pComponentPrivate->pOutputBufHeader != NULL)
    {
            pComponentPrivate->bNewOutputBufRequired = 1;
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnInputOutputBuffers: returning last held buffer in process !!!! \n",__LINE__);
        pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pComponentPrivate->pOutputBufHeader
                );
        pComponentPrivate->pOutputBufHeader = NULL;

        pComponentPrivate->nUnhandledFillThisBuffers--;
    }
    if(pComponentPrivate->pInputBufHeader  != NULL)
    {
            pComponentPrivate->inputBufFilledLen = 0;
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnInputOutputBuffers: returning last held input buffer in process !!!! \n",__LINE__);
        pComponentPrivate->cbInfo.EmptyBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pComponentPrivate->pInputBufHeader
                );
        pComponentPrivate->pInputBufHeader  = NULL;

        pComponentPrivate->nUnhandledEmptyThisBuffers--;
    }
    while(1)
    {
        int status;
        struct timespec tv;
        int fdmax;


        FD_ZERO (&pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->cmdPipe[0], &pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds);

        fdmax = pComponentPrivate->dataPipeInputBuf[0];
        if (pComponentPrivate->dataPipeOutputBuf[0] > fdmax)
            fdmax = pComponentPrivate->dataPipeOutputBuf[0];
        tv.tv_sec = 0;
        tv.tv_nsec = 5;

        sigset_t set;
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv, &set);
        if(status == 0)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Component Time Out !!!!! \n",__LINE__);
            break;
        }

        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: Inside whil loop \n", __LINE__);
        if (FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds))
        {

            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: calling read on Output buffer pipe \n", __LINE__);
            ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }
            OMX_PRCOMM2(pComponentPrivate->dbg, "Return Output Buffer\n");
            pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pOutputBufHeader
                );
                pComponentPrivate->nUnhandledFillThisBuffers--;



        }
        else if (FD_ISSET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds))
        {

            ret = read(pComponentPrivate->dataPipeInputBuf[0], &pInputBufHeader, sizeof(pInputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Input Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }
            pInputBufHeader->nFilledLen = 0;
            OMX_PRCOMM2(pComponentPrivate->dbg, "Return Input Buffer\n");
            pComponentPrivate->cbInfo.EmptyBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pInputBufHeader
                );
           pComponentPrivate->nUnhandledEmptyThisBuffers--;

        }
    }

    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Remaining InputBuff [%d], Remaining OutputBuf [%d] !!!!! \n",__LINE__,pComponentPrivate->nUnhandledEmptyThisBuffers, pComponentPrivate->nUnhandledFillThisBuffers );
EXIT:
    return eError;
}

OMX_ERRORTYPE ReturnInputBuffers(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_S32 ret = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pInputBufHeader;
    int fdmax;
    fdmax = pComponentPrivate->dataPipeInputBuf[0];
    if (pComponentPrivate->dataPipeOutputBuf[0] > fdmax)
    fdmax = pComponentPrivate->dataPipeOutputBuf[0];

    pComponentPrivate->first_TS = 0;
    pComponentPrivate->first_buff = 1;
    if(pComponentPrivate->pInputBufHeader  != NULL)
    {
            pComponentPrivate->inputBufFilledLen = 0;
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnInputOutputBuffers: returning last held input buffer in process !!!! \n",__LINE__);
        pComponentPrivate->cbInfo.EmptyBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pComponentPrivate->pInputBufHeader
                );
        pComponentPrivate->pInputBufHeader  = NULL;
        pComponentPrivate->nUnhandledEmptyThisBuffers--;
    }

    while(1)
    {
        int status;
        struct timespec tv;

        FD_ZERO (&pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->cmdPipe[0], &pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds);

        tv.tv_sec = 0;
        tv.tv_nsec = 5;

        sigset_t set;
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv, &set);
        if(status == 0)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnInputBuffers :Component Time Out !!!!! \n",__LINE__);
            break;
        }

        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: Inside whil loop \n", __LINE__);
        if (FD_ISSET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds))
        {

            ret = read(pComponentPrivate->dataPipeInputBuf[0], &pInputBufHeader, sizeof(pInputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Input Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }
            pInputBufHeader->nFilledLen = 0;
            OMX_PRCOMM2(pComponentPrivate->dbg, "Return Input Buffer\n");
            pComponentPrivate->cbInfo.EmptyBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pInputBufHeader
                    );
        pComponentPrivate->nUnhandledEmptyThisBuffers--;

        }
        else
        {
            break;
        }
    }
EXIT:
    return eError;
}

OMX_ERRORTYPE ReturnOutputBuffers(PCMDEC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_S32 ret = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pOutputBufHeader;
    int status;
    struct timespec tv;

    int fdmax;
    fdmax = pComponentPrivate->dataPipeInputBuf[0];
    if (pComponentPrivate->dataPipeOutputBuf[0] > fdmax)
    fdmax = pComponentPrivate->dataPipeOutputBuf[0];

    /* Time stamp issue[ Removed as this would cause timestamp issue when output port alone is reconfigured.
    The component would have copied the input data and returned the buffer.
    When reset, the Time stamp would correspond to 2nd buffer but output would correspond to first bufffer stored in the internal buffer */

    /*pComponentPrivate->first_TS = 0;
    pComponentPrivate->first_buff = 1;*/
    // Time stamp issue ]
    pComponentPrivate->bNewOutputBufRequired = 1;
//  OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnOutputBuffers: bNewOutputBufRequired: %d !!!! \n",__LINE__,pComponentPrivate->bNewOutputBufRequired);

    if(pComponentPrivate->pOutputBufHeader != NULL)
    {
            pComponentPrivate->bNewOutputBufRequired = 1;
        OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnOutputBuffers: returning last held buffer in process !!!! \n",__LINE__);
        pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pComponentPrivate->pOutputBufHeader
                );
        pComponentPrivate->pOutputBufHeader = NULL;

        pComponentPrivate->nUnhandledFillThisBuffers--;
    }
    while(1)
    {

        FD_ZERO (&pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->cmdPipe[0], &pComponentPrivate->rfds);
            FD_SET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds);

        tv.tv_sec = 0;
        tv.tv_nsec = 5;

        sigset_t set;
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv, &set);
        if(status == 0)
        {
            OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnOutputBuffers: Component Time Out !!!!! \n",__LINE__);
            break;
        }

        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: Inside whil loop \n", __LINE__);
        if (FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds))
        {

            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: PCMDEC: calling read on Output buffer pipe \n", __LINE__);
            ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }
            OMX_PRCOMM2(pComponentPrivate->dbg, "Return Output Buffer\n");
            pOutputBufHeader->nFilledLen = 0;
            pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pOutputBufHeader
                );
        pComponentPrivate->nUnhandledFillThisBuffers--;

        }
        else
        {
            break;
        }

    }

    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnOutputBuffers: EXIT  !!!!! \n",__LINE__);
EXIT:
    return eError;
}

