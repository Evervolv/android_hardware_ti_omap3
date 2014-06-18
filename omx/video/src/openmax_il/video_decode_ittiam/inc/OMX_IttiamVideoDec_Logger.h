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
/*  File Name         : OMX_IttiamVideoDec_Logger.h                          */
/*                                                                           */
/*  Description       : Contains MACROS required for logging			     */
/*																			 */
/*  List of Functions :                                                      */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History:                                                        */
/*          DD MM YYYY   Author(s)       Changes                             */
/*          01 12 2010   Ittiam          Draft                               */
/*                                                                           */
/*****************************************************************************/

#ifndef OMX_VIDDEC_LOGGER__H
#define OMX_VIDDEC_LOGGER__H

/* Control to enable/disable logs  */
#define ENABLE_PRINTS      			  (1)

/* Controls to enable/disable different Logging levels */
#define ENABLE_COMPONENT_STATUS_LOGS  (0) /* Enables component status logs   */
#define ENABLE_OMX_IL_API_LOGS        (0) /* Enables logs in VideoDecoder.c  */
#define ENABLE_BUFFER_TRACK_LOGS      (0) /* Enables logs in HandleBuffers.c */
#define ENABLE_BUFFER_METADATA_LOGS   (0) /* Enables printing of metadata of */
										  /* I/O buffers                     */
#define ENABLE_BUFFER_COUNT_LOGS      (0) /* Enables Buffercount of I/O logs */

#if ENABLE_COMPONENT_STATUS_LOGS
	#define LOG_STATUS_OMX_IL_CMDS        (0) /* Enables logs of cmd handling */
	#define LOG_STATUS_ARM_PROC_API       (0) /* Enables logs in split_dec.c  */
	#define LOG_STATUS_LCML_API_INTERFACE (0) /* Logs in split_dec_dsp.c      */
	#define LOG_STATUS_VIDDEC_UTILS       (0) /* Enables logs of various      */
											  /* helper functions             */
#endif

/* Controls to enable/disable log features  */
#define ENABLE_LINE_NUMBERS           (1)
#define ENABLE_FILE_NAME              (0)
#define ENABLE_FUNCTION_NAME		  (0)
#define ENABLE_TIMESTAMP			  (0)
#define ENABLE_THREAD_ID			  (0)
#define ENABLE_PROCESS_ID			  (0)

/* Function that prints both system and kernel times so that */
/* they can be synced 										 */
void OMX_VIDDEC_sync_kernel_time();

#if ENABLE_PRINTS

#if ENABLE_FILE_NAME
	#define FILE_FMT   ":%s"
	#define FILE_NAME  , __FILE__
#else
	#define FILE_FMT
	#define FILE_NAME
#endif

#if ENABLE_FUNCTION_NAME
	#define  FN_FMT ":%s"
	#define  FN     , __FUNCTION__
#else
	#define  FN_FMT
	#define  FN
#endif

#if ENABLE_LINE_NUMBERS
	#define  LINE_FMT ":%d"
	#define  LINE      , __LINE__
#else
	#define  LINE_FMT
	#define  LINE
#endif

#if ENABLE_TIMESTAMP
	#define PRINT_KERNEL_TIME   OMX_VIDDEC_sync_kernel_time();
	#define TS_FMT " Time :%ld.%06ld"
	#define TS     , time.tv_sec, time.tv_usec
#else
	#define TS_FMT
	#define TS
#endif

#if ENABLE_THREAD_ID
	#define TID_FMT " Thread ID:%u"
	#define TID     , gettid()
#else
	#define TID_FMT
	#define TID
#endif

#if ENABLE_PROCESS_ID
	#define PID_FMT " Process ID:%u"
	#define PID      , getpid()
#else
	#define PID_FMT
	#define PID
#endif

#if ENABLE_TIMESTAMP
#define LOG_PRINT(format, list...)      			     		  \
{																  \
	struct timeval   time;    									  \
	gettimeofday(&time, NULL); 									  \
	PRINT_KERNEL_TIME;                               	      	  \
	LOGD(format FILE_FMT FN_FMT LINE_FMT TID_FMT  PID_FMT TS_FMT, \
		 ##list FILE_NAME FN     LINE     TID       PID    TS);   \
}
#else /* ENABLE_TIMESTAMP */
#define LOG_PRINT(format, list...)      			     		  \
	LOGD(format FILE_FMT FN_FMT LINE_FMT TID_FMT  PID_FMT TS_FMT, \
		 ##list FILE_NAME FN     LINE     TID       PID    TS);
#endif /* ENABLE_TIMESTAMP */

#if ENABLE_COMPONENT_STATUS_LOGS
	#if LOG_STATUS_OMX_IL_CMDS
	#define LOG_STATUS_OMX_CMDS(format, list...)  LOG_PRINT(format, ##list)
	#else
	#define LOG_STATUS_OMX_CMDS(format, list...)
	#endif

	#if LOG_STATUS_ARM_PROC_API
	#define LOG_STATUS_ARM_API(format, list...)  LOG_PRINT(format, ##list)
	#else
	#define LOG_STATUS_ARM_API(format, list...)
	#endif

	#if LOG_STATUS_LCML_API_INTERFACE
	#define LOG_STATUS_LCML_API(format, list...)  LOG_PRINT(format, ##list)
	#else
	#define LOG_STATUS_LCML_API(format, list...)
	#endif

	#if LOG_STATUS_VIDDEC_UTILS
	#define LOG_STATUS_UTILS(format, list...)  LOG_PRINT(format, ##list)
	#else
	#define LOG_STATUS_UTILS(format, list...)
	#endif
#else /* ENABLE_COMPONENT_STATUS_LOGS */
	#define LOG_STATUS_OMX_CMDS(format, list...)
	#define LOG_STATUS_ARM_API(format, list...)
	#define LOG_STATUS_LCML_API(format, list...)
	#define LOG_STATUS_UTILS(format, list...)
#endif /* ENABLE_COMPONENT_STATUS_LOGS */

#if ENABLE_OMX_IL_API_LOGS
#define LOG_API(format, list...)  LOG_PRINT(format, ##list)
#else
#define LOG_API(format, list...)
#endif

#if ENABLE_BUFFER_TRACK_LOGS
#define LOG_BUF(format, list...)  LOG_PRINT(format, ##list)
#else
#define LOG_BUF(format, list...)
#endif

#if ENABLE_BUFFER_METADATA_LOGS
#define LOG_BUF_META(format, list...)  LOG_PRINT(format, ##list)
#else
#define LOG_BUF_META(format, list...)
#endif

#if ENABLE_BUFFER_COUNT_LOGS
#define LOG_BUF_COUNT(format, list...)  LOG_PRINT(format, ##list)
#else
#define LOG_BUF_COUNT(format, list...)
#endif

#else /* ENABLE_PRINTS */

#define LOG_STATUS_OMX_CMDS(format, list...)
#define LOG_STATUS_ARM_API(format, list...)
#define LOG_STATUS_LCML_API(format, list...)
#define LOG_STATUS_UTILS(format, list...)
#define LOG_API(format, list...)
#define LOG_BUF(format, list...)
#define LOG_BUF_META(format, list...)
#define LOG_BUF_COUNT(format, list...)

#endif /* ENABLE_PRINTS */

#endif /* OMX_VIDDEC_LOGGER__H */
