# Owlette

It's repository of my android messaging app called Owlette that uses i2p(i2pd) for sending messages. It possibly might get slightly simplified due to my lack of knowledge in android development.

# How to build

## Install requared packages

Just ensure you have them:

```bash
sudo apt-get install g++ openjdk-17-jdk gradle
 ```

## Install Android SDK

You can find it here: https://developer.android.com/studio#downloads

```bash
mkdir android-sdk
cd android-sdk
wget https://dl.google.com/android/repository/commandlinetools-linux-8092744_latest.zip
unzip commandlinetools-linux-8092744_latest.zip
./cmdline-tools/bin/sdkmanager --sdk_root=/ANY/DIRECTORY/FOR/SDK "build-tools;33.0.1" "cmake;3.22.1" "ndk;23.2.8568313"
```

## Clone repository

```bash
git clone --recurse-submodules https://github.com/amistix/owlette.git
cd owlette
```

## Compile application

```bash
# path to installed java
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk
# path to --sdk_root
export ANDROID_HOME=~/android-sdk
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/23.2.8568313
sh build.sh
./gradlew clean assembleDebug
```

## Install it on android device

```bash
adb install app/build/outputs/apk/debug/owlette-debug.apk    
```
