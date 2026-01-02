#include "ui/ScrollView.h"
#include <algorithm>
#include <cmath>

namespace ui 
{

    ScrollView::ScrollView() {}
    ScrollView::~ScrollView() {}

    void ScrollView::setContainerHeight(float height) {_containerHeight = height;}
    float ScrollView::getContainerHeight() {return _containerHeight;}

    void ScrollView::focus(vec2<float> pos) 
    {
        _dragging = true;
        _lastTouchY = pos.y;
        _velocityY = 0; 
    }

    void ScrollView::scroll(vec2<float> pos) 
    {
        if (!_dragging) return;
        if (_containerHeight < _size.y) return;

        float deltaY = pos.y - _lastTouchY;
        _lastTouchY = pos.y;

        _scrollY += deltaY;
        
        _velocityY = _velocityY * 0.7f + deltaY * 0.3f;

        // clamp scroll range
        float minScroll = _size.y - _containerHeight;
        float maxScroll = 0.0f;
        _scrollY = std::clamp(_scrollY, minScroll, maxScroll);
    }

    void ScrollView::unfocus(vec2<float> pos) {_dragging = false;}

    void ScrollView::draw() 
    {
        if (!_dragging && std::abs(_velocityY) > 0.5f) {
            _scrollY += _velocityY;

            float minScroll = _size.y - _containerHeight;
            float maxScroll = 0.0f;

            if (_scrollY < minScroll) 
            {
                _scrollY = minScroll;
                _velocityY = 0;
            } else if (_scrollY > maxScroll) 
            {
                _scrollY = maxScroll;
                _velocityY = 0;
            } else _velocityY *= 0.95f; 
        } 
        else if (!_dragging) _velocityY = 0; 
        
        vec2<float> viewPortSize = getViewport();

        float px = _pos.x;
        float py = viewPortSize.y - (_pos.y + _size.y);

        glEnable(GL_SCISSOR_TEST);
        glScissor(px, py, _size.x, _size.y);

        drawSelf();

        for (View* child : _children) {
            vec2<float> oldPos = child->getPosition();
            child->setPosition(vec2<float>{oldPos.x, oldPos.y + _scrollY});
            child->draw();
            child->setPosition(vec2<float>{oldPos.x, oldPos.y});
        }

        glDisable(GL_SCISSOR_TEST);
    }

    View* ScrollView::hitTest(vec2<float> position) 
    {
        float adjustedY = position.y - _scrollY;
        
        for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
            View* child = *it;
            
            if (child->contains(vec2<float>{position.x, position.y - _scrollY})) {
                View* deeper = child->hitTest(vec2<float>{position.x, adjustedY});
                return deeper ? deeper : child;
            }
        }
        return contains(position) ? this : nullptr;
    }

    void ScrollView::setScrollValue(float x)
    {
        _scrollY = x * (_size.y - _containerHeight);
    }

    float ScrollView::getScrollValue()
    {
        return _scrollY / (_size.y - _containerHeight);
    }
}