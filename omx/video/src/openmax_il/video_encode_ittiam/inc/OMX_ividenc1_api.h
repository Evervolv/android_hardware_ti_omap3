/*****************************************************************************/
/*                                                                           */
/*                              SPLIT ENCODER                                */
/*                     ITTIAM SYSTEMS PVT LTD, BANGALORE                     */
/*                             COPYRIGHT(C) 2008                             */
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
/*  File Name         : OMX_ividenc1_api.h                                   */
/*                                                                           */
/*  Description       : This file contains function prototye of IVIDENC1     */
/*                      function for CTPI                                    */
/*                                                                           */
/*  List of Functions : <List the functions defined in this file>            */
/*                                                                           */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*  Revision History  :                                                      */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         27 06 2008   Omji            Creation                             */
/*                                                                           */
/*****************************************************************************/
#ifndef _OMX_IVIDENC1_API_H_
#define _OMX_IVIDENC1_API_H_

/*****************************************************************************/
/* File Includes															 */
/*****************************************************************************/

/*****************************************************************************/
/* Constant Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/
typedef void    *Engine_Handle;

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Structure                                                                 */
/*****************************************************************************/

/*****************************************************************************/
/* Extern Variable Declarations                                              */
/*****************************************************************************/
extern  void    *g_pv_engine_handle;

/*****************************************************************************/
/* Extern Function Declarations                                              */
/*****************************************************************************/
#if 0
extern IVIDENC1_Handle OMX_IVIDENC1_create(Engine_Handle e, String name,
                                                  IVIDENC1_Params *params);
#else
extern IVIDENC1_Handle OMX_IVIDENC1_create(Engine_Handle e, String name,
                                                  ITTVIDENC1_IMOD_Params_t *params);
#endif												  

extern Int OMX_IVIDENC1_process(IVIDENC1_Handle handle, 
                                       IVIDEO1_BufDescIn *inBufs,
                                       XDM_BufDesc *outBufs, 
                                       IVIDENC1_InArgs *inArgs, 
                                       IVIDENC1_OutArgs *outArgs);

extern Int OMX_IVIDENC1_control(IVIDENC1_Handle handle, 
                                       IVIDENC1_Cmd id,
                                       IVIDENC1_DynamicParams *params, 
                                       IVIDENC1_Status *status);

extern Void OMX_IVIDENC1_delete(IVIDENC1_Handle handle);

extern Int OMX_IVIDENC1_process_wait(IVIDENC1_Handle handle);

#endif /* _OMX_IVIDENC1_API_H_ */
