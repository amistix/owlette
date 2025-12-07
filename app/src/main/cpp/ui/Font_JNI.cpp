#include <jni.h>
#include "ui/FontRenderer.h"
#include <GLES2/gl2.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <cstring>

#define LOG_TAG "JNI_FontRenderer"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Globals for the font atlas
extern int g_fontWidth;
extern int g_fontHeight;

extern unsigned char* g_fontPixels;

extern std::vector<int> g_glyphX;
extern std::vector<int> g_glyphW;

extern std::string g_charset; 

// Called from Java FontRenderer.initFont()
extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_FontRenderer_nativeSetFontData(
        JNIEnv* env,
        jclass,
        jobject buffer,
        jint width,
        jint height,
        jintArray glyphX,
        jintArray glyphW,
        jstring chars)
{
    LOGD("Loading font atlas: %dx%d\n", width, height);
    g_fontWidth = width;
    g_fontHeight = height;

    // --- Copy atlas pixels ---
    g_fontPixels = (unsigned char*) env->GetDirectBufferAddress(buffer);

    // --- Copy glyph X & W ---
    jsize len = env->GetArrayLength(glyphX);
    g_glyphX.resize(len);
    g_glyphW.resize(len);

    env->GetIntArrayRegion(glyphX, 0, len, g_glyphX.data());
    env->GetIntArrayRegion(glyphW, 0, len, g_glyphW.data());

    // --- Copy charset ---
    const char* cstr = env->GetStringUTFChars(chars, nullptr);
    g_charset = cstr;
    env->ReleaseStringUTFChars(chars, cstr);

    // [OPTIONAL] Upload to OpenGL once
    // createGLTextureForFontAtlas();

    LOGD("Font loaded. Atlas=%dx%d, glyphs=%d\n", width, height, (int)len);
}
