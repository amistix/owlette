#pragma once

#include "ui/TextView.h"
#include <string>
#include <functional>

namespace ui {

class EditTextView : public TextView {
public:
    EditText();
    ~EditText();

    void setFocused(bool focused);
    bool isFocused() const { return _focused; }
    
    void updateText(const std::string& text);  // Called from JNI
    void update(float deltaTime);
    
    void drawSelf() override;

private:
    void drawCursor();
    
    bool _focused = false;
    float _cursorBlinkTime = 0.0f;
    bool _showCursor = true;
};

} // namespace ui

#endif