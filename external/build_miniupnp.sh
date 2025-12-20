#!/bin/bash
set -e

CMAKE_VERSION=3.22.1

function build {
	mkdir -p build out/$CPU
	cd build

	cmake \
	-G "Unix Makefiles" \
	-DUPNPC_BUILD_SHARED=False \
	-DUPNPC_BUILD_TESTS=False \
	-DUPNPC_BUILD_SAMPLE=False \
	-DANDROID_NATIVE_API_LEVEL=$API \
	-DANDROID_ABI=$CPU \
	-DCMAKE_BUILD_TYPE=Release \
	-DANDROID_NDK=$ANDROID_NDK_HOME \
	-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
	-DCMAKE_INSTALL_PREFIX=../out/$CPU \
	..

	echo "Building..."
	cmake --build . -- libminiupnpc-static
	make install

	cd ..
	rm -rf build
}



cd miniupnp/miniupnpc
rm -rf build out

# add cmake from Android SDK to PATH
PATH=$ANDROID_HOME/cmake/$CMAKE_VERSION/bin:$PATH

API=21
CPU=arm64-v8a
build
