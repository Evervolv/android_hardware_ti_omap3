/*****************************************************************************/
/*                                                                           */
/*                             MP3 Decoder                                   */
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
/*  File Name        : OMX_Mp3Dec_CompThread.c                               */
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
#include <sys/select.h>
#include <memory.h>
#include <fcntl.h>
#include <signal.h>

#include <dbapi.h>
#include <string.h>
#include <stdio.h>

#include "OMX_IttiamMp3Dec_Utils.h"

/* ================================================================================= * */
/**
* @fn MP3DEC_ComponentThread() This is component thread that keeps listening for
* commands or event/messages/buffers from application .
*
* @param pThreadData This is thread argument.
*
* @pre          None
*
* @post         None
*
*  @return      OMX_ErrorNone = Always
*
*  @see         None
*/
/* ================================================================================ * */
void* MP3DEC_ComponentThread (void* pThreadData)
{
    int status;
    struct timespec tv;
    int fdmax;
    OMX_U32 nRet;

    OMX_S32 IsInputBufAvail=0;
    OMX_S32 IsOutputBufAvail=0;

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    MP3DEC_COMPONENT_PRIVATE* pComponentPrivate = (MP3DEC_COMPONENT_PRIVATE*)pThreadData;
    OMX_COMPONENTTYPE *pHandle = pComponentPrivate->pHandle;



    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Entering ComponentThread \n",__LINE__);
#ifdef __PERF_INSTRUMENTATION__
    pComponentPrivate->pPERFcomp = PERF_Create(PERF_FOURCC('A', 'C', 'D', '_'),
                                               PERF_ModuleComponent |
                                               PERF_ModuleAudioDecode);
#endif
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
        {
            FD_SET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds);
        }

	if(pComponentPrivate->bNewOutputBufRequired == 1)
	{
			OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: IsOutputBufAvail: %d\n",__LINE__, IsOutputBufAvail );
        IsOutputBufAvail = 0;
	FD_SET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds);
	}

        status = 0;
        tv.tv_sec = 1;
        tv.tv_nsec = 0;

        //This is to repeatedly call process() as long as conditions are met,instead of waiting on pselect() One second timeout
        if(1 == pComponentPrivate->iEOS)
        {
            tv.tv_sec  = 0;
            tv.tv_nsec = (10 * 1000 * 1000); // 10 milliseconds
        }

        sigset_t set;
        sigemptyset (&set);
        sigaddset (&set, SIGALRM);
        status = pselect (fdmax+1, &pComponentPrivate->rfds, NULL, NULL, &tv, &set);

		OMX_PRCOMM2(pComponentPrivate->dbg, ":: Component Thread running in while loop, status: %d \n", status);
		OMX_PRCOMM2(pComponentPrivate->dbg, ":: IsInputBufAvail: %d  IsOutputBufAvail : %d \n", IsInputBufAvail, IsOutputBufAvail );

        if (pComponentPrivate->bExitCompThrd == 1)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Comp Thrd Exiting here...\n",__LINE__);
            goto EXIT;
        }

        if (0 == status)
        {
//SF [
// has been reported as an error by Customers in the past. So let us not print this.
//            OMX_ERROR4(pComponentPrivate->dbg, "\n\n\n!!!!!  Component Time Out !!!!!!!!!!!! \n");
            //OMX_ERROR4(pComponentPrivate->dbg, ":: IsInputBufAvail: %d  IsOutputBufAvail : %d \n", IsInputBufAvail, IsOutputBufAvail );
//SF ]
        }
        else if (-1 == status)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "%d :: Error in Select\n", __LINE__);
            pComponentPrivate->cbInfo.EventHandler (pHandle,
                                                    pHandle->pApplicationPrivate,
                                                    OMX_EventError,
                                                    OMX_ErrorHardware,
                                                    OMX_TI_ErrorSevere,
                                                    "Error from COmponent Thread in select");
            goto EXIT;
        }
        else if (FD_ISSET (pComponentPrivate->cmdPipe[0], &pComponentPrivate->rfds))
        {

            //OMX_ERROR4(pComponentPrivate->dbg, ":: CMD pipe is set \n");
            OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: CMD pipe is set in Component Thread\n",__LINE__);
            pComponentPrivate->InputPortFlushed = 0;
            pComponentPrivate->OutputPortFlushed = 0;

            nRet = MP3DEC_HandleCommand (pComponentPrivate);

			/*It is better to update these flags here, as in cases like Flush, the status of these
			flags might become invalid.*/
			if(pComponentPrivate->InputPortFlushed)
				IsInputBufAvail = 0;
			if(pComponentPrivate->OutputPortFlushed)
				IsOutputBufAvail = 0;

            if (nRet == EXIT_COMPONENT_THRD)
            {
                OMX_PRINT1(pComponentPrivate->dbg, "Exiting from Component thread\n");

                OMX_PRSTATE2(pComponentPrivate->dbg, "******************* Component State Set to Loaded\n\n");

                pComponentPrivate->curState = OMX_StateLoaded;
#ifdef __PERF_INSTRUMENTATION__
                PERF_Boundary(pComponentPrivate->pPERFcomp,PERF_BoundaryComplete | PERF_BoundaryCleanup);
#endif
                if(pComponentPrivate->bPreempted==0)
                {
                    pComponentPrivate->cbInfo.EventHandler(
                                                           pHandle, pHandle->pApplicationPrivate,
                                                           OMX_EventCmdComplete,
                                                           OMX_ErrorNone,pComponentPrivate->curState, NULL);
                }
                else
                {
                    OMX_ERROR4(pComponentPrivate->dbg, "OMX_EventError:: OMX_ErrorPortUnpopulated at CompThread line %d\n", __LINE__);
                    pComponentPrivate->cbInfo.EventHandler(pHandle,
                                                           pHandle->pApplicationPrivate,
                                                           OMX_EventError,
                                                           OMX_ErrorResourcesLost,
                                                           OMX_TI_ErrorMajor,
                                                           NULL);
                    pComponentPrivate->bPreempted = 0;
                }
            }
        }

	else if((FD_ISSET (pComponentPrivate->dataPipeInputBuf[0], &pComponentPrivate->rfds)))
	{

        // OMX_ERROR4(pComponentPrivate->dbg, ":: data INPUT  pipe is set, disabling watch on input \n");
         //LOGI( " data INPUT  pipe is set, disabling watch on input \n");
    	 IsInputBufAvail = 1;
	}
	else if((FD_ISSET (pComponentPrivate->dataPipeOutputBuf[0], &pComponentPrivate->rfds)))
	{

	IsOutputBufAvail = 1;
	pComponentPrivate->bNewOutputBufRequired = 2;
        //OMX_ERROR4(pComponentPrivate->dbg, ":: data OUTPUT  pipe is set, disabling watch on output \n");
        //LOGI(" data OUTPUT  pipe is set, disabling watch on output \n");
        }
         if ( (IsInputBufAvail || pComponentPrivate->iEOS) && IsOutputBufAvail && pComponentPrivate->curState == OMX_StateExecuting &&
			(!pComponentPrivate->reconfigOutputPort) )
		{
            int ret;
	    OMX_PRCOMM2(pComponentPrivate->dbg, "%d :: Received both input and output buffer \n",__LINE__);
	    //LOGI( "%d :: Received both input and output buffer \n",__LINE__);
           // OMX_ERROR4(pComponentPrivate->dbg, "Component Thrd:: %d:: Calling HandleDataBufFromApp  \n", __LINE__);

	   eError = MP3DEC_HandleDataBuf_FromApp (pComponentPrivate);

            if (eError != OMX_ErrorNone)
            {
                OMX_ERROR2(pComponentPrivate->dbg, "%d :: Error From HandleDataBuf_FromApp\n",__LINE__);
                break;
            }

           //OMX_ERROR4(pComponentPrivate->dbg, "Component Thrd:: %d:: Enabling watch on input  \n", __LINE__);
			if(pComponentPrivate->inputBufFilledLen == 0)
            {
				IsInputBufAvail = 0;
            }

           if(pComponentPrivate->bNewOutputBufRequired == 1)
           {
            //OMX_ERROR4(pComponentPrivate->dbg, "Component Thrd:: %d:: Enabling watch on output  \n", __LINE__);
	    IsOutputBufAvail = 0;
           }
	}
    }
 EXIT:



    pComponentPrivate->bCompThreadStarted = 0;

#ifdef __PERF_INSTRUMENTATION__
    PERF_Done(pComponentPrivate->pPERFcomp);
#endif
    OMX_PRINT1(pComponentPrivate->dbg, "%d :: Exiting ComponentThread \n",__LINE__);
    return (void*)OMX_ErrorNone;
}
