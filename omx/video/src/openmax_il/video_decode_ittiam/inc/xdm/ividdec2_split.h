/*
 *  Copyright 2008
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/**
 *  @file       ti/xdais/dm/ividdec2.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDDEC2   xDM Video Decoder Interface
 *
 *  This is an xDM video decoder interface.
 */

#ifndef ti_xdais_dm_IVIDDEC2_SPLIT_
#define ti_xdais_dm_IVIDDEC2_SPLIT_

#include <ialg.h>
#include <xdas.h>
#include "xdm.h"
#include "ivideo.h"


#ifdef __cplusplus
extern "C" {
#endif



/** @ingroup    ti_xdais_dm_split_IVIDDEC2 */
/*@{*/

/**
 *  @brief      This must be the first field of all IVIDDEC2FRONT
 *              instance objects.
 */
typedef struct IVIDDEC2FRONT_Obj {
    struct IVIDDEC2FRONT_Fxns *fxns;
} IVIDDEC2FRONT_Obj;


/**
 *  @brief      Opaque handle to an IVIDDEC2FRONT objects.
 */
typedef struct IVIDDEC2FRONT_Obj  *IVIDDEC2FRONT_Handle;

/**
 *  @brief      This must be the first field of all IVIDDEC2BACK
 *              instance objects.
 */
typedef struct IVIDDEC2BACK_Obj {
    struct IVIDDEC2BACK_Fxns *fxns;
} IVIDDEC2BACK_Obj;


/**
 *  @brief      Opaque handle to an IVIDDEC2BACK objects.
 */
typedef struct IVIDDEC2BACK_Obj  *IVIDDEC2BACK_Handle;

/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 *
 *  @remarks    All fields correspond to latest IVIDDEC2FRONT_Fxns::process()
 *              call on the particular instance of the decoder.
 *
 *  @sa         IVIDDEC2FRONT_Fxns::control()
 */
typedef struct IVIDDEC2FRONT_Status {
    XDAS_Int32 size;            /**< @sizeField */
    XDM_ContextInfo contextInfo;/**< Context info.
                                 *
                                 *   @remarks   Only valid when the cmd issued
                                 *              to control() is
                                 *              XDM_GETCONTEXTINFO.
                                 */
    XDAS_Int32 nextPartRequiredFlag;  /**< Flag indicating whether the
                                 *   next part in the split codec must be
                                 *   called or not.
                                 *
                                 *   @remarks   If this field is TRUE, the
                                 *              @c type union should be
                                 *              interpreted using the
                                 *              @c partial path.
                                 *
                                 *   @remarks   If this field is FALSE, the
                                 *              @c type union should be
                                 *              interpreted using the
                                 *              @c fullStatus path.
                                 */

    union {
        struct {
			XDAS_Int32 extendedError;   /**< @extendedErrorField */
            XDM_Context context;/**< Conditional context for the next codec
                                 *   part (if required).
                                 *
                                 *   @remarks   @c nextPartRequiredFlag
                                 *              indicates whether this
                                 *              field holds any contents
                                 *              or not.
                                 *
                                 *   @remarks   Except for the command
                                 *              XDM_GETCONTEXTINFO,
                                 *              the application must initialize
                                 *              this field with the context
                                 *              buffers required by the codec.
                                 */
        } partial;              /**< Data type used when the results in the
                                 *   status struct are incomplete and require
                                 *   a call into the next codec part.
                                 */
        IVIDDEC2_Status fullStatus; /**< Complete status results.
                                     *
                                     *   @remarks   This part of the @c type
                                     *              union should be used if
                                     *              @c nextPartRequiredFlag is
                                     *              XDAS_FALSE.
                                     */
    } type;                     /**< Type union identifier */
} IVIDDEC2FRONT_Status;


/**
 *  @brief      Defines the run time output arguments for
 *              all IVIDDEC2FRONT instance objects.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC2FRONT_Fxns::process()
 */
typedef struct IVIDDEC2FRONT_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesConsumed;   /**< Number of bytes consumed during the
                                 *   process() call.
                                 */
} IVIDDEC2FRONT_OutArgs;





/**
 *  @brief      Defines all of the operations on IVIDDEC2FRONT objects.
 */
typedef struct IVIDDEC2FRONT_Fxns {
    IALG_Fxns   ialg;             /**< xDAIS algorithm interface.
                                   *
                                   *   @sa      IALG_Fxns
                                   */

/**
 *  @brief      Basic video decoding call.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[in,out] context      Context provided to, and managed by the
 *                              split codec.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @remarks    process() is a blocking call.  When process() returns, the
 *              algorithm's processing is complete.
 *
 *  @remarks    The application must call IVIDDEC2FRONT::control() with
 *              #XDM_GETCONTEXTINFO to obtain the minimum buffer sizes
 *              the codec requires.
 *
 *  @remarks    @c context includes input and output buffers, intermediate
 *              buffers, and other values used by the codec.  See
 *              #XDM_Context for more details.
 *
 *  @pre        @c context->numInBufs will indicate the total number of input
 *              buffers supplied for input frame, and conditionally, the
 *              encoders MB data buffer.
 *
 *  @pre        If IVIDDEC2_DynamicParams::mbDataFlag was set to #XDAS_FALSE
 *              in a previous control() call, the application only needs to
 *              provide buffers for reconstruction frames.
 *
 *  @pre        If IVIDDEC2_DynamicParams::mbDataFlag was set to #XDAS_TRUE
 *              in a previous control() call,
 *              @c context->outBufs[context->numOutBufs - 1] is a buffer descriptor
 *              into which the algorithm will write MB data for each macro
 *              block.  The size of the MB data buffer will vary based on the
 *              decoder type.  H.264 may generate N264 bytes per MB, while
 *              Mpeg2 may generate NMP2 bytes.  The exact size of the buffers
 *              should be obtained by calling the algorithm's control() method
 *              with XDM_GETBUFINFO.
 *
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IVIDDEC2_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDDEC2FRONT_OutArgs structure.
 *
 *  @pre        @c context must not be NULL, and must point to a valid
 *              XDM_Context structure.
 *
 *  @pre        @c context->inBufs[0].buf must not be NULL, and must point to
 *              a valid buffer of data that is at least
 *              @c context->inBufs[0].bufSize bytes in length.
 *
 *  @pre        @c context->outBufs[0] must not be NULL, and must point to
 *              a valid buffer of data that is at least
 *              @c context->outBufs[0].bufSize bytes in length.
 *
 *  @pre        @c context->numInOutBufs must be zero (0).
 *
 *  @pre        The buffers in @c context->inBufs and @c context->outBufs are
 *              physically contiguous and owned by the calling application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c inArgs.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of
 *              @c context->inBufs, with the exception of
 *              @c context->inBufs[].accessMask.
 *              That is, the data and buffers pointed to by these parameters
 *              must be treated as read-only.
 *
 *  @post       The algorithm <b>must</b> modify the contents of
 *              @c context->inBufs[].accessMask and appropriately indicate the
 *              mode in which each of the buffers in @c context->inBufs were
 *              read.  For example, if the algorithm only read from
 *              @c context->inBufs[0].buf using the algorithm processor, it
 *              could utilize #XDM_SETACCESSMODE_READ to update the appropriate
 *              @c accessMask fields.
 *              The application <i>may</i> utilize these
 *              returned values to appropriately manage cache.
 *
 *  @post       The buffers in @c context->inBufs and @c context->outBufs are
 *              not owned by the calling application.
 *
 *  @post       The buffers in @c context->algContext and
 *              @c context->intermediateBufs[] are owned by the calling
 *              application, but must be provided - unmodified - to the
 *              next part in this split codec.
 *
 *  @retval     IVIDDEC2_EOK            @copydoc IVIDDEC2_EOK
 *  @retval     IVIDDEC2_EFAIL          @copydoc IVIDDEC2_EFAIL
 *                                      See IVIDDEC2_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     IVIDDEC2_EUNSUPPORTED   @copydoc IVIDDEC2_EUNSUPPORTED
 */
    XDAS_Int32 (*process)(IVIDDEC2FRONT_Handle handle, IVIDDEC2_InArgs *inArgs,
        XDM_Context *context, IVIDDEC2FRONT_OutArgs *outArgs);


/**
 *  @brief      Control behavior of an algorithm.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  params          Dynamic parameters.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @pre        @c handle must be a valid algorithm instance handle.
 *
 *  @pre        @c params must not be NULL, and must point to a valid
 *              IVIDDEC2_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDDEC2FRONT_Status structure.
 *
 *  @pre        If a buffer is provided in the @c status->data field,
 *              it must be physically contiguous and owned by the calling
 *              application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @post       If a buffer was provided in the @c status->data field,
 *              it is owned by the calling application.
 *
 *  @retval     IVIDDEC2_EOK            @copydoc IVIDDEC2_EOK
 *  @retval     IVIDDEC2_EFAIL          @copydoc IVIDDEC2_EFAIL
 *                                      See IVIDDEC2_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     IVIDDEC2_EUNSUPPORTED   @copydoc IVIDDEC2_EUNSUPPORTED
 */
    XDAS_Int32 (*control)(IVIDDEC2FRONT_Handle handle, IVIDDEC2_Cmd id,
        IVIDDEC2_DynamicParams *params, IVIDDEC2FRONT_Status *status);

} IVIDDEC2FRONT_Fxns;


/**
 *  @brief      Defines all of the operations on IVIDDEC2BACK objects.
 */
typedef struct IVIDDEC2BACK_Fxns {
    IALG_Fxns   ialg;             /**< xDAIS algorithm interface.
                                   *
                                   *   @sa      IALG_Fxns
                                   */

/**
 *  @brief      Basic video decoding call.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in,out] context      Context provided to, and managed by the
 *                              split codec.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @remarks    process() is a blocking call.  When process() returns, the
 *              algorithm's processing is complete.
 *
 *  @pre        @c context must not be NULL, and must point to a valid
 *              XDM_Context structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDDEC2_OutArgs structure.
 *
 *  @post       The buffers in @c context are
 *              owned by the calling application.
 *
 *  @retval     IVIDDEC2_EOK            @copydoc IVIDDEC2_EOK
 *  @retval     IVIDDEC2_EFAIL          @copydoc IVIDDEC2_EFAIL
 *                                      See IVIDDEC2_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     IVIDDEC2_EUNSUPPORTED   @copydoc IVIDDEC2_EUNSUPPORTED
 */
    XDAS_Int32 (*process)(IVIDDEC2BACK_Handle handle, XDM_Context *context,
        IVIDDEC2_OutArgs *outArgs);


/**
 *  @brief      Control behavior of an algorithm.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  context         Codec context.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @pre        @c handle must be a valid algorithm instance handle.
 *
 *  @pre        @c context must not be NULL, and must point to a valid
 *              XDM_Context structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDDEC2_Status structure.
 *
 *  @pre        If a buffer is provided in the @c status->data field,
 *              it must be physically contiguous and owned by the calling
 *              application.
 *
 *  @post       If a buffer was provided in the @c status->data field,
 *              it is owned by the calling application.
 *
 *  @retval     IVIDDEC2_EOK            @copydoc IVIDDEC2_EOK
 *  @retval     IVIDDEC2_EFAIL          @copydoc IVIDDEC2_EFAIL
 *                                      See IVIDDEC2_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     IVIDDEC2_EUNSUPPORTED   @copydoc IVIDDEC2_EUNSUPPORTED
 */
    XDAS_Int32 (*control)(IVIDDEC2BACK_Handle handle,
        XDM_Context *context, IVIDDEC2_Status *status);

} IVIDDEC2BACK_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.xdais.dm; 1, 0, 4,159; 4-21-2008 16:29:58; /db/wtree/cring/dais-k02x/src/
 */

