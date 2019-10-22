LOCAL_PATH := $(call my-dir)

APPABI := $(TARGET_ARCH_ABI)
PATH_LIBS = $(LOCAL_PATH)/../../../../libs/$(APPABI)
###########################
#
# ffmpeg shared library
#
###########################
include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := $(PATH_LIBS)/libavcodec.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avdevice
LOCAL_SRC_FILES := $(PATH_LIBS)/libavdevice.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := $(PATH_LIBS)/libavfilter.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := $(PATH_LIBS)/libavformat.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := $(PATH_LIBS)/libavutil.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := $(PATH_LIBS)/libswresample.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := $(PATH_LIBS)/libswscale.so
#LOCAL_EXPORT_C_INCLUDES := include
include $(PREBUILT_SHARED_LIBRARY)


#include $(CLEAR_VARS)
#LOCAL_MODULE := postproc
#LOCAL_SRC_FILES := $(PATH_LIBS)/libpostproc.so
#LOCAL_EXPORT_C_INCLUDES := include
#include $(PREBUILT_SHARED_LIBRARY)
