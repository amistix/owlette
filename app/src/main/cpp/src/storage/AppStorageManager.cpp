#include "storage/AppStorageManager.h"
#include <jni.h>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "AppStorageManager", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "AppStorageManager", __VA_ARGS__)

extern JavaVM* g_vm;
extern jobject g_activity;

// ✅ Cache the class reference at startup
static jclass g_appStorageManagerClass = nullptr;

namespace storage
{
    // ✅ Call this during app initialization (from Java thread)
    void initAppStorageManagerClass(JNIEnv* env) {
        if (g_appStorageManagerClass) {
            return;  // Already initialized
        }
        
        jclass localClass = env->FindClass("org/amistix/owlette/AppStorageManager");
        if (localClass) {
            g_appStorageManagerClass = (jclass)env->NewGlobalRef(localClass);
            env->DeleteLocalRef(localClass);
            LOGD("AppStorageManager class cached successfully");
        } else {
            LOGE("Failed to find AppStorageManager class during init");
        }
    }
    
    static JNIEnv* getEnv() {
        if (!g_vm) return nullptr;
        
        JNIEnv* env = nullptr;
        jint result = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
        
        if (result == JNI_EDETACHED) {
            result = g_vm->AttachCurrentThread(&env, nullptr);
            if (result != JNI_OK) {
                LOGE("Failed to attach thread");
                return nullptr;
            }
        }
        
        return env;
    }

    void writeDest(const std::string& content)
    {
        JNIEnv* env = getEnv();
        if (!env || !g_activity || !g_appStorageManagerClass) {
            LOGE("Not initialized: env=%p, activity=%p, class=%p", 
                 env, g_activity, g_appStorageManagerClass);
            return;
        }

        // ✅ Use cached class instead of FindClass
        jmethodID constructor = env->GetMethodID(g_appStorageManagerClass, "<init>", "(Landroid/content/Context;)V");
        if (!constructor) {
            LOGE("Failed to find constructor");
            env->ExceptionDescribe();
            env->ExceptionClear();
            return;
        }

        jobject appStorageManagerObj = env->NewObject(g_appStorageManagerClass, constructor, g_activity);
        if (!appStorageManagerObj) {
            LOGE("Failed to create AppStorageManager object");
            env->ExceptionDescribe();
            env->ExceptionClear();
            return;
        }

        jmethodID mid = env->GetMethodID(g_appStorageManagerClass, "writeFileConfig", "(Ljava/lang/String;)V");
        if (!mid) {
            LOGE("Failed to find writeFileConfig method");
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(appStorageManagerObj);
            return;
        }

        jstring jcontent = env->NewStringUTF(content.c_str());
        env->CallVoidMethod(appStorageManagerObj, mid, jcontent);
        
        if (env->ExceptionCheck()) {
            LOGE("Exception in writeFileConfig");
            env->ExceptionDescribe();
            env->ExceptionClear();
        }

        env->DeleteLocalRef(jcontent);
        env->DeleteLocalRef(appStorageManagerObj);
        
        LOGD("Successfully wrote dest file");
    }

    std::string readDest()
    {
        JNIEnv* env = getEnv();
        if (!env || !g_activity || !g_appStorageManagerClass) {
            LOGE("Not initialized");
            return "";
        }

        jmethodID constructor = env->GetMethodID(g_appStorageManagerClass, "<init>", "(Landroid/content/Context;)V");
        if (!constructor) {
            LOGE("Failed to find constructor");
            return "";
        }

        jobject appStorageManagerObj = env->NewObject(g_appStorageManagerClass, constructor, g_activity);
        if (!appStorageManagerObj) {
            LOGE("Failed to create AppStorageManager object");
            return "";
        }

        jmethodID mid = env->GetMethodID(g_appStorageManagerClass, "readConfig", "()Ljava/lang/String;");
        if (!mid) {
            LOGE("Failed to find readConfig method");
            env->DeleteLocalRef(appStorageManagerObj);
            return "";
        }

        jstring result = (jstring)env->CallObjectMethod(appStorageManagerObj, mid);
        
        if (env->ExceptionCheck()) {
            LOGE("Exception in readConfig");
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(appStorageManagerObj);
            return "";
        }

        std::string str;
        if (result != nullptr) {
            const char* charStr = env->GetStringUTFChars(result, nullptr);
            str = std::string(charStr);
            env->ReleaseStringUTFChars(result, charStr);
            env->DeleteLocalRef(result);
        }

        env->DeleteLocalRef(appStorageManagerObj);
        
        LOGD("Successfully read dest file: %s", str.c_str());
        return str;
    }

    void writePeer(const std::string& content)
    {
        JNIEnv* env = getEnv();
        if (!env || !g_activity || !g_appStorageManagerClass) {
            LOGE("Not initialized: env=%p, activity=%p, class=%p", 
                 env, g_activity, g_appStorageManagerClass);
            return;
        }

        // ✅ Use cached class instead of FindClass
        jmethodID constructor = env->GetMethodID(g_appStorageManagerClass, "<init>", "(Landroid/content/Context;)V");
        if (!constructor) {
            LOGE("Failed to find constructor");
            env->ExceptionDescribe();
            env->ExceptionClear();
            return;
        }

        jobject appStorageManagerObj = env->NewObject(g_appStorageManagerClass, constructor, g_activity);
        if (!appStorageManagerObj) {
            LOGE("Failed to create AppStorageManager object");
            env->ExceptionDescribe();
            env->ExceptionClear();
            return;
        }

        jmethodID mid = env->GetMethodID(g_appStorageManagerClass, "writeFilePeer", "(Ljava/lang/String;)V");
        if (!mid) {
            LOGE("Failed to find writeFilePeer method");
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(appStorageManagerObj);
            return;
        }

        jstring jcontent = env->NewStringUTF(content.c_str());
        env->CallVoidMethod(appStorageManagerObj, mid, jcontent);
        
        if (env->ExceptionCheck()) {
            LOGE("Exception in writeFilePeer");
            env->ExceptionDescribe();
            env->ExceptionClear();
        }

        env->DeleteLocalRef(jcontent);
        env->DeleteLocalRef(appStorageManagerObj);
        
        LOGD("Successfully wrote dest file");
    }

    std::string readPeer()
    {
        JNIEnv* env = getEnv();
        if (!env || !g_activity || !g_appStorageManagerClass) {
            LOGE("Not initialized");
            return "";
        }

        jmethodID constructor = env->GetMethodID(g_appStorageManagerClass, "<init>", "(Landroid/content/Context;)V");
        if (!constructor) {
            LOGE("Failed to find constructor");
            return "";
        }

        jobject appStorageManagerObj = env->NewObject(g_appStorageManagerClass, constructor, g_activity);
        if (!appStorageManagerObj) {
            LOGE("Failed to create AppStorageManager object");
            return "";
        }

        jmethodID mid = env->GetMethodID(g_appStorageManagerClass, "readPeer", "()Ljava/lang/String;");
        if (!mid) {
            LOGE("Failed to find readPeer method");
            env->DeleteLocalRef(appStorageManagerObj);
            return "";
        }

        jstring result = (jstring)env->CallObjectMethod(appStorageManagerObj, mid);
        
        if (env->ExceptionCheck()) {
            LOGE("Exception in readPeer");
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(appStorageManagerObj);
            return "";
        }

        std::string str;
        if (result != nullptr) {
            const char* charStr = env->GetStringUTFChars(result, nullptr);
            str = std::string(charStr);
            env->ReleaseStringUTFChars(result, charStr);
            env->DeleteLocalRef(result);
        }

        env->DeleteLocalRef(appStorageManagerObj);
        
        LOGD("Successfully read peer file: %s", str.c_str());
        return str;
    }
    
    void cleanupAppStorageManagerClass(JNIEnv* env) {
        if (g_appStorageManagerClass) {
            env->DeleteGlobalRef(g_appStorageManagerClass);
            g_appStorageManagerClass = nullptr;
        }
    }
}