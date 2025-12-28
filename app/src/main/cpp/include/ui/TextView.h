#pragma once 
#include <string>
#include "ui/View.h"
#include <GLES2/gl2.h>
#include "ui/FontAtlas.h"

namespace ui 
{
    class TextView: public View
    {
    public:
        TextView ();
        ~TextView ();

        void setText(const std::string& text);
        void setColorText(float r, float g, float b, float a);
        void setFontAtlas(FontAtlas* atlas);

        virtual void drawSelf() override;

    protected:
        std::string _text = "Hello, Owlette!";
        float _text_r = 1.0f, _text_g = 1.0f, _text_b = 1.0f, _text_a = 1.0f;
        FontAtlas* _atlas;
    };
    
}