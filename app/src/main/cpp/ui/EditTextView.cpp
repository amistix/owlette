#include "ui/EditTextView.h"
#include "ui/FontRenderer.h"
#include <GLES2/gl2.h>

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "OWLETTE", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "OWLETTE", __VA_ARGS__)

extern JavaVM* g_vm;
extern jobject g_activity;  // This should be defined globally, not in class

namespace ui {

EditTextView* EditTextView::_focusedInstance = nullptr;

EditTextView::EditTextView() {
    setColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
    setColorText(0.0f, 0.0f, 0.0f, 1.0f);  // Black text
}

EditTextView::~EditTextView() {
}

void EditTextView::setFocused(bool focused) {
    LOGI("EditTextView setFocused: %d", focused);
    _focused = focused;
    _cursorBlinkTime = 0.0f;
    _showCursor = true;
    
    if (focused) {
        _focusedInstance = this;
        openKeyboard();
    } else {
        if (_focusedInstance == this) {
            _focusedInstance = nullptr;
        }
        closeKeyboard();
    }
}

void EditTextView::updateText(const std::string& text) {
    setText(text);  // Replace text, don't append
}

void EditTextView::update(float deltaTime) {
    if (_focused) {
        _cursorBlinkTime += deltaTime;
        if (_cursorBlinkTime >= 0.5f) {
            _cursorBlinkTime = 0.0f;
            _showCursor = !_showCursor;
        }
    }
}

void EditTextView::drawSelf() {
    auto [absX, absY] = getAbsolutePosition();
    
    // Draw white background
    View::drawSelf();
    
    // Draw black text
    if (!_text.empty()) {
        drawText(_text, absX + 10, absY + 10, 0.0f, 0.0f, 0.0f, 1.0f, 48.0f);
    }
}

void EditTextView::openKeyboard() {
    if (!g_vm) {
        LOGE("g_vm is null");
        return;
    }
    if (!g_activity) {
        LOGE("g_activity is null");
        return;
    }

    JNIEnv* env = nullptr;
    g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass cls = env->GetObjectClass(g_activity);
    if (!cls) {
        LOGE("Failed to get activity class");
        return;
    }

    jmethodID mid = env->GetMethodID(cls, "showKeyboard", "(Ljava/lang/String;)V");
    if (!mid) {
        LOGE("showKeyboard() method NOT FOUND");
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    LOGI("Calling showKeyboard()");
    jstring jText = env->NewStringUTF(_text.c_str());
    env->CallVoidMethod(g_activity, mid, jText);

    if (env->ExceptionCheck()) {
        LOGE("Exception while calling showKeyboard()");
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

void EditTextView::closeKeyboard() {
    if (!g_vm || !g_activity) return;
    
    JNIEnv* env = nullptr;
    bool needsDetach = false;
    
    jint result = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) {
        g_vm->AttachCurrentThread(&env, nullptr);
        needsDetach = true;
    }
    
    jclass activityClass = env->GetObjectClass(g_activity);
    jmethodID method = env->GetMethodID(activityClass, "hideKeyboard", "()V");
    env->CallVoidMethod(g_activity, method);
    env->DeleteLocalRef(activityClass);
    
    if (needsDetach) {
        g_vm->DetachCurrentThread();   
    }
}

} // namespace ui