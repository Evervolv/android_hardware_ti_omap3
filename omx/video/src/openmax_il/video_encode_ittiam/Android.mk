LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= \
	src/OMX_VideoEnc_Thread.c \
	src/OMX_VideoEnc_Utils.c  \
	src/OMX_VideoEncoder.c    \
	src/OMX_ividenc1_api.c    \
	src/OMX_ividenc1_utils.c  \
    src/OMX_DSPWait_Thread.c   \
    src/OMX_Callback_Thread.c

LOCAL_C_INCLUDES := $(TI_OMX_COMP_C_INCLUDES) \
 	    $(TI_OMX_VIDEO)/video_encode_ittiam/inc \
	    $(TI_OMX_VIDEO)/video_encode_ittiam/inc/import \
        $(HARDWARE_TI_OMAP3_BASE)/camera/inc \
        frameworks/av/include/media/stagefright \
        frameworks/native/include/media/hardware \


LOCAL_SHARED_LIBRARIES := $(TI_OMX_COMP_SHARED_LIBRARIES) \
                          libOMX.TI.mp4.splt.Encoder \
                          libOMX.TI.h264.splt.Encoder

LOCAL_CFLAGS := $(TI_OMX_CFLAGS) -DOMAP_2430 \
                -UVINAY_LOGGING \
                -UQUE_LOGGING \
                -USPLTENC_UTILS_DEBUG_ENABLE \
                -DSET_SEI_VUI_PARAMS \
		-DDSPALIGN_128BYTE \
		-D__OPENCORE_STORE_INPUT_BUFFER_ADDRESS__ \
		-U__ANALYSE_MEMORY__

LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel

LOCAL_MODULE:= libOMX.TI.720P.Encoder
LOCAL_MODULE_TAGS:= optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := bin/libOMX.TI.mp4.splt.Encoder.so \
						bin/libOMX.TI.h264.splt.Encoder.so
LOCAL_MODULE_TAGS:= optional
include $(BUILD_MULTI_PREBUILT)
