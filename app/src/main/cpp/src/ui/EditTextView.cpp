#include "ui/EditTextView.h"
#include "ui/FontRenderer.h"
#include <GLES2/gl2.h>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "EditTextView", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "EditTextView", __VA_ARGS__)

extern JavaVM* g_vm;
extern jobject g_activity;
extern jobject g_keyboardTriggerer;

namespace ui {

EditTextView* EditTextView::_focusedInstance = nullptr;

EditTextView::EditTextView() {}
EditTextView::~EditTextView() {}

void EditTextView::setFocused(bool focused) 
{
    _focused = focused;
    
    if (focused) 
    {
        _focusedInstance = this;
        openKeyboard();
    } 
    else 
    {
        if (_focusedInstance == this) _focusedInstance = nullptr;
        closeKeyboard();
    }
}

void EditTextView::updateText(const std::string& text) 
{
    setText(text);
}

void EditTextView::clearText()
{
    if (!g_vm || !g_keyboardTriggerer) {
        LOGE("Cannot clear text - no keyboard reference");
        return;
    }
    
    JNIEnv* env = nullptr;
    g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (!env) return;
    
    jclass cls = env->GetObjectClass(g_keyboardTriggerer);
    if (!cls) return;
    
    jmethodID mid = env->GetMethodID(cls, "clearText", "()V");
    if (!mid) 
    {
        LOGE("setText method not found");
        env->ExceptionDescribe();
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return;
    }
    env->CallVoidMethod(g_keyboardTriggerer, mid);
    
    if (env->ExceptionCheck()) 
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    
    env->DeleteLocalRef(cls);
    
    _text = "";
}

void EditTextView::drawSelf()
{
    if (!isShownOnScreen()) return;
    
    vec2<float> absPos = getAbsolutePosition();
    View::drawSelf();
    
    if (!_atlas) return;
    
    if (!_text.empty()) 
    {
        drawText(_text, absPos.x, absPos.y, 
                _text_color.x, _text_color.y, _text_color.z, _text_color.w, 
                *_atlas, this);
    }
    else 
    {
        drawText("Type here", absPos.x, absPos.y, 
                0.5f, 0.5f, 0.5f, 0.6f,  // Gray placeholder
                *_atlas, this);
    }
    
    if (_focused) onFocused();
}

void EditTextView::openKeyboard()
{
    if (!g_vm || !g_keyboardTriggerer) {
        LOGE("Cannot open keyboard - no reference");
        return;
    }
    
    JNIEnv* env = nullptr;
    g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (!env) return;
    
    jclass cls = env->GetObjectClass(g_keyboardTriggerer);
    if (!cls) {
        LOGE("Failed to get KeyboardTriggerer class");
        return;
    }
    
    jmethodID mid = env->GetMethodID(cls, "showKeyboard", "(Ljava/lang/String;)V");
    if (!mid) 
    {
        LOGE("showKeyboard method not found");
        env->ExceptionDescribe();
        env->ExceptionClear();
        env->DeleteLocalRef(cls);
        return;
    }
    
    jstring jText = env->NewStringUTF(_text.c_str());
    env->CallVoidMethod(g_keyboardTriggerer, mid, jText);
    
    if (env->ExceptionCheck()) 
    {
        LOGE("Exception calling showKeyboard");
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    
    env->DeleteLocalRef(jText);
    env->DeleteLocalRef(cls);
    
    LOGD("Keyboard opened with text: %s", _text.c_str());
}

void EditTextView::setOnFocusedListener(std::function<void()> f) 
{ 
    _onFocusedFunc = f;
}

void EditTextView::onFocused() 
{
    if (_onFocusedFunc) _onFocusedFunc();
}

void EditTextView::closeKeyboard() 
{
    if (!g_vm || !g_keyboardTriggerer) {
        LOGE("Cannot close keyboard - no reference");
        return;
    }
    
    JNIEnv* env = nullptr;
    bool needsDetach = false;
    
    jint result = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) 
    {
        g_vm->AttachCurrentThread(&env, nullptr);
        needsDetach = true;
    }
    
    if (!env) return;
    
    jclass cls = env->GetObjectClass(g_keyboardTriggerer);
    if (!cls) {
        if (needsDetach) g_vm->DetachCurrentThread();
        return;
    }
    
    jmethodID method = env->GetMethodID(cls, "hideKeyboard", "()V");
    if (method) {
        env->CallVoidMethod(g_keyboardTriggerer, method);
        
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
    
    env->DeleteLocalRef(cls);
    
    if (needsDetach) g_vm->DetachCurrentThread();
    
    LOGD("Keyboard closed");
}

}