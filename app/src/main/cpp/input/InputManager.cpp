#include "input/InputManager.h"

namespace input {

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::init(JNIEnv* env, jobject activity) {
    _env = env;
    _activity = activity;
    
    jclass activityClass = env->GetObjectClass(activity);
    _showKeyboardMethod = env->GetMethodID(activityClass, "showKeyboard", "()V");
    _hideKeyboardMethod = env->GetMethodID(activityClass, "hideKeyboard", "()V");
}

void InputManager::setActiveEditText(EditTextView* editText) {
    if (_activeEditText) {
        _activeEditText->setFocused(false);
    }
    
    _activeEditText = editText;
    
    if (_activeEditText) {
        _activeEditText->setFocused(true);
        showKeyboard();
    } else {
        hideKeyboard();
    }
}

void InputManager::showKeyboard() {
    if (_showKeyboardMethod && _env && _activity) {
        _env->CallVoidMethod(_activity, _showKeyboardMethod);
    }
}

void InputManager::hideKeyboard() {
    if (_hideKeyboardMethod && _env && _activity) {
        _env->CallVoidMethod(_activity, _hideKeyboardMethod);
    }
}

void InputManager::onTextChanged(const std::string& text) {
    if (_activeEditText) {
        _activeEditText->updateText(text);
    }
}

} // namespace ui