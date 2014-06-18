LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE := ia_mp3_dec_lib
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel, --no-enum-size-warning, --no-wchar-size-warning

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= \
	OMX_Mp3Dec_CompThread.c		\
	OMX_Mp3Dec_Utils.c		\
	OMX_Mp3Decoder.c

LOCAL_C_INCLUDES := $(TI_OMX_COMP_C_INCLUDES) \
	$(TI_OMX_SYSTEM)/common/inc		\
	$(TI_OMX_AUDIO)/mp3_dec_ittiam/inc

LOCAL_SHARED_LIBRARIES := $(TI_OMX_COMP_SHARED_LIBRARIES) \
        liblog

LOCAL_STATIC_LIBRARIES := ia_mp3_dec_lib

LOCAL_LDLIBS += \
	-lpthread \
	-ldl \
	-lsdl

LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel,--no-enum-size-warning,--no-wchar-size-warning
	
LOCAL_CFLAGS := $(TI_OMX_CFLAGS) -DOMAP_2430 -D__OMX_DEBUG__

LOCAL_MODULE:= libOMX.ITTIAM.MP3.decode
LOCAL_MODULE_TAGS:= optional

include $(BUILD_SHARED_LIBRARY)
