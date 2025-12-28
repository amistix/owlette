#include "ui/Font_JNI.h"

#define LOG_TAG "JNI_FontRenderer"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Globals for the font atlas
extern JavaVM* g_vm;

FontAtlas* initFont(float textSize)
{
    if (!g_vm) {
        LOGE("JVM not set, cannot request font");
        return nullptr;
    }

    JNIEnv* env = nullptr;
    bool needDetach = false;
    
    // Try to get env
    int getEnvResult = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (getEnvResult == JNI_EDETACHED) {
        // Need to attach this thread
        if (g_vm->AttachCurrentThread(&env, nullptr) != 0) {
            LOGE("Failed to attach thread");
            return nullptr;
        }
        needDetach = true;
    } else if (getEnvResult != JNI_OK) {
        LOGE("Failed to get JNI env");
        return nullptr;
    }

    jclass javaRendererClass = env->FindClass("org/amistix/owlette/FontRenderer");

    if (!javaRendererClass) {
        LOGE("Failed to get FontRenderer class");
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }

    // Get the static method
    jmethodID mid = env->GetStaticMethodID(javaRendererClass, "initFont",
        "(F)Lorg/amistix/owlette/JavaFontAtlas;");
    if (!mid) {
        LOGE("Failed to find initFont method");
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }
    
    // Call Java method
    jobject resultObj = env->CallStaticObjectMethod(javaRendererClass, mid, (jfloat)textSize);
    
    // Check for exceptions first
    if (env->ExceptionCheck()) {
        LOGE("Exception calling initFont");
        env->ExceptionDescribe();
        env->ExceptionClear();
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }
    
    // Check if result is null
    if (!resultObj) {
        LOGE("initFont returned null");
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }

    jclass javaFontAtlasClass = env->GetObjectClass(resultObj);
    if (!javaFontAtlasClass) {
        LOGE("Failed to get JavaFontAtlas class");
        env->DeleteLocalRef(resultObj);
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }

    // Get field IDs
    jfieldID fontSizeField = env->GetFieldID(javaFontAtlasClass, "fontSize", "F");
    jfieldID bufferField = env->GetFieldID(javaFontAtlasClass, "buffer", "Ljava/nio/ByteBuffer;");
    jfieldID widthField = env->GetFieldID(javaFontAtlasClass, "width", "I");
    jfieldID heightField = env->GetFieldID(javaFontAtlasClass, "height", "I");
    jfieldID glyphXField = env->GetFieldID(javaFontAtlasClass, "glyphX", "[I");
    jfieldID glyphWField = env->GetFieldID(javaFontAtlasClass, "glyphW", "[I");
    jfieldID charsField = env->GetFieldID(javaFontAtlasClass, "chars", "Ljava/lang/String;");

    if (!fontSizeField || !bufferField || !widthField || !heightField || 
        !glyphXField || !glyphWField || !charsField) {
        LOGE("Failed to get one or more field IDs");
        env->DeleteLocalRef(javaFontAtlasClass);
        env->DeleteLocalRef(resultObj);
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }

    // Get field values
    jfloat fontSize = env->GetFloatField(resultObj, fontSizeField);
    jobject buffer = env->GetObjectField(resultObj, bufferField);
    jint width = env->GetIntField(resultObj, widthField);
    jint height = env->GetIntField(resultObj, heightField);
    jintArray glyphX = (jintArray)env->GetObjectField(resultObj, glyphXField);
    jintArray glyphW = (jintArray)env->GetObjectField(resultObj, glyphWField);
    jstring chars = (jstring)env->GetObjectField(resultObj, charsField);

    // Validate data
    if (!buffer || !glyphX || !glyphW || !chars || width <= 0 || height <= 0) {
        LOGE("Invalid data from Java: buffer=%p glyphX=%p glyphW=%p chars=%p width=%d height=%d",
             buffer, glyphX, glyphW, chars, width, height);
        if (buffer) env->DeleteLocalRef(buffer);
        if (glyphX) env->DeleteLocalRef(glyphX);
        if (glyphW) env->DeleteLocalRef(glyphW);
        if (chars) env->DeleteLocalRef(chars);
        env->DeleteLocalRef(javaFontAtlasClass);
        env->DeleteLocalRef(resultObj);
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }
    
    // Create atlas
    FontAtlas* atlas = new FontAtlas();
    atlas->fontSize = fontSize;
    atlas->textureWidth = width;
    atlas->textureHeight = height;
    
    // Copy pixel data
    int size = width * height;
    atlas->pixels = new unsigned char[size];
    
    unsigned char* src = (unsigned char*)env->GetDirectBufferAddress(buffer);
    if (src) {
        std::memcpy(atlas->pixels, src, size);
        LOGD("Copied %d bytes of pixel data", size);
        
        // ✅ ADD DETAILED PIXEL INSPECTION
        int nonZeroCount = 0;
        int firstNonZero = -1;
        int sampleSize = std::min(size, 5000);
        
        for (int i = 0; i < sampleSize; i++) {
            if (atlas->pixels[i] > 0) {
                nonZeroCount++;
                if (firstNonZero == -1) {
                    firstNonZero = i;
                }
            }
        }
        
        LOGD("Pixel inspection: checked first %d pixels", sampleSize);
        LOGD("  Non-zero pixels: %d (%.1f%%)", nonZeroCount, 100.0f * nonZeroCount / sampleSize);
        LOGD("  First non-zero at index: %d", firstNonZero);
        
        if (firstNonZero >= 0) {
            LOGD("  Value at first non-zero: %d", atlas->pixels[firstNonZero]);
        }
        
        // Log a horizontal slice through the middle to visualize
        int midY = height / 2;
        std::string pixelRow = "";
        for (int x = 0; x < std::min(60, width); x++) {
            int idx = midY * width + x;
            unsigned char val = atlas->pixels[idx];
            if (val > 200) pixelRow += "#";
            else if (val > 128) pixelRow += "+";
            else if (val > 64) pixelRow += ".";
            else if (val > 16) pixelRow += "·";
            else pixelRow += " ";
        }
        LOGD("  Mid-row sample: [%s]", pixelRow.c_str());
        
        // Sample around where 'O' should be (start of "Owlette")
        if (firstNonZero >= 0) {
            int oX = 0; // 'O' is the first capital letter
            // Find 'O' in charset
            const char* cstr = env->GetStringUTFChars(chars, nullptr);
            std::string charset = cstr;
            env->ReleaseStringUTFChars(chars, cstr);
            
            size_t oIndex = charset.find('O');
            if (oIndex != std::string::npos) {
                LOGD("  'O' is at charset index %zu", oIndex);
            }
        }
        
    } else {
        LOGE("Failed to get direct buffer address from Java");
        delete atlas;
        env->DeleteLocalRef(buffer);
        env->DeleteLocalRef(glyphX);
        env->DeleteLocalRef(glyphW);
        env->DeleteLocalRef(chars);
        env->DeleteLocalRef(javaFontAtlasClass);
        env->DeleteLocalRef(resultObj);
        env->DeleteLocalRef(javaRendererClass);
        if (needDetach) g_vm->DetachCurrentThread();
        return nullptr;
    }
    
    // Copy glyph data
    jsize lenX = env->GetArrayLength(glyphX);
    jsize lenW = env->GetArrayLength(glyphW);
    
    if (lenX != lenW) {
        LOGE("Glyph array length mismatch: X=%d W=%d", lenX, lenW);
    }
    
    atlas->glyphX.resize(lenX);
    atlas->glyphW.resize(lenW);
    
    env->GetIntArrayRegion(glyphX, 0, lenX, atlas->glyphX.data());
    env->GetIntArrayRegion(glyphW, 0, lenW, atlas->glyphW.data());
    
    // Copy charset
    const char* cstr = env->GetStringUTFChars(chars, nullptr);
    if (cstr) {
        atlas->charset = cstr;
        env->ReleaseStringUTFChars(chars, cstr);
        LOGD("Font atlas created: size=%.1f, dims=%dx%d, glyphs=%zu, charset='%s'",
             fontSize, width, height, atlas->glyphX.size(), atlas->charset.c_str());
    } else {
        LOGE("Failed to get charset string");
    }

    LOGD("=== Font Atlas Created ===");
    LOGD("Font size: %.1f", fontSize);
    LOGD("Texture: %dx%d", width, height);
    LOGD("Charset length: %zu", atlas->charset.length());
    LOGD("Glyph count: %zu", atlas->glyphX.size());
    
    // Log first few glyphs
    for (size_t i = 0; i < std::min(size_t(10), atlas->glyphX.size()); i++) {
        LOGD("  Glyph %zu ('%c'): x=%d w=%d", 
             i, atlas->charset[i], atlas->glyphX[i], atlas->glyphW[i]);
    }

    // Clean up local references
    env->DeleteLocalRef(chars);
    env->DeleteLocalRef(glyphX);
    env->DeleteLocalRef(glyphW);
    env->DeleteLocalRef(buffer);
    env->DeleteLocalRef(javaFontAtlasClass);
    env->DeleteLocalRef(resultObj);
    env->DeleteLocalRef(javaRendererClass);
    
    if (needDetach) {
        g_vm->DetachCurrentThread();
    }
    
    return atlas;
}