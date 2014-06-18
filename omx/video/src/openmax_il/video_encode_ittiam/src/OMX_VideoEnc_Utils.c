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
#include <wchar.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <malloc.h>
#include <memory.h>
#include <fcntl.h>

#include <dbapi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* TI files */
#include "ittiam_datatypes.h"
#include "std.h"
#include "ialg.h"
#include "xdas.h"
#include "xdm.h"
#include "ivideo.h"
#include "ividenc1.h"

#include "iv_enc1_api_consts.h"
#include "iv_enc1_extension_structs.h"
#include "iv_enc1_api_structs.h"
#include "iv_enc_export.h"

/*------- Program Header Files -----------------------------------------------*/
#include "OMX_VideoEnc_Utils.h"
#include "OMX_VideoEncoder.h"
#include "OMX_VideoEnc_Thread.h"
#include "OMX_VideoEnc_DSP.h"
#include "OMX_ividenc1_api.h"
#include <MetadataBufferType.h>
#include <VideoMetadata.h>

#include "profile.h"

#include "OMX_ividenc1_api_structs.h"

#define DSP_MMU_FAULT_HANDLING

static int node_count = 0;
static int node_count_dealloc = 0;
int gSplitVidEncInBufCount;
int gSplitVidEncOutBufCount;

extern h264profile_print_final_stats();

//static char string[] = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345";
/*******************************************************************************
*  EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/
/*--------data declarations --------------------------------------------------*/
/*--------function prototypes ------------------------------------------------*/

/*
* function to create the split encoder
*/
OMX_ERRORTYPE OMX_VIDENC_Create(VIDENC_COMPONENT_PRIVATE *pComponentPrivate);

/*
* This is the API function for demarcation of NAL Unit . It parses the bit stream
* buffer for two consecutive NAL start codes and returns after updating nal offset
* and nal size
*/
WORD32 h264_parse_start_code_prefixes(UWORD8 *pu1_buffer,
                                      UWORD32 u4_fill_size,
                                      UWORD32 *pu4_nal_off,
                                      UWORD32 *pu4_nal_size);

/*
* This function initializes all the required pointers required during the process
*/
void initialize_pointers(VIDENC_COMPONENT_PRIVATE *pComponentPrivate, int index,
                        OMX_BUFFERHEADERTYPE *pInpBufHead,
                        OMX_BUFFERHEADERTYPE *pOutBufHead,
                        XDAS_Int8           *apOutBuf[],
                        XDAS_Int32          anOutBufSizes[]);

/*
* This function makes all the control calls to the codec
*/
OMX_ERRORTYPE control_calls(VIDENC_COMPONENT_PRIVATE *pComponentPrivate);

/*
* This function sends out the output buffer back to the application
*/
OMX_ERRORTYPE send_buffer_to_dsp_wait_thread(VIDENC_COMPONENT_PRIVATE *pComponentPrivate,
                                            int index,
                                            OMX_BUFFERHEADERTYPE *pOutBufHead,
                                            OMX_BUFFERHEADERTYPE *pInpBufHead);

/*
* Gets a new output buffer for processing
*/
OMX_BUFFERHEADERTYPE* get_new_output_buffer(VIDENC_COMPONENT_PRIVATE *pComponentPrivate,
                                            OMX_ERRORTYPE *omxError);

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*--------data declarations --------------------------------------------------*/


/*--------function prototypes ------------------------------------------------*/

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*--------data declarations --------------------------------------------------*/
struct DSP_UUID H264VESOCKET_TI_UUID = {
    0x63A3581A, 0x09D7, 0x4AD0, 0x80, 0xB8, {
    0x5F, 0x2C, 0x4D, 0x4D, 0x59, 0xC9
    }
};

struct DSP_UUID MP4VESOCKET_TI_UUID = {
    0x98c2e8d8, 0x4644, 0x11d6, 0x81, 0x18, {
        0x00, 0xb0, 0xd0, 0x8d, 0x72, 0x9f
    }
};

struct DSP_UUID USN_UUID = {
    0x79A3C8B3, 0x95F2, 0x403F, 0x9A, 0x4B, {
        0xCF, 0x80, 0x57, 0x73, 0x05, 0x41
    }
};

/*--------macro definitions ---------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
  * ListCreate()
  *
  * Function call in OMX_ComponentInit(). Creates the List Head of the Component Memory List.
  *
  * @param pListHead VIDENC_NODE double pointer with the List Header of the Memory List.
  *
  * @retval OMX_ErrorNone
  *               OMX_ErrorInsufficientResources if the malloc fails
  *
  **/
/*-----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_ListCreate(struct OMX_TI_Debug *dbg, struct VIDENC_NODE** pListHead)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    *pListHead = (VIDENC_NODE*)malloc(sizeof(VIDENC_NODE)); /* need to malloc!!! */
    if (*pListHead == NULL)
    {
        OMX_TRACE4(*dbg, "malloc() out of memory error\n");
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorInsufficientResources);
    }

    OMX_TRACE1(*dbg, "Create MemoryListHeader[%p]\n", *pListHead);
    memset(*pListHead, 0x0, sizeof(VIDENC_NODE));

OMX_CONF_CMD_BAIL:
    return eError;
}

/*-----------------------------------------------------------------------------*/
/**
  * ListAdd()
  *
  * Called inside VIDENC_MALLOC Macro to add a new node to Component Memory List
  *
  * @param pListHead VIDENC_NODE Points List Header of the Memory List.
  *                pData OMX_PTR points to the new allocated data.
  * @retval OMX_ErrorNone
  *               OMX_ErrorInsufficientResources if the malloc fails
  *
  **/
/*-----------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_ListAdd(struct OMX_TI_Debug *dbg,
				 struct VIDENC_NODE* pListHead,
				 OMX_PTR  pData,
				 int     size)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

#ifdef __ANALYSE_MEMORY__
    VIDENC_NODE* pTmp = NULL;
    VIDENC_NODE* pNewNode = NULL;
    int i = 0;

	char *cmp = (char *)pData;


	for(i = 0; i < (PADDING_SIZE/2); i++)
		cmp[i] = 'x';

	/*for(i = 0; i < (PADDING_SIZE/20); i++)
		ALOGD("HEADER:%x %x %x %x %x %x %x %x %x %x",
			cmp[i*10 + 0], cmp[i*10 + 1], cmp[i*10 + 2], cmp[i*10 + 3],
			cmp[i*10 + 4], cmp[i*10 + 5], cmp[i*10 + 6], cmp[i*10 + 7],
			cmp[i*10 + 8], cmp[i*10 + 9]);*/

	cmp = cmp + size + PADDING_SIZE/2;

	for(i = 0; i < (PADDING_SIZE/2); i++)
		cmp[i] = 'x';

	/*for(i = 0; i < (PADDING_SIZE/20); i++)
		ALOGD("FOOTER:%x %x %x %x %x %x %x %x %x %x",
		cmp[i*10 + 0], cmp[i*10 + 1], cmp[i*10 + 2],
		cmp[i*10 + 3], cmp[i*10 + 4], cmp[i*10 + 5],
		cmp[i*10 + 6], cmp[i*10 + 7], cmp[i*10 + 8], cmp[i*10 + 9]);*/


	cmp = (char *)pData;

    pNewNode = (VIDENC_NODE*)malloc(sizeof(VIDENC_NODE)); /* need to malloc!!! */
    node_count++;
    //ALOGD("Alloc Node count = %d", node_count);

    if (pNewNode == NULL)
    {
        OMX_TRACE4(*dbg, "malloc() out of memory error\n");
        OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorInsufficientResources);
    }
    memset(pNewNode, 0x0, sizeof(VIDENC_NODE));
    pNewNode->pData = cmp;
    pNewNode->size = size;
    pNewNode->pNext = NULL;

    pTmp = pListHead;

    while (pTmp->pNext != NULL)
    {
        pTmp = pTmp->pNext;
    }
    pTmp->pNext = pNewNode;

OMX_CONF_CMD_BAIL:
#endif
    return eError;
}

/*-----------------------------------------------------------------------------*/
/**
  * ListRemove()
  *
  * Called inside VIDENC_FREE Macro remove  node from Component Memory List and free the memory pointed by the node.
  *
  * @param pListHead VIDENC_NODE Points List Header of the Memory List.
  *                pData OMX_PTR points to the new allocated data.
  * @retval OMX_ErrorNone
  *
  *
  **/
/*-----------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_ListRemove(struct OMX_TI_Debug *dbg, struct VIDENC_NODE* pListHead,
                                    OMX_PTR pData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
#ifdef __ANALYSE_MEMORY__
    VIDENC_NODE* pNode = NULL;
    VIDENC_NODE* pTmp = NULL;
    char *cmp;

    pNode = pListHead;

	pData = (char *)pData -(PADDING_SIZE/2);


	//ALOGD("In OMX_VIDENC_ListRemove");
    while (pNode->pNext != NULL)
    {
        if (pNode->pNext->pData == pData)
        {
			int i = 0;
            pTmp = pNode->pNext;
            cmp = (char *)pTmp->pData;
            pNode->pNext = pTmp->pNext;
            int status;

            node_count_dealloc++;
			//ALOGD("In OMX_VIDENC_ListRemove: Alloc Node count = %d De alloc Node count = %d",node_count, node_count_dealloc);

			for(i = 0; i < (PADDING_SIZE/2); i++)
			{
				if('x' != cmp[i])
					break;
			}

			if(i == (PADDING_SIZE/2))
				//ALOGD("HEADER PASS");
			else
			{
				for(i = 0; i < (PADDING_SIZE/20); i++)
					ALOGD("HEADER REMOVE:%x %x %x %x %x %x %x %x %x %x",
					 cmp[i*10 + 0], cmp[i*10 + 1], cmp[i*10 + 2], cmp[i*10 + 3],
					 cmp[i*10 + 4], cmp[i*10 + 5], cmp[i*10 + 6], cmp[i*10 + 7],
					 cmp[i*10 + 8], cmp[i*10 + 9]);
			}

			cmp = cmp + PADDING_SIZE/2 + pTmp->size;

			for(i = 0; i < (PADDING_SIZE/2); i++)
			{
				if('x' != cmp[i])
					break;
			}

			if(i == (PADDING_SIZE/2))
				ALOGD("FOOTER PASS");
			else
			{
				for(i = 0; i < (PADDING_SIZE/20); i++)
				ALOGD("FOOTER REMOVE:%x %x %x %x %x %x %x %x %x %x",
				cmp[i*10 + 0], cmp[i*10 + 1], cmp[i*10 + 2], cmp[i*10 + 3],
				cmp[i*10 + 4], cmp[i*10 + 5], cmp[i*10 + 6], cmp[i*10 + 7],
				cmp[i*10 + 8], cmp[i*10 + 9]);
			}

			ALOGD("Deallocated = %u", pTmp->pData);
			free(pTmp->pData);
			free(pTmp);
			pTmp = NULL;
			break;
        }
        pNode = pNode->pNext;
    }
#else

free(pData);
#endif

    return eError;
}

/*-----------------------------------------------------------------------------*/
/**
  * ListDestroy()
  *
  * Called inside OMX_ComponentDeInit()  Remove all nodes and free all the memory in the Component Memory List.
  *
  * @param pListHead VIDENC_NODE Points List Header of the Memory List.
  *
  * @retval OMX_ErrorNone
  *
  *
  **/
/*-----------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_ListDestroy(struct OMX_TI_Debug *dbg, struct VIDENC_NODE* pListHead)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    VIDENC_NODE* pTmp = NULL;
    VIDENC_NODE* pNode = NULL;
    pNode = pListHead;

    while (pNode->pNext != NULL)
    {
        pTmp = pNode->pNext;
        pNode->pNext=pTmp->pNext;
        if (pTmp->pData != NULL)
        {
			char *cmp = (char *)pTmp->pData;
            OMX_TRACE0(*dbg, "Remove MemoryNode[%p] -> [%p]\n", pTmp, pTmp->pData);

			ALOGD("Deallocated = %x", pTmp->pData);

            node_count_dealloc++;
			//ALOGD("Node count = %d", node_count_dealloc);
            free(pTmp->pData);
            pTmp->pData = NULL;
        }
        free(pTmp);
        pTmp = NULL;
    }

    OMX_TRACE1(*dbg, "Destroy MemoryListHeader[%p]\n", pListHead);
    free(pListHead);
    return eError;
}



/*---------------------------------------------------------------------------------------*/
/**
  *  OMX_VIDENC_EmptyDataPipes
  * @param
  *
  * @retval OMX_NoError              Success, ready to roll
  *
 **/
/*---------------------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_EmptyDataPipes (void* pThreadData)
{
    fd_set rfds;
    int nRet = -1;
    int fdmax = -1;
    int status = -1;
    struct timeval tv;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE* pBufHead = NULL;
    VIDENC_COMPONENT_PRIVATE* pComponentPrivate = NULL;

    OMX_CONF_CHECK_CMD(pThreadData, 1, 1);


    pComponentPrivate = (VIDENC_COMPONENT_PRIVATE*)pThreadData;

    /** Looking for highest number of file descriptor
        for pipes inorder to put in select loop */

    fdmax = pComponentPrivate->nFree_oPipe[0];

    if (pComponentPrivate->nFilled_iPipe[0] > fdmax)
    {
        fdmax = pComponentPrivate->nFilled_iPipe[0];
    }

    while (1)
    {
        FD_ZERO (&rfds);
        FD_SET (pComponentPrivate->nFree_oPipe[0], &rfds);
        FD_SET (pComponentPrivate->nFilled_iPipe[0], &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 250*1000;

        status = select(fdmax+1, &rfds, NULL, NULL, &tv);

        if (0 == status)
        {
            OMX_TRACE2(pComponentPrivate->dbg, "selectEmpty() = 0\n");
            break;
        }
        else if (-1 == status)
        {
            break;
        }
        else
        {
            if ((FD_ISSET(pComponentPrivate->nFilled_iPipe[0], &rfds)))
            {
                nRet = read(pComponentPrivate->nFilled_iPipe[0],
                            &pBufHead,
                            sizeof(pBufHead));
                OMX_PRBUFFER2(pComponentPrivate->dbg, "Flusing pipe nFilled_iPipe[0]!\n");
                if (nRet == -1)
                {
                    OMX_PRBUFFER3(pComponentPrivate->dbg, "Error while reading from nFilled_iPipe\n");
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
                }

                pBufHead->nFilledLen = 0;


                pComponentPrivate->num_inp_buf_returned++;
                pComponentPrivate->sCbData.EmptyBufferDone(pComponentPrivate->pHandle,
                                                           pComponentPrivate->pHandle->pApplicationPrivate,
                                                           pBufHead);
            }
            if (FD_ISSET(pComponentPrivate->nFree_oPipe[0], &rfds))
            {
                OMX_PRBUFFER2(pComponentPrivate->dbg, "Flusing pipe nFree_oPipe[0]!\n");
                nRet = read(pComponentPrivate->nFree_oPipe[0],
                            &pBufHead,
                            sizeof(pBufHead));
                if (nRet == -1)
                {
                    OMX_PRBUFFER3(pComponentPrivate->dbg, "Error while reading from nFree_oPipe\n");
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
                }

                pBufHead->nFilledLen = 0;

                pComponentPrivate->num_out_buf_returned++;
                pComponentPrivate->sCbData.FillBufferDone(pComponentPrivate->pHandle,
                                                          pComponentPrivate->pHandle->pApplicationPrivate,
                                                          pBufHead);
            }
        }
    }

    {
        OMX_S32 nRet;
        OMX_S32 nFlush = FLUSH_PIPES;

        //ALOGD("EmptyDataPipes : cleaned up the io pipes");

        //ALOGD("Sending the flush event to the other threads");
        nRet = write(pComponentPrivate->nDsp_wait_thread_event_pipe[1], &nFlush, sizeof(OMX_COMMANDTYPE));

        //ALOGD("Waiting for the other threads to come back");
        /* Wait on the DSP Wait thread to exit */
        /*pthread_mutex_lock(&pComponentPrivate->m_DSPWait_Thread_FlushDone_mutex);
        pthread_cond_wait(&pComponentPrivate->m_DSPWait_Thread_FlushDone_Cond, &pComponentPrivate->m_DSPWait_Thread_FlushDone_mutex);
        pthread_mutex_unlock(&pComponentPrivate->m_DSPWait_Thread_FlushDone_mutex);*/

        {
            int status = -1;
            int fdmax = -1;
            fd_set rfds;
            sigset_t set;
            OMX_COMMANDTYPE eCmd = -1;

            /* Wait on the DSP Wait thread to complete the flush command */
            fdmax = pComponentPrivate->nDSP_wait_videnc_comm_thread[0];

            FD_ZERO (&rfds);
            FD_SET (pComponentPrivate->nDSP_wait_videnc_comm_thread[0], &rfds);

            sigemptyset(&set);
            sigaddset(&set,SIGALRM);
            status = pselect(fdmax+1, &rfds, NULL, NULL, NULL, &set);

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
                if (FD_ISSET(pComponentPrivate->nDSP_wait_videnc_comm_thread[0], &rfds))
                {
                    nRet = read(pComponentPrivate->nDSP_wait_videnc_comm_thread[0], &eCmd, sizeof(eCmd));
                    if (nRet == -1)
                    {
                        OMX_PRCOMM4(pComponentPrivate->dbg, "Error while reading from nDSP_wait_videnc_comm_thread\n");
                        OMX_VIDENC_SET_ERROR_BAIL(eError,
                                                  OMX_ErrorHardware,
                                                  pComponentPrivate);
                    }

                    if(FLUSH_PIPES_COMPLETED != eCmd)
                    {
                        ALOGE("Unknown command received from the DSP wait thread");
                    }
                }
            }
        }

        ALOGD("Empty Data pipes : Other threads completed the flushing");
        ALOGD("Num Inp Buf Issued %lu :: Num Inp Buf Returned %lu :: ", pComponentPrivate->num_inp_buf_issued, pComponentPrivate->num_inp_buf_returned);
        ALOGD("Num Out Buf Issued %lu :: Num Out Buf Returned %lu :: ", pComponentPrivate->num_out_buf_issued, pComponentPrivate->num_out_buf_returned);

    }

OMX_CONF_CMD_BAIL:
    return eError;
}


/*---------------------------------------------------------------------------------------*/
/**
  *  OMX_VIDENC_HandleError() will handle the error and pass the component to Invalid
  *  State, and send the event to the client.
  * @param eError - OMX_ERRORTYPE that occur.
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_Error_BadParameter   The input parameter pointer is null
 **/
/*---------------------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleError(VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                     OMX_ERRORTYPE eErrorCmp)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pComponentPrivate->bHandlingFatalError = OMX_TRUE;

    OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                             OMX_EventError,
                             eErrorCmp,
                             OMX_TI_ErrorSevere,
                             NULL);

    switch (eErrorCmp)
    {
        case OMX_ErrorBadParameter:
        case OMX_ErrorPortUnresponsiveDuringAllocation:
        case OMX_ErrorUnsupportedIndex:
        case OMX_ErrorInsufficientResources:
            goto OMX_CONF_CMD_BAIL;
        default:
            ;
    }

    pComponentPrivate->bHideEvents = OMX_TRUE;

    eError = eErrorCmp;
    pComponentPrivate->eState = OMX_StateInvalid;

    OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                             OMX_EventError,
                             OMX_ErrorInvalidState,
                             OMX_TI_ErrorCritical,
                             NULL);

OMX_CONF_CMD_BAIL:
    pComponentPrivate->bHandlingFatalError = OMX_FALSE;
    return eError;
}


/*-----------------------------------------------------------------------------*/
/**
  * Disable Port()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *
  **/
/*-----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleCommandDisablePort (VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                                   OMX_U32 nParam1)
{
    OMX_COMPONENTTYPE* pHandle = NULL;
    VIDENC_NODE* pMemoryListHead = NULL;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    VIDEOENC_PORT_TYPE* pCompPortIn = NULL;
    VIDEOENC_PORT_TYPE* pCompPortOut = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pHandle         = (OMX_COMPONENTTYPE*)pComponentPrivate->pHandle;
    pCompPortIn     = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT];
    pCompPortOut    = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT];
    pPortDefIn      = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut     = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;
    pMemoryListHead = pComponentPrivate->pMemoryListHead;

    OMX_DBG_CHECK_CMD(pComponentPrivate->dbg, pHandle, pPortDefIn, pPortDefOut);


    OMX_PRBUFFER2(pComponentPrivate->dbg, "Flushing Pipes!\n");
    eError = OMX_VIDENC_EmptyDataPipes (pComponentPrivate);
    OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRBUFFER3,
                          "Flushing pipes failed (%x).\n", eError);

    if (VIDENC_INPUT_PORT == nParam1)
    {

        while ((pPortDefIn->bPopulated))
        {
            /*Send event*/
            pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
            pthread_cond_wait(&pComponentPrivate->unpopulate_cond, &pComponentPrivate->videoe_mutex_app);
            pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
            break;
        }

        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                             OMX_EventCmdComplete,
                             OMX_CommandPortDisable,
                             VIDENC_INPUT_PORT,
                             NULL);

    }
    else if (VIDENC_OUTPUT_PORT == nParam1)
    {
        while ((pPortDefOut->bPopulated))
        {
            /*Send event*/
            pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
            pthread_cond_wait(&pComponentPrivate->unpopulate_cond, &pComponentPrivate->videoe_mutex_app);
            pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
            break;
        }
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                             OMX_EventCmdComplete,
                             OMX_CommandPortDisable,
                             VIDENC_OUTPUT_PORT,
                             NULL);

    }
    else if (OMX_ALL == nParam1)
    {
        while ((pPortDefIn->bPopulated) || (pPortDefOut->bPopulated))
        {
            /*Send events*/
            pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
            pthread_cond_wait(&pComponentPrivate->unpopulate_cond, &pComponentPrivate->videoe_mutex_app);
            pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
            break;
        }
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                             OMX_EventCmdComplete,
                             OMX_CommandPortDisable,
                             VIDENC_INPUT_PORT,
                             NULL);
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                             OMX_EventCmdComplete,
                             OMX_CommandPortDisable,
                             VIDENC_OUTPUT_PORT,
                             NULL);
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*-----------------------------------------------------------------------------*/
/**
  * Enable Port()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *
  **/
/*-----------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_HandleCommandEnablePort (VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                                  OMX_U32 nParam1)
{
    OMX_U32 nTimeout = 0x0;
    OMX_COMPONENTTYPE* pHandle = NULL;
    VIDENC_NODE* pMemoryListHead = NULL;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pHandle = (OMX_COMPONENTTYPE*)pComponentPrivate->pHandle;
    pPortDefIn      = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut     = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;
    pMemoryListHead = pComponentPrivate->pMemoryListHead;

    nTimeout = 0x0;
    if(VIDENC_INPUT_PORT == nParam1)
    {
        if (pComponentPrivate->eState != OMX_StateLoaded)
        {
            pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
            while (!pPortDefIn->bPopulated)
            {
                pthread_cond_wait(&pComponentPrivate->populate_cond, &pComponentPrivate->videoe_mutex_app);
            }
        }
        pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                             OMX_EventCmdComplete,
                                             OMX_CommandPortEnable,
                                             VIDENC_INPUT_PORT,
                                             NULL);
    }
    else if(VIDENC_OUTPUT_PORT == nParam1)
    {
        if (pComponentPrivate->eState != OMX_StateLoaded)
        {
            pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
            while(!pPortDefOut->bPopulated)
            {
                pthread_cond_wait(&pComponentPrivate->populate_cond, &pComponentPrivate->videoe_mutex_app);
            }
        }
        pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventCmdComplete,
                                         OMX_CommandPortEnable,
                                         VIDENC_OUTPUT_PORT,
                                         NULL);
    }
    else if(OMX_ALL == nParam1)
    {
        if (pComponentPrivate->eState != OMX_StateLoaded)
        {
            pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
            while(!pPortDefOut->bPopulated && !pPortDefIn->bPopulated)
            {
                pthread_cond_wait(&pComponentPrivate->populate_cond, &pComponentPrivate->videoe_mutex_app);
                break;
            }
        }
        pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventCmdComplete,
                                         OMX_CommandPortEnable,
                                         VIDENC_INPUT_PORT,
                                         NULL);
        OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                        OMX_EventCmdComplete,
                                        OMX_CommandPortEnable,
                                        VIDENC_OUTPUT_PORT,
                                        NULL);
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_OMX_VIDENC_HandleCommandFlush()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param phandle LCML_DSP_INTERFACE handle for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_HandleCommandFlush(VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                            OMX_U32 nParam1,
                                            OMX_BOOL bInternalFlush)
{
    OMX_COMPONENTTYPE* pHandle = NULL;
    VIDENC_NODE* pMemoryListHead = NULL;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    VIDEOENC_PORT_TYPE* pCompPortIn = NULL;
    VIDEOENC_PORT_TYPE* pCompPortOut = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pCompPortIn     = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT];
    pCompPortOut    = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT];
    pPortDefIn      = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut     = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;
    pHandle         = (OMX_COMPONENTTYPE*)pComponentPrivate->pHandle;
    pMemoryListHead = pComponentPrivate->pMemoryListHead;

    ALOGD("Num Inp Buf Issued %lu :: Num Inp Buf Returned %lu :: ", pComponentPrivate->num_inp_buf_issued, pComponentPrivate->num_inp_buf_returned);
    ALOGD("Num Out Buf Issued %lu :: Num Out Buf Returned %lu :: ", pComponentPrivate->num_out_buf_issued, pComponentPrivate->num_out_buf_returned);

    if (VIDENC_INPUT_PORT == nParam1)
    {

        pComponentPrivate->bFlushComplete = OMX_FALSE;

        if (bInternalFlush == OMX_FALSE)
        {
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandFlush,
                                     VIDENC_INPUT_PORT,
                                     NULL);
        }
    }
    else if (VIDENC_OUTPUT_PORT == nParam1)
    {

        pComponentPrivate->bFlushComplete = OMX_FALSE;

        if (bInternalFlush == OMX_FALSE)
        {
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandFlush,
                                     VIDENC_OUTPUT_PORT,
                                     NULL);
        }
    }
    else if(OMX_ALL == nParam1)
    {
        pComponentPrivate->bFlushComplete = OMX_FALSE;
        if (bInternalFlush == OMX_FALSE)
        {
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandFlush,
                                     VIDENC_INPUT_PORT,
                                     NULL);

            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandFlush,
                                     VIDENC_OUTPUT_PORT,
                                     NULL);
        }
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_HandleCommand()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param phandle LCML_DSP_INTERFACE handle for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSet (VIDENC_COMPONENT_PRIVATE* pComponentPrivate,
                                                OMX_U32 nParam1)
{
    VIDENC_NODE* pMemoryListHead = NULL;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pPortDefIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;
    pMemoryListHead = pComponentPrivate->pMemoryListHead;

    switch (nParam1)
    {
    case OMX_StateIdle:
        eError = OMX_VIDENC_HandleCommandStateSetIdle (pComponentPrivate);
        OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRSTATE3,
                              "Failed to move to Idle state (%x).\n", eError);
        break;
    case OMX_StateExecuting:
        eError = OMX_VIDENC_HandleCommandStateSetExecuting (pComponentPrivate);
        OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRSTATE3,
                              "Failed to move to Execute state (%x).\n", eError);
        break;
    case OMX_StateLoaded:
        eError = OMX_VIDENC_HandleCommandStateSetLoaded (pComponentPrivate);
        OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRSTATE3,
                              "Failed to move to Loaded state (%x).\n", eError);
        break;
    case OMX_StatePause:
        eError = OMX_VIDENC_HandleCommandStateSetPause (pComponentPrivate);
        OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRSTATE3,
                              "Failed to move to Pause state (%x).\n", eError);
        break;
    case OMX_StateInvalid:
            if (pComponentPrivate->eState == OMX_StateInvalid)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorSameState,
                                         OMX_TI_ErrorSevere,
                                         NULL);
            }
            else
            {
                pComponentPrivate->eState = OMX_StateInvalid;
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorInvalidState,
                                         OMX_TI_ErrorSevere,
                                         NULL);
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventCmdComplete,
                                         OMX_CommandStateSet,
                                         OMX_StateInvalid,
                                         NULL);
            }
            break;
        case OMX_StateWaitForResources:
            if (pComponentPrivate->eState == OMX_StateWaitForResources)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorSameState,
                                         OMX_TI_ErrorMinor,
                                         NULL);
            }
            else if (pComponentPrivate->eState == OMX_StateLoaded)
            {
                pComponentPrivate->eState = OMX_StateWaitForResources;
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventCmdComplete,
                                         OMX_CommandStateSet,
                                         OMX_StateWaitForResources,
                                         NULL);
            }
            else
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorIncorrectStateTransition,
                                         OMX_TI_ErrorMinor,
                                         NULL);
            }
            break;
        case OMX_StateMax:
            break;
    } /* End of Switch */

OMX_CONF_CMD_BAIL:
     ALOGD("Setting nPendingStateChangeRequests to 0");
     /* Set state change request reference count to 0 */
     /*
     ASSUMPTION: The state transition command issued from the IL client will not be multiple commands at a time.
     i.e., after a state transition command is issued to the OMX IL component, the IL client will do GetState to
           verify the completion of the state transition, although it may not wait until EventHandler callback from
           the OMX IL component.
     */
    if(pthread_mutex_lock(&pComponentPrivate->mutexStateChangeRequest)) {
       return OMX_ErrorUndefined;
    }

    pComponentPrivate->nPendingStateChangeRequests = 0;

    /* Signal the thread waiting on this*/
    pthread_cond_signal(&(pComponentPrivate->StateChangeCondition));

    if(pthread_mutex_unlock(&pComponentPrivate->mutexStateChangeRequest)) {
       return OMX_ErrorUndefined;
    }

    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_HandleCommandStateSet()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param phandle LCML_DSP_INTERFACE handle for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetIdle(VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_U8 nCount = 0;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    VIDEOENC_PORT_TYPE* pCompPortIn = NULL;
    VIDEOENC_PORT_TYPE* pCompPortOut = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;


    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pCompPortIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT];
    pCompPortOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT];
    pPortDefIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;

    switch (pComponentPrivate->eState)
    {
        case OMX_StateIdle:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorSameState,
                                     OMX_TI_ErrorMinor,
                                     NULL);
            break;
        case OMX_StateInvalid:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorIncorrectStateTransition,
                                     OMX_TI_ErrorMajor,
                                     NULL);
            break;
        case OMX_StateLoaded:
        case OMX_StateWaitForResources:
#ifdef __PERF_INSTRUMENTATION__
                PERF_Boundary(pComponentPrivate->pPERFcomp,
                              PERF_BoundaryStart | PERF_BoundarySetup);
#endif
            if ( pPortDefIn->bEnabled == OMX_TRUE || pPortDefOut->bEnabled == OMX_TRUE )
            {
                pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
                while ( (!pPortDefIn->bPopulated) || (!pPortDefOut->bPopulated))
                {
                    pthread_cond_wait(&pComponentPrivate->populate_cond, &pComponentPrivate->videoe_mutex_app);
                }
                pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
            }

            /* Make sure the DSP node has been deleted first to cover
                   any idle->loaded->idle or idle->wfr->idle combinations */
            if (pComponentPrivate->bCodecStarted == OMX_TRUE ||
                pComponentPrivate->bCodecLoaded == OMX_TRUE)
            {

                OMX_IVIDENC1_delete(pComponentPrivate->pCodecHandle);

                pComponentPrivate->pCodecHandle = NULL;

                pComponentPrivate->bCodecStarted = OMX_FALSE;
                pComponentPrivate->bCodecLoaded = OMX_FALSE;
            }


#ifdef RESOURCE_MANAGER_ENABLED

            OMX_PRMGR2(pComponentPrivate->dbg, "Setting CallBack In Video Encoder component\n");
            pComponentPrivate->cRMCallBack.RMPROXY_Callback = (void*)OMX_VIDENC_ResourceManagerCallBack;
            switch (pPortDefOut->format.video.eCompressionFormat)
            {
                case OMX_VIDEO_CodingAVC:
                    ALOGD("^^^ OMX_VIDEO_CodingAVC requesting for 800");
                    eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                         RMProxy_RequestResource,
                         OMX_720P_Encode_COMPONENT,
                         750, /* 660, 430 */
                         3456,
                         &(pComponentPrivate->cRMCallBack));

                    break;
                case OMX_VIDEO_CodingMPEG4:
                case OMX_VIDEO_CodingH263:
                    ALOGD("^^^ OMX_VIDEO_CodingMPEG4 or OMX_VIDEO_CodingH263 requesting for 660");
                    eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                             RMProxy_RequestResource,
                             OMX_720P_Encode_COMPONENT,
                             660,/* 660, 430 */
                             3456,
                             &(pComponentPrivate->cRMCallBack));

                    break;

                default:
                OMX_PRSTATE4(pComponentPrivate->dbg, "Unsupported compression format (%d)\n",
                             pPortDefOut->format.video.eCompressionFormat);
                    OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorUnsupportedSetting);
            }

            if (eError != OMX_ErrorNone)
            {
                ALOGE("OMX_VIDENC_HandleCommandStateSetIdle : %s : %d Could not set on Resource Manager", __FUNCTION__, __LINE__);
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorHardware,
                                         OMX_TI_ErrorSevere,
                                         NULL);
            }

            ALOGD("^^^ Request to resource manager succeeded");

            /* Resource Manager Proxy Calls */
            if (pCompPortOut->pPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H264_Encode_COMPONENT, 0); */
                OMX_PRMGR2(pComponentPrivate->dbg, "RMProxy_SendCommand: Setting state to Idle from Loaded\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateIdle,
                                             3456,
                                             NULL);
            }
            else if (pCompPortOut->pPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_MPEG4_Encode_COMPONENT, 0); */
                OMX_PRMGR2(pComponentPrivate->dbg, "RMProxy_SendCommand: Setting state to Idle from Loaded\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateIdle,
                                             3456,
                                             NULL);
            }
            else if (pCompPortOut->pPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H263_Encode_COMPONENT, 0); */
                OMX_PRMGR2(pComponentPrivate->dbg, "RMProxy_SendCommand: Setting state to Idle from Loaded\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_H263_Encode_COMPONENT,
                                             OMX_StateIdle,
                                             3456,
                                             NULL);
            }
            if (eError != OMX_ErrorNone)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorHardware,
                                         OMX_TI_ErrorSevere,
                                         NULL);
            }

            if (eError == OMX_ErrorNone)
            {
                pComponentPrivate->eState = OMX_StateIdle;
            //TODO: Add comment why this is moved here
            eError = OMX_VIDENC_Create(pComponentPrivate);
            pComponentPrivate->bCodecLoaded = OMX_TRUE;

            if(eError != OMX_ErrorNone)
            {
                pComponentPrivate->eState = OMX_StateInvalid;

                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorInvalidState,
                                         OMX_TI_ErrorCritical,
                                         NULL);
                goto OMX_CONF_CMD_BAIL;
            }

            OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRDSP4,
                                  "OMX VE UTILS >> Failed to create component\n");

#ifdef __PERF_INSTRUMENTATION__
                PERF_Boundary(pComponentPrivate->pPERFcomp,
                            PERF_BoundaryComplete | PERF_BoundarySetup);
#endif
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventCmdComplete,
                                         OMX_CommandStateSet,
                                         OMX_StateIdle,
                                         NULL);
            }
            else if (eError == OMX_ErrorInsufficientResources)
            {
                pComponentPrivate->eState = OMX_StateWaitForResources;
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorInsufficientResources,
                                         OMX_TI_ErrorMajor,
                                         NULL);
            }
#else /* WinCE MM will not use Linux RM, so do this... */
            pComponentPrivate->eState = OMX_StateIdle;
        eError = OMX_VIDENC_Create(pComponentPrivate);
            pComponentPrivate->bCodecLoaded = OMX_TRUE;
#ifdef __PERF_INSTRUMENTATION__
            PERF_Boundary(pComponentPrivate->pPERFcomp,
                          PERF_BoundaryComplete | PERF_BoundarySetup);
#endif
            if(eError != OMX_ErrorNone)
            {
                pComponentPrivate->eState = OMX_StateInvalid;

                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorInvalidState,
                                         OMX_TI_ErrorCritical,
                                         NULL);
                goto OMX_CONF_CMD_BAIL;
            }

            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandStateSet,
                                     OMX_StateIdle,
                                     NULL);
#endif
            break;
        case OMX_StateExecuting:
        case OMX_StatePause:
             pComponentPrivate->bIsStopped = OMX_TRUE;

             ALOGD("Num Inp Buf Issued %lu :: Num Inp Buf Returned %lu :: ", pComponentPrivate->num_inp_buf_issued, pComponentPrivate->num_inp_buf_returned);
             ALOGD("Num Out Buf Issued %lu :: Num Out Buf Returned %lu :: ", pComponentPrivate->num_out_buf_issued, pComponentPrivate->num_out_buf_returned);

#ifdef __PERF_INSTRUMENTATION__
            PERF_Boundary(pComponentPrivate->pPERF,
                          PERF_BoundaryComplete | PERF_BoundarySteadyState);
#endif

            PROFILE_FINAL_PRINT;
            h264profile_print_final_stats();
            /* To stop the codec nothing to be done */
            OMX_PRDSP2(pComponentPrivate->dbg, "MMCodecControlStop called...\n");

            pComponentPrivate->bDSPStopAck = OMX_FALSE;

            for (nCount = 0; nCount < pPortDefIn->nBufferCountActual; nCount++)
            {
						/*ALOGD("Buffer[%d]:port[%d] -> %p [memset %lu bytes]",
                                  nCount,
                                  VIDENC_INPUT_PORT,
                                  pCompPortIn->pBufferPrivate[nCount]->pBufferHdr,
                                  pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->nAllocLen);*/
                OMX_PRBUFFER2(pComponentPrivate->dbg, "Buffer[%d]:port[%d] -> %p [OWNER = %d]\n",
                          nCount,
                          VIDENC_INPUT_PORT,
                          pCompPortIn->pBufferPrivate[nCount]->pBufferHdr,
                          pCompPortIn->pBufferPrivate[nCount]->eBufferOwner);

                if (pCompPortIn->pBufferPrivate[nCount]->eBufferOwner == VIDENC_BUFFER_WITH_DSP ||
                    pCompPortIn->pBufferPrivate[nCount]->eBufferOwner == VIDENC_BUFFER_WITH_COMPONENT)
                {
                    OMX_PRBUFFER1(pComponentPrivate->dbg, "Buffer[%d]:port[%d] -> %p [SEND BACK TO SUPPLIER]\n",
                              nCount,
                              VIDENC_INPUT_PORT,
                              pCompPortIn->pBufferPrivate[nCount]->pBufferHdr);

                    if (pCompPortIn->hTunnelComponent == NULL)
                    {

                        pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->nFilledLen = 0;
						/*ALOGD("iFBuffer[%d]:port[%d] -> %p [memset %lu bytes]",
                                  nCount,
                                  VIDENC_INPUT_PORT,
                                  pCompPortIn->pBufferPrivate[nCount]->pBufferHdr,
                                  pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->nAllocLen);*/
                        OMX_PRBUFFER1(pComponentPrivate->dbg, "Buffer[%d]:port[%d] -> %p [memset %lu bytes]\n",
                                  nCount,
                                  VIDENC_INPUT_PORT,
                                  pCompPortIn->pBufferPrivate[nCount]->pBufferHdr,
                                  pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->nAllocLen);

/*                        memset(pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->pBuffer,*/
/*                               0x0,*/
/*                               pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->nAllocLen);*/

                        pCompPortIn->pBufferPrivate[nCount]->eBufferOwner = VIDENC_BUFFER_WITH_CLIENT;
#ifdef __PERF_INSTRUMENTATION__
                        PERF_SendingFrame(pComponentPrivate->pPERFcomp,
                                          pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->pBuffer,
                                          0,
                                          PERF_ModuleHLMM);
#endif
                    }
                    else
                    {
                        pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->nFilledLen = 0;
                        pCompPortIn->pBufferPrivate[nCount]->eBufferOwner = VIDENC_BUFFER_WITH_TUNNELEDCOMP;
#ifdef __PERF_INSTRUMENTATION__
                        PERF_SendingFrame(pComponentPrivate->pPERFcomp,
                                          pCompPortIn->pBufferPrivate[nCount]->pBufferHdr->pBuffer,
                                          0,
                                          PERF_ModuleLLMM);
#endif
                        eError = OMX_FillThisBuffer(pCompPortIn->hTunnelComponent,
                                                    pCompPortIn->pBufferPrivate[nCount]->pBufferHdr);
                        OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRBUFFER4,
                                          "FillThisBuffer failed (%x).\n", eError);
                    }
                }
            }



#ifdef RESOURCE_MANAGER_ENABLED /* Resource Manager Proxy Calls */
            if (pCompPortOut->pPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H264_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateIdle,
                                             3456,
                                             NULL);
            }
            else if (pCompPortOut->pPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_MPEG4_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateIdle,
                                             3456,
                                             NULL);
            }
            else if (pCompPortOut->pPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H263_Encode_COMPONENT, 0); */
                OMX_PRMGR2(pComponentPrivate->dbg, "Setting Idle state from Executing to RMProxy\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_H263_Encode_COMPONENT,
                                             OMX_StateIdle,
                                             3456,
                                             NULL);
            }
            if (eError != OMX_ErrorNone)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorHardware,
                                         OMX_TI_ErrorMajor,
                                         NULL);
            }

#endif

            OMX_PRBUFFER2(pComponentPrivate->dbg, "Flushing Pipes!\n");
            eError = OMX_VIDENC_EmptyDataPipes (pComponentPrivate);
            OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg, OMX_PRBUFFER3,
                              "Flushing pipes failed (%x).\n", eError);

            pComponentPrivate->eState = OMX_StateIdle;
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandStateSet,
                                     OMX_StateIdle,
                                     NULL);
            ALOGD("LOADED_TO_IDLE SUCCESSFULLY Handled");

            break;
        default:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorIncorrectStateTransition,
                                     OMX_TI_ErrorMinor,
                                     NULL);
    }
OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_HandleCommandStateSet()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param phandle LCML_DSP_INTERFACE handle for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetExecuting(VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pPortDefIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;

    switch (pComponentPrivate->eState)
    {
        case OMX_StateExecuting:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorSameState, OMX_TI_ErrorMinor, NULL);
            break;
        case OMX_StateIdle:

            /* reset the buffers triggered to DSP index to zero */
            pComponentPrivate->dsp720PEncoderBufferTrigIndex = 0;
            pComponentPrivate->bIsStopped = OMX_FALSE;
            OMX_CONF_CIRCULAR_BUFFER_RESTART(pComponentPrivate->sCircularBuffer);
        case OMX_StatePause:


            if (pComponentPrivate->bCodecStarted == OMX_FALSE)
            {
                pComponentPrivate->bCodecStarted = OMX_TRUE;
            }

#ifdef RESOURCE_MANAGER_ENABLED /* Resource Manager Proxy Calls */
            if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
            {
                /* TODO: Disable RM Send for now */
            OMX_PRMGR2(pComponentPrivate->dbg, "Setting executing state to RMProxy\n");
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H264_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateExecuting,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
            {
                /* TODO: Disable RM Send for now */
            OMX_PRMGR2(pComponentPrivate->dbg, "Setting executing state to RMProxy\n");
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_MPEG4_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateExecuting,
                                             3456,
                                             NULL);

            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H263_Encode_COMPONENT, 0); */
            OMX_PRMGR2(pComponentPrivate->dbg, "Setting executing state to RMProxy\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_H263_Encode_COMPONENT,
                                             OMX_StateExecuting,
                                             3456,
                                             NULL);
            }
            if (eError != OMX_ErrorNone)
            {
                pComponentPrivate->eState = OMX_StateWaitForResources;
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorHardware,
                                         OMX_TI_ErrorMajor,
                                         NULL);
            }
#endif
            pComponentPrivate->eState = OMX_StateExecuting;
#ifdef __PERF_INSTRUMENTATION__
            PERF_Boundary(pComponentPrivate->pPERFcomp,
                          PERF_BoundaryStart | PERF_BoundarySteadyState);
#endif
            /*Send state change notificaiton to Application*/
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandStateSet,
                                     OMX_StateExecuting,
                                     NULL);
            break;
        default:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorIncorrectStateTransition,
                                     OMX_TI_ErrorMinor,
                                     NULL);
    }
OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_HandleCommandStateSetPause()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param phandle LCML_DSP_INTERFACE handle for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetPause (VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pPortDefIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;


    switch (pComponentPrivate->eState)
    {
        case OMX_StatePause:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorSameState,
                                     OMX_TI_ErrorMinor,
                                     NULL);
            break;
        case OMX_StateIdle:
        case OMX_StateExecuting:
#ifdef __PERF_INSTRUMENTATION__
            PERF_Boundary(pComponentPrivate->pPERFcomp,
                          PERF_BoundaryComplete | PERF_BoundarySteadyState);
#endif

            OMX_PRDSP2(pComponentPrivate->dbg, "MMCodecControlPaused called...\n");


            pComponentPrivate->bDSPStopAck = OMX_FALSE;

            pComponentPrivate->eState = OMX_StatePause;

            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandStateSet,
                                     OMX_StatePause,
                                     NULL);
            break;
        default:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorIncorrectStateTransition,
                                     OMX_TI_ErrorMinor,
                                     NULL);
    }

OMX_CONF_CMD_BAIL:
    return eError;
}

/*----------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_HandleCommandStateSetLoaded()
  *
  * Called by component thread, handles commands sent by the app.
  *
  * @param phandle LCML_DSP_INTERFACE handle for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*----------------------------------------------------------------------------*/
OMX_ERRORTYPE OMX_VIDENC_HandleCommandStateSetLoaded (VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    pPortDefIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;

    switch (pComponentPrivate->eState)
    {
        case OMX_StateLoaded:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorSameState,
                                     OMX_TI_ErrorMinor,
                                     NULL);
            break;
        case OMX_StateWaitForResources:
        OMX_PRSTATE2(pComponentPrivate->dbg, "Transitioning from WFR to Loaded\n");
    #ifdef RESOURCE_MANAGER_ENABLED
            if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H264_Encode_COMPONENT, 0); */
            OMX_PRMGR2(pComponentPrivate->dbg, "RMProxy_SendCommand: Setting state to Loaded from WFR\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateLoaded,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_MPEG4_Encode_COMPONENT, 0); */
            OMX_PRMGR2(pComponentPrivate->dbg, "RMProxy_SendCommand: Setting state to Loaded from WFR\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_720P_Encode_COMPONENT,
                                             OMX_StateLoaded,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H263_Encode_COMPONENT, 0); */
            OMX_PRMGR2(pComponentPrivate->dbg, "RMProxy_SendCommand: Setting state to Loaded from WFR\n");
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_StateSet,
                                             OMX_H263_Encode_COMPONENT,
                                             OMX_StateLoaded,
                                             3456,
                                             NULL);
            }
            if (eError != OMX_ErrorNone)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorHardware,
                                         OMX_TI_ErrorMajor,
                                         NULL);
                break;
            }
    #endif
            pComponentPrivate->eState = OMX_StateLoaded;

            #ifdef __PERF_INSTRUMENTATION__
                PERF_Boundary(pComponentPrivate->pPERFcomp,
                              PERF_BoundaryComplete | PERF_BoundaryCleanup);
            #endif
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandStateSet,
                                     OMX_StateLoaded,
                                     NULL);
            break;
        case OMX_StateIdle:
            OMX_PRSTATE2(pComponentPrivate->dbg, "Transitioning from Idle to Loaded\n");
            ALOGD("Num Inp Buf Issued %lu :: Num Inp Buf Returned %lu :: ", pComponentPrivate->num_inp_buf_issued, pComponentPrivate->num_inp_buf_returned);
            ALOGD("Num Out Buf Issued %lu :: Num Out Buf Returned %lu :: ", pComponentPrivate->num_out_buf_issued, pComponentPrivate->num_out_buf_returned);

#ifdef __PERF_INSTRUMENTATION__
            PERF_Boundary(pComponentPrivate->pPERFcomp,
                          PERF_BoundaryStart | PERF_BoundaryCleanup);
#endif
             // TODO : write comment why this code is moved here
            //  intent : MOVE_DELETE_BEFORE_CHANGING_OPP_LEVEL
             /* Make sure the DSP node has been deleted */
             if (pComponentPrivate->bCodecStarted == OMX_TRUE || pComponentPrivate->bCodecLoaded == OMX_TRUE)
             {

                PROFILE_FINAL_PRINT;
                h264profile_print_final_stats();
                OMX_IVIDENC1_delete(pComponentPrivate->pCodecHandle);
                pComponentPrivate->pCodecHandle = NULL;

                 pComponentPrivate->bCodecStarted = OMX_FALSE;
                 pComponentPrivate->bCodecLoaded = OMX_FALSE;
             }

            if ( pPortDefIn->bEnabled == OMX_TRUE || pPortDefOut->bEnabled == OMX_TRUE )
            {
                pthread_mutex_lock(&pComponentPrivate->videoe_mutex_app);
                while ( (pPortDefIn->bPopulated) || (pPortDefOut->bPopulated))
                {
                    pthread_cond_wait(&pComponentPrivate->unpopulate_cond, &pComponentPrivate->videoe_mutex_app);
                }
                pthread_mutex_unlock(&pComponentPrivate->videoe_mutex_app);
            }

#ifdef RESOURCE_MANAGER_ENABLED /* Resource Manager Proxy Calls */
            if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H264_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_FreeResource,
                                             OMX_720P_Encode_COMPONENT,
                                             0,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_MPEG4_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_FreeResource,
                                             OMX_720P_Encode_COMPONENT,
                                             0,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
            {
                /* TODO: Disable RM Send for now */
                /* eError = RMProxy_SendCommand(pHandle, RMProxy_RequestResource, OMX_H263_Encode_COMPONENT, 0); */
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_FreeResource,
                                             OMX_H263_Encode_COMPONENT,
                                             0,
                                             3456,
                                             NULL);
            }
            if (eError != OMX_ErrorNone)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                         OMX_EventError,
                                         OMX_ErrorHardware,
                                         OMX_TI_ErrorMajor,
                                         NULL);
            }
#endif


             OMX_CONF_BAIL_IF_ERROR(eError);

#ifdef __KHRONOS_CONF__
            pComponentPrivate->bPassingIdleToLoaded = OMX_FALSE;
#endif
            pComponentPrivate->eState = OMX_StateLoaded;


#ifdef __PERF_INSTRUMENTATION__
                PERF_Boundary(pComponentPrivate->pPERFcomp,
                              PERF_BoundaryComplete | PERF_BoundaryCleanup);
#endif
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventCmdComplete,
                                     OMX_CommandStateSet,
                                     OMX_StateLoaded,
                                     NULL);

            break;
        default:
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,
                                     OMX_EventError,
                                     OMX_ErrorIncorrectStateTransition,
                                     OMX_TI_ErrorMinor,
                                     NULL);
    }
OMX_CONF_CMD_BAIL:
    return eError;
}

/*---------------------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_Process_FilledOutBuf()
  *
  * Called by component thread, handles filled output buffers from DSP.
  *
  * @param pComponentPrivate private component structure for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*---------------------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_Process_FilledOutBuf(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_BUFFERHEADERTYPE* pBufHead)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    VIDENC_BUFFER_PRIVATE* pBufferPrivate = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);
    //ALOGD("^^^ Entering OMX_VIDENC_Process_FilledOutBuf buffer 0x%X private 0x%X", pBufHead, pBufHead->pOutputPortPrivate);

    pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;

    if (pComponentPrivate->bCodecStarted == OMX_TRUE)
    {

        pBufferPrivate = pBufHead->pOutputPortPrivate;

        //ALOGD("^^^ OMX_VIDENC_Process_FilledOutBuf buffer initing");
        if((pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4) ||
            (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingH263) ||
            pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
        {
            pBufHead->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
        }

        //ALOGD("^^^ OMX_VIDENC_Process_FilledOutBuf buffer check for eos");
        if (pBufHead->nFlags & OMX_BUFFERFLAG_EOS)
        {
            /* trigger event handler */
            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,  OMX_EventBufferFlag, 0x1, pBufHead->nFlags, NULL);

            /* clear flag */
            pComponentPrivate->nFlags = 0;
        }

        //ALOGD("^^^ OMX_VIDENC_Process_FilledOutBuf buffer check for mark data");
        if (pBufHead->pMarkData != NULL)
        {
            /* trigger event handler if we are supposed to */
            if (pBufHead->hMarkTargetComponent == pComponentPrivate->pHandle &&
                pBufHead->pMarkData)
            {
                OMX_VIDENC_EVENT_HANDLER(pComponentPrivate,  OMX_EventMark, 0x0, 0x0,
                                        pBufHead->pMarkData);
            }
        }
        //ALOGD("^^^ OMX_VIDENC_Process_FilledOutBuf buffer setting ownership 0x%X", pBufferPrivate);
        pBufferPrivate->eBufferOwner = VIDENC_BUFFER_WITH_CLIENT;
#ifdef __PERF_INSTRUMENTATION__
        PERF_SendingBuffer(pComponentPrivate->pPERFcomp,
                           pBufHead->pBuffer,
                           pBufHead->nFilledLen,
                           PERF_ModuleHLMM);
#endif

        /*{
            struct timeval currTime;
            gettimeofday(&currTime, NULL);
            currTime.tv_sec = currTime.tv_sec * 1000 * 1000;
            currTime.tv_usec += currTime.tv_sec;

            ALOGD("FTBD %lu", currTime.tv_usec);
        }*/

        pComponentPrivate->num_out_buf_returned++;


/*Sumukh >>> Delete this code / modify it after SEI NAL units issue is resolved */
#if 0
      //if(OMX_FALSE == pComponentPrivate->bHeaderGenerate)
      {
		int offset = 0;
		int startOffset = 0;
		int endOffset = 0;
		char *ptr = pBufHead->pBuffer;

		while(pBufHead->nFilledLen > (offset + 4))
        {
			if(ptr[offset] == 0x00)
			{
				int strip = 0;

				if((ptr[offset] == 0x00) && (ptr[offset + 1] == 0x00) && (ptr[offset + 2] == 0x00) && (ptr[offset + 3] == 0x01))
					strip = 1;

				if((0 == strip) && (offset > 1))
				{
					offset--;
					if((ptr[offset] == 0x00) && (ptr[offset + 1] == 0x00) && (ptr[offset + 2] == 0x00) && (ptr[offset + 3] == 0x01))
						strip = 1;
					else
						offset++;
				}

				if(strip == 1)
				{

						startOffset = endOffset;
						endOffset   = offset;
					ALOGD("%x %x %x %x %x %x %x %x", ptr[startOffset -2], ptr[startOffset -1], ptr[startOffset], ptr[startOffset+1], ptr[startOffset+2], ptr[startOffset+3], ptr[startOffset+4], ptr[startOffset+5]);
					ALOGD("pBufHead->nFilledLen = %d", pBufHead->nFilledLen);
					if(offset != 0)
					{
						unsigned int size = endOffset - startOffset - 4;
						pBufHead->pBuffer[startOffset] = size >> 24;
						pBufHead->pBuffer[startOffset + 1] = (size >> 16) & (0xff);
						pBufHead->pBuffer[startOffset + 2] = (size >> 8) & (0xff);
						pBufHead->pBuffer[startOffset + 3] = (size) & (0xff);
						ALOGD("NAL size = %d", size);
					}

					//memmove((void *)(ptr + offset), (const void *)(ptr + offset + 4),pBufHead->nFilledLen - offset - 4);
					//pBufHead->nFilledLen = pBufHead->nFilledLen - 4;

					ALOGD("%x %x %x %x %x %x %x %x", ptr[startOffset -2], ptr[startOffset -1], ptr[startOffset], ptr[startOffset+1], ptr[startOffset+2], ptr[startOffset+3], ptr[startOffset+4], ptr[startOffset+5]);
					ALOGD("pBufHead->nFilledLen = %d", pBufHead->nFilledLen);
				}
			}
				offset = offset + 2;
		}

		startOffset = endOffset;
		endOffset   = pBufHead->nFilledLen;

		unsigned int size = endOffset - startOffset - 4;
		pBufHead->pBuffer[startOffset] = size >> 24;
		pBufHead->pBuffer[startOffset + 1] = (size >> 16) & (0xff);
		pBufHead->pBuffer[startOffset + 2] = (size >> 8) & (0xff);
		pBufHead->pBuffer[startOffset + 3] = (size) & (0xff);
	}
	/*else
	{
		int offset = 0;
		char *ptr = pBufHead->pBuffer;

		while(pBufHead->nFilledLen > (offset + 4))
        {
			if(ptr[offset] == 0x00)
			{
				int strip = 0;

				if((ptr[offset] == 0x00) && (ptr[offset + 1] == 0x00) && (ptr[offset + 2] == 0x00) && (ptr[offset + 3] == 0x01))
					strip = 1;

				if((0 == strip) && (offset > 1))
				{
					offset--;
					if((ptr[offset] == 0x00) && (ptr[offset + 1] == 0x00) && (ptr[offset + 2] == 0x00) && (ptr[offset + 3] == 0x01))
						strip = 1;
					else
						offset++;
				}

				if(strip == 1)
				{

					ALOGD("Header pBufHead->nFilledLen = %d", pBufHead->nFilledLen);

					memmove((void *)(ptr + offset), (const void *)(ptr + offset + 4),pBufHead->nFilledLen - offset - 4);
					pBufHead->nFilledLen = pBufHead->nFilledLen - 4;

					pBufHead->pBuffer[0] = (pBufHead->nFilledLen -2) >> 8;
					pBufHead->pBuffer[1] = (pBufHead->nFilledLen ) & (0xff);

					ALOGD("Header pBufHead->nFilledLen = %d", pBufHead->nFilledLen);
				}
			}
				offset = offset + 2;
		}
	}*/

		/*pBufHead->pBuffer[0] = (pBufHead->nFilledLen - 4) >> 24;
		pBufHead->pBuffer[1] = ((pBufHead->nFilledLen - 4) >> 16) & (0xff);
		pBufHead->pBuffer[2] = ((pBufHead->nFilledLen - 4) >> 8) & (0xff);
		pBufHead->pBuffer[3] = ((pBufHead->nFilledLen - 4)) & (0xff);*/
#endif
/*Shr dump code
{
ALOGD("SHR :: DUMP_O/P_BUF Size=%d",pBufHead->nFilledLen);
	FILE *fp;
	fp = fopen("/tmp/h264_fbd.dump", "ab");
	fwrite(pBufHead->pBuffer, pBufHead->nFilledLen, 1, fp);
	fclose(fp);
} 	*/


        pComponentPrivate->sCbData.FillBufferDone(pComponentPrivate->pHandle,
                                                  pComponentPrivate->pHandle->pApplicationPrivate,
                                                  pBufHead);
    }
    else
    {
         //ALOGD("^^^ OMX_VIDENC_Process_FilledOutBuf codec not started");
    }
OMX_CONF_CMD_BAIL:
    //ALOGD("^^^ Exiting OMX_VIDENC_Process_FilledOutBuf buffer 0x%X", pBufHead);
    return eError;
}

/*---------------------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_Process_FreeInBuf()
  *
  * Called by component thread, handles free input buffers from DSP.
  *
  * @param pComponentPrivate private component structure for this instance of the component
  *
  * @retval OMX_ErrorNone                  success, ready to roll
  *         OMX_ErrorInsufficientResources if the malloc fails
  **/
/*---------------------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_Process_FreeInBuf(VIDENC_COMPONENT_PRIVATE* pComponentPrivate, OMX_BUFFERHEADERTYPE* pBufHead)
{
//ALOGD("Entering OMX_VIDENC_Process_FreeInBuf");
    VIDENC_NODE* pMemoryListHead = NULL;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_HANDLETYPE hTunnelComponent = NULL;
    VIDENC_BUFFER_PRIVATE* pBufferPrivate = NULL;

    OMX_CONF_CHECK_CMD(pComponentPrivate, 1, 1);

    hTunnelComponent = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->hTunnelComponent;
    pMemoryListHead = pComponentPrivate->pMemoryListHead;

    /*pBufHead is checked for NULL*/
    OMX_DBG_CHECK_CMD(pComponentPrivate->dbg, pBufHead, 1, 1);
    pBufferPrivate = pBufHead->pInputPortPrivate;

    if (hTunnelComponent != NULL)
    {
        pBufferPrivate->eBufferOwner = VIDENC_BUFFER_WITH_TUNNELEDCOMP;
#ifdef __PERF_INSTRUMENTATION__
        PERF_SendingFrame(pComponentPrivate->pPERFcomp,
                          PREF(pBufHead,pBuffer),
                          pBufHead->nFilledLen,
                          PERF_ModuleLLMM);
#endif

        eError = OMX_FillThisBuffer(hTunnelComponent, pBufHead);
        OMX_DBG_BAIL_IF_ERROR(eError, pComponentPrivate->dbg,
                              OMX_PRBUFFER4, "FillThisBuffer failed (%x)", eError);
    }
    else
    {

        pBufferPrivate->eBufferOwner = VIDENC_BUFFER_WITH_CLIENT;
#ifdef __PERF_INSTRUMENTATION__
        PERF_SendingFrame(pComponentPrivate->pPERFcomp,
                          PREF(pBufHead,pBuffer),
                          0,
                          PERF_ModuleHLMM);
#endif

        /*{
            struct timeval currTime;
            gettimeofday(&currTime, NULL);
            currTime.tv_sec = currTime.tv_sec * 1000 * 1000;
            currTime.tv_usec += currTime.tv_sec;

            ALOGD("ETBD %lu", currTime.tv_usec);
        }*/

        pComponentPrivate->num_inp_buf_returned++;
        pComponentPrivate->sCbData.EmptyBufferDone(pComponentPrivate->pHandle,
                                              pComponentPrivate->pHandle->pApplicationPrivate,
                                              pBufHead);
   }

OMX_CONF_CMD_BAIL:
    return eError;
}




#ifdef RESOURCE_MANAGER_ENABLED
/*-----------------------------------------------------------------------------*/
/**
  * OMX_VIDENC_ResourceManagerCallBack()
  *
  * Called from Resource Manager()
  *
 *
  *
  **/
/*-----------------------------------------------------------------------------*/
void OMX_VIDENC_ResourceManagerCallBack(RMPROXY_COMMANDDATATYPE cbData)
{
    OMX_COMMANDTYPE Cmd = OMX_CommandStateSet;
    OMX_STATETYPE state = OMX_StateIdle;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)cbData.hComponent;
    VIDENC_COMPONENT_PRIVATE *pCompPrivate = NULL;

    pCompPrivate = (VIDENC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
    OMX_PRMGR2(pCompPrivate->dbg, "OMX_VIDENC_ResourceManagerCallBack\n");
    OMX_PRMGR2(pCompPrivate->dbg, "Arguments:\ncbData.RM_Error = %dcbData.RM_Cmd = %d\n", *(cbData.RM_Error), cbData.RM_Cmd);
    if (*(cbData.RM_Error) == OMX_ErrorResourcesPreempted)
    {
        if (pCompPrivate->eState== OMX_StateExecuting ||
            pCompPrivate->eState == OMX_StatePause)
        {

        pCompPrivate->sCbData.EventHandler (
                            pHandle, pHandle->pApplicationPrivate,
                            OMX_EventError,
                           OMX_ErrorResourcesPreempted,OMX_TI_ErrorMinor,
                            "Componentn Preempted\n");

            OMX_PRSTATE2(pCompPrivate->dbg, "Send command to Idle from RM CallBack\n");
        OMX_SendCommand(pHandle, Cmd, state, NULL);
        pCompPrivate->bPreempted = 1;

        }
    }
    else if (*(cbData.RM_Error) == OMX_RmProxyCallback_ResourcesAcquired)
    {
        pCompPrivate->sCbData.EventHandler (
                            pHandle, pHandle->pApplicationPrivate,
                            OMX_EventResourcesAcquired, 0,0,
                            NULL);
        OMX_PRSTATE2(pCompPrivate->dbg, "Send command to Executing from RM CallBack\n");
        OMX_SendCommand(pHandle, Cmd, OMX_StateExecuting, NULL);
    }
    else if (*(cbData.RM_Error) == OMX_RmProxyCallback_FatalError)
    {
        OMX_ERROR4(pCompPrivate->dbg, "Fatal Error from DSP via RM\n");
        OMX_VIDENC_FatalErrorRecover(pCompPrivate);
    }

}
#endif

/*---------------------------------------------------------------------------------------*/
/**
  *  Function to create split encoder
  *
  *
  *
  * @retval OMX_NoError              Success, ready to roll
  *
  **/
/*---------------------------------------------------------------------------------------*/

OMX_ERRORTYPE OMX_VIDENC_Create(VIDENC_COMPONENT_PRIVATE *pComponentPrivate)
{
//ALOGD("Entering OMX_VIDENC_Create ");
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE *pOutPortDef;
    //IVIDENC1_Params sParams;
    ITTVIDENC1_IMOD_Params_t sParams;
    IVIDENC1_DynamicParams *pDynParams;
    IVIDENC1_Status        sStatus;
    XDAS_Int32             nStatus;
    XDAS_Int32             inputChromaFormat;
    int nSrcFrameRate, nTgtFrameRate;
    int nFrameWidth, nFrameHeight;

    /* Derive local variables */
    pDynParams    = &pComponentPrivate->sDynParams;
    pInPortDef    = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    pOutPortDef   = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;
    nFrameWidth   = pInPortDef->format.video.nFrameWidth;
    nFrameHeight  = pInPortDef->format.video.nFrameHeight;

    pComponentPrivate->nTargetFrameRate = pInPortDef->format.video.xFramerate;
    if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        pComponentPrivate->pQuantization->nQpI = 28;
        pComponentPrivate->pQuantization->nQpP = 28;
        pComponentPrivate->pQuantization->nQpB = 28;
    }
    else if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
             pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
    {
        pComponentPrivate->pQuantization->nQpI = 7;
        pComponentPrivate->pQuantization->nQpP = 7;
        pComponentPrivate->pQuantization->nQpB = 7;
    }
    pComponentPrivate->nTargetBitRate = pOutPortDef->format.video.nBitrate;


    /* Source frame rate is set same as Target frame rate */
    nSrcFrameRate = (pComponentPrivate->nTargetFrameRate >> 16);
    nTgtFrameRate = (pComponentPrivate->nTargetFrameRate >> 16);
    inputChromaFormat = pInPortDef->format.video.eColorFormat;

    nSrcFrameRate = (nSrcFrameRate * 1000);
    nTgtFrameRate = (nTgtFrameRate * 1000);
    switch(inputChromaFormat)
    {
        case OMX_COLOR_FormatYUV420Planar:
            inputChromaFormat = XDM_YUV_420P;
            break;
        case OMX_COLOR_FormatCbYCrY:
            inputChromaFormat = XDM_YUV_422ILE;
            break;
        case OMX_COLOR_FormatYCbYCr:
            inputChromaFormat = IVEA_YUV_422I_LE_16;
            break;
        default:
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
    }


    /* Set Create time parameters */
    sParams.s_ividenc_params.size                  = sizeof(IVIDENC1_Params);
    sParams.s_ividenc_params.dataEndianness        = XDM_BYTE;
    sParams.s_ividenc_params.encodingPreset        = 0;
    sParams.s_ividenc_params.inputChromaFormat     = inputChromaFormat;
    sParams.s_ividenc_params.inputContentType      = IVIDEO_PROGRESSIVE;
    sParams.s_ividenc_params.maxBitRate            = 20000000;
    sParams.s_ividenc_params.maxFrameRate          = 30000;
    sParams.s_ividenc_params.maxHeight             = nFrameHeight;
    sParams.s_ividenc_params.maxWidth              = nFrameWidth;
    sParams.s_ividenc_params.maxInterFrameInterval = 1;
    sParams.s_ividenc_params.rateControlPreset     = 2;//2=Non Low Delay, 5=Const QP
    sParams.s_ividenc_params.reconChromaFormat     = XDM_CHROMA_NA;

    if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        sParams.s_ividenc_params.size                  = sizeof(ITTVIDENC1_IMOD_Params_t);
        IMP4E_SET_CODING_TOOLS_TO_DEFAULT(sParams.s_ext.i4_coding_tools1);
        IMP4E_SET_CODING_TOOLS_TO_DEFAULT(sParams.s_ext.i4_coding_tools2);

        sParams.s_ext.i4_size = sizeof(ivea_ext_params_t);
        sParams.s_ext.i4_min_tgt_dimensions_xy = nFrameHeight | nFrameWidth << 16;
        sParams.s_ext.i4_max_search_range_xy = MAX_SEARCH_RANGE_H264_Y |
                            MAX_SEARCH_RANGE_X << 16;
        IVEA_SET_DEBLK_PRESET(sParams.s_ext.i4_coding_tools1, IVEA_ENABLE_DEBLK);
        IVEA_SET_QPEL_PRESET(sParams.s_ext.i4_coding_tools1, IVEA_ENABLE_QPEL_ME);
        IVEA_SET_MB_QP_ACT_PRESET(sParams.s_ext.i4_coding_tools1, IVEA_DISABLE_MB_QP_ACT);
        ALOGD("sParams.s_ext.i4_coding_tools1 %d",sParams.s_ext.i4_coding_tools1);
        sParams.s_ext.i4_coding_tools2 = DSP_ENCODING_PERCENTAGE;
        SET_DEINTERLACE_PRESET(sParams.s_ext.i4_dsp_processing1, IVEA_SIMPLE_DEINTERLACE);
        sParams.s_ext.i4_dsp_processing2 = 0;
    }


    if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        pComponentPrivate->pCodecHandle
            = OMX_IVIDENC1_create(g_pv_engine_handle, "OMX_VIDEO_CodingAVC", &sParams);
ALOGD("Creating AVC Codec handle");

        /* Initialize the H264 encoder related function pointers */
        pComponentPrivate->pf_dsp_trig      = h264enc_atci_dsp_trig;
        pComponentPrivate->pf_arm_process   = h264enc_atci_arm_process;
        pComponentPrivate->pf_dsp_wait      = h264enc_atci_dsp_wait;
    }
    else if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
             pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
    {
        pComponentPrivate->pCodecHandle
            = OMX_IVIDENC1_create(g_pv_engine_handle, "OMX_VIDEO_CodingMPEG4", &sParams);
ALOGD("Creating MPEG4 Codec handle");
         /* Initialize the Mpeg4 encoder related function pointers */
        pComponentPrivate->pf_dsp_trig      = spltenc_atci_dsp_trig;
        pComponentPrivate->pf_arm_process   = spltenc_atci_arm_process;
        pComponentPrivate->pf_dsp_wait      = spltenc_atci_dsp_wait;
    }

    if(NULL == pComponentPrivate->pCodecHandle) {
ALOGD("Failed to create Codec handle");
        eError = OMX_ErrorInvalidState;
        goto OMX_CONF_CMD_BAIL;
    }

    /* Any control call to set params */
    pDynParams->size     = sizeof(IVIDENC1_DynamicParams);
    pDynParams->captureWidth     = nFrameWidth;
    pDynParams->forceFrame       = IVIDEO_NA_FRAME;
    pDynParams->generateHeader   = XDM_ENCODE_AU;
    pDynParams->inputHeight      = nFrameHeight;
    pDynParams->inputWidth       = nFrameWidth;
    pDynParams->interFrameInterval = 1;
    pDynParams->intraFrameInterval = pComponentPrivate->nIntraFrameInterval;
    pDynParams->mbDataFlag         = 0;
    pDynParams->refFrameRate       = nSrcFrameRate;
    pDynParams->targetBitRate      = pComponentPrivate->nTargetBitRate;
    pDynParams->targetFrameRate    = nTgtFrameRate;


    nStatus
        = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                               XDM_SETPARAMS,
                               pDynParams,
                               &sStatus);

    if(IVIDENC1_EOK != nStatus)
    {
        eError = OMX_ErrorUndefined;
        goto OMX_CONF_CMD_BAIL;
    }


    /* Set RC parameters */
    {
        ITTVIDENC1_IMOD_DynamicParams_t sDynParamExt;
        ivea_ext_rc_otp_t *pRcParam;

        sDynParamExt.s_ividenc_dyn_params.size
            = sizeof(ITTVIDENC1_IMOD_DynamicParams_t);
        pRcParam = (ivea_ext_rc_otp_t *)&sDynParamExt.s_ext;

        if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
        {
            pComponentPrivate->pQuantization->nQpP = 28;
            pComponentPrivate->pQuantization->nQpB = 28;
        }
        else if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
                 pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
        {
            pComponentPrivate->pQuantization->nQpP = 7;
            pComponentPrivate->pQuantization->nQpB = 7;
        }
        pRcParam->i4_size = sizeof(ivea_ext_rc_otp_t);
        pRcParam->i4_init_i_qp = pComponentPrivate->pQuantization->nQpI;
        pRcParam->i4_init_p_qp = pComponentPrivate->pQuantization->nQpP;
        pRcParam->i4_init_b_qp = pComponentPrivate->pQuantization->nQpB;
        pRcParam->i4_buffer_delay = 1000;
        pRcParam->i4_stuffing_disabled = 1;
        pRcParam->i4_limit_vbv_to_std_def_buf_size = 0;


        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_SET_RC_OTP_PRMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus) {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }

    }

#ifdef SET_SEI_VUI_PARAMS
    /*VUI OTP parameters for H264 encoder*/
    if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
        ITTVIDENC1_IMOD_DynamicParams_t sDynParamExt;
        ivea_ext_vui_otp_t s_vui_otp;
       // ALOGD("Doing a get parameter call on VUI OTP params");

        //sStatus.data.buf = malloc(10 * 1024);
		VIDENC_MALLOC(sStatus.data.buf,
		  10240,
		  OMX_U8,
		  pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_OTP_GET_SEI_VUI_PARAMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }

        memcpy(&s_vui_otp, sStatus.data.buf, sizeof(ivea_ext_vui_otp_t));

        /* Setting the VUI Params */
        printf("Setting the OTP for VUI \n");
        s_vui_otp.i4_size = sizeof(ivea_ext_vui_otp_t);
        s_vui_otp.i4_enable_sei_vui_params = ENABLE_SEI_VUI;
        s_vui_otp.i4_aspect_ratio_x = PIXEL_ASPECT_RATIO_X;
        s_vui_otp.i4_aspect_ratio_y = PIXEL_ASPECT_RATIO_Y;
        s_vui_otp.i4_pixel_range = PIXEL_ASPECT_RATIO_RANGE;

        memcpy(sDynParamExt.s_ext.i4_dyn_param, &s_vui_otp, sizeof(ivea_ext_vui_otp_t));

        /* Set the control parameters */
        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_OTP_SET_SEI_VUI_PARAMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }
        //free(sStatus.data.buf);
        VIDENC_FREE(sStatus.data.buf, pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);
    }
    /* Setting pixel aspect ratio Mpeg4 encoder*/
    else if (pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
             pOutPortDef->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
    {
        ITTVIDENC1_IMOD_DynamicParams_t sDynParamExt;
        ivea_ext_par_prms_t s_par_param;
        ALOGD("Getting the default PAR parameters \n");

        //sStatus.data.buf = malloc(10 * 1024);
        VIDENC_MALLOC(sStatus.data.buf,
		10240,
		OMX_U8,
		pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_GET_PAR_PRMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }

        memcpy(&s_par_param, sStatus.data.buf, sizeof(ivea_ext_par_prms_t));

        /* Setting the VUI Params */
        s_par_param.i4_size       = sizeof(ivea_ext_par_prms_t);
        s_par_param.i4_par_width  = 1;
        s_par_param.i4_par_height = 1;

        memcpy(sDynParamExt.s_ext.i4_dyn_param, &s_par_param, sizeof(ivea_ext_par_prms_t));

        /* Set the control parameters */
        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_SET_PAR_PRMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }
        //free(sStatus.data.buf);
        VIDENC_FREE(sStatus.data.buf, pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);
    }
#endif  //SET_SEI_VUI_PARAMS

    /* Set VBR parameters */
    if(2 == sParams.s_ividenc_params.rateControlPreset)//VIDEO_STORAGE
    {
        ITTVIDENC1_IMOD_DynamicParams_t sDynParamExt;
        ivea_vbr_params_t   s_vbr_param;

        sDynParamExt.s_ividenc_dyn_params.size
            = sizeof(ITTVIDENC1_IMOD_DynamicParams_t);

        //sStatus.data.buf = malloc(10 * 1024);
        VIDENC_MALLOC(sStatus.data.buf,
		10240,
		OMX_U8,
		pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_GET_VBR_PARAMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }

        memcpy(&s_vbr_param, sStatus.data.buf, sizeof(s_vbr_param));
        s_vbr_param.i4_size = sizeof(s_vbr_param);



        /* Modify the required parameters */
        s_vbr_param.i4_avg_channel_bandwidth = pComponentPrivate->nTargetBitRate;
        s_vbr_param.i4_peak_channel_bandwidth = (pComponentPrivate->nTargetBitRate * 3) >> 1;

        memcpy(sDynParamExt.s_ext.i4_dyn_param, &s_vbr_param, sizeof(s_vbr_param));


        nStatus
            = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   IVEA_SET_VBR_PARAMS,
                                   (IVIDENC1_DynamicParams *)&sDynParamExt,
                                   &sStatus);

        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            goto OMX_CONF_CMD_BAIL;
        }
        //free(sStatus.data.buf);
        VIDENC_FREE(sStatus.data.buf, pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);
    }

    /* Reset the control call flags */
    pComponentPrivate->bControlCallNeeded           = OMX_FALSE;
    pComponentPrivate->bControlCallNeededForBitrate = OMX_FALSE;
    pComponentPrivate->bControlCallNeededForQp      = OMX_FALSE;

OMX_CONF_CMD_BAIL:
    return eError;
}

/*---------------------------------------------------------------------------------------*/
/**
  *  Function to issue process call to split encoder
  *
  *
  *
  * @retval OMX_NoError              Success, ready to roll
  *
  **/
/*---------------------------------------------------------------------------------------*/
OMX_ERRORTYPE send_buffer_to_dsp_wait_thread(VIDENC_COMPONENT_PRIVATE *pComponentPrivate,
                                            int index,
                                            OMX_BUFFERHEADERTYPE *pOutBufHead,
                                            OMX_BUFFERHEADERTYPE *pInpBufHead)
{
//ALOGD("Entering::send_buffer_to_dsp_wait_thread");
//ALOGD("Params:INdex=%d",index);
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    FRAME_ENCODE_BUFFER *frmEncBufSpecifics = NULL;
    int nRet = -1;

    if(OMX_TRUE == pComponentPrivate->bHeaderGenerate)
    {
        /* This is the output of the header encode so send out only the      */
        /* output buffer, since we will re use the same intput buffer for the*/
        /* first frame encoding                                              */
        pOutBufHead->nFilledLen = pComponentPrivate->sOutArgs[index].bytesGenerated;
        if(IVIDEO_I_FRAME == pComponentPrivate->sOutArgs[index].encodedFrameType)
        {
            pOutBufHead->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
        }

        eError = OMX_VIDENC_Process_FilledOutBuf(pComponentPrivate, pOutBufHead);
        if (eError != OMX_ErrorNone)
        {
            OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_VIDENC_Process_FilledOutBuf");

            OMX_VIDENC_EVENT_HANDLER(pComponentPrivate, OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            OMX_VIDENC_BAIL_IF_ERROR(eError, pComponentPrivate);
        }
    }
    else
    {
        /* TODO - write the input and output buffer to the pipe. The DSP wait thread will */
        /* wait on these buffers and perform the ARM processing and then send out the     */
        /* buffers back to the Application                                                */
        /*pOutBufHead->nFilledLen = pComponentPrivate->sOutArgs[index].bytesGenerated;
        if(IVIDEO_I_FRAME == pComponentPrivate->sOutArgs[index].encodedFrameType)
        {
            pOutBufHead->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
        }*/

		//frmEncBufSpecifics = (FRAME_ENCODE_BUFFER *)malloc(sizeof(FRAME_ENCODE_BUFFER));
    	VIDENC_MALLOC(frmEncBufSpecifics,
			  sizeof(FRAME_ENCODE_BUFFER),
			  FRAME_ENCODE_BUFFER,
			  pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

		if(NULL == frmEncBufSpecifics)
		{
			ALOGE("malloc failed!!");
			eError = OMX_ErrorInsufficientResources;
			goto OMX_CONF_CMD_BAIL;
		}

        frmEncBufSpecifics->pInputBuffer     = pInpBufHead;
        frmEncBufSpecifics->pOutputBuffer    = pOutBufHead;
        frmEncBufSpecifics->index            = index;

        nRet = write(pComponentPrivate->nDsp_wait_thread_data_pipe[1], &frmEncBufSpecifics, sizeof(frmEncBufSpecifics));
        if (nRet == -1)
        {
            OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorUndefined,
                                   pComponentPrivate->dbg, OMX_PRCOMM4,
                                   "Failed to write to nDsp_wait_thread_data_pipe.\n");
        }
    }

OMX_CONF_CMD_BAIL:
        return eError;
}

OMX_ERRORTYPE control_calls(VIDENC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE                   eError = OMX_ErrorNone;
    IVIDENC1_Status                 sStatus;
    int                             nStatus;

    nStatus = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                   XDM_SETPARAMS,
                                   &pComponentPrivate->sDynParams,
                                   &sStatus);
    if(IVIDENC1_EOK != nStatus)
    {
        eError = OMX_ErrorUndefined;
        OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
        goto OMX_CONF_CMD_BAIL;
    }

    /* ASSUMPTION: Rate control preset is 2 */
    if(OMX_TRUE == pComponentPrivate->bControlCallNeededForBitrate)
    {
        ITTVIDENC1_IMOD_DynamicParams_t sDynParamExt;
        ivea_vbr_params_t   s_vbr_param;

        sDynParamExt.s_ividenc_dyn_params.size = sizeof(ITTVIDENC1_IMOD_DynamicParams_t);

        //sStatus.data.buf = malloc(10 * 1024);
		VIDENC_MALLOC(sStatus.data.buf,
		10240,
		OMX_U8,
		pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

        nStatus = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                       IVEA_GET_VBR_PARAMS,
                                       (IVIDENC1_DynamicParams *)&sDynParamExt,
                                       &sStatus);
        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
            goto OMX_CONF_CMD_BAIL;
        }

        memcpy(&s_vbr_param, sStatus.data.buf, sizeof(s_vbr_param));
        s_vbr_param.i4_size = sizeof(s_vbr_param);

        /* Modify the required parameters */
        s_vbr_param.i4_avg_channel_bandwidth = pComponentPrivate->nTargetBitRate;
        s_vbr_param.i4_peak_channel_bandwidth = (pComponentPrivate->nTargetBitRate * 3) >> 1;

        memcpy(sDynParamExt.s_ext.i4_dyn_param, &s_vbr_param, sizeof(s_vbr_param));

        nStatus = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                       IVEA_SET_VBR_PARAMS,
                                       (IVIDENC1_DynamicParams *)&sDynParamExt,
                                       &sStatus);
        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
            goto OMX_CONF_CMD_BAIL;
        }
        //free(sStatus.data.buf);
        VIDENC_FREE(sStatus.data.buf, pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);
    }

    if(OMX_TRUE == pComponentPrivate->bControlCallNeededForQp)
    {
        ITTVIDENC1_IMOD_DynamicParams_t sDynParamExt;
        ivea_ext_rc_otp_t *pRcParam;

        sDynParamExt.s_ividenc_dyn_params.size = sizeof(ITTVIDENC1_IMOD_DynamicParams_t);
        pRcParam = (ivea_ext_rc_otp_t *)&sDynParamExt.s_ext;

        pRcParam->i4_size = sizeof(ivea_ext_rc_otp_t);
        pRcParam->i4_init_i_qp = pComponentPrivate->pQuantization->nQpI;
        pRcParam->i4_init_p_qp = pComponentPrivate->pQuantization->nQpP;
        pRcParam->i4_init_b_qp = pComponentPrivate->pQuantization->nQpB;
        pRcParam->i4_buffer_delay = 1000;
        pRcParam->i4_stuffing_disabled = 1;
        pRcParam->i4_limit_vbv_to_std_def_buf_size = 0;

        nStatus = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                       IVEA_SET_RC_OTP_PRMS,
                                       (IVIDENC1_DynamicParams *)&sDynParamExt,
                                       &sStatus);
        if(IVIDENC1_EOK != nStatus) {
            eError = OMX_ErrorUndefined;
            OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
            goto OMX_CONF_CMD_BAIL;
        }
    }

    pComponentPrivate->sDynParams.forceFrame = IVIDEO_NA_FRAME;
    pComponentPrivate->bControlCallNeeded    = OMX_FALSE;

OMX_CONF_CMD_BAIL:
    return eError;
}

void initialize_pointers(VIDENC_COMPONENT_PRIVATE *pComponentPrivate, int index,
                OMX_BUFFERHEADERTYPE *pInpBufHead,
                OMX_BUFFERHEADERTYPE *pOutBufHead,
                XDAS_Int8           *apOutBuf[],
                XDAS_Int32          anOutBufSizes[])
{
//ALOGD("Entering:Initializing pointers");
    int     nFrameWidth, nFrameHeight;
    OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef;
        XDAS_Int8  *pBuf;

    pInPortDef    = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
    nFrameWidth   = pInPortDef->format.video.nFrameWidth;
    nFrameHeight  = pInPortDef->format.video.nFrameHeight;
//Shruthi Change starts

if (pComponentPrivate->pCompPort[0]->VIDEncBufferType == EncoderMetadataPointers)
             {
		//ALOGD("SHRLOG:%s %d:Entered IF EncoderMetadataPointers ",__FUNCTION__,__LINE__);
                             OMX_U32 *pTempBuffer;
                             OMX_U32 nMetadataBufferType;
                             OMX_PTR pBufferOrig;
                             //ALOGE("OMX_VIDENC_Queue_H264_Buffer :: pBufHead->pBuffer %p", pBufHead->pBuffer);
                             pTempBuffer = (OMX_U32 *) (pInpBufHead->pBuffer);
                             nMetadataBufferType = *pTempBuffer;

                             if(nMetadataBufferType == kMetadataBufferTypeCameraSource)
                             {
				//ALOGD("SHRLOG:%s %d:nMetadataBufferType == kMetadataBufferTypeCameraSource ",__FUNCTION__,__LINE__);
                                             //ALOGE("OMX_VIDENC_Queue_H264_Buffer :: kMetadataBufferTypeCameraSource");
                                             video_metadata_t* pVideoMetadataBuffer;
                                             pVideoMetadataBuffer = (video_metadata_t*) ((OMX_U32 *)(pInpBufHead->pBuffer));
                                             pBufferOrig = pVideoMetadataBuffer->handle;
                                             pInpBufHead->nOffset = pVideoMetadataBuffer->offset;
					     pComponentPrivate->sInBufs[index].bufDesc[0].buf     = (XDAS_Int8 *)pBufferOrig;
					    pBuf=(XDAS_Int8 *)pBufferOrig;
//ALOGD(" %s :: pInpBufHead->pBuffer=0x%x pBufferOrig add= 0x%x pBufferOrig=0x%x INDEX = %d",__FUNCTION__, pInpBufHead->pBuffer,&pBufferOrig,pBufferOrig,index);
/*******************DUMP THE INPUT BUFFER**************************
	FILE *fd=NULL;
	fd=fopen("/tmp/capture.dump","ab");
	if(fd==NULL)
	{
		ALOGD("Failed to open capture dump file");
	}
	else
	{
		ALOGD("Dump file opened");		
	}  
	ALOGD("size of buf = %d",pInpBufHead->nAllocLen);
	fwrite(pComponentPrivate->sInBufs[index].bufDesc[0].buf,1,pInpBufHead->nAllocLen,fd);

	fclose(fd);
*******************************************************************/
                             }
	    }

//Shruthi Change ends
    /*pComponentPrivate->pInpBuf[index] = pInpBufHead;
    pComponentPrivate->pOutBuf[index] = pOutBufHead;*/
    /* Input Buffer Descriptor */
    pComponentPrivate->sInBufs[index].numBufs = 1;
    pComponentPrivate->sInBufs[index].frameWidth  = nFrameWidth;
    pComponentPrivate->sInBufs[index].frameHeight = nFrameHeight;
    pComponentPrivate->sInBufs[index].framePitch  = nFrameWidth;
//Shruthi Change start
	if (pComponentPrivate->pCompPort[0]->VIDEncBufferType == EncoderMetadataPointers)
	{
	    //ALOGD("SHRLOG:%s %d (XDAS_Int8 *)pBufferOrig location=0x%x,pInpBufHead->pBuffer=0x%x",__FUNCTION__,__LINE__,pBufferOrig,pInpBufHead->pBuffer);
  	   // pComponentPrivate->sInBufs[index].bufDesc[0].buf     = (XDAS_Int8 *) &pBufferOrig;
	    //ALOGD("\nFINAL=0x%x\nOrig_cast=0x%x\nOrig=0x%x",pComponentPrivate->sInBufs[index].bufDesc[0].buf );
	}
	else
	{
	    pComponentPrivate->sInBufs[index].bufDesc[0].buf     = (XDAS_Int8 *)pInpBufHead->pBuffer;
	}
//Shruthi Change ends
    pComponentPrivate->sInBufs[index].bufDesc[0].bufSize = (XDAS_Int32)pInpBufHead->nAllocLen;

    if(OMX_COLOR_FormatYUV420Planar == pInPortDef->format.video.eColorFormat)
    {
//ALOGD("OMX_COLOR_FormatYUV420Planar is enabled");
        XDAS_Int32 nFrameSize;
    //    XDAS_Int8  *pBuf;
        nFrameSize = (nFrameWidth * nFrameHeight);
//Shruthi Change start
	if (pComponentPrivate->pCompPort[0]->VIDEncBufferType == EncoderMetadataPointers)
	{
	//ALOGD("SHRLOG:%s %d:pBuf       = (XDAS_Int8 *)pBufferOrig; ",__FUNCTION__,__LINE__ );
      //  pBuf       = (XDAS_Int8 *)&pBufferOrig;//pInpBufHead->pBuffer;
	}
	else
	{
        pBuf       = (XDAS_Int8 *)pInpBufHead->pBuffer;//pInpBufHead->pBuffer;
	}
//Shruthi Change ends

        pComponentPrivate->sInBufs[index].numBufs            = 3;
        /* Luma */
        pComponentPrivate->sInBufs[index].bufDesc[0].buf     = pBuf;
        pComponentPrivate->sInBufs[index].bufDesc[0].bufSize = nFrameSize;
        pBuf                      += nFrameSize;
        /* Chroma (Cb) */
        pComponentPrivate->sInBufs[index].bufDesc[1].buf     = pBuf;
        pComponentPrivate->sInBufs[index].bufDesc[1].bufSize = (nFrameSize >> 2);
        pBuf                      += (nFrameSize >> 2);

        /* Chroma (Cr) */
        pComponentPrivate->sInBufs[index].bufDesc[2].buf     = pBuf;
        pComponentPrivate->sInBufs[index].bufDesc[2].bufSize = (nFrameSize >> 2);
        pBuf                      += (nFrameSize >> 2);
    }

    /* Output buffer Descriptor */
    apOutBuf[0]       = (XDAS_Int8 *)pOutBufHead->pBuffer;//pOutBufHead->pBuffer;
    anOutBufSizes[0]  = (XDAS_Int32)pOutBufHead->nAllocLen;//pOutBufHead->nAllocLen;
    pComponentPrivate->sOutBufs[index].numBufs  = 1;
    pComponentPrivate->sOutBufs[index].bufs     = apOutBuf;
    pComponentPrivate->sOutBufs[index].bufSizes = anOutBufSizes;
    /* Input Arguments */
    pComponentPrivate->sInArgs[index].size    = sizeof(IVIDENC1_InArgs);
    pComponentPrivate->sInArgs[index].inputID = 1;
    pComponentPrivate->sInArgs[index].topFieldFirstFlag = 0; /* Does not matter */

    /* Output Arguments */
    pComponentPrivate->sOutArgs[index].size   = sizeof(IVIDENC1_OutArgs);
    pOutBufHead->nTimeStamp = pInpBufHead->nTimeStamp;//pInpBufHead->nTimeStamp;
    pOutBufHead->nFlags = pInpBufHead->nFlags;//pInpBufHead->nFlags;

}

OMX_BUFFERHEADERTYPE* get_new_output_buffer(VIDENC_COMPONENT_PRIVATE *pComponentPrivate,
                                            OMX_ERRORTYPE *omxError)
{
//ALOGD("ENTERING::get_new_output_buffer");
    OMX_BUFFERHEADERTYPE*   pOutputBufHead       = NULL;
    int                     nRet                 = -1;
    OMX_ERRORTYPE           eError               = OMX_ErrorNone;
    VIDENC_BUFFER_PRIVATE*  pOutputBufferPrivate = NULL;
    int                     status               = -1;
    int                     fdmax                = -1;
    fd_set                  rfds;
    sigset_t                set;

    FD_ZERO (&rfds);
    FD_SET (pComponentPrivate->nFree_oPipe[0], &rfds);

    fdmax = pComponentPrivate->nFree_oPipe[0];
    sigemptyset(&set);
    sigaddset(&set,SIGALRM);

    //ALOGD("Waiting on the output buffer");

    status = pselect(fdmax+1, &rfds, NULL, NULL, NULL, &set);

    if (0 == status)
    {
        ALOGD("The timeout value is specified as NULL should never come here");
        sched_yield();
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
    else if(FD_ISSET(pComponentPrivate->nFree_oPipe[0], &rfds))/* &&
                    (pComponentPrivate->eState != OMX_StatePause &&
                     pComponentPrivate->eState != OMX_StateIdle))*/
    {

        nRet = read(pComponentPrivate->nFree_oPipe[0], &pOutputBufHead, sizeof(pOutputBufHead));
        if (nRet == -1)
        {
            OMX_ERROR4(pComponentPrivate->dbg, "Error while reading from the pipe\n");
            OMX_CONF_SET_ERROR_BAIL(eError, OMX_ErrorHardware);
        }

        if (pOutputBufHead != NULL)
        {
            pOutputBufferPrivate = (VIDENC_BUFFER_PRIVATE*)pOutputBufHead->pOutputPortPrivate;
        }
        else
        {
            ALOGE("Got an NULL output buffer!!");
        }

        OMX_DBG_CHECK_CMD(pComponentPrivate->dbg, pOutputBufHead, pOutputBufferPrivate, 1);
        pOutputBufferPrivate->bReadFromPipe = OMX_TRUE;
    }

OMX_CONF_CMD_BAIL:
    *omxError = eError;
    return pOutputBufHead;
}

OMX_ERRORTYPE OMX_VIDENC_process(VIDENC_COMPONENT_PRIVATE *pComponentPrivate,
                                OMX_BUFFERHEADERTYPE *pInpBufHead,
                                OMX_BUFFERHEADERTYPE *pOutBufHead)
{
//ALOGD("Entering :: OMX_VIDENC_process");
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    IVIDENC1_DynamicParams *pDynParams;
    XDAS_Int8           *apOutBuf[1];
    XDAS_Int32          anOutBufSizes[1];
    Int     nStatus;
    IVIDENC1_Status     sStatus;
    int index = pComponentPrivate->dsp720PEncoderBufferTrigIndex;
    //static frm=1;	
    if(pInpBufHead == NULL || pOutBufHead == NULL)
    {
            OMX_ERROR4(pComponentPrivate->dbg,"Invalid arguments");
            return OMX_ErrorUndefined;
    }
    //ALOGD("Inside OMX_VIDENC_process");
    pDynParams = &pComponentPrivate->sDynParams;

    /* Derive local variables */
    initialize_pointers(pComponentPrivate, index, pInpBufHead, pOutBufHead, apOutBuf, anOutBufSizes);

    /* Make control call if required */
    if(OMX_TRUE == pComponentPrivate->bControlCallNeeded)
    {
        control_calls(pComponentPrivate);
    }

    /* First time when you come in, header generate is true */
    if(OMX_TRUE == pComponentPrivate->bHeaderGenerate)
    {
        instance_ctxt_t *ps_hdl;
        IVIDENC1_Handle ps_codec_hdl;

        ps_hdl                      = (instance_ctxt_t *)pComponentPrivate->pCodecHandle;
        ps_codec_hdl                = ps_hdl->ps_codec_hdl;

        pDynParams->generateHeader  = XDM_GENERATE_HEADER;
        nStatus = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                       XDM_SETPARAMS,
                                       pDynParams,
                                       &sStatus);
        if(IVIDENC1_EOK != nStatus)
        {
                eError = OMX_ErrorUndefined;
                OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
                goto OMX_CONF_CMD_BAIL;
        }
	//ALOGD("%s %d pf_dsp_trig_1",__FUNCTION__,__LINE__);
	//ALOGD("%s % d :: dsp_trig passed inbuf add= 0x%x",__FUNCTION__,__LINE__,(pComponentPrivate->sInBufs[index].bufDesc[0].buf));
	        
	nStatus = pComponentPrivate->pf_dsp_trig(ps_codec_hdl,
                                                 &pComponentPrivate->sInBufs[index],
                                                 &pComponentPrivate->sOutBufs[index],
                                                 &pComponentPrivate->sInArgs[index],
                                                 &pComponentPrivate->sOutArgs[index],
                                                 index);
        if(IVIDENC1_EOK != nStatus)
        {
                eError = OMX_ErrorUndefined;
                OMX_ERROR4(pComponentPrivate->dbg,"failed spltenc_atci_dsp_trig");
                goto OMX_CONF_CMD_BAIL;
        }

        pDynParams->generateHeader = XDM_ENCODE_AU;
        nStatus = OMX_IVIDENC1_control(pComponentPrivate->pCodecHandle,
                                       XDM_SETPARAMS,
                                       pDynParams,
                                       &sStatus);
        if(IVIDENC1_EOK != nStatus)
        {
                eError = OMX_ErrorUndefined;
                OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
                goto OMX_CONF_CMD_BAIL;
        }
        PROFILE_INIT;

        if(OMX_VIDEO_CodingAVC == pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef->format.video.eCompressionFormat)
        {
            /* Now get the boundary of SPS and PPS */
            UWORD32 u4_nal_offset, u4_nal_size;
            UWORD32 u4_bytes_gen = pComponentPrivate->sOutArgs[index].bytesGenerated;
            UWORD32 u4_sps_size;
            UWORD32 u4_pps_size;
            OMX_U8 *pu1_hdr_stream;

            /* Temporary buffer to hold SPS and PPS */
            //pu1_hdr_stream = malloc(1024);
            VIDENC_MALLOC(pu1_hdr_stream,
			1024,
			OMX_U8,
			pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

            if(NULL == pu1_hdr_stream)
            {
                eError = OMX_ErrorUndefined;
                OMX_ERROR4(pComponentPrivate->dbg,"failed OMX_IVIDENC1_control");
                goto OMX_CONF_CMD_BAIL;
            }

            /* Copy the header into temp stream buffer */
            memcpy(pu1_hdr_stream, pOutBufHead->pBuffer, u4_bytes_gen);

            /* Find the nal size and offset for SPS */
            h264_parse_start_code_prefixes(pu1_hdr_stream, u4_bytes_gen, &u4_nal_offset, &u4_nal_size);
            // With Froyo, need to include entire size including start code (u4_sps_size).
            // Function in pvomxencnode\src\pvmf_omx_enc_node.cpp that searches for SPS NAL requires this.
            // Therefore the hack is not necessary.
            u4_sps_size = u4_nal_offset + u4_nal_size;
            u4_pps_size = (u4_bytes_gen - u4_sps_size);

            pOutBufHead->nFilledLen = u4_sps_size;

            /* Send out the header encoded output buffer, and get a new output buffer for me to send PPS */
            OMX_VIDENC_Process_FilledOutBuf(pComponentPrivate, pOutBufHead);

            pOutBufHead =  get_new_output_buffer(pComponentPrivate, &eError);
            if(pOutBufHead == NULL)
            {
                    eError = OMX_ErrorUndefined;
                    goto OMX_CONF_CMD_BAIL;
            }

            // The PPS processing is differnt in opencore. It requires the hack (doesn't use NAL search function).
            // u4_nal_offset represents SPS start code size. Assume same size for PPS. Start code size = 4.

            /* Copy the PPS from temp stream buffer into pOutBufHead*/
            memcpy(pOutBufHead->pBuffer, (pu1_hdr_stream +  u4_sps_size), u4_pps_size);

            pOutBufHead->nFilledLen = u4_pps_size;// - u4_nal_offset;

            /* Send out the header encoded output buffer, and get a new output buffer for me to encode frame */
            OMX_VIDENC_Process_FilledOutBuf(pComponentPrivate, pOutBufHead);

           /* free the temp stream buffer */
           //free(pu1_hdr_stream);
           VIDENC_FREE(pu1_hdr_stream, pComponentPrivate->pMemoryListHead, pComponentPrivate->dbg);

           pOutBufHead =  get_new_output_buffer(pComponentPrivate, &eError);
           if(pOutBufHead == NULL)
           {
                eError = OMX_ErrorUndefined;
                goto OMX_CONF_CMD_BAIL;
           }
           /* Reinitialize according to the new buffer pointers */
           initialize_pointers(pComponentPrivate, index, pInpBufHead, pOutBufHead, apOutBuf, anOutBufSizes);
        }
        else // MPEG4
        {
            /* Send out the header encoded output buffer, and get a new output */
            /* buffer for frame encode                                         */
            pOutBufHead->nFlags = OMX_BUFFERFLAG_CODECCONFIG;
            send_buffer_to_dsp_wait_thread(pComponentPrivate, index, pOutBufHead, NULL);

            pOutBufHead =  get_new_output_buffer(pComponentPrivate, &eError);
            if(pOutBufHead == NULL)
            {
                eError = OMX_ErrorUndefined;
                goto OMX_CONF_CMD_BAIL;
            }
            //ALOGD("Got new output buffer");

            /* Derive local variables */
            initialize_pointers(pComponentPrivate, index, pInpBufHead, pOutBufHead, apOutBuf, anOutBufSizes);
        }

        pComponentPrivate->bHeaderGenerate   = OMX_FALSE;
        //ALOGD("Buffering State = HDR_DONE");
    }

    {
        /* At this point after first call, hdr generate has been done via    */
        /* dsp trigger, and we are back to encode au state now header        */
        /* generate is false and we must encode a frame in the next output   */
        /* buffer.                                                           */
        /* Assumption : a second output buffer is available!                 */
        instance_ctxt_t *ps_hdl;
        IVIDENC1_Handle ps_codec_hdl;

        ps_hdl          = (instance_ctxt_t *)pComponentPrivate->pCodecHandle;
        ps_codec_hdl    = ps_hdl->ps_codec_hdl;

        //ALOGD("DSP TRIG INDEX %d", index);
	//ALOGD("%s %d pf_dsp_trig_2",__FUNCTION__,__LINE__);
	//ALOGD("%s % d :: dsp_trig passed inbuf add= 0x%x",__FUNCTION__,__LINE__,(pComponentPrivate->sInBufs[index].bufDesc[0].buf));
	nStatus = pComponentPrivate->pf_dsp_trig(ps_codec_hdl,
                                                &pComponentPrivate->sInBufs[index],
                                                &pComponentPrivate->sOutBufs[index],
                                                &pComponentPrivate->sInArgs[index],
                                                &pComponentPrivate->sOutArgs[index],
                                                index);
        if(IVIDENC1_EOK != nStatus)
        {
            eError = OMX_ErrorUndefined;
            OMX_ERROR4(pComponentPrivate->dbg,"failed spltenc_atci_dsp_trig");
            goto OMX_CONF_CMD_BAIL;
        }

        send_buffer_to_dsp_wait_thread(pComponentPrivate, index, pOutBufHead, pInpBufHead);

        pComponentPrivate->dsp720PEncoderBufferTrigIndex++;
        index++;


        {
            /* Increment the buffers triggered counter inorder to    */
            /* keep track of the number of buffers that has already  */
            /* been triggered to the DSP. This variable is mutex     */
            /* protected                                             */
            if (pthread_mutex_lock(&(pComponentPrivate->mbuffersTriggeredMutex)) != 0)
            {
                OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorHardware,
                                       pComponentPrivate->dbg, OMX_TRACE4,
                                       "pthread_mutex_lock() failed.\n");
            }

            pComponentPrivate->buffersTriggered++;

            if (pthread_mutex_unlock(&(pComponentPrivate->mbuffersTriggeredMutex)) != 0)
            {
                OMX_DBG_SET_ERROR_BAIL(eError, OMX_ErrorHardware,
                                       pComponentPrivate->dbg, OMX_TRACE4,
                                       "pthread_mutex_unlock() failed.\n");
            }
        }

        pComponentPrivate->dsp720PEncoderBufferTrigIndex = pComponentPrivate->dsp720PEncoderBufferTrigIndex % MAX_BUFFERS_TO_DSP;
        index = index % MAX_BUFFERS_TO_DSP;
    }

OMX_CONF_CMD_BAIL:
        return eError;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : h264_parse_start_code_prefixes                           */
/*                                                                           */
/*  Description   : This is the API function for demarcation of NAL Unit     */
/*                  It parses the bit stream buffer for two consecutive NAL  */
/*                  start codes and returns after updating nal offset and    */
/*                  nal size,                                                */
/*                                                                           */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : Looks for two consecutive start code prefixes (0x000001) */
/*                                                                           */
/*                                                                           */
/*  Outputs       : 1.  If nal demarcated by two start codes is found,       */
/*                  updates nal offset and size                              */
/*                  2.  If nal with  one start prefix is found, updates nal  */
/*                  offset and size assuming it is the last nal in buffer    */
/*                  3.  If no start code prefixes are found returns error    */
/*                                                                           */
/*  Returns       : Returns 0 for success (start code found)                 */
/*                         -1 for error (start code not found)               */
/*                                                                           */
/*                                                                           */
/*  Issues        : <List any issues or problems with this function>         */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         19 03 2010   Jay             Draft                                */
/*                                                                           */
/*****************************************************************************/

WORD32 h264_parse_start_code_prefixes(UWORD8 *pu1_buffer, UWORD32 u4_fill_size, UWORD32 *pu4_nal_off, UWORD32 *pu4_nal_size)

{
//ALOGD("Entering ::  h264_parse_start_code_prefixes");
    UWORD32 u4_count = 0, u4_start_code_count = 0;
    UWORD32 u4_nal_off = 0;

    /* Look for the Start Code prefix 0x000001  */
    while(u4_count < (u4_fill_size - 2))
    {
        if( (pu1_buffer[u4_count]     == 0x00) &&
            (pu1_buffer[u4_count + 1] == 0x00) &&
            (pu1_buffer[u4_count + 2] == 0x01)
        )
        {
              u4_start_code_count++;
              if(u4_start_code_count == 1)
              {
                    /* first start code prefix found */
                    u4_nal_off = u4_count + 3;
              }
              else if(u4_start_code_count == 2)
              {
                  UWORD32 u4_xtra_zeroes = 0;
                  /* second start code prefix found */

                  /* Exclude the extra zeroes at the end of NAL from the size */
                  while(pu1_buffer[u4_count - u4_xtra_zeroes - 1] == 0x00)
                    u4_xtra_zeroes++;

                  *pu4_nal_off = u4_nal_off;
                  *pu4_nal_size = u4_count - u4_nal_off - u4_xtra_zeroes;
                  return 0;
              }
        }
        u4_count++;
    }

    /* If only one start code prefix found update  nal offset,size
       else  return error */
    if(u4_start_code_count == 1)
    {
        *pu4_nal_off = u4_nal_off;
        *pu4_nal_size = u4_fill_size - u4_nal_off;
        return (0);
    }
    else
    {
        *pu4_nal_off = u4_fill_size;
        *pu4_nal_size = 0;
        return (-1);
    }
}

/* ========================================================================== */
/**
  *  VIDDEC_FatalErrorRecover() function destroys the codec when error has occured. LCML resources
  *                           and RM resources are freed.
  *
  * @param
  *     pComponentPrivate            Component private structure
  *
  * @retval OMX_NoError              Success, ready to roll
  *         OMX_ErrorUndefined
  *         OMX_ErrorInsufficientResources   Not enough memory
 **/
/* ========================================================================== */

void OMX_VIDENC_FatalErrorRecover(VIDENC_COMPONENT_PRIVATE* pComponentPrivate)
{
//ALOGD("eNTERING::MX_VIDENC_FatalErrorRecover");
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefIn = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE* pPortDefOut = NULL;

    OMX_PRDSP1(pComponentPrivate->dbg, "+++ENTERING\n");
    //ALOGD("\n Entering VIDENC_FatalErrorRecover");


    if(pComponentPrivate == NULL)
    {
        return;
    }
    else
    {
        pPortDefIn = pComponentPrivate->pCompPort[VIDENC_INPUT_PORT]->pPortDef;
        pPortDefOut = pComponentPrivate->pCompPort[VIDENC_OUTPUT_PORT]->pPortDef;

        if (pComponentPrivate->bCodecStarted == OMX_TRUE || pComponentPrivate->bCodecLoaded == OMX_TRUE)
        {
            OMX_IVIDENC1_delete(pComponentPrivate->pCodecHandle);
            pComponentPrivate->pCodecHandle = NULL;

            pComponentPrivate->bCodecStarted = OMX_FALSE;
            pComponentPrivate->bCodecLoaded = OMX_FALSE;
        }
    }

#ifdef RESOURCE_MANAGER_ENABLED /* Resource Manager Proxy Calls */
            if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
            {
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_FreeResource,
                                             OMX_720P_Encode_COMPONENT,
                                             0,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
            {
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_FreeResource,
                                             OMX_720P_Encode_COMPONENT,
                                             0,
                                             3456,
                                             NULL);
            }
            else if (pPortDefOut->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
            {
                eError = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                             RMProxy_FreeResource,
                                             OMX_H263_Encode_COMPONENT,
                                             0,
                                             3456,
                                             NULL);
            }
            if (eError != OMX_ErrorNone)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "::RMProxy_FreeResource failed in FatalErrorRecover\n");
            }
            eError = RMProxy_Deinitalize();
            if (eError != OMX_ErrorNone)
            {
                OMX_ERROR4(pComponentPrivate->dbg, "::From RMProxy_Deinitalize\n");
            }

#endif

    /* regardless of success from above,
       still send the Invalid State error to client */

    eError = OMX_ErrorInvalidState;
    OMX_VIDENC_HandleError(pComponentPrivate, eError);
    OMX_PRDSP1(pComponentPrivate->dbg, "---EXITING(0x%x)\n",eError);
    return;
}
