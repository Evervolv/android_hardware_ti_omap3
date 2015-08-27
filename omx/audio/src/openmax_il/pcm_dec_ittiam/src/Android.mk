LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional


include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= \
        OMX_PcmDec_CompThread.c \
        OMX_PcmDec_Utils.c \
        OMX_PcmDecoder.c

LOCAL_C_INCLUDES := \
        $(TI_OMX_COMP_C_INCLUDES) \
        $(TI_OMX_SYSTEM)/common/inc \
        $(TI_OMX_AUDIO)/pcm_dec_ittiam/inc

LOCAL_SHARED_LIBRARIES := \
        $(TI_OMX_COMP_SHARED_LIBRARIES) \
        liblog

LOCAL_LDLIBS += \
        -lpthread \
        -ldl \
        -lsdl

LOCAL_CFLAGS := $(TI_OMX_CFLAGS) -DOMAP_2430

LOCAL_MODULE:= libOMX.ITTIAM.PCM.decode
LOCAL_MODULE_TAGS:= optional

include $(BUILD_SHARED_LIBRARY)
