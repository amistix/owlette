#!/bin/bash
set -e

JNI_PATH=app/src/main/jni
APP_PLATFORM=android-21

mkdir -p app/build/ndk
NDK_OUT=app/build/ndk/obj
NDK_LIBS_OUT=app/build/ndk/libs

cd binary/jni
sh build.sh
cd ..

$ANDROID_NDK_HOME/ndk-build \
  NDK_PROJECT_PATH=app/src/main \
  APP_BUILD_SCRIPT=app/jni/Android.mk \
  NDK_APPLICATION_MK=app/jni/Application.mk \
  APP_PLATFORM=$APP_PLATFORM \
  APP_ABI=arm64-v8a \
  NDK_OUT=$NDK_OUT \
  NDK_LIBS_OUT=$NDK_LIBS_OUT


