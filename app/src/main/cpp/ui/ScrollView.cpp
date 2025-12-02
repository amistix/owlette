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

        float deltaY = y - _lastTouchY;
        _lastTouchY = y;

        _scrollY += deltaY;
        _velocityY = deltaY;  // record current scroll speed

        // clamp scroll range
        float minScroll = _height - _containerHeight;   // bottom
        float maxScroll = 0.0f;                         // top
        _scrollY = std::clamp(_scrollY, minScroll, maxScroll);
    }

    void ScrollView::unfocus(float x, float y) {
        _dragging = false;
    }

    void ScrollView::draw() 
    {
        if (!_dragging && std::abs(_velocityY) > 0.1f) {
            _scrollY += _velocityY;

            // clamp scroll range
            float minScroll = _height - _containerHeight;
            float maxScroll = 0.0f;

            if (_scrollY < minScroll) {
                _scrollY = minScroll;
                _velocityY = 0;
            } else if (_scrollY > maxScroll) {
                _scrollY = maxScroll;
                _velocityY = 0;
            } else {
                _velocityY *= _friction;  // apply friction
            }
        }
        auto[vw, vh] = getViewport();

        int px = _x;
        int py = vh - (_y + _height);     
        glEnable(GL_SCISSOR_TEST);
        glScissor(px, py, _width, _height);

        drawSelf();

        for (View* child : _children) {
            child->_scrollApplied = _scrollY / (float)vh * -2.0f;
            child->draw();
            child->_scrollApplied = 0.0f;
        }

        glDisable(GL_SCISSOR_TEST);
    }

}
