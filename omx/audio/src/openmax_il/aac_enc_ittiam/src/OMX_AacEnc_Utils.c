/*****************************************************************************/
/*                                                                           */
/*                        Ittiam Audio Standards                             */
/*                                                                           */
/*                   ITTIAM SYSTEMS PVT LTD, BANGALORE                       */
/*                          COPYRIGHT(C) 2004                                */
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
/*  File Name        : ia_enhaacplus_enc_error_codes.h                     */
/*                                                                           */
/*  Description      : Module specific error codes definitions               */
/*                                                                           */
/*  List of Functions: None                                                  */
/*                                                                           */
/*  Issues / Problems: None                                                  */
/*                                                                           */
/*  Revision History :                                                       */
/*                                                                           */
/*        DD MM YYYY       Author                Changes                     */
/*        29 07 2005       Ittiam                Created                     */
/*                                                                           */
/*****************************************************************************/


/* ------compilation control switches -------------------------*/
/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#ifdef UNDER_CE
#include <windows.h>
#include <oaf_osal.h>
#include <omx_core.h>
#include <stdlib.h>
#else
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
#include <errno.h>
#include <dlfcn.h>
#endif
#include <dbapi.h>
#include <string.h>
#include <stdio.h>
#include "OMX_IttiamAacEnc_Utils.h"
#include "OMX_IttiamAacEnc_CompThread.h"
#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM
#include <ResourceManagerProxyAPI.h>
#endif

#include "ia_heaacv2_enc_omx.h"


ia_heaacv2_enc_params_t IA_HEAACV2_ENC_OCP_PARAMS = {
            sizeof(ia_heaacv2_enc_params_t),
                44100,         /* SampleRate                  */
                128000,        /* bitRate                     */
                1,             /* channelMode -Default STEREO                    */
                2,             /* dataEndianness-Default LITTLE ENDIAN                  */
                0,             /* Encoding Mode -Default CBR           */
                1,             /* InputFormat -Default interleaved                    */
                16,            /* InputBitsPerSample          */
                576000,        /* MaxBitrate                  */
                0,             /* DualMonoMode                */
                0,             /* CrcFlag                         */
                0,             /* AncFlag                         */
                0,             /* lfeFlag                         */
                2,             /* noChannels                  */
                1,             /* aacClassic                  */
                0,             /* psEnable                    */
                0,             /* dualMono                    */
                0,             /* downmix                     */
                0,             /* useSpeechConfig                 */
                0,             /* fNoStereoPreprocessing      */
                2,             /* invQuant                        */
                1,             /* useTns                          */
                1,             /* Use ADTS                        */
                0,             /* Use ADIF                        */
                0,             /* full_bandwidth                  */
                0x00,          /* channel mask                    */
                0,             /* i_num_coup_chan                     */
                0              /* write_program_config_element    */

};



/* ========================================================================== */
/**
* @AACENCFill_InitParams () This function is used by the component thread to
* fill the all of its initialization parameters, buffer deatils  etc
* to params structure,
*
* @param pComponent  handle for this instance of the component
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */
/*static AACENC_COMPONENT_PRIVATE *pComponentPrivate_CC;*/

OMX_ERRORTYPE AACENCFill_InitParams(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 nIpBuf =0 ,nIpBufSize=0 ,nOpBuf = 0 ,nOpBufSize=0;
//    OMX_U32 i = 0;
//    char *ptr;


    OMX_COMPONENTTYPE  *pHandle = (OMX_COMPONENTTYPE  *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = pHandle->pComponentPrivate;

    OMX_U16 HigherBitsSamplingRate          = 0;
    OMX_U16 FramesPerOutBuf                 = 0;
    OMX_U16 Channels                        = 0;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL: AACENCFill_InitParams\n ",__LINE__);
    nIpBuf = pComponentPrivate->pInputBufferList->numBuffers;
    pComponentPrivate->nRuntimeInputBuffers = nIpBuf;
    nIpBufSize = pComponentPrivate->pPortDef[INPUT_PORT]->nBufferSize;
    nOpBuf = pComponentPrivate->pOutputBufferList->numBuffers;
    nOpBufSize = pComponentPrivate->pPortDef[OUTPUT_PORT]->nBufferSize;
    /*saving a copy a number of output buffers */
    pComponentPrivate->nRuntimeOutputBuffers = nOpBuf;

    /*recovering the value for the number of frames per Ouput Buffer */
    FramesPerOutBuf = (OMX_U16)pComponentPrivate->FramesPer_OutputBuffer;
    OMX_PRBUFFER1(pComponentPrivate->dbg, "%d :: UTIL: Frames per output buffer = %d \n\n",__LINE__, FramesPerOutBuf);


    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: ------ Buffer Details -----------\n",__LINE__);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Input  Buffer Count = %ld \n",__LINE__,nIpBuf);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Input  Buffer Size = %ld\n",__LINE__,nIpBufSize);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Output Buffer Count = %ld\n",__LINE__,nOpBuf);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Output Buffer Size = %ld\n",__LINE__,nOpBufSize);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: ------ Buffer Details ------------\n",__LINE__);

    pComponentPrivate->unNumChannels   = (OMX_U16)pComponentPrivate->aacParams[OUTPUT_PORT]->nChannels;       /*Number of Channels*/

    pComponentPrivate->ia_heaacv2_enc_params.noChannels = (OMX_U16)pComponentPrivate->aacParams[OUTPUT_PORT]->nChannels;       /*Number of Channels*/


    /* splitting values for Sample rate and bit rate */
    pComponentPrivate->ulSamplingRate  = (OMX_U16)pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate;       /*Sampling rate*/
    pComponentPrivate->ia_heaacv2_enc_params.sampleRate =  (OMX_U16)pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate;       /*Sampling rate*/



    HigherBitsSamplingRate             =(OMX_U16)((pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate >>16)& 0xFFFF);

    pComponentPrivate->unBitrate       = pComponentPrivate->aacParams[OUTPUT_PORT]->nBitRate;                /*Bit rate 2bytes*/
    pComponentPrivate->ia_heaacv2_enc_params.bitRate = pComponentPrivate->aacParams[OUTPUT_PORT]->nBitRate;                /*Bit rate 2bytes*/

    pComponentPrivate->nObjectType = pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile; /*Object Type */

    if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile == OMX_AUDIO_AACObjectLC)
    {
        pComponentPrivate->ia_heaacv2_enc_params.aacClassic = 1;
        pComponentPrivate->ia_heaacv2_enc_params.psEnable = 0;
        OMX_PRDSP2(pComponentPrivate->dbg, "OMX_AUDIO_AAC profile \n");
    }
    else if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile == OMX_AUDIO_AACObjectHE)
    {
        pComponentPrivate->ia_heaacv2_enc_params.aacClassic = 0;
        pComponentPrivate->ia_heaacv2_enc_params.psEnable = 0;
        OMX_PRDSP2(pComponentPrivate->dbg, "OMX_AUDIO_HEAAC profile \n");
    }
    else if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile == OMX_AUDIO_AACObjectHE_PS)
    {
        pComponentPrivate->ia_heaacv2_enc_params.aacClassic = 0;
        pComponentPrivate->ia_heaacv2_enc_params.psEnable = 1;
        OMX_PRDSP2(pComponentPrivate->dbg, "OMX_AUDIO_HEAACv2 profile \n");
    }

    /*   Remaping Number of channels for SN */
    /*   SN does use 0: Mono and  1: stereo      */
    if(pComponentPrivate->unNumChannels==2)
    {
        Channels=1;
    }
    else if (pComponentPrivate->unNumChannels==1)
    {
        Channels=0;
    }

    pComponentPrivate->ia_heaacv2_enc_params.noChannels = pComponentPrivate->aacParams[OUTPUT_PORT]->nChannels;

    if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACStreamFormat == OMX_AUDIO_AACStreamFormatRAW ||
        pComponentPrivate->aacParams[OUTPUT_PORT]->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4FF)
    {
        pComponentPrivate->File_Format = 0;
        pComponentPrivate->ia_heaacv2_enc_params.use_ADTS=0;
        pComponentPrivate->ia_heaacv2_enc_params.use_ADIF=0;
            OMX_PRDSP2(pComponentPrivate->dbg, "OMX_AUDIO_AACStreamFormatRAW \n");
    }
    else if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACStreamFormat == OMX_AUDIO_AACStreamFormatADIF)
    {
        pComponentPrivate->File_Format = 1;
        pComponentPrivate->ia_heaacv2_enc_params.use_ADTS = 0;
        pComponentPrivate->ia_heaacv2_enc_params.use_ADIF = 1;
            OMX_PRDSP2(pComponentPrivate->dbg, "OMX_AUDIO_AACStreamFormatADIF \n");
    }
    else if((pComponentPrivate->aacParams[OUTPUT_PORT]->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4ADTS) ||
            (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP2ADTS) )
    {
        pComponentPrivate->File_Format = 2;
        pComponentPrivate->ia_heaacv2_enc_params.use_ADTS = 1;
        pComponentPrivate->ia_heaacv2_enc_params.use_ADIF = 0;
            OMX_PRDSP2(pComponentPrivate->dbg, "OMX_AUDIO_AACStreamFormatMP2ADTS \n");
    }




    pComponentPrivate->bPortDefsAllocated = 1;
    pComponentPrivate->bBypassDSP = 0;
    pComponentPrivate->bNoIdleOnStop= OMX_FALSE;


EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL: Exiting Fill_InitParams\n",__LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL: Returning = 0x%x\n",__LINE__,eError);
    return eError;
}

/* ========================================================================== */
/**
* AACENC_StartComponentThread() This function is called by the component to create
* the component thread, command pipe, data pipe .
*
* @param pComponent  handle for this instance of the component
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */

OMX_ERRORTYPE AACENC_StartComponentThread(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
#ifdef UNDER_CE
    pthread_attr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.__inheritsched = PTHREAD_EXPLICIT_SCHED;
    attr.__schedparam.__sched_priority = OMX_AUDIO_DECODER_THREAD_PRIORITY;
#endif


    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Entering  AACENC_StartComponentThread\n", __LINE__);
    /* Initialize all the variables*/
    pComponentPrivate->bIsStopping = 0;
    pComponentPrivate->bIsThreadstop = 0;
    pComponentPrivate->bIsEOFSent = 0;

    /* create the pipe used to send buffers to the thread */
    eError = pipe (pComponentPrivate->cmdDataPipe);
    if (eError)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  AACENC_StartComponentThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* Ceating two separate pipes for Input and Output Buffers*/
    /* create the pipe used to send Input buffers to the thread */
    eError = pipe (pComponentPrivate->dataPipeInputBuf);
    if (eError)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  AACENC_StartComponentThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* create the pipe used to send Output buffers to the thread */
    eError = pipe (pComponentPrivate->dataPipeOutputBuf);
    if (eError)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  AACENC_StartComponentThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* create the pipe used to send commands to the thread */
    eError = pipe (pComponentPrivate->cmdPipe);
    if (eError) {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  AACENC_StartComponentThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

   pComponentPrivate->i_init_done = 0;
   pComponentPrivate->ia_heaacv2_enc_input_args.size = sizeof(ia_heaacv2_enc_input_args_t);
   pComponentPrivate->ia_heaacv2_enc_output_args.size = sizeof(ia_heaacv2_enc_output_args_t);
   pComponentPrivate->ia_heaacv2_enc_params.size = sizeof(ia_heaacv2_enc_params_t);

   /* Initializing params to defaults */
   pComponentPrivate->ia_heaacv2_enc_params = IA_HEAACV2_ENC_OCP_PARAMS;

   OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC_StartComponentThread : Initialized HE-AAC params to default\n", __LINE__);

    /* create the pipe used to send commands to the thread */
#ifdef UNDER_CE
    eError = pthread_create (&(pComponentPrivate->ComponentThread), &attr, ComponentThread, pComponentPrivate);
#else
    eError = pthread_create (&(pComponentPrivate->ComponentThread), NULL, ComponentThread, pComponentPrivate);
#endif
    if (eError || !pComponentPrivate->ComponentThread) {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Inside  AACENC_StartComponentThread\n", __LINE__);
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    OMX_PRDSP1(pComponentPrivate->dbg, "%d :: pComponent[%x] AACENC_StartComponentThread\n", __LINE__, (int)pComponent) ;
    OMX_PRDSP1(pComponentPrivate->dbg, "%d :: pHandle[%x] AACENC_StartComponentThread\n", __LINE__, (int)pHandle) ;
    pComponentPrivate->bCompThreadStarted = 1;

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Exiting from AACENC_StartComponentThread\n", __LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Returning = 0x%x\n",__LINE__,eError);
    return eError;
}

/* ========================================================================== */
/**
* AACENC_FreeCompResources() This function is called by the component during
* de-init to close component thread, Command pipe, data pipe.
*
* @param pComponent  handle for this instance of the component
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */

OMX_ERRORTYPE AACENC_FreeCompResources(OMX_HANDLETYPE pComponent)
{
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OMX_U32 nIpBuf = 0;
    OMX_U32 nOpBuf = 0;

    OMX_PRINT1(pComponentPrivate->dbg, " %d :: Entering AACENC_FreeCompResources\n",__LINE__);
    if (pComponentPrivate->bPortDefsAllocated) {
        nIpBuf = pComponentPrivate->pPortDef[INPUT_PORT]->nBufferCountActual;
        nOpBuf = pComponentPrivate->pPortDef[OUTPUT_PORT]->nBufferCountActual;
    }

    if (pComponentPrivate->bCompThreadStarted) {
    OMX_CLOSE_PIPE(pComponentPrivate->dataPipeInputBuf[0],err);
        OMX_CLOSE_PIPE(pComponentPrivate->dataPipeInputBuf[1],err);
        OMX_CLOSE_PIPE(pComponentPrivate->dataPipeOutputBuf[0],err);
        OMX_CLOSE_PIPE(pComponentPrivate->dataPipeOutputBuf[1],err);
        OMX_CLOSE_PIPE(pComponentPrivate->cmdPipe[0],err);
        OMX_CLOSE_PIPE(pComponentPrivate->cmdPipe[1],err);
        OMX_CLOSE_PIPE(pComponentPrivate->cmdDataPipe[0],err);
        OMX_CLOSE_PIPE(pComponentPrivate->cmdDataPipe[1],err);
    }

    if (pComponentPrivate->bPortDefsAllocated) {
        OMX_MEMFREE_STRUCT(pComponentPrivate->pPortDef[INPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->pPortDef[OUTPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->aacParams[INPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->aacParams[OUTPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->pcmParam[INPUT_PORT]);
        OMX_MEMFREE_STRUCT(pComponentPrivate->pcmParam[OUTPUT_PORT]);
    }
    pComponentPrivate->bPortDefsAllocated = 0;

#ifndef UNDER_CE
    OMX_PRDSP1(pComponentPrivate->dbg, "\n\n FreeCompResources: Destroying mutexes.\n\n");
    pthread_mutex_destroy(&pComponentPrivate->InLoaded_mutex);
    pthread_cond_destroy(&pComponentPrivate->InLoaded_threshold);

    pthread_mutex_destroy(&pComponentPrivate->InIdle_mutex);
    pthread_cond_destroy(&pComponentPrivate->InIdle_threshold);

    pthread_mutex_destroy(&pComponentPrivate->AlloBuf_mutex);
    pthread_cond_destroy(&pComponentPrivate->AlloBuf_threshold);
#else
    pComponentPrivate->bPortDefsAllocated = 0;
    OMX_DestroyEvent(&(pComponentPrivate->InLoaded_event));
    OMX_DestroyEvent(&(pComponentPrivate->InIdle_event));
    OMX_DestroyEvent(&(pComponentPrivate->AlloBuf_event));
#endif

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Exiting AACENC_FreeCompResources()\n",__LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Returning = 0x%x\n",__LINE__,eError);

    return eError;
}

/* ========================================================================== */
/**
* @AACENC_CleanupInitParams() This function is called by the component during
* de-init to free structues that are been allocated at intialization stage
*
* @param pComponent  handle for this instance of the component
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */
OMX_ERRORTYPE AACENC_CleanupInitParams(OMX_HANDLETYPE pComponent)
{
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Entering AACENC_CleanupInitParams()\n", __LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Exiting Successfully AACENC_CleanupInitParams()\n",__LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Returning = 0x%x\n",__LINE__,eError);
    return eError;
}

/* ========================================================================== */
/**
* AACENC_StopComponentThread() This function is called by the component during
* de-init to close component thread, Command pipe, data pipe .
*
* @param pComponent  handle for this instance of the component
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */

OMX_ERRORTYPE AACENC_StopComponentThread(OMX_HANDLETYPE pComponent)
{
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = (AACENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_ERRORTYPE threadError = OMX_ErrorNone;
    int pthreadError = 0;

    OMX_PRINT1(pComponentPrivate->dbg, " %d :: UTIL: Entering AACENC_StopComponentThread\n",__LINE__);

    /*Join the component thread*/
    pComponentPrivate->bIsThreadstop = 1;
    write (pComponentPrivate->cmdPipe[1], &pComponentPrivate->bIsThreadstop, sizeof(OMX_U16));
    OMX_PRSTATE1(pComponentPrivate->dbg, "UTIL: pComponentPrivate->bIsThreadstop = %ld \n",pComponentPrivate->bIsThreadstop);
    pthreadError = pthread_join (pComponentPrivate->ComponentThread,(void*)&threadError);
    if (0 != pthreadError)
    {
        eError = OMX_ErrorHardware;
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error closing ComponentThread - pthreadError = %d\n",__LINE__,pthreadError);
        goto EXIT;
    }

    /*Check for the errors*/
    if (OMX_ErrorNone != threadError && OMX_ErrorNone != eError)
    {
        eError = OMX_ErrorInsufficientResources;
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while closing Component Thread\n",__LINE__);
        goto EXIT;

    }
EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, " %d :: UTIL: Exiting AACENC_StopComponentThread\n",__LINE__);
    return eError;
}


/* ========================================================================== */
/**
* AACENCHandleCommand() This function is called by the component when ever it
* receives the command from the application
*
* @param pComponentPrivate  Component private data
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */

OMX_U32 AACENCHandleCommand(AACENC_COMPONENT_PRIVATE *pComponentPrivate)
{

    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponentPrivate->pHandle;
    OMX_COMMANDTYPE command;
    OMX_STATETYPE commandedState;
    OMX_S32 commandData;
    OMX_ERRORTYPE rm_error = OMX_ErrorNone;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_S32 ret = 0;
    OMX_U32 i=0;
//    char *p  = "start";

    int inputPortFlag = 0;
    int outputPortFlag = 0;

    OMX_PRSTATE1(pComponentPrivate->dbg, "%d :: AACENC: Entering AACENCHandleCommand Function - curState = %d\n",__LINE__,pComponentPrivate->curState);
    ret = read (pComponentPrivate->cmdPipe[0], &command, sizeof (command));
    OMX_TRACE1(pComponentPrivate->dbg, "%d :: AACENC: Command pipe has been read = %ld \n",__LINE__,ret);
    if (ret == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Reading from the Data pipe\n", __LINE__);
        eError = OMX_ErrorHardware;
        goto EXIT;
    }
    ret = read (pComponentPrivate->cmdDataPipe[0], &commandData, sizeof (commandData));
    OMX_TRACE1(pComponentPrivate->dbg, "%d :: AACENC: Command data pipe has been read = %ld \n",__LINE__,ret);
    if (ret == -1)
    {
        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Reading from the Data pipe\n", __LINE__);
        eError = OMX_ErrorHardware;
        goto EXIT;
    }

#ifdef __PERF_INSTRUMENTATION__
    PERF_ReceivedCommand(pComponentPrivate->pPERFcomp,command,commandData,PERF_ModuleLLMM);
#endif

    OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENCHandleCommand :: Command is = %d\n",__LINE__,command);
    if (command == OMX_CommandStateSet)
    {
        commandedState = (OMX_STATETYPE)commandData;
        if ( pComponentPrivate->curState==commandedState)
        {
            pComponentPrivate->cbInfo.EventHandler(pHandle,
                pHandle->pApplicationPrivate,
                OMX_EventError,
                OMX_ErrorSameState,
                OMX_TI_ErrorMinor,
                NULL);
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Same State Given by Application\n",__LINE__);
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: AACENC: State Given is: %d \n", __LINE__, commandedState);
        }
        else
        {
            switch(commandedState)
            {
            case OMX_StateIdle:
                OMX_PRDSP2(pComponentPrivate->dbg, "%d: AACENCHandleCommand: Cmd Idle \n",__LINE__);
                OMX_PRDSP2(pComponentPrivate->dbg, "AACENC: curstate = %d\n",pComponentPrivate->curState);
                if (pComponentPrivate->curState == OMX_StateLoaded)
                {

#ifdef __PERF_INSTRUMENTATION__
                    PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryStart | PERF_BoundarySetup);
#endif

                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: OMX_StateLoaded [INPUT_PORT]->bPopulated  %d \n",__LINE__,pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated);
                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: OMX_StateLoaded [INPUT_PORT]->bEnabled    %d \n",__LINE__,pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled);
                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: OMX_StateLoaded [OUTPUT_PORT]->bPopulated %d \n",__LINE__,pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated);
                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: OMX_StateLoaded [OUTPUT_PORT]->bEnabled   %d \n",__LINE__,pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled);

                    if (pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated &&  pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled)  {
                        inputPortFlag = 1;
                    }

                    if (!pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated && !pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled) {
                        inputPortFlag = 1;
                    }

                    if (pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated && pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled) {
                        outputPortFlag = 1;
                    }

                    if (!pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated && !pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled) {
                        outputPortFlag = 1;
                    }

                    OMX_PRCOMM2(pComponentPrivate->dbg, "inputPortFlag = %d\n",inputPortFlag);
                    OMX_PRCOMM2(pComponentPrivate->dbg, "outputPortFlag = %d\n",outputPortFlag);
                    if (!(inputPortFlag && outputPortFlag))
                    {

                        pComponentPrivate->InLoaded_readytoidle = 1;

#ifndef UNDER_CE
                            pthread_mutex_lock(&pComponentPrivate->InLoaded_mutex);
                            pthread_cond_wait(&pComponentPrivate->InLoaded_threshold, &pComponentPrivate->InLoaded_mutex);
                            pthread_mutex_unlock(&pComponentPrivate->InLoaded_mutex);

#else
                            OMX_WaitForEvent(&(pComponentPrivate->InLoaded_event));
#endif
                    }

                    //Initialize params as per User settings
                    eError = AACENCFill_InitParams(pHandle);
                    if(eError != OMX_ErrorNone)
                    {
                        OMX_ERROR4(pComponentPrivate->dbg, "Error: Failed to initialize Init Params\n");
                        goto EXIT;
                    }

                    pComponentPrivate->bNewOutputBufRequired = 1;

                    OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: About to call ia_heaacv2_enc_ocp_init\n", __LINE__);

                    //Call Component Init here
                    eError = ia_heaacv2_enc_ocp_init(&pComponentPrivate->pHeaacv2EncHandle,&pComponentPrivate->ia_heaacv2_enc_params);
                    if(eError != OMX_ErrorNone)
                    {
                        OMX_ERROR4(pComponentPrivate->dbg, "Error: Failed to initialize Ittiam AAC Encoder\n");
                        goto EXIT;
                    }

                    //Allocate a temporary buffer for handling left-over input
                    OMX_MALLOC_STRUCT_SIZE( pComponentPrivate->pInternalInputBuffer, 10 * 8192, OMX_S8);
                    memset(pComponentPrivate->pInternalInputBuffer, 0, 10 * 8192);

                    pComponentPrivate->nInternalFilledLen = 0;


                    // Allocate a temporary buffer for interleaving non-interleave input
                    // Minimum input buffer - 1024 samples - stereo & Word16 = 4096
                    if( (pComponentPrivate->pcmParam[INPUT_PORT]->bInterleaved == 0) &&
                        (pComponentPrivate->ia_heaacv2_enc_params.noChannels == 2) )
                        {
                            OMX_MALLOC_STRUCT_SIZE( pComponentPrivate->pInternalInputBuffer_frame, 4096, OMX_S8);
                            memset(pComponentPrivate->pInternalInputBuffer_frame, 0, 4096);
                        }

                    /* need check the resource with RM */
                    OMX_PRINT2(pComponentPrivate->dbg, "%d :: AACENC: About to call RMProxy_SendCommand\n", __LINE__);
#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM

                    pComponentPrivate->rmproxyCallback.RMPROXY_Callback = (void *) AACENC_ResourceManagerCallback;
                    rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_RequestResource, OMX_AAC_Encoder_COMPONENT,AACENC_CPU_USAGE, 3456, &(pComponentPrivate->rmproxyCallback));

                    OMX_PRDSP1(pComponentPrivate->dbg, "%d :: AACENC: Returned from RMProxy_SendCommand\n", __LINE__);
                    OMX_PRDSP1(pComponentPrivate->dbg, "%d :: AACENC: RMProxy_SendCommand returned %d\n", __LINE__,rm_error);
                    if(rm_error == OMX_ErrorNone)
                    {
                        /* resource is available */
                        pComponentPrivate->curState = OMX_StateIdle;

#ifdef __PERF_INSTRUMENTATION__
                        PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundarySetup);
#endif
                        rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet, OMX_AAC_Encoder_COMPONENT, OMX_StateIdle, 3456, NULL);
                    }
                    else if(rm_error == OMX_ErrorInsufficientResources)
                    {
                        /* resource is not available, need set state to OMX_StateWaitForResources */
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Insufficient resources\n", __LINE__);
                        pComponentPrivate->curState = OMX_StateWaitForResources;
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventCmdComplete,
                            OMX_CommandStateSet,
                            pComponentPrivate->curState,
                            NULL);
                        OMX_ERROR2(pComponentPrivate->dbg, "%d :: AACENC: OMX_ErrorInsufficientResources\n", __LINE__);
                    }
                    pComponentPrivate->curState = OMX_StateIdle;
                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pComponentPrivate->curState,
                        NULL);
#else
                    pComponentPrivate->curState = OMX_StateIdle;
                    pComponentPrivate->cbInfo.EventHandler( pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pComponentPrivate->curState,
                        NULL);


#endif
                    }
                    else if (pComponentPrivate->curState == OMX_StateExecuting)
                    {

#ifdef __PERF_INSTRUMENTATION__
                        PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundarySteadyState);
#endif

                        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: Setting Component to OMX_StateIdle from OMX_StateExecuting\n",__LINE__);

                        pComponentPrivate->bIsStopping = 1;

            pComponentPrivate->nNumOutputBufPending=0;

            //Shall return all buffers:
            eError = ReturnInputOutputBuffers(pComponentPrivate);
            if (eError != OMX_ErrorNone)
                        {
                          OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Return I/O Buffers  %x\n",__LINE__, eError);
                          goto EXIT;
                        }

            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: After Returned all buffers\n", __LINE__);

            pComponentPrivate->curState = OMX_StateIdle; //is it correct ?
            pComponentPrivate->StateTransitiontoIdleFromExecOngoing = 0;
            pComponentPrivate->cbInfo.EventHandler( pHandle,
                    pHandle->pApplicationPrivate,
                    OMX_EventCmdComplete,
                    OMX_CommandStateSet,
                    pComponentPrivate->curState,
                    NULL);


                    }
                    else if(pComponentPrivate->curState == OMX_StatePause)
                    {

#ifdef __PERF_INSTRUMENTATION__
                        PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundarySteadyState);
#endif

            //Shall return all buffers:
            eError = ReturnInputOutputBuffers(pComponentPrivate);
                    if (eError != OMX_ErrorNone)
                        {
                          OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Return I/O Buffers  %x\n",__LINE__, eError);
                          goto EXIT;
                        }
                        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: After Returned all buffers\n", __LINE__);


                        pComponentPrivate->curState = OMX_StateIdle;

#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM

                        rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet, OMX_AAC_Encoder_COMPONENT, OMX_StateIdle, 3456, NULL);
#endif
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventCmdComplete,
                            OMX_CommandStateSet,
                            pComponentPrivate->curState,
                            NULL);
                    }
                    else
                    {
                        /* This means, it is invalid state from application */
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: AACENC: OMX_ErrorIncorrectStateTransition\n",__LINE__);
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventError,
                            OMX_ErrorIncorrectStateTransition,
                            OMX_TI_ErrorMinor,
                            "Invalid State Error");
                    }
                    break;


                case OMX_StateExecuting:
                    OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENCHandleCommand: Cmd Executing \n",__LINE__);
                    if(pComponentPrivate->curState == OMX_StateIdle)
                    {
                        pComponentPrivate->bIsStopping = 0;
                        OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d:: AACENCHandleCommand Here :\n",__LINE__);
                        // You can send some control commands to component before calling process here

                    }
                    else if (pComponentPrivate->curState == OMX_StatePause)
                    {
                        pComponentPrivate->nNumInputBufPending =0;
                        pComponentPrivate->nNumOutputBufPending = 0;
                    }
                    else
                    {
                        OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d:: AACENCHandleCommand Here :\n",__LINE__);
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventError,
                            OMX_ErrorIncorrectStateTransition,
                            OMX_TI_ErrorMinor,
                            "Invalid State Error");
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Invalid State Given by Application\n",__LINE__);
                        goto EXIT;
                    }

#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM
                    rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet, OMX_AAC_Encoder_COMPONENT, OMX_StateExecuting, 3456, NULL);

#endif

                    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d:: AACENCHandleCommand Here :\n",__LINE__);
                    pComponentPrivate->curState = OMX_StateExecuting; /* --- Transition to Executing --- */

#ifdef __PERF_INSTRUMENTATION__
                    PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryStart | PERF_BoundarySteadyState);
#endif

                    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d::Transitioned to OMX_StateExecuting , \n",__LINE__);
                    /*Send state change notificaiton to Application */
                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pComponentPrivate->curState,
                        NULL);
                    break;


                case OMX_StateLoaded:
                    OMX_PRSTATE1(pComponentPrivate->dbg, "%d :: AACENC: AACENCHandleCommand: Cmd Loaded - curState = %d\n",__LINE__,pComponentPrivate->curState);
                    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pInputBufferList->numBuffers = %d \n",__LINE__,pComponentPrivate->pInputBufferList->numBuffers);
                    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: AACENC: pComponentPrivate->pOutputBufferList->numBuffers = %d \n",__LINE__,pComponentPrivate->pOutputBufferList->numBuffers);
                    if (pComponentPrivate->curState == OMX_StateWaitForResources)
                    {
                        OMX_PRDSP1(pComponentPrivate->dbg, "%d :: AACENC: AACENCHandleCommand: Cmd Loaded\n",__LINE__);

#ifdef __PERF_INSTRUMENTATION__
                        PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryStart | PERF_BoundaryCleanup);
#endif
                        pComponentPrivate->curState = OMX_StateLoaded;

#ifdef __PERF_INSTRUMENTATION__
                        PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundaryCleanup);
#endif

                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventCmdComplete,
                            OMX_CommandStateSet,
                            pComponentPrivate->curState,
                            NULL);
                        pComponentPrivate->bLoadedCommandPending = OMX_FALSE;
                        break;
                    }
                    OMX_PRSTATE1(pComponentPrivate->dbg, "%d :: AACENC: Inside OMX_StateLoaded State: \n",__LINE__);
                    if (pComponentPrivate->curState != OMX_StateIdle &&
                        pComponentPrivate->curState != OMX_StateWaitForResources)
                    {
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventError,
                            OMX_ErrorIncorrectStateTransition,
                            OMX_TI_ErrorMinor,
                            "Incorrect State Transition");
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Invalid State Given by Application\n",__LINE__);
                        goto EXIT;
                    }

#ifdef __PERF_INSTRUMENTATION__
                    PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryStart | PERF_BoundaryCleanup);
#endif
                    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pComponentPrivate->pInputBufferList->numBuffers = %d \n",pComponentPrivate->pInputBufferList->numBuffers);
                    OMX_PRBUFFER2(pComponentPrivate->dbg, "AACENC: pComponentPrivate->pOutputBufferList->numBuffers = %d \n",pComponentPrivate->pOutputBufferList->numBuffers);

                    if (pComponentPrivate->pInputBufferList->numBuffers ||
                        pComponentPrivate->pOutputBufferList->numBuffers)
                    {
                        pComponentPrivate->InIdle_goingtoloaded = 1;
#ifndef UNDER_CE
                            pthread_mutex_lock(&pComponentPrivate->InIdle_mutex);
                            pthread_cond_wait(&pComponentPrivate->InIdle_threshold, &pComponentPrivate->InIdle_mutex);
                            pthread_mutex_unlock(&pComponentPrivate->InIdle_mutex);

#else
                            OMX_WaitForEvent(&(pComponentPrivate->InIdle_event));
#endif

                    }

                    /* Now Deinitialize the component No error should be returned from this function. It should clean the system as much as possible */
                    OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: Before CodecControlDestroy \n",__LINE__);
                       if ( NULL !=  pComponentPrivate->pInternalInputBuffer)
                         OMX_MEMFREE_STRUCT( pComponentPrivate->pInternalInputBuffer);

                    // free temp buffers used for interleaving
                    if( (pComponentPrivate->pcmParam[INPUT_PORT]->bInterleaved == 0) &&
                        (pComponentPrivate->ia_heaacv2_enc_params.noChannels == 2) )
                        {
                           if ( NULL !=  pComponentPrivate->pInternalInputBuffer_frame)
                             OMX_MEMFREE_STRUCT( pComponentPrivate->pInternalInputBuffer_frame);
                        }

                    /* Destroy Component */
                    eError = ia_heaacv2_enc_ocp_deinit(pComponentPrivate->pHeaacv2EncHandle);


#ifdef __PERF_INSTRUMENTATION__
                    PERF_SendingCommand(pComponentPrivate->pPERF, -1, 0, PERF_ModuleComponent);
#endif

                    OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: After CodecControlDestroy \n",__LINE__);
                    if (eError != OMX_ErrorNone)
                    {

                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: ia_heaacv2_enc_ocp_deinit : no.  %x\n",__LINE__, eError);
                        goto EXIT;
                    }
                    OMX_PRDSP1(pComponentPrivate->dbg, "%d :: AACENCHandleCommand: Cmd Loaded\n",__LINE__);
#ifndef UNDER_CE
                    pComponentPrivate->bCodecDestroyed = OMX_TRUE;

#endif
                    OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: After CodecControlDestroy \n",__LINE__);
                    OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENCHandleCommand: Cmd Loaded\n",__LINE__);
                    eError = EXIT_COMPONENT_THRD;
                    /* Send StateChangeNotification to application */

                    pComponentPrivate->curState = OMX_StateLoaded;
                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pComponentPrivate->curState,
                        NULL);

                    break;


                case OMX_StatePause:
                    if (pComponentPrivate->curState != OMX_StateExecuting &&
                        pComponentPrivate->curState != OMX_StateIdle)
                    {
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventError,
                            OMX_ErrorIncorrectStateTransition,
                            OMX_TI_ErrorMinor,
                            "Incorrect State Transition");
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: Invalid State Given by Application\n",__LINE__);
                        goto EXIT;
                    }

                    pComponentPrivate->curState = OMX_StatePause;

                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pComponentPrivate->curState,
                        NULL);

#ifdef __PERF_INSTRUMENTATION__
                    PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundarySteadyState);
#endif
                    break;


                case OMX_StateWaitForResources:
                    if (pComponentPrivate->curState == OMX_StateLoaded)
                    {

#ifdef RESOURCE_MANAGER_ENABLED_ITTIAM
                        rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet, OMX_AAC_Encoder_COMPONENT, OMX_StateWaitForResources, 3456, NULL);
#endif

                        pComponentPrivate->curState = OMX_StateWaitForResources;
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventCmdComplete,
                            OMX_CommandStateSet,
                            pComponentPrivate->curState,
                            NULL);
                    }
                    else
                    {
                        pComponentPrivate->cbInfo.EventHandler(pHandle,
                            pHandle->pApplicationPrivate,
                            OMX_EventError,
                            OMX_ErrorIncorrectStateTransition,
                            OMX_TI_ErrorMinor,
                            "Incorrect State Transition");
                    }
                    break;


                case OMX_StateInvalid:
                    OMX_PRSTATE2(pComponentPrivate->dbg, "%d: HandleCommand: Cmd OMX_StateInvalid:\n",__LINE__);
                    if (pComponentPrivate->curState != OMX_StateWaitForResources &&
                        pComponentPrivate->curState != OMX_StateInvalid &&
                        pComponentPrivate->curState != OMX_StateLoaded)
                    {
                        eError = ia_heaacv2_enc_ocp_deinit(pComponentPrivate->pHeaacv2EncHandle);
                        if( eError != OMX_ErrorNone )
                        {
                                                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error: in ia_heaacv2_enc_ocp_deinit \n",__LINE__);
                        }
                    }

                    pComponentPrivate->curState = OMX_StateInvalid;
                    pComponentPrivate->cbInfo.EventHandler( pHandle,
                        pHandle->pApplicationPrivate,
                        OMX_EventError,
                        OMX_ErrorInvalidState,
                        OMX_TI_ErrorSevere,
                        NULL);

                    AACENC_CleanupInitParams(pHandle);
                    break;

                case OMX_StateMax:
                    OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENCHandleCommand: Cmd OMX_StateMax::\n",__LINE__);
                    break;
            } /* End of Switch */
        }
    }


    else if (command == OMX_CommandMarkBuffer)
    {
        OMX_PRDSP2(pComponentPrivate->dbg, "AACENC: command OMX_CommandMarkBuffer received %d\n",__LINE__);
        if(!pComponentPrivate->pMarkBuf)
        {
            OMX_PRDSP1(pComponentPrivate->dbg, "AACENC: command OMX_CommandMarkBuffer received %d\n",__LINE__);
            /* TODO Need to handle multiple marks */
            pComponentPrivate->pMarkBuf = (OMX_MARKTYPE *)(commandData);
        }
    }
    else if (command == OMX_CommandPortDisable)
    {
        if (!pComponentPrivate->bDisableCommandPending)
        {
            if(commandData == 0x0 || commandData == -1)
            {
                /* disable port */
                pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled = OMX_FALSE;
            }
            if(commandData == 0x1 || commandData == -1)
            {

                pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled = OMX_FALSE;

                if (pComponentPrivate->curState == OMX_StateExecuting)
                {
                    pComponentPrivate->bNoIdleOnStop = OMX_TRUE;
                    OMX_PRINT2(pComponentPrivate->dbg, "AACENC: About to stop socket node line %d\n",__LINE__);

                        pComponentPrivate->bIsStopping = 1;

                }
            }
        }

        if(commandData == 0x0)
        {
            if(!pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated)
            {
                /* return cmdcomplete event if input unpopulated */
                pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                        pHandle->pApplicationPrivate,
                                                        OMX_EventCmdComplete,
                                                        OMX_CommandPortDisable,INPUT_PORT, NULL);
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
            if (!pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated)
            {
                /* return cmdcomplete event if output unpopulated */
                pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                        pHandle->pApplicationPrivate,
                                                        OMX_EventCmdComplete,
                                                        OMX_CommandPortDisable,OUTPUT_PORT, NULL);
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
            if (!pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated &&
                !pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated)
            {

                /* return cmdcomplete event if input & output unpopulated */
                pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                        pHandle->pApplicationPrivate,
                                                        OMX_EventCmdComplete,
                                                        OMX_CommandPortDisable,INPUT_PORT, NULL);

                pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                        pHandle->pApplicationPrivate,
                                                        OMX_EventCmdComplete,
                                                        OMX_CommandPortDisable,OUTPUT_PORT, NULL);
                pComponentPrivate->bDisableCommandPending = 0;
            }
            else
            {
                pComponentPrivate->bDisableCommandPending = 1;
                pComponentPrivate->bDisableCommandParam = commandData;
            }
        }
    }

    else if (command == OMX_CommandPortEnable)
    {
        if (!pComponentPrivate->bEnableCommandPending)
        {
            if(commandData == 0x0 || commandData == -1)
            {
                /* enable in port */
                OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: setting input port to enabled\n");
                pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled = OMX_TRUE;
                OMX_PRCOMM2(pComponentPrivate->dbg, "WAKE UP!! HandleCommand: In utils setting output port to enabled. \n");
                if(pComponentPrivate->AlloBuf_waitingsignal)
                {
                     pComponentPrivate->AlloBuf_waitingsignal = 0;
                }
                OMX_PRCOMM2(pComponentPrivate->dbg, "AACENC: pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled = %d\n",pComponentPrivate->pPortDef[INPUT_PORT]->bEnabled);

            }
            if(commandData == 0x1 || commandData == -1)
            {
                /* enable out port */
                if(pComponentPrivate->AlloBuf_waitingsignal)
                {
                     pComponentPrivate->AlloBuf_waitingsignal = 0;
#ifndef UNDER_CE
                     pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
                     pthread_cond_signal(&pComponentPrivate->AlloBuf_threshold);
                     pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);
#else
                     OMX_SignalEvent(&(pComponentPrivate->AlloBuf_event));
#endif
                }
                if (pComponentPrivate->curState == OMX_StateExecuting)
                {

                    pComponentPrivate->bDspStoppedWhileExecuting = OMX_FALSE;

                    OMX_PRCOMM1(pComponentPrivate->dbg, "AACENC: About to start socket node line %d\n",__LINE__);


        }
                OMX_PRCOMM1(pComponentPrivate->dbg, "AACENC: setting output port to enabled\n");
                pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled = OMX_TRUE;
                OMX_PRCOMM1(pComponentPrivate->dbg, "AACENC: pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled = %d\n",pComponentPrivate->pPortDef[OUTPUT_PORT]->bEnabled);
            }
        }

        if(commandData == 0x0)
        {
            if (pComponentPrivate->curState == OMX_StateLoaded || pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated)
            {
                pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                       pHandle->pApplicationPrivate,
                                                       OMX_EventCmdComplete,
                                                       OMX_CommandPortEnable,
                                                       INPUT_PORT,
                                                       NULL);
                pComponentPrivate->bEnableCommandPending = 0;
            }
            else
            {
                pComponentPrivate->bEnableCommandPending = 1;
                pComponentPrivate->nEnableCommandParam = commandData;
            }
        }

        else if(commandData == 0x1)
        {
            if (pComponentPrivate->curState == OMX_StateLoaded || pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated)
            {
                pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                        pHandle->pApplicationPrivate,
                                                        OMX_EventCmdComplete,
                                                        OMX_CommandPortEnable,
                                                        OUTPUT_PORT,
                                                        NULL);
                pComponentPrivate->bEnableCommandPending = 0;
            }
            else
            {
                pComponentPrivate->bEnableCommandPending = 1;
                pComponentPrivate->nEnableCommandParam = commandData;
            }
        }

        else if(commandData == -1)
        {
            if (pComponentPrivate->curState == OMX_StateLoaded ||
                (pComponentPrivate->pPortDef[INPUT_PORT]->bPopulated
                && pComponentPrivate->pPortDef[OUTPUT_PORT]->bPopulated))
            {
                pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                       pHandle->pApplicationPrivate,
                                                       OMX_EventCmdComplete,
                                                       OMX_CommandPortEnable,
                                                       INPUT_PORT,
                                                       NULL);
                pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                       pHandle->pApplicationPrivate,
                                                       OMX_EventCmdComplete,
                                                       OMX_CommandPortEnable,
                                                       OUTPUT_PORT,
                                                       NULL);
                pComponentPrivate->bEnableCommandPending = 0;
                AACENCFill_InitParamsEx(pHandle);
                OMX_PRDSP1(pComponentPrivate->dbg, "\nAACENC: calling fillexparams \n");
            }
            else
            {
                pComponentPrivate->bEnableCommandPending = 1;
                pComponentPrivate->nEnableCommandParam = commandData;
            }
        }

#ifndef UNDER_CE
                     pthread_mutex_lock(&pComponentPrivate->AlloBuf_mutex);
                     pthread_cond_signal(&pComponentPrivate->AlloBuf_threshold);
                     pthread_mutex_unlock(&pComponentPrivate->AlloBuf_mutex);
#else
                     OMX_SignalEvent(&(pComponentPrivate->AlloBuf_event));
#endif

    }


    else if (command == OMX_CommandFlush)
    {



        if (pComponentPrivate->nNumInputBufPending)
        {

            OMX_PRDSP2(pComponentPrivate->dbg, "%d :: AACENC: Inside OMX_CommandFlush Command \n",__LINE__);
            if(commandData == 0x0 || commandData == -1)
            {
                pComponentPrivate->nInternalFilledLen = 0;
                if (pComponentPrivate->nUnhandledEmptyThisBuffers == 0)  {
                    pComponentPrivate->bFlushInputPortCommandPending = OMX_FALSE;
                    pComponentPrivate->nOutStandingFillDones = 0;
                    OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: AACENCHandleCommand::Flushing input port \n",__LINE__);

                    OMX_PRINT2(pComponentPrivate->dbg, "%d :: Calling OCP_FLUSH on Ittiam AAC Encoder \n",__LINE__);
                    eError = ia_heaacv2_enc_ocp_control ( pComponentPrivate->pHeaacv2EncHandle, OCP_FLUSH, NULL);
                    if (eError != OMX_ErrorNone)
                        {
                             goto EXIT;
                        }
                }
                else {
                    pComponentPrivate->bFlushInputPortCommandPending = OMX_TRUE;
                }
            }
        }
        else
        {
             OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: UTIL: Flushing input port \n",__LINE__);
             pComponentPrivate->nOutStandingEmptyDones = 0;
             for (i=0; i < MAX_NUM_OF_BUFS; i++)
             {
                pComponentPrivate->pInputBufHdrPending[i] = NULL;
             }
             pComponentPrivate->nNumInputBufPending=0;
             for (i=0; i < pComponentPrivate->pInputBufferList->numBuffers; i++)
             {

                pComponentPrivate->cbInfo.EmptyBufferDone (
                               pComponentPrivate->pHandle,
                               pComponentPrivate->pHandle->pApplicationPrivate,
                               pComponentPrivate->pInputBufferList->pBufHdr[i]
                               );
             }
             pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventCmdComplete,
                                                    OMX_CommandFlush,INPUT_PORT, NULL);

        }



        if (pComponentPrivate->nNumOutputBufPending)
        {
            if(commandData == 0x1 || commandData == -1)
            {
                if (pComponentPrivate->nUnhandledFillThisBuffers == 0)  {
                    pComponentPrivate->bFlushOutputPortCommandPending = OMX_FALSE;
                    pComponentPrivate->nOutStandingEmptyDones=0;
                    OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: AACENCHandleCommand::Flushing ouput queue \n",__LINE__);
                    OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: AACENCHandleCommand::Flushing ouput port  \n",__LINE__);

//Flush all Buffers
                 if (eError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
            }
                else {
                    pComponentPrivate->bFlushOutputPortCommandPending = OMX_TRUE;
                }
            }
        }
        else
        {
            OMX_PRCOMM1(pComponentPrivate->dbg, "%d :: UTIL: Flushing output port \n",__LINE__);
            pComponentPrivate->nOutStandingFillDones = 0;
            for (i=0; i < MAX_NUM_OF_BUFS; i++)
            {
                pComponentPrivate->pOutputBufHdrPending[i] = NULL;
            }
            pComponentPrivate->nNumOutputBufPending=0;

            ReturnOutputBuffers(pComponentPrivate);

            pComponentPrivate->cbInfo.EventHandler( pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventCmdComplete,
                                                    OMX_CommandFlush,OUTPUT_PORT, NULL);
        }

    }

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Exiting AACENCHandleCommand Function\n",__LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENC: Returning %d\n",__LINE__,eError);
    return eError;
}


/* ========================================================================== */
/**
* AACENCHandleDataBuf_FromApp() This function is called by the component when ever it
* receives the buffer from the application
*
* @param pComponentPrivate  Component private data
* @param pBufHeader Buffer from the application
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */
OMX_ERRORTYPE AACENCHandleDataBuf_FromApp(AACENC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
//    OMX_DIRTYPE eDir = 0;
//    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_S32   bytesConsumed = 0;
    OMX_BUFFERHEADERTYPE* pInputBufHeader;
    OMX_BUFFERHEADERTYPE* pOutputBufHeader;
//    OMX_U8* pInputBuffer = pComponentPrivate->pInternalInputBuffer;
    OMX_S32 ret = 0;
    OMX_S32 status = 0;
    OMX_S32 extended_err_code = 0;
    OMX_S32 min_frame_size;
    OMX_S32 XDM_FATALERROR = 15;
//--[[ GB Patch START : junghyun.you@lge.com [2012.05.31]
    // Tushar - [ - For calculating o/p buffer timestamp instead of copying i/p buffer timestamp
    OMX_TICKS bufferDuration =0;
    // Tushar - ] - For calculating o/p buffer timestamp instead of copying i/p buffer timestamp
//--]] GB Patch END
    if(pComponentPrivate->ia_heaacv2_enc_params.aacClassic==1)
        min_frame_size = 1024;
    else
        min_frame_size = 2048;

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL: Entering AACENCHandleDataBuf_FromApp - curState = %d\n",__LINE__,pComponentPrivate->curState);


    pInputBufHeader = NULL;
    pOutputBufHeader = NULL;

    if( !(pComponentPrivate->bFirstOutputBuffer))
    {
        // In case of first input buffer, we dont consume any input,
        // just give config header as output
        // Read new input only
        if(pComponentPrivate->nInternalFilledLen < ((OMX_S32)(min_frame_size * sizeof(OMX_S16) * pComponentPrivate->ia_heaacv2_enc_params.noChannels)) \
                                                    && (pComponentPrivate->EOSfromApp == 0))
        {
            ret = read(pComponentPrivate->dataPipeInputBuf[0], &pInputBufHeader, sizeof(pInputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Input Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: pInputBufHeader:%p \n",__LINE__, pInputBufHeader);
            OMX_PRCOMM2(pComponentPrivate->dbg, "InputBytes:%d \n",pInputBufHeader->nFilledLen);
                pComponentPrivate->nLatestTimeStamp = pInputBufHeader->nTimeStamp;
            pComponentPrivate->samplesEncoded = 0;

            if (pInputBufHeader->nFlags & OMX_BUFFERFLAG_EOS)
            {
                //Just return Input Buffer
                OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: EOS came from the Application !!\n",__LINE__);
                //pInputBufHeader->nFilledLen = 0;
            }

            memcpy(pComponentPrivate->pInternalInputBuffer+pComponentPrivate->nInternalFilledLen, pInputBufHeader->pBuffer , pInputBufHeader->nFilledLen);
            pComponentPrivate->nInternalFilledLen += pInputBufHeader->nFilledLen;
            pInputBufHeader->nFilledLen = 0;

            if(pInputBufHeader != NULL)
            {
                    /* Store time stamp information */
                    pComponentPrivate->timestampBufIndex[pComponentPrivate->IpBufindex] = pInputBufHeader->nTimeStamp;
                    pComponentPrivate->tickcountBufIndex[pComponentPrivate->IpBufindex] = pInputBufHeader->nTickCount;
                    pComponentPrivate->IpBufindex++;
                    pComponentPrivate->IpBufindex %= pComponentPrivate->pPortDef[OMX_DirOutput]->nBufferCountActual;
            }
        }
    }

    if(pComponentPrivate->bNewOutputBufRequired)
    {
        /* Dequeue an Output Buffer */
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Read op buffer from pipe \n",__LINE__);
        ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
        if (ret == -1)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
            eError = OMX_ErrorHardware;
            goto EXIT;
        }
        pComponentPrivate->pOutputBufHeader = pOutputBufHeader;
        OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: pOutputBufHeader:%p \n",__LINE__, pOutputBufHeader);
        pComponentPrivate->bNewOutputBufRequired = 0;
        pOutputBufHeader->nFilledLen = 0;
    }
    else
    {
        pOutputBufHeader = pComponentPrivate->pOutputBufHeader;
        OMX_ERROR4(pComponentPrivate->dbg,"----pOutputBufHeader --- :%x \n",(pOutputBufHeader));
    }



    {
    /* Make sure that output buffer is issued to output stream only when
    * there is an outstanding input buffer already issued on input stream
        */
        OMX_PRBUFFER1(pComponentPrivate->dbg, "%d :: UTIL: Buffer Dir = output\n",__LINE__);
        OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL: pComponentPrivate->bIsStopping = %ld\n",__LINE__, pComponentPrivate->bIsStopping);
        OMX_PRDSP1(pComponentPrivate->dbg, "%d :: UTIL: pComponentPrivate->bBypassDSP  = %ld\n",__LINE__, pComponentPrivate->bBypassDSP);
        OMX_PRSTATE1(pComponentPrivate->dbg, "%d :: UTIL: pComponentPrivate->curState = %d\n",__LINE__,pComponentPrivate->curState);

        pComponentPrivate->nUnhandledFillThisBuffers--;
        pComponentPrivate->nUnhandledEmptyThisBuffers--;

#ifdef ANDROID
        if (pComponentPrivate->bFirstOutputBuffer){
            // if this is the first output buffer, fill the config data, then return the buffer (skip DSP)
            AACENCWriteConfigHeader(pComponentPrivate, pOutputBufHeader);
            OMX_PRINT2(pComponentPrivate->dbg, "%d :: UTIL: AACENCWriteConfigHeader = %p\n",__LINE__, pOutputBufHeader->pBuffer);
            OMX_PRINT2(pComponentPrivate->dbg, "%d :: UTIL: AACENCWriteConfigHeader wrote = %d bytes\n",__LINE__, pOutputBufHeader->nFilledLen);
            pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pOutputBufHeader
                );
            pComponentPrivate->bNewOutputBufRequired = 1;
            pComponentPrivate->pOutputBufHeader = NULL;
            pComponentPrivate->bFirstOutputBuffer = 0;
            goto EXIT;
        }
#endif

        if (!(pComponentPrivate->bIsStopping))
        {

            OMX_PRBUFFER1(pComponentPrivate->dbg, "%d :: UTIL: Sending OUTPUT BUFFER to Codec = %p\n",__LINE__,pOutputBufHeader->pBuffer);
#ifdef __PERF_INSTRUMENTATION__
            PERF_SendingFrame(pComponentPrivate->pPERFcomp,
                PREF(pOutputBufHeader,pBuffer),
                0,
                PERF_ModuleCommonLayer);
#endif

            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: UTIL: pComponentPrivate = %p\n",__LINE__,pComponentPrivate);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: UTIL: pOutputBufHeader = %p\n",__LINE__,pOutputBufHeader);
            OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: UTIL: pOutputBufHeader->pBuffer = %p\n",__LINE__,pOutputBufHeader->pBuffer);
            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: UTIL: pComponentPrivate->curState = %d\n",__LINE__,pComponentPrivate->curState);
            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: [HandleData_FromApp]pComponentPrivate->curState = %d\n",__LINE__,pComponentPrivate->curState);

            if (pComponentPrivate->curState == OMX_StateExecuting)
            {
                OMX_PRCOMM2(pComponentPrivate->dbg, "InputChannels:%d \n",pComponentPrivate->ia_heaacv2_enc_params.noChannels);

                pComponentPrivate->ia_heaacv2_enc_input_args.numInSamples = ((pComponentPrivate->nInternalFilledLen/sizeof(OMX_S16))/pComponentPrivate->ia_heaacv2_enc_params.noChannels);
                pComponentPrivate->ia_heaacv2_enc_output_args.i_ittiam_err_code = 0;

                OMX_PRCOMM2(pComponentPrivate->dbg, "numInSamples:%d \n",pComponentPrivate->ia_heaacv2_enc_input_args.numInSamples);

            //  if (pComponentPrivate->nInternalFilledLen >= (min_frame_size * sizeof(OMX_S16) * pComponentPrivate->ia_heaacv2_enc_params.noChannels) )
                {
                    OMX_S8 *input_buffer;
                    OMX_S32 input_buf_len;

                    // Handling Interleave and non-interleave mode

                    OMX_PRCOMM2(pComponentPrivate->dbg,"INPUT_PORT Interleave mode: %d\n",(pComponentPrivate->pcmParam[INPUT_PORT]->bInterleaved));
                    // OMX_PRCOMM2(pComponentPrivate->dbg,"INPUT_PORT nChannels: %d\n",(pComponentPrivate->pcmParam[INPUT_PORT]->nChannels));

                    if( (pComponentPrivate->pcmParam[INPUT_PORT]->bInterleaved == 0) &&
                        (pComponentPrivate->ia_heaacv2_enc_params.noChannels == 2) ) // Non-interleave mode
                    {
                        OMX_S32 i = 0;
                        // OMX_S32 index = 0 ;//pComponentPrivate->nInternalindex;
                        OMX_S32 offset = 1024; // 4096 by 2 for offset; by 2 for OMX_S16
                        OMX_S16 *small_inbuf = (OMX_S16 *)pComponentPrivate->pInternalInputBuffer_frame;
                        OMX_S16 *big_inbuf = (OMX_S16 *)pComponentPrivate->pInternalInputBuffer;

                        OMX_PRCOMM2(pComponentPrivate->dbg,"\n NON-interleave mode\n");

                        for (i=0; i<1024; i++)
                        {
                            small_inbuf[(i<<1)  ] = big_inbuf[i ];        // Left channel
                            small_inbuf[(i<<1)+1] = big_inbuf[i + offset]; // Right channel
                        }
                        //pComponentPrivate->nInternalindex += 1024;    // min samples per frame per channel

                        input_buffer = pComponentPrivate->pInternalInputBuffer_frame;
                        input_buf_len = 4096; // in bytes for stereo of 1024 samples per channel
                    }
                    else
                    {
                        input_buffer = pComponentPrivate->pInternalInputBuffer;
                        input_buf_len = pComponentPrivate->nInternalFilledLen;

                        OMX_PRCOMM2(pComponentPrivate->dbg, "\n Interleave mode\n");
                    }


                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Calling ia_heaacv2_enc_ocp_process\n",__LINE__);
                    //printf("Frame No. : [%d]\r", pComponentPrivate->i_frame_count);

                    pOutputBufHeader->nFlags = 0;

					if( pComponentPrivate->ia_heaacv2_enc_input_args.numInSamples >= min_frame_size)
					{
                        status = ia_heaacv2_enc_ocp_process(pComponentPrivate->pHeaacv2EncHandle,
                                        &pComponentPrivate->ia_heaacv2_enc_input_args,
                                        input_buffer,
                                        input_buf_len,
                                        &pComponentPrivate->ia_heaacv2_enc_output_args,
                                        pOutputBufHeader->pBuffer + pOutputBufHeader->nFilledLen,
                                        pOutputBufHeader->nAllocLen);
					}
					else
					{
						OMX_PRCOMM2(pComponentPrivate->dbg,"%s:%d Skipping process because of zero size buffer", __FUNCTION__, __LINE__);
						status = 0;
						pComponentPrivate->ia_heaacv2_enc_output_args.numInSamples = 0;
						pComponentPrivate->ia_heaacv2_enc_output_args.bytesGenerated = 0;
					}

                    if(0 != status)
                    {
                        OMX_ERROR4(pComponentPrivate->dbg, " %d :: Error in ocp_process :%d !!! \n",__LINE__, status);
                    }

                    if( status != OCP_EOK)
                    {
                        extended_err_code = pComponentPrivate->ia_heaacv2_enc_output_args.i_ittiam_err_code;

                        OMX_ERROR4(pComponentPrivate->dbg, "OCP Process Returned Status: 0x%x\n",extended_err_code);

                        // We need to signal to the application that error occured ?
                        if( ((extended_err_code >> XDM_FATALERROR) & 0x1) ) //only in case of fatal errors
                        {
                            pComponentPrivate->cbInfo.EventHandler ( pComponentPrivate->pHandle,
                                                         pComponentPrivate->pHandle->pApplicationPrivate,
                                                         OMX_EventError,
                                                         extended_err_code,
                                                         0,
                                                         NULL);
                        }
                    }

                    bytesConsumed    =  (pComponentPrivate->ia_heaacv2_enc_output_args.numInSamples * pComponentPrivate->ia_heaacv2_enc_params.noChannels * sizeof(OMX_S16));

                    pComponentPrivate->samplesEncoded += pComponentPrivate->ia_heaacv2_enc_output_args.numInSamples;

                    if(pComponentPrivate->framemode == 0)
                    {
                        pOutputBufHeader->nFilledLen += pComponentPrivate->ia_heaacv2_enc_output_args.bytesGenerated;
                    }
                    else
                    {
                        pOutputBufHeader->nFilledLen = pComponentPrivate->ia_heaacv2_enc_output_args.bytesGenerated;
                    }

                    pComponentPrivate->i_frame_count++;
                    /* Timestamp offset assigned assuming microseconds units for timestamp */
//--[[ GB Patch START : junghyun.you@lge.com [2012.05.31]
#if 0 // org ittiam code
                    pOutputBufHeader->nTimeStamp =  pComponentPrivate->nLatestTimeStamp +
                        (OMX_S64)((pComponentPrivate->samplesEncoded * 1000000) / pComponentPrivate->ia_heaacv2_enc_params.sampleRate);
#else
                    pOutputBufHeader->nTimeStamp =  pComponentPrivate->temp_TS;
                    // FIX ME: (1) should be replaced with # of frames encoded in one o/p buffer
                    bufferDuration = (OMX_TICKS)((pComponentPrivate->samplesEncoded * 1000000) /
                                              pComponentPrivate->ia_heaacv2_enc_params.sampleRate);
                    /* Update time stamp information */
                    pComponentPrivate->temp_TS += bufferDuration;
#endif               
                    // Tushar - ] - For calculating o/p buffer timestamp instead of copying i/p buffer timestamp
//--]] GB Patch END
                    OMX_PRCOMM2(pComponentPrivate->dbg, "-----------FrameCount:%d----------- \n", pComponentPrivate->i_frame_count);
                    OMX_PRCOMM2(pComponentPrivate->dbg, "GeneratedBytes:%d \n", pComponentPrivate->ia_heaacv2_enc_output_args.bytesGenerated);
                    OMX_PRCOMM2(pComponentPrivate->dbg, "BytesConsumed:%d \n", bytesConsumed);


                    input_buf_len -= bytesConsumed;

                    if(input_buf_len < 0 )
                        input_buf_len = 0;

                    if( (pComponentPrivate->pcmParam[INPUT_PORT]->bInterleaved == 0) &&
                        (pComponentPrivate->ia_heaacv2_enc_params.noChannels == 2) ) // Non-interleave mode
                    {
                        pComponentPrivate->nInternalFilledLen -= 4096; // bytes = 1024 samples (Word16) stereo

                    //  pComponentPrivate->pInternalInputBuffer += 4096; // bytes = 1024 samples (Word16) stereo
                    }
                    else
                    {
                        pComponentPrivate->nInternalFilledLen = input_buf_len;
                    }

                    OMX_PRCOMM2(pComponentPrivate->dbg, "Calling memmove with Internal Filled Len:%d \n", pComponentPrivate->nInternalFilledLen);

                    memmove(pComponentPrivate->pInternalInputBuffer,
                            pComponentPrivate->pInternalInputBuffer + bytesConsumed,
                            pComponentPrivate->nInternalFilledLen);

                    OMX_PRCOMM2(pComponentPrivate->dbg, "Input Buffer Filled Len Remaining:%d \n", pComponentPrivate->nInternalFilledLen);

                }
            /*  else
                {
                    if(pComponentPrivate->EOSfromApp == 1)
                        pComponentPrivate->nInternalFilledLen = 0;
                }   */


                if( NULL != pInputBufHeader )
                {

                    if(pInputBufHeader->nFlags & OMX_BUFFERFLAG_EOS)
                    {
                         //Dump the input held internally
                         //pComponentPrivate->nInternalFilledLen = 0;

                        pComponentPrivate->EOSfromApp = 1;

                        //adding this to resolve issues with test4 sonal
                        pInputBufHeader->nFlags = 0;

                        OMX_ERROR4(pComponentPrivate->dbg, "Return Input Buffer\n");

                        pComponentPrivate->cbInfo.EmptyBufferDone (
                            pComponentPrivate->pHandle,
                            pComponentPrivate->pHandle->pApplicationPrivate,
                            pInputBufHeader
                            );

/*
                        {
                            pOutputBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;

                            pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                pComponentPrivate->pHandle->pApplicationPrivate,
                                OMX_EventBufferFlag,
                                OUTPUT_PORT,
                                OMX_BUFFERFLAG_EOS, //pOutputBufHeader->nFlags,
                                //pComponentPrivate->pOutputBufferList->pBufHdr[0]->nOutputPortIndex,
                                //pComponentPrivate->pOutputBufferList->pBufHdr[0]->nFlags,
                                NULL);

                        }*/

                        //pOutputBufHeader->nFlags = 0;
                    }
                    else if(pInputBufHeader->nFilledLen == 0)
                    {

                        OMX_ERROR1(pComponentPrivate->dbg, "Return Input Buffer\n");
                        pComponentPrivate->cbInfo.EmptyBufferDone (
                            pComponentPrivate->pHandle,
                            pComponentPrivate->pHandle->pApplicationPrivate,
                            pInputBufHeader
                            );
                    }
                }

                  if((pComponentPrivate->nInternalFilledLen == 0) && (pComponentPrivate->EOSfromApp == 1))
                  {

                     OMX_PRINT2(pComponentPrivate->dbg, "%d :: UTIL: Component Detected EOS\n",__LINE__);

                      pOutputBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;

                      pComponentPrivate->EOSfromApp++;

                      pComponentPrivate->cbInfo.EventHandler(pComponentPrivate->pHandle,
                                             pComponentPrivate->pHandle->pApplicationPrivate,
                                             OMX_EventBufferFlag,
                                             OUTPUT_PORT,
                                             OMX_BUFFERFLAG_EOS, //pOutputBufHeader->nFlags,
                                             NULL);
                  }

                     if(pComponentPrivate->framemode == 0)
                     {
                        // Return the output buffer when no more space left
                        // Dont wait for output buffer to be completely filled, if EOS has come.

#define AACENC_MAX_CHANNELS 2
#define IA_EAACPLUSENC_OUT_BUF_SIZE (6144/8+30)*AACENC_MAX_CHANNELS /* ADTS/ADIF header*/

                       if(((pOutputBufHeader->nAllocLen - pOutputBufHeader->nFilledLen ) < ( IA_EAACPLUSENC_OUT_BUF_SIZE)) ||
                            ( pOutputBufHeader->nFlags & OMX_BUFFERFLAG_EOS) || pComponentPrivate->EOSfromApp)
                        {
                            OMX_ERROR4(pComponentPrivate->dbg, "Return Output Buffer, framemode : 0 \n");

                            pComponentPrivate->bNewOutputBufRequired = 1;

                            pComponentPrivate->nUnhandledFillThisBuffers--;

                            pComponentPrivate->cbInfo.FillBufferDone (
                                pComponentPrivate->pHandle,
                                pComponentPrivate->pHandle->pApplicationPrivate,
                                pOutputBufHeader
                                );

                            pComponentPrivate->pOutputBufHeader = NULL;
                        }
                     }
                     else
                     {
                        // Return output buffer after each frame encode

                        if ( pComponentPrivate->EOSfromApp || (pOutputBufHeader->nFlags & OMX_BUFFERFLAG_EOS) || pComponentPrivate->ia_heaacv2_enc_output_args.bytesGenerated > 0)
                        {
                            OMX_ERROR1(pComponentPrivate->dbg, "Return Output Buffer , framemode : 1, pOutputBufHeader->nFlags :0x%x\n", pOutputBufHeader->nFlags);

                            OMX_ERROR1(pComponentPrivate->dbg, " (pOutputBufHeader->nFlags& EOS) :0x%x\n", (pOutputBufHeader->nFlags & OMX_BUFFERFLAG_EOS ));

                            OMX_ERROR1(pComponentPrivate->dbg, "Setting bNewOutputBufRequired to 1\n");

                            pComponentPrivate->bNewOutputBufRequired = 1;

                            pComponentPrivate->nUnhandledFillThisBuffers--;
                            pComponentPrivate->cbInfo.FillBufferDone (
                                pComponentPrivate->pHandle,
                                pComponentPrivate->pHandle->pApplicationPrivate,
                                pOutputBufHeader
                                );

                            pComponentPrivate->pOutputBufHeader = NULL;
                        }
                    }

                  }
                  else if (pComponentPrivate->curState == OMX_StatePause)
                  {
                      OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: UTIL: pComponentPrivate->nNumOutputBufPending++ = %d \n",__LINE__,(int)pComponentPrivate->nNumOutputBufPending++);
                      OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: UTIL: pBufHeader = %p \n",__LINE__, pOutputBufHeader);
                      pComponentPrivate->pOutputBufHdrPending[pComponentPrivate->nNumOutputBufPending++] = pOutputBufHeader;
                  }


        }

        if (pComponentPrivate->bFlushOutputPortCommandPending) {
            OMX_SendCommand( pComponentPrivate->pHandle,
                OMX_CommandFlush,
                1,NULL);
        }
    }

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: UTIL:Exiting from  IttiamHandleDataBuf_FromApp ..........>>>>>\n",__LINE__);
    return eError;

}




/* ========================================================================== */
/**
* AACENCFill_InitParamsEx () This function is used by the component thread to
* fill the all of its initialization parameters, buffer deatils  etc
* *
* @param pComponent  handle for this instance of the component
*
* @pre
*
* @post
*
* @return none
*/
/* ========================================================================== */
OMX_ERRORTYPE AACENCFill_InitParamsEx(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 nIpBuf,nIpBufSize,nOpBuf,nOpBufSize;
//    OMX_U16 i;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AACENC_COMPONENT_PRIVATE *pComponentPrivate = pHandle->pComponentPrivate;
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: AACENCFill_InitParams\n ",__LINE__);
    nIpBuf = pComponentPrivate->pInputBufferList->numBuffers;
    nIpBufSize = pComponentPrivate->pPortDef[INPUT_PORT]->nBufferSize;
    nOpBuf = pComponentPrivate->pOutputBufferList->numBuffers;
    nOpBufSize = pComponentPrivate->pPortDef[OUTPUT_PORT]->nBufferSize;

    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: ------ Buffer Details -----------\n",__LINE__);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Input  Buffer Count = %ld \n",__LINE__,nIpBuf);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Input  Buffer Size = %ld\n",__LINE__,nIpBufSize);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Output Buffer Count = %ld\n",__LINE__,nOpBuf);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: Output Buffer Size = %ld\n",__LINE__,nOpBufSize);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: ------ Buffer Details ------------\n",__LINE__);

    OMX_PRBUFFER2(pComponentPrivate->dbg, "[AACENCFill_InitParamsEx] nOpBuf = %d\n", (int)nOpBuf);
    OMX_PRBUFFER2(pComponentPrivate->dbg, "[AACENCFill_InitParamsEx] pComponentPrivate->pOutputBufferList->numBuffers = %d\n",pComponentPrivate->pOutputBufferList->numBuffers);
    pComponentPrivate->bPortDefsAllocated = 1;
    pComponentPrivate->bBypassDSP = 0;

EXIT:
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Exiting Fill_InitParams\n",__LINE__);
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Returning = 0x%x\n",__LINE__,eError);
    return eError;
}


OMX_ERRORTYPE AACENCWriteConfigHeader(AACENC_COMPONENT_PRIVATE *pComponentPrivate, OMX_BUFFERHEADERTYPE *pBufHdr)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 nPosition = 0;
//    OMX_U32 nNumBitsWritten = 0;
//    OMX_U32 nBytePosition = 0;
//    OMX_U8  nBitPosition =  0;
    OMX_U8  nBits = 0;
    OMX_U32 tempData = 0;
    OMX_U8 rateIndex = 0;

    //nBytePosition = nPosition / 8;  //add this back if we need to handle more than 4 bytes (U32).
    //nBitPosition =  nPosition % 8;
    memset(pBufHdr->pBuffer, 0x0, pBufHdr->nAllocLen); // make sure we start with zeroes

    OMX_PRINT2(pComponentPrivate->dbg, "profile is %d\n", pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile);
    if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile == OMX_AUDIO_AACObjectLC)
    {
        OMX_U16 nBuf = 0;
        OMX_U16 nBuf2 = 0;

        nBits = 5; //audioObjectType
        nPosition += nBits;
        tempData = AACENC_OBJ_TYP_LC << (16-nPosition);
        // OMX_PRINT2(pComponentPrivate->dbg, "profile is LC, tempData = 2 << (32-5) = %ld\n", tempData);
        nBuf = tempData;

        nBits = 4; //SamplingFrequencyIndex
        nPosition += nBits;
        rateIndex = AACEnc_GetSampleRateIndexL(pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate);
        tempData = rateIndex << (16-nPosition);
        nBuf |= tempData;
        // OMX_PRBUFFER2(pComponentPrivate->dbg, "CONFIG BUFFER = %d\n\n", nBuf);

        nBits = 4; //channelConfiguration
        nPosition += nBits;
        tempData = pComponentPrivate->aacParams[OUTPUT_PORT]->nChannels << (16-nPosition);
        nBuf |= tempData;

        nBuf2 = (nBuf >> 8) | (nBuf << 8); /* Changing Endianess */

        OMX_PRBUFFER2(pComponentPrivate->dbg, "CONFIG BUFFER = %d\n\n", nBuf2);

        memcpy(pBufHdr->pBuffer, &nBuf2, sizeof(OMX_U16));
        pBufHdr->nFilledLen = sizeof(OMX_U16);

    }
    else
    {
        OMX_U32 nBuf = 0;
        OMX_U32 nBuf2 = 0;

        OMX_U32 object_type = 0;

        if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile == OMX_AUDIO_AACObjectHE)
    {
            object_type = AACENC_OBJ_TYP_HEAAC;
    }
    else if (pComponentPrivate->aacParams[OUTPUT_PORT]->eAACProfile == OMX_AUDIO_AACObjectHE_PS)
    {
            object_type = AACENC_OBJ_TYP_PS;
    }

        //audioObjectType
        nBits = 5;
        nPosition += nBits;
        tempData = object_type << (32-nPosition);
    nBuf = tempData;

        // AAC Sampling Frequency Index
        nBits = 4;
        nPosition += nBits;
        rateIndex = AACEnc_GetSampleRateIndexL((pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate)/2);
        tempData = rateIndex << (32-nPosition);
        nBuf |= tempData;
        // OMX_PRBUFFER2(pComponentPrivate->dbg, "CONFIG BUFFER = %d\n\n", nBuf);

        // ChannelConfiguration
        nBits = 4;
        nPosition += nBits;
        tempData = pComponentPrivate->aacParams[OUTPUT_PORT]->nChannels << (32-nPosition);
        nBuf |= tempData;

        // Extension Sampling Frequency Index
        nBits = 4;
    nPosition += nBits;
    rateIndex = AACEnc_GetSampleRateIndexL(pComponentPrivate->aacParams[OUTPUT_PORT]->nSampleRate);
        tempData = rateIndex << (32-nPosition);
    nBuf |= tempData;

        //audioObjectType
        nBits = 5;
    nPosition += nBits;
        tempData = object_type << (32-nPosition);
    nBuf |= tempData;

        /* Changing Endianess */
        {
            OMX_U32 byte1, byte2, byte3, byte4;

            byte1 = (nBuf & 0x000000FF);
            byte2 = ((nBuf & 0x0000FF00)>>8);
            byte3 = ((nBuf & 0x00FF0000)>>16);
            byte4 = ((nBuf & 0xFF000000)>>24);

            nBuf2 = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | (byte4);
        }

    OMX_PRBUFFER2(pComponentPrivate->dbg, "CONFIG BUFFER = %d\n\n", nBuf2);

        memcpy(pBufHdr->pBuffer, &nBuf2, sizeof(OMX_U32));

        pBufHdr->nFilledLen = sizeof(OMX_U32);
    }

    pBufHdr->nFlags = NORMAL_BUFFER;  // clear any other flags then add the needed ones
    pBufHdr->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
    pBufHdr->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;

    return eError;
}

/*=======================================================================*/
/*! @fn AACEnc_GetSampleRateIndexL

 * @brief Gets the sample rate index

 * @param  aRate : Actual Sampling Freq

 * @Return  Index

 */
/*=======================================================================*/
int AACEnc_GetSampleRateIndexL( const int aRate)
{
    int index = 0;
    OMXDBG_PRINT(stderr, PRINT, 2, 0, "%d::aRate:%d\n",__LINE__,aRate);

    switch( aRate ){
    case 96000:
        index = 0;
        break;
    case 88200:
        index = 1;
        break;
    case 64000:
        index = 2;
        break;
    case 48000:
        index = 3;
        break;
    case 44100:
        index = 4;
        break;
    case 32000:
        index = 5;
        break;
    case 24000:
        index = 6;
        break;
    case 22050:
        index = 7;
        break;
    case 16000:
        index = 8;
        break;
    case 12000:
        index = 9;
        break;
    case 11025:
        index = 10;
        break;
    case 8000:
        index = 11;
        break;
    default:
        OMXDBG_PRINT(stderr, PRINT, 2, 0, "Invalid sampling frequency\n");
        break;
    }

    OMXDBG_PRINT(stderr, PRINT, 1, 0, "%d::index:%d\n",__LINE__,index);
    return index;
}




OMX_ERRORTYPE ReturnInputOutputBuffers(AACENC_COMPONENT_PRIVATE *pComponentPrivate)
{
    int ret = 0;
        OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pInputBufHeader;
    OMX_BUFFERHEADERTYPE *pOutputBufHeader;

    //Reset EOS flag here, as this will be called once the component is transitioning from
    //StateExecuting to StateIdle

    pComponentPrivate->EOSfromApp = 0;

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

    while(1)
    {
        int status;
        struct timespec tv;
        int fdmax;
        fdmax = pComponentPrivate->dataPipeInputBuf[0];
        if (pComponentPrivate->dataPipeOutputBuf[0] > fdmax)
            fdmax = pComponentPrivate->dataPipeOutputBuf[0];

        FD_ZERO (&pComponentPrivate->rfds);
        FD_SET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds);
        FD_SET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds);

        tv.tv_sec = 0;
        tv.tv_nsec = 0;

        status = select (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv);
        if(status == 0)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Component Time Out !!!!! \n",__LINE__);
            break;
        }

        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: Inside whil loop \n", __LINE__);
        if (FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds))
        {

            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: calling read on Output buffer pipe \n", __LINE__);
            ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }

            OMX_ERROR4(pComponentPrivate->dbg, "Return Output Buffer\n");
            pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pOutputBufHeader
                );

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
            OMX_ERROR4(pComponentPrivate->dbg, "Return Input Buffer\n");
            pComponentPrivate->cbInfo.EmptyBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pInputBufHeader
                );

        }
    }
EXIT:
    return eError;
}

OMX_ERRORTYPE ReturnOutputBuffers(AACENC_COMPONENT_PRIVATE *pComponentPrivate)
{
    int ret = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pOutputBufHeader;

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

    while(1)
    {
        int status;
        struct timespec tv;
        int fdmax;
        fdmax = pComponentPrivate->dataPipeInputBuf[0];
        if (pComponentPrivate->dataPipeOutputBuf[0] > fdmax)
            fdmax = pComponentPrivate->dataPipeOutputBuf[0];

        FD_ZERO (&pComponentPrivate->rfds);
        FD_SET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds);

        tv.tv_sec = 1;
        tv.tv_nsec = 0;

        status = select (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv);
        if(status == 0)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Component Time Out !!!!! \n",__LINE__);
            break;
        }

        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: Inside whil loop \n", __LINE__);
        if (FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds))
        {

            OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: calling read on Output buffer pipe \n", __LINE__);
            ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
            if (ret == -1)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
            }

            OMX_ERROR4(pComponentPrivate->dbg, "Return Output Buffer\n");
            pComponentPrivate->cbInfo.FillBufferDone (
                pComponentPrivate->pHandle,
                pComponentPrivate->pHandle->pApplicationPrivate,
                pOutputBufHeader
                );

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

OMX_ERRORTYPE ReturnThisOutputBuffer(AACENC_COMPONENT_PRIVATE *pComponentPrivate)
{
    int ret = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pOutputBufHeader;

    if(pComponentPrivate->pOutputBufHeader == NULL)
    {
        OMX_PRSTATE2(pComponentPrivate->dbg, "%d :: AACENC: calling read on Output buffer pipe \n", __LINE__);
        ret = read(pComponentPrivate->dataPipeOutputBuf[0], &pOutputBufHeader, sizeof(pOutputBufHeader));
        if (ret == -1)
        {
                OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error while reading from the Output Buffer pipe\n",__LINE__);
                eError = OMX_ErrorHardware;
                goto EXIT;
        }

        pComponentPrivate->pOutputBufHeader= pOutputBufHeader;

    }

    pComponentPrivate->bNewOutputBufRequired = 1;

    pComponentPrivate->cbInfo.FillBufferDone (
                    pComponentPrivate->pHandle,
                    pComponentPrivate->pHandle->pApplicationPrivate,
                    pComponentPrivate->pOutputBufHeader
                    );

    pComponentPrivate->pOutputBufHeader = NULL;

    pComponentPrivate->nUnhandledFillThisBuffers--;

    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: ReturnThisOutputBuffer: EXIT  !!!!! \n",__LINE__);

EXIT:
    return eError;
}

