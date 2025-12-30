#include "ui/EditTextView.h"
#include "ui/FontRenderer.h"
#include <GLES2/gl2.h>

extern JavaVM* g_vm;
extern jobject g_activity;

namespace ui {

EditTextView* EditTextView::_focusedInstance = nullptr;

EditTextView::EditTextView() {
    setColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
    setColorText(0.0f, 0.0f, 0.0f, 1.0f);  // Black text
}

EditTextView::~EditTextView() {
}

void EditTextView::setFocused(bool focused) {
    _focused = focused;
    
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
    setText(text);
}

void EditTextView::drawSelf() {
    if (!isShownOnScreen()) return;
    auto [absX, absY] = getAbsolutePosition();
    
    // Draw white background
    View::drawSelf();
    
    // Draw black text
    if (!_text.empty() && _atlas) {
        drawText(_text, absX + 10, absY + 10, _text_r, _text_g, _text_b, _text_a, *_atlas, this);
    }

    if(_focused) onFocused();
}

void EditTextView::openKeyboard() {
    if (!g_vm) {
        return;
    }
    if (!g_activity) {
        return;
    }

    JNIEnv* env = nullptr;
    g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass cls = env->GetObjectClass(g_activity);
    if (!cls) {
        return;
    }

    jmethodID mid = env->GetMethodID(cls, "showKeyboard", "(Ljava/lang/String;)V");
    if (!mid) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    jstring jText = env->NewStringUTF(_text.c_str());
    env->CallVoidMethod(g_activity, mid, jText);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

void EditTextView::setOnFocusedListener(std::function<void()> f)
{ _onFocusedFunc = f;}

void EditTextView::onFocused()
{if(_onFocusedFunc) _onFocusedFunc();}

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

}