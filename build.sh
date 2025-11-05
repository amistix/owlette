#!/bin/bash
set -e

ASSETS_PATH=app/src/main/assets
APP_PLATFORM=android-21

mkdir -p app/build/ndk
NDK_OUT=app/build/ndk/obj
NDK_LIBS_OUT=app/build/ndk/libs

export NDK_MODULE_PATH=$(pwd)/external

cd external
sh build.sh
cd ..

cp -R $NDK_MODULE_PATH/i2pd/contrib/certificates \
  $ASSETS_PATH/certificates