#pragma once 
#include "ui/View.h"
#include <GLES2/gl2.h>

namespace ui 
{
    class ScrollView: public View
    {
    public:
        ScrollView ();
        ~ScrollView ();

        void focus(float x, float y);
        void unfocus(float x, float y);
        void scroll(float x, float y);

        virtual void draw() override;

        void setContainerHeight(float height);

    private:
        float _scrollY;
        float _deltaY;
        float _lastTouchY;

        float _containerHeight;

        bool _dragging;
    };
}