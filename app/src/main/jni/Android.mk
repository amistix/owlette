LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := native-lib
LOCAL_SRC_FILES := native-lib.cpp
LOCAL_CPPFLAGS  := -std=c++17
include $(BUILD_SHARED_LIBRARY)