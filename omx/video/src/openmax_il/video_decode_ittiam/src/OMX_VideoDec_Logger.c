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
/*  File Name         : OMX_VideoDec_Logger.c                                */
/*                                                                           */
/*  Description       : Function to print log messages                       */
/*                                                                           */
/*  List of Functions :	This File Prints both the kernel time    	         */
/*                      and the system time.                                 */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         05 02 2009   Ittiam          Creation                             */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <utils/Log.h>
#include "OMX_IttiamVideoDec_Logger.h"

/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/
#undef LOG_TAG
#define LOG_TAG "720P_VIDDEC_LOGGER"

/*****************************************************************************/
/*  Function Name : OMX_VIDDEC_sync_kernel_time                              */
/*  Description   : Prints both the kernel time and the system time          */
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
void OMX_VIDDEC_sync_kernel_time()
{
	static int print_kernel_time = 0;
	if(print_kernel_time)
	{
		return;
	}
	else
	{
		FILE   *fp;
		char   buf[101];
		int    num_bytes;
		struct timeval   sysTime;

		print_kernel_time = 0;
		fp = fopen("/proc/sched_debug", "rb");
		num_bytes = fread(buf, 1, 100, fp);
		gettimeofday(&sysTime, NULL);
		buf[num_bytes] = '\0';
		ALOGD("KERNEL TIME is %s\n", buf);
		ALOGD("SYSTEM TIME is %ld.%06ld\n", sysTime.tv_sec, sysTime.tv_usec);
		fclose(fp);
		print_kernel_time++;
		return;
	}
}



