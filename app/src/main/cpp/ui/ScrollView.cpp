#include "ui/ScrollView.h"
#include <algorithm>

namespace ui {

    ScrollView::ScrollView() {}
    ScrollView::~ScrollView() {}

    void ScrollView::setContainerHeight(float height)
    {
        _containerHeight = height;
    }

    void ScrollView::focus(float x, float y) {
        _dragging = true;
        _lastTouchY = y;
    }

    void ScrollView::scroll(float x, float y) {
        if (!_dragging) return;

        float _deltaY = y - _lastTouchY;
        _lastTouchY = y;

        _scrollY += _deltaY;

        // clamp scroll range
        float minScroll = _height - _containerHeight;   // bottom
        float maxScroll = 0.0f;                      // top

        _scrollY = std::clamp(_scrollY, minScroll, maxScroll);
    }

    void ScrollView::unfocus(float x, float y) {
        _dragging = false;
    }

    void ScrollView::draw() 
    {
        // 1) Clip using glScissor
        auto[vw, vh] = getViewport();

        int px = _x;
        int py = vh - (_y + _height);      // Y is inverted for glScissor
        glEnable(GL_SCISSOR_TEST);
        glScissor(px, py, _width, _height);

        // 2) Draw background (normal View draw)
        drawSelf();

        // 3) Draw children shifted by scrollY
        for (View *child : _children)
        {
            int oldY = child->getY();
            child->setPosition(child->getX(), oldY + (int)_scrollY);
            child->draw();
            child->setPosition(child->getX(), oldY);
        }

        glDisable(GL_SCISSOR_TEST);
    }

}
