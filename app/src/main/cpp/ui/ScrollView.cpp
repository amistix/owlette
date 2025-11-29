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
        auto[vw, vh] = getViewport();

        int px = _x;
        int py = vh - (_y + _height);     
        glEnable(GL_SCISSOR_TEST);
        glScissor(px, py, _width, _height);

        drawSelf();

        // for (View *child : _children)
        // {
        //     int oldY = child->getY();
        //     child->setPosition(child->getX(), oldY + (int)_scrollY);
        //     child->draw();
        //     child->setPosition(child->getX(), oldY);
        // }
        for (View* child : _children) {
            child->_scrollApplied = _scrollY / (float)vh * -2.0f;
            child->draw();
            child->_scrollApplied = 0.0f;
        }

        glDisable(GL_SCISSOR_TEST);
    }

}
