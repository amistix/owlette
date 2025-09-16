#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_org_amistix_owlette_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "@anon";
    return env->NewStringUTF(hello.c_str());
}
