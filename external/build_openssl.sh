#!/bin/bash
set -e

HOST_OS="$(uname -s)"

if [ -z "${ANDROID_NDK_HOME:-}" ]; then
  echo "Failed! ANDROID_NDK_HOME is empty"
  exit 1#!/bin/bash
set -e

HOST_OS="$(uname -s)"

if [ -z "${ANDROID_NDK_HOME:-}" ]; then
  echo "Failed! ANDROID_NDK_HOME is empty"
  exit 1
fi

export ANDROID_NDK_ROOT="$ANDROID_NDK_HOME"
cd "$(dirname "$0")/openssl"

echo "Cleaning OpenSSL..."
make distclean >/dev/null 2>&1 || true
rm -rf out

if [[ "$HOST_OS" == *"NT"* ]]; then
  export PATH="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/windows-x86_64/bin:$PATH"
else
  export PATH="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH"
fi

API=21
CPU=arm64-v8a

build() {
  mkdir -p "out/$CPU"

  echo "Configuring OpenSSL (disabling only SVE2)..."
  
  # Explicitly set compilers and flags
  export CC="aarch64-linux-android${API}-clang"
  export CXX="aarch64-linux-android${API}-clang++"
  export CFLAGS="-fPIC"
  
  ./Configure \
    android-arm64 \
    -D__ANDROID_API__="$API" \
    -fPIC \
    no-shared \
    no-tests \
    disable-poly1305 \
    --prefix="$PWD/out/$CPU"

  echo "Building OpenSSL..."
  make -j"$(nproc)"

  make install_sw
  
  echo "✓ OpenSSL built (SVE2 disabled, other assembly enabled)"
}

build
fi

export ANDROID_NDK_ROOT="$ANDROID_NDK_HOME"
cd "$(dirname "$0")/openssl"

# Thorough clean
echo "Cleaning OpenSSL..."
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

  echo "Configuring OpenSSL for $CPU (no assembly)..."
  ./Configure \
    "$TARGET" \
    -D__ANDROID_API__="$API" \
    -fPIC \
    no-shared \
    no-tests \
    no-asm \
    --prefix="$PWD/out/$CPU"

  echo "Building OpenSSL..."
  make -j"$(nproc)"

  echo "Installing OpenSSL..."
  make install_sw
  
  echo "✓ OpenSSL built successfully without assembly optimizations"
}

build