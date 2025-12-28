#include "ui/TextView.h"
#include "ui/FontRenderer.h"

namespace ui 
{
    TextView::TextView () {}
    TextView::~TextView() {}

    void TextView::setText(const std::string& text)
    { _text = text; }

    void TextView::setColorText(float r, float g, float b, float a)
    { _text_r = r; _text_g = g; _text_b = b; _text_a = a; }

    void TextView::setFontAtlas(FontAtlas* atlas)
    {
        _atlas = atlas;
    }

    void TextView::drawSelf() 
    {
        auto [absX, absY] = getAbsolutePosition();

        View::drawSelf();
        if (_atlas) drawText(_text, absX, absY, _text_r, _text_g, _text_b, _text_a, *_atlas);
    }
}