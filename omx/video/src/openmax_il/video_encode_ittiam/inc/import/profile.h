/*****************************************************************************/
/*                                                                           */
/*                  WMV9 Simple/Main Profile Video Decoder                   */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2005                             */
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
/*  File Name         : c_coding_example.h                                   */
/*                                                                           */
/*  Description       : This file contains all the necessary examples to     */
/*                      establish a consistent use of Ittiam C coding        */
/*                      standards (based on Indian Hill C Standards)         */
/*                                                                           */
/*  List of Functions : <List the functions defined in this file>            */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         09 03 2007   Akhila          Draft                                */
/*                                                                           */
/*****************************************************************************/

#ifndef _PROFILE_H
#define _PROFILE_H

#define PROFILE_ENABLE 1

typedef enum
{
    PROFILE_ATCI,
    PROFILE_ATCI_TRIG_DSP,
    PROFILE_ATCI_ARM_PROC,
    PROFILE_ARM_PROC,
    PROFILE_DSP_WAIT,
    PROFILE_PLUGIN_CYCLES,
    MAX_NUM_PROFILE_TYPES
}profile_type_e;

#if (!(defined WIN32)  && (PROFILE_ENABLE))
#define PEAK_WINDOW 4

/* Data structure to store profile details*/
typedef struct
{
    unsigned long long u8_init_time;
    unsigned long long u8_final_time;
    UWORD32 u4_num_calls;
    UWORD32 u4_frame_time;
    UWORD32 au4_peak_buffer[PEAK_WINDOW];
    unsigned long long u4_avg_time;
    UWORD32 u4_peak_time;
}profile_info_t;

/* Function prototypes*/
extern void h264profile_init_globals();
extern void h264profile_get_init_time(WORD32 i4_h264profile_type);
extern void h264profile_get_final_time(WORD32 i4_h264profile_type);
extern void h264profile_print_stats();
extern void h264profile_collect_plugin_cycles(UWORD32 i4_plugin_cycles);
extern void h264profile_get_stats(UWORD32 *);

#define PROFILE_INIT h264profile_init_globals();
#define PROFILE_START(x)       h264profile_get_init_time(x)
#define PROFILE_STOP(x)        h264profile_get_final_time(x)
#define PROFILE_FRAME_PRINT    h264profile_print_frame_stats()
#define PROFILE_FINAL_PRINT    h264profile_print_final_stats()
#define PROFILE_PUT_DATA(x, y) h264profile_collect_data(x, y)
#define PROFILE_GET_STATS(x)   h264profile_get_stats(x)
#else/*PROFILE_ENABLE*/
#define PROFILE_INIT
#define PROFILE_START(x)
#define PROFILE_STOP(x)
#define PROFILE_FRAME_PRINT
#define PROFILE_FINAL_PRINT
#define PROFILE_PUT_DATA
#define PROFILE_GET_STATS(x)
#endif/*PROFILE_ENABLE*/
#endif
