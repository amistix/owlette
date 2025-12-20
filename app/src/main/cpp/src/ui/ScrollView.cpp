#include "ui/ScrollView.h"
#include <algorithm>
#include <cmath>

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
        _velocityY = 0;  // Reset velocity when starting drag
    }

    void ScrollView::scroll(float x, float y) {
        if (!_dragging) return;

        float deltaY = y - _lastTouchY;
        _lastTouchY = y;

        _scrollY += deltaY;
        
        // Smooth velocity: blend current delta with previous velocity
        _velocityY = _velocityY * 0.7f + deltaY * 0.3f;

        // clamp scroll range
        float minScroll = _height - _containerHeight;
        float maxScroll = 0.0f;
        _scrollY = std::clamp(_scrollY, minScroll, maxScroll);
    }

    void ScrollView::unfocus(float x, float y) {
        _dragging = false;
    }

    void ScrollView::draw() 
    {
        if (!_dragging && std::abs(_velocityY) > 0.5f) {  // Higher threshold
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
                _velocityY *= 0.95f;  // Stronger friction (was 0.9)
            }
        } else if (!_dragging) {
            _velocityY = 0;  // Stop completely when velocity is too low
        }
        
        auto[vw, vh] = getViewport();

        int px = _x;
        int py = vh - (_y + _height);     
        glEnable(GL_SCISSOR_TEST);
        glScissor(px, py, _width, _height);

        drawSelf();

        for (View* child : _children) {
            int old_y = child->getY();
            child->setPosition(child->getX(), old_y + _scrollY);
            child->draw();
            child->setPosition(child->getX(), old_y);
        }

        glDisable(GL_SCISSOR_TEST);
    }

    View* ScrollView::hitTest(float x, float y) {
        float adjustedY = y - _scrollY;
        
        for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
            View* child = *it;
            
            if (child->contains(x, y - _scrollY)) {
                View* deeper = child->hitTest(x, adjustedY);
                return deeper ? deeper : child;
            }
        }
        return contains(x, y) ? this : nullptr;
    }
}