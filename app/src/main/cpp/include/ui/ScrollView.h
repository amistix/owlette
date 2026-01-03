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

        void focus(vec2<float> pos);
        void unfocus(vec2<float> pos);
        void scroll(vec2<float> pos);

        float getScrollValue();
        void setScrollValue(float x);

        virtual void draw() override;
        virtual ui::View* hitTest(vec2<float> position) override;

        void setContainerHeight(float height);
        float getContainerHeight();

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