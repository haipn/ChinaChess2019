LOCAL_PATH := $(call my-dir)
# Chess engine
include $(CLEAR_VARS)
# add `-DANDROID_DEBUG` to `LOCAL_CFLAGS` to enable debug
LOCAL_CFLAGS := -Wall -g 
LOCAL_LDFLAGS := -Wl
LOCAL_LDLIBS := -llog
LOCAL_C_INCLUDES += D\:\\Sdk\\ndk-bundle\\platforms\\android-19\\arch-arm\\usr\\include
LOCAL_MODULE    := engine
LOCAL_SRC_FILES := engine_jni.cpp
LOCAL_SRC_FILES += engine.cpp bkdata.cpp
include $(BUILD_SHARED_LIBRARY)
