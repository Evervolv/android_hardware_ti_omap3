LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE := ia_heaacv2_dec_lib
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= \
        OMX_AacDec_CompThread.c \
        OMX_AacDec_Utils.c \
        OMX_AacDecoder.c

LOCAL_C_INCLUDES := \
        $(TI_OMX_COMP_C_INCLUDES) \
        $(TI_OMX_SYSTEM)/common/inc \
        $(TI_OMX_AUDIO)/aac_dec_ittiam/inc

LOCAL_SHARED_LIBRARIES := \
        $(TI_OMX_COMP_SHARED_LIBRARIES) \
        liblog

LOCAL_STATIC_LIBRARIES := ia_heaacv2_dec_lib

LOCAL_LDLIBS += \
        -ldl \

LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel,--no-enum-size-warning,--no-wchar-size-warning

LOCAL_CFLAGS := $(TI_OMX_CFLAGS) -DOMAP_2430

LOCAL_MODULE:= libOMX.ITTIAM.AAC.decode
LOCAL_MODULE_TAGS:= optional

include $(BUILD_SHARED_LIBRARY)
