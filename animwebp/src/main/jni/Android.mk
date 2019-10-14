MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH :=$(MY_LOCAL_PATH)
ENABLE_SHARED  := 0

#################################################################################
## img2webp
#
#include $(CLEAR_VARS)
#$(info Current Path: $(LOCAL_PATH))
#
#LOCAL_SRC_FILES := \
#    WebpProcessor.c \
#
#LOCAL_CFLAGS := $(WEBP_CFLAGS)
#LOCAL_STATIC_LIBRARIES := imageio_util imagedec webpmux webpdemux \
#                          webp
#
#LOCAL_MODULE := img2webp
#
#include $(BUILD_SHARED_LIBRARY)

################################################################################
# anim_maker

include $(CLEAR_VARS)
$(info Current Path: $(LOCAL_PATH))

LOCAL_SRC_FILES := \
    AnimWebPMaker.cpp \
    native_AnimWebPMaker.cpp \

LOCAL_CFLAGS := $(WEBP_CFLAGS)
LOCAL_STATIC_LIBRARIES := imageio_util imagedec webpmux webpdemux \
                          webp

LOCAL_MODULE := anim_webp_maker
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

include $(MY_LOCAL_PATH)/libwebp/Android.mk
include $(MY_LOCAL_PATH)/imageio/Android.mk
