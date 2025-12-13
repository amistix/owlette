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
        virtual ui::View* hitTest(float x, float y) override;

        void setContainerHeight(float height);

    private:
        float _scrollY = 0;
        float _deltaY = 0;
        float _lastTouchY = 0;

        float _containerHeight = 0;
        float _velocityY = 0;
        float _friction = 0.99f;

        bool _dragging = false;
    };
}