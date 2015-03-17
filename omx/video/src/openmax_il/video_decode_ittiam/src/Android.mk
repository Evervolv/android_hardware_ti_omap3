LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE := ittiam_h264
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE := ittiam_wmv9
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_ARM_MODE:= arm

LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES:= \
        OMX_VideoDec_Thread.c \
        OMX_VideoDec_HandleBuffers.c \
        OMX_VideoDec_HandleCommands.c \
        OMX_VideoDec_LoadDefaults.c \
        OMX_VideoDec_Logger.c \
        OMX_VideoDec_Utils.c \
        OMX_VideoDecoder.c \
        OMX_VideoDec_Parse.c \
        split_dec.c \
        split_dec_utils.c \
        split_dsp_dec.c \
        mpeg4_asc_api.c

LOCAL_C_INCLUDES := \
        $(TI_OMX_COMP_C_INCLUDES) \
        $(TI_OMX_VIDEO)/video_decode_ittiam/inc \
        $(TI_OMX_VIDEO)/video_decode_ittiam/inc/xdm \
        $(TI_OMX_VIDEO)/video_decode_ittiam/inc/misc \
        hardware/ti/omap3/ion \
        hardware/ti/omap3/hwc \
        system/core/include/cutils \
        system/core/include/system \
        hardware/ti/omap3/omx/ducati/domx/system/omx_core/inc \
        hardware/libhardware/include
      #  hardware/ti/omap3/liboverlay

ifeq ($(PERF_INSTRUMENTATION),1)
LOCAL_C_INCLUDES += \
        $(TI_OMX_SYSTEM)/perf/inc
endif

LOCAL_SHARED_LIBRARIES := \
        $(TI_OMX_COMP_SHARED_LIBRARIES) \
        libion \
        libbridge \
	libhardware

LOCAL_STATIC_LIBRARIES := \
        ittiam_h264 \
        ittiam_wmv9

ifeq ($(PERF_INSTRUMENTATION),1)
LOCAL_SHARED_LIBRARIES += \
        libPERF
endif

LOCAL_CFLAGS := \
        $(TI_OMX_CFLAGS) \
        -DANDROID \
        -DOMAP_2430 \
        -DCONFIG_3630 \
        -Wall \
        -DPARSE_HEADER \
        -DDSPALIGN_128BYTE \
        -URESOURCE_MANAGER_ENABLED \
        -UCHECK_SUM_ENABLE

LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel

LOCAL_MODULE:= libOMX.TI.720P.Decoder
LOCAL_MODULE_TAGS:= optional

include $(BUILD_SHARED_LIBRARY)
