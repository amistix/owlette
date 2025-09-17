#include <jni.h>
#include <string>
#include "Daemon.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_org_amistix_owlette_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string message = "hi from i2pd!";
    return env->NewStringUTF(message.c_str());
}
