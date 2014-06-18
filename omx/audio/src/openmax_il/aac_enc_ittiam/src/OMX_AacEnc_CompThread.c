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
#else
#include <wchar.h>
#include <dbapi.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#endif
#include "OMX_IttiamAacEncoder.h"
#include "OMX_IttiamAacEnc_Utils.h"
#include "OMX_IttiamAacEnc_CompThread.h"


void* ComponentThread (void* pThreadData)
{
    int status;
    struct timespec tv;
    int fdmax;
//    int ret = 0;
    //fd_set rfds;
    OMX_U32 nRet;
    OMX_S32 IsInputBufAvail=0;
    OMX_S32 IsOutputBufAvail=0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
//    OMX_BUFFERHEADERTYPE *pInputBufHeader;
//    OMX_BUFFERHEADERTYPE *pOutputBufHeader;



    /* Recover the pointer to my component specific data */
    AACENC_COMPONENT_PRIVATE* pComponentPrivate = (AACENC_COMPONENT_PRIVATE*)pThreadData;
    OMX_COMPONENTTYPE *pHandle = pComponentPrivate->pHandle;


#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->pPERFcomp = PERF_Create(PERF_FOURCC('A', 'A', 'C', 'E'),
                                                   PERF_ModuleComponent |
                                                   PERF_ModuleAudioDecode);
#endif

    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Entering ComponentThread\n", __LINE__);
    fdmax = pComponentPrivate->cmdPipe[0];

    if (pComponentPrivate->dataPipeInputBuf[0] > fdmax)
        fdmax = pComponentPrivate->dataPipeInputBuf[0];

    if (pComponentPrivate->dataPipeOutputBuf[0] > fdmax)
        fdmax = pComponentPrivate->dataPipeOutputBuf[0];

    while (1)
    {
        FD_ZERO (&pComponentPrivate->rfds);
        FD_SET (pComponentPrivate->cmdPipe[0], &pComponentPrivate->rfds);

        /* Enable watch on input/output, if no input/output buf available*/
        if(!IsInputBufAvail)
        FD_SET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds);

        if(pComponentPrivate->bNewOutputBufRequired==1)
        {
            IsOutputBufAvail = 0;
            FD_SET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds);
        }

        status = 0;
        tv.tv_sec = 1;
        tv.tv_nsec = 0;

#ifndef UNDER_CE
        sigset_t set;
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv, &set);
#else
        status = select (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv);
#endif

        if (pComponentPrivate->bIsThreadstop == 1) {
            OMX_ERROR4(pComponentPrivate->dbg, ":: Comp Thrd Exiting here...\n");
            goto EXIT;
        }

        if (status == 0)
        {


            OMX_ERROR4(pComponentPrivate->dbg, "%d : bIsStopping = %ld\n",__LINE__, pComponentPrivate->bIsStopping);
            OMX_ERROR4(pComponentPrivate->dbg, "%d : bIsThreadstop = %ld\n",__LINE__, pComponentPrivate->bIsThreadstop);
            if (pComponentPrivate->bIsThreadstop == 1)
            {
                OMX_PRINT1(pComponentPrivate->dbg, "%d  :: OMX_AACENC_ComponentThread \n",__LINE__);
                pComponentPrivate->bIsStopping = 0;
                pComponentPrivate->bIsThreadstop = 0;
                pComponentPrivate->bIsEOFSent = 0;
                OMX_PRINT1(pComponentPrivate->dbg, "%d :: OMX_AACENC_ComponentThread \n",__LINE__);
                if (pComponentPrivate->curState != OMX_StateIdle)
                {
                    OMX_PRINT1(pComponentPrivate->dbg, "%d ::OMX_AACENC_ComponentThread \n",__LINE__);
                    goto EXIT;
                }
             }
             OMX_ERROR4(pComponentPrivate->dbg, "%d :: Component Time Out !!!!! \n",__LINE__);
        }
        else if(status == -1)
        {
            OMX_ERROR2(pComponentPrivate->dbg, "%d :: Error in Select\n", __LINE__);
            pComponentPrivate->cbInfo.EventHandler (pHandle, pHandle->pApplicationPrivate,
                                                    OMX_EventError,
                                                    OMX_ErrorHardware,
                                                    OMX_TI_ErrorSevere,
                                                    "Error from Component Thread in select");
            exit(1);

        }
        /*else if((FD_ISSET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds)))
        {

            OMX_ERROR4(pComponentPrivate->dbg, ":: data INPUT  pipe is set \n");
            IsInputBufAvail = 1;
            }
        else if((FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds)))
        {
            IsOutputBufAvail = 1;
            OMX_ERROR4(pComponentPrivate->dbg, ":: data OUTPUT  pipe is set \n");
        }*/
        else if(FD_ISSET (pComponentPrivate->cmdPipe[0], &pComponentPrivate->rfds))
        {
            OMX_PRDSP2(pComponentPrivate->dbg, "%d :: pHandle: %p \n",__LINE__,pHandle);
            /* Do not accept any command when the component is stopping */
            OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: CMD pipe is set in Component Thread\n",__LINE__);
            nRet = AACENCHandleCommand (pComponentPrivate);
            if (nRet == EXIT_COMPONENT_THRD)
            {
                OMX_ERROR2(pComponentPrivate->dbg, " %d :: Exiting from Component thread\n",__LINE__);

                AACENC_CleanupInitParams(pHandle);
                if(eError != OMX_ErrorNone)
                {
                    OMX_ERROR4(pComponentPrivate->dbg, "%d :: AACENC_CleanupInitParams returned error\n",__LINE__);
                    goto EXIT;
                }
                OMX_PRBUFFER2(pComponentPrivate->dbg, "%d :: ARM Side Resources Have Been Freed\n",__LINE__);

                pComponentPrivate->curState = OMX_StateLoaded;

#ifdef __PERF_INSTRUMENTATION__
                PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundaryCleanup);
#endif


                if(pComponentPrivate->bPreempted==0){
#if 0
                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                           pHandle->pApplicationPrivate,
                                                           OMX_EventCmdComplete,
                                                           OMX_ErrorNone,pComponentPrivate->curState,
                                                           NULL);
#endif
                }
                else{
                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                           pHandle->pApplicationPrivate,
                                                           OMX_EventError,
                                                           OMX_ErrorResourcesLost,
                                                           OMX_TI_ErrorMajor,
                                                           NULL);
                    pComponentPrivate->bPreempted = 0;
                }

            pComponentPrivate->bLoadedCommandPending = OMX_FALSE;
            }

        }
        else if((FD_ISSET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds)))
        {

            OMX_ERROR1(pComponentPrivate->dbg, ":: data INPUT  pipe is set \n");
            IsInputBufAvail = 1;
            }
        else if((FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds)))
        {
            pComponentPrivate->bNewOutputBufRequired = 2;
            IsOutputBufAvail = 1;
            OMX_ERROR1(pComponentPrivate->dbg, ":: data OUTPUT  pipe is set \n");
        }
        /* Waiting for both input and output buffer to be available*/
                if ( (IsInputBufAvail || pComponentPrivate->EOSfromApp) && IsOutputBufAvail &&
                        (pComponentPrivate->curState == OMX_StateExecuting) )
                {

                    OMX_PRCOMM2(pComponentPrivate->dbg, "%d ::!!! Received both input and output buffer !!!\n",__LINE__);

                    eError = AACENCHandleDataBuf_FromApp(pComponentPrivate);

                    IsInputBufAvail = 0;

               if(pComponentPrivate->bNewOutputBufRequired)
                   {
                 //OMX_ERROR4(pComponentPrivate->dbg, "Component Thrd:: %d:: Enabling watch on output  \n", __LINE__);
                 IsOutputBufAvail = 0;
                   }

                    if (eError != OMX_ErrorNone)
                    {
                        OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error From AACENCHandleDataBuf_FromApp: %d \n",__LINE__, eError);
                        break;
                    }
        }
        else if(IsOutputBufAvail && (pComponentPrivate->curState != OMX_StatePause)
                && (pComponentPrivate->StateTransitiontoIdleFromExecOngoing == 1))
        {
            //just return this op buffer as no more input buffers will be provided by app,
            //as the request has come to transition to state idle
            OMX_ERROR4(pComponentPrivate->dbg, "Component Thrd:: call to ReturnThisOutputBuffer %d:: \n", __LINE__);
            ReturnThisOutputBuffer(pComponentPrivate);
        }
    }

EXIT:

#ifdef __PERF_INSTRUMENTATION__
    PERF_Done(pComponentPrivate->pPERFcomp);
#endif
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Exiting ComponentThread\n", __LINE__);
    return (void*)OMX_ErrorNone;
}

