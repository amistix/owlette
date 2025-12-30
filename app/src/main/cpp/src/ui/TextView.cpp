#include "ui/TextView.h"
#include "ui/FontRenderer.h"

namespace ui 
{
    TextView::TextView () {}
    TextView::~TextView() {}

    void TextView::setText(const std::string& text) {_text = text;}

    void TextView::setColorText(vec4<float> color) {_text_color = color;}

    void TextView::setFontAtlas(FontAtlas* atlas) {_atlas = atlas;}

    void TextView::drawSelf() 
    {
        if (!isShownOnScreen()) return;
        
        vec2<float> absPos = getAbsolutePosition();

        View::drawSelf();
        if (_atlas) drawText(_text, absPos.x, absPos.y, _text_color.x, _text_color.y, _text_color.z, _text_color.w, *_atlas, this);
    }
}