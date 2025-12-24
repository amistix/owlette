#!/bin/bash
set -e

HOST_OS="$(uname -s)"

# Fail early if NDK is not set
if [ -z "${ANDROID_NDK_HOME:-}" ]; then
  echo "Failed! ANDROID_NDK_HOME is empty"
  exit 1
fi

export ANDROID_NDK_ROOT="$ANDROID_NDK_HOME"

cd openssl

# Clean only BEFORE configure (important for OpenSSL 3.x)
make distclean >/dev/null 2>&1 || true
rm -rf out

# Select correct toolchain
if [[ "$HOST_OS" == *"NT"* ]]; then
  export PATH="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/windows-x86_64/bin:$PATH"
else
  export PATH="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH"
fi

API=21
CPU=arm64-v8a
TARGET=android-arm64

build() {
  mkdir -p "out/$CPU"

  echo "Configuring OpenSSL for $CPU..."
  ./Configure \
    "$TARGET" \
    no-shared \
    no-tests \
    -D__ANDROID_API__="$API" \
    --prefix="$PWD/out/$CPU"

  echo "Building OpenSSL for $CPU..."
  make -j"$(nproc)"

  make install_sw
}

build
