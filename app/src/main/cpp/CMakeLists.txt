cmake_minimum_required(VERSION 3.18)

# Project name and language
project(i2pd LANGUAGES CXX)

# Minimum Android API level
set(CMAKE_ANDROID_API 21)

# ABI (optional, Gradle usually handles this)
set(CMAKE_ANDROID_ARCH_ABI "arm64-v8a")

# Set C++ standard and enable RTTI/Exceptions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable exceptions and RTTI explicitly
add_compile_options(-fexceptions -frtti -DANDROID_BINARY -DANDROID -D__ANDROID__ -DUSE_UPNP -Wno-deprecated-declarations)

# PIE for executable linking
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Paths (equivalent to your Application.mk variables) ---
set(IFADDRS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/android-ifaddrs)
set(BOOST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/boost)
set(MINIUPNP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/miniupnp)
set(OPENSSL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/openssl)

set(I2PD_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/i2pd)
set(LIB_SRC_PATH ${I2PD_SRC_PATH}/libi2pd)
set(LIB_CLIENT_SRC_PATH ${I2PD_SRC_PATH}/libi2pd_client)
set(LANG_SRC_PATH ${I2PD_SRC_PATH}/i18n)
set(DAEMON_SRC_PATH ${I2PD_SRC_PATH}/daemon)

# --- Prebuilt static libraries ---
add_library(boost_program_options STATIC IMPORTED)
set_target_properties(boost_program_options PROPERTIES
    IMPORTED_LOCATION ${BOOST_PATH}/build/out/${CMAKE_ANDROID_ARCH_ABI}/lib/libboost_program_options-clang-mt-a64-1_84.a
    INTERFACE_INCLUDE_DIRECTORIES ${BOOST_PATH}/build/out/${CMAKE_ANDROID_ARCH_ABI}/include/boost-1_84
)

add_library(crypto STATIC IMPORTED)
set_target_properties(crypto PROPERTIES
    IMPORTED_LOCATION ${OPENSSL_PATH}/out/${CMAKE_ANDROID_ARCH_ABI}/lib/libcrypto.a
    INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_PATH}/out/${CMAKE_ANDROID_ARCH_ABI}/include
)

add_library(ssl STATIC IMPORTED)
set_target_properties(ssl PROPERTIES
    IMPORTED_LOCATION ${OPENSSL_PATH}/out/${CMAKE_ANDROID_ARCH_ABI}/lib/libssl.a
    INTERFACE_LINK_LIBRARIES crypto
    INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_PATH}/out/${CMAKE_ANDROID_ARCH_ABI}/include
)

add_library(miniupnpc STATIC IMPORTED)
set_target_properties(miniupnpc PROPERTIES
    IMPORTED_LOCATION ${MINIUPNP_PATH}/miniupnpc/out/${CMAKE_ANDROID_ARCH_ABI}/lib/libminiupnpc.a
    INTERFACE_INCLUDE_DIRECTORIES ${MINIUPNP_PATH}/miniupnpc/out/${CMAKE_ANDROID_ARCH_ABI}/include
)

# --- Collect all source files ---
file(GLOB LIB_SRC_FILES "${LIB_SRC_PATH}/*.cpp")
file(GLOB LIB_CLIENT_SRC_FILES "${LIB_CLIENT_SRC_PATH}/*.cpp")
file(GLOB LANG_SRC_FILES "${LANG_SRC_PATH}/*.cpp")

set(SRC_FILES
    DaemonAndroid.cpp
    ${IFADDRS_PATH}/ifaddrs.cpp
    ${IFADDRS_PATH}/bionic_netlink.cpp
    ${LIB_SRC_FILES}
    ${LIB_CLIENT_SRC_FILES}
    ${LANG_SRC_FILES}
    ${DAEMON_SRC_PATH}/UnixDaemon.cpp
    ${DAEMON_SRC_PATH}/Daemon.cpp
    ${DAEMON_SRC_PATH}/UPnP.cpp
    ${DAEMON_SRC_PATH}/HTTPServer.cpp
    ${DAEMON_SRC_PATH}/I2PControl.cpp
    ${DAEMON_SRC_PATH}/I2PControlHandlers.cpp
    ${DAEMON_SRC_PATH}/i2pd.cpp
)


add_library(i2pd STATIC ${SRC_FILES})

# Include directories
target_include_directories(i2pd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IFADDRS_PATH}
    ${LIB_SRC_PATH}
    ${LIB_CLIENT_SRC_PATH}
    ${LANG_SRC_PATH}
    ${DAEMON_SRC_PATH}
)

# Link prebuilt libraries
target_link_libraries(i2pd
    boost_program_options
    crypto
    ssl
    miniupnpc
    z      
    log
    EGL
    GLESv2
)

add_library(native-lib SHARED 
    native-lib.cpp 
    ui/View.cpp 
    "input/Dispatcher.cpp"
    ui/ScrollView.cpp
)
target_include_directories(native-lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(native-lib i2pd)