#!/bin/bash
set -e

BOOST_VERSION=1.84.0
BOOST_LIBS=program_options

function build {
	echo "Configuring and building..."
	CXXFLAGS="-std=c++20" \
	NCPU=$(nproc) \
	./build-android.sh \
	--boost=$BOOST_VERSION \ 
	--arch=$CPU \
	--link=static \
	--target-version=$API \
	--with-libraries=$BOOST_LIBS \
	--layout=system \
    --variant=release \
	--toolset=gcc \
	$ANDROID_NDK_HOME
}

cd boost

# disable verbose output
sed -i -E -e 's/d\+2/d\+0/' build-android.sh

API=21
CPU=arm64-v8a
build

