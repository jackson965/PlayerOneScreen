LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := runmain
SDL_PATH := ../sdl
FFMPEG_PATH := ../ffmpeg
SCR_PATH := scr

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/$(SDL_PATH)/include \
$(LOCAL_PATH)/$(FFMPEG_PATH)/include \
$(LOCAL_PATH)/$(SCR_PATH)/include

  # Add your application source files here...
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.c)

#$(warning  LOCAL_PATH = $(LOCAL_PATH))
$(warning  LOCAL_SRC_FILES = $(LOCAL_SRC_FILES))

LOCAL_CFLAGS += -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-declarations
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -landroid

LOCAL_SHARED_LIBRARIES := avcodec avdevice avfilter avformat avutil  swresample swscale SDL2

include $(BUILD_SHARED_LIBRARY)
