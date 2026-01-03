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
        std::string getText();

        void setColorText(vec4<float> color);

        void setFontAtlas(FontAtlas* atlas);
        FontAtlas* getFontAtlas();

        virtual void drawSelf() override;

    protected:
        std::string _text = "Hello, Owlette!";
        vec4<float> _text_color;
        FontAtlas* _atlas;
    };
    
}