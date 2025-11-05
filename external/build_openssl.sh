#!/bin/bash

set -e

HOST_OS=`uname -a`

function build {
	mkdir -p out/$CPU

	echo "Configuring OpenSSL for $CPU..."
	./Configure \
	--prefix="$PWD/out/$CPU" \
	$TARGET \
	no-shared \
	no-tests \
	-D__ANDROID_API__=$API \
	-Wno-macro-redefined

	echo "Building OpenSSL for $CPU..."
	make -j $(nproc) > out/build.log

	make install_sw >> out/build.log

	make clean
}

export ANDROID_NDK_ROOT=$ANDROID_NDK_HOME

cd openssl
rm -rf out

if [[ "$HOST_OS" == *"_NT-"* ]]; then
  PATH=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/windows-x86_64/bin:$PATH
else
  PATH=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH
fi

if [[ -f 'Makefile' ]]; then
	make clean
fi

API=21
CPU=arm64-v8a
TARGET=android-arm64
build
