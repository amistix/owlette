#pragma once

#include "ui/TextView.h"
#include <jni.h>
#include <string>

namespace ui {

class EditTextView : public TextView {
public:
    EditTextView();  // Fixed: was EditText
    ~EditTextView(); // Fixed: was ~EditText

    static EditTextView* getFocusedInstance()
    { return _focusedInstance; }

    void setFocused(bool focused);
    bool isFocused() const { return _focused; }
    
    void updateText(const std::string& text);
    void openKeyboard();
    void closeKeyboard();
    
    void update(float deltaTime);
    void drawSelf() override;
    
private:
    void drawCursor();
    static EditTextView* _focusedInstance;
    bool _focused = false;
    float _cursorBlinkTime = 0.0f;
    bool _showCursor = true;
};

} // namespace ui