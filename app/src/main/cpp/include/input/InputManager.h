#pragma once
#include <jni.h>
#include "ui/EditTextView.h"

namespace input {

class InputManager {
public:
    static InputManager& getInstance();
    
    void init(JNIEnv* env, jobject activity);
    void setActiveEditTextView(EditTextView* editText);
    
    void showKeyboard();
    void hideKeyboard();
    void onTextChanged(const std::string& text);
    
private:
    InputManager() = default;
    
    JNIEnv* _env = nullptr;
    jobject _activity = nullptr;
    EditText* _activeEditText = nullptr;
    
    jmethodID _showKeyboardMethod = nullptr;
    jmethodID _hideKeyboardMethod = nullptr;
};

} // namespace ui

#endif