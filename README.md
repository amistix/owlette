# Owlette

```bash
wget https://dl.google.com/android/repository/commandlinetools-linux-8092744_latest.zip
unzip commandlinetools-linux-8092744_latest.zip
./cmdline-tools/bin/sdkmanager --sdk_root=. "build-tools;33.0.1" "cmake;3.22.1" "ndk;23.2.8568313"
```

```bash
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk
export ANDROID_HOME=~/android-sdk
export ANDROID_NDK_HOME=~/android-sdk/ndk/23.2.8568313
sh build.sh
./gradlew clean assembleDebug
```

```bash
adb install app/build/outputs/apk/debug/owlette-debug.apk    
```
