#include "ui/EditTextView.h"
#include "ui/FontRenderer.h"
#include <GLES2/gl2.h>

namespace ui {

EditTextView::EditText() {}

EditTextView::~EditText() {}

void EditTextView::setFocused(bool focused) {
    _focused = focused;
    _cursorBlinkTime = 0.0f;
    _showCursor = true;
}

void EditTextView::updateText(const std::string& text) {
    setText(_text + text);
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
    
    // Draw cursor
    if (_focused && _showCursor) {
        drawCursor();
    }
}

void EditTextView::drawCursor() {
    auto [absX, absY] = getAbsolutePosition();
    auto [vw, vh] = getViewport();
    
    // Cursor position after text
    float textWidth = _text.length() * 20.0f;
    float cursorX = absX + 10 + textWidth;
    float cursorY = absY + 10;
    float cursorHeight = 40.0f;
    float cursorWidth = 2.0f;
    
    // Convert to normalized coordinates
    float x1 = (cursorX / vw) * 2.0f - 1.0f;
    float y1 = (cursorY / vh) * -2.0f + 1.0f;
    float x2 = x1 + (cursorWidth / vw) * 2.0f;
    float y2 = y1 - (cursorHeight / vh) * 2.0f;
    
    glUseProgram(getRectProgram());
    
    GLfloat verts[] = {
        x1, y1,
        x2, y1,
        x1, y2,
        x2, y2
    };
    
    glUniform2f(getOffsetLoc(), 0.0f, 0.0f);
    glUniform2f(getSizeLoc(), 1.0f, 1.0f);
    glUniform4f(getColorLoc(), 0.0f, 0.0f, 0.0f, 1.0f);  // Black cursor
    glUniform1f(getScrollLoc(), _scrollApplied);
    
    glEnableVertexAttribArray(getPosLoc());
    glVertexAttribPointer(getPosLoc(), 2, GL_FLOAT, GL_FALSE, 0, verts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(getPosLoc());
}

} // namespace ui