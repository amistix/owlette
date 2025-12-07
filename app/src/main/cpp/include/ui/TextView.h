#pragma once 
#include <string>
#include "ui/View.h"
#include <GLES2/gl2.h>

namespace ui 
{
    class TextView: public View
    {
    public:
        TextView ();
        ~TextView ();

        void setText(const std::string& text);
        void setColorText(float r, float g, float b, float a);

        virtual void drawSelf() override;

    private:
        std::string _text = "Hello, Owlette!";

    protected:
        float _text_r, _text_g, _text_b, _text_a = 0.0f;
    };
    
}