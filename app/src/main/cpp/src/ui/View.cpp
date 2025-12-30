#include "ui/View.h"
#include <GLES2/gl2.h>

namespace ui
{
    View::View() {}

    View::~View() {}

    vec2<float> View::getPosition() {return _pos;}

    vec2<float> View::getSize() {return _size;}

    View* View::getParent() {return _parent;}

    std::vector<View*>& View::getChildren() {return _children;}

    vec2<float> View::getViewport()
    {
        if (_parent) return _parent->getViewport();
        return _viewport;   
    }

    void View::addChild(View* childView)
    {
        _children.push_back(childView);
        childView->_parent = this;
    }

    void View::setSize(vec2<float> size)
    {
        _size = size;
    }

    void View::setColor(vec4<float> color)
    {
        _color = color;
    }

    void View::setPosition(vec2<float> pos)
    {
        _pos = pos;
    }

    void View::setViewport(vec2<float> viewportSize)
    {
        _viewport = viewportSize;
    }

    vec2<float> View::getAbsolutePosition(vec2<float> ownerPosition)
    {
        vec2<float> absPos = _pos + ownerPosition;

        if (_parent) return _parent->getAbsolutePosition(absPos);
        else return absPos;
    }

    void View::drawSelf()
    {
        glUseProgram(getRectProgram());

        vec2<float> viewPortSize = getViewport();
        vec2<float> absPos = getAbsolutePosition();

        // Convert pixel coordinates → OpenGL (-1 to 1)
        float xNorm = ( absPos.x / viewPortSize.x ) * 2.0f - 1.0f;
        float yNorm = ( absPos.y / viewPortSize.y ) * -2.0f + 1.0f;
        float wNorm = ( _size.x  / viewPortSize.x ) * 2.0f;
        float hNorm = ( _size.y  / viewPortSize.y ) * -2.0f;

        GLfloat verts[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };

        glUniform2f(getOffsetLoc(), xNorm, yNorm);
        glUniform2f(getSizeLoc(),   wNorm, hNorm);
        glUniform4f(getColorLoc(),  _color.x, _color.y, _color.z, _color.w);

        glEnableVertexAttribArray(getPosLoc());
        glVertexAttribPointer(getPosLoc(), 2, GL_FLOAT, GL_FALSE, 0, verts);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(getPosLoc());
    }

    void View::draw()
    {
        if (!isShownOnScreen()) return;
    
        drawSelf();
        for (View *child : _children)
        {
            child->draw();
        }
    }

    void View::destroy()
    {
        for (View *child : _children)
        {
            child->destroy();
            delete child;
        }
        _children.clear();
    }

    void View::onTouchDown(vec2<float> eventPosition)
    {if (_onTouchDownFunc) _onTouchDownFunc(eventPosition);};
    void View::onTouchMove(vec2<float> eventPosition)
    {if (_onTouchMoveFunc) _onTouchMoveFunc(eventPosition);};
    void View::onTouchUp(vec2<float> eventPosition)
    {if (_onTouchUpFunc) _onTouchUpFunc(eventPosition);};

    void View::setOnTouchDownListener(std::function<void(vec2<float>)> f)
    { _onTouchDownFunc = f;}
    void View::setOnTouchUpListener(std::function<void(vec2<float>)> f)
    { _onTouchUpFunc = f;}
    void View::setOnTouchMoveListener(std::function<void(vec2<float>)> f)
    { _onTouchMoveFunc = f;}

    bool View::isShownOnScreen()
    {
        vec2<float> viewPortSize = getViewport();
        vec2<float> absPos = getAbsolutePosition();

        return absPos.x + _size.x > 0 && absPos.x < viewPortSize.x &&
               absPos.y + _size.y > 0 && absPos.y < viewPortSize.y;
    }

    View* View::hitTest(vec2<float> position) 
    {
        for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
            View* child = *it;

            if (child->contains(position) && child->isHittable()) {
                View* deeper = child->hitTest(position);
                return deeper ? deeper : child;
            }
        }

        return contains(position) ? this : nullptr;
    }

    void View::setHittable(bool state) {_hittable = state;}

    bool View::contains(vec2<float> hitPointPosition) 
    {
        vec2<float> absPos = getAbsolutePosition();

        return hitPointPosition.x >= absPos.x && hitPointPosition.x <= absPos.x + _size.x &&
               hitPointPosition.y >= absPos.y && hitPointPosition.y <= absPos.y + _size.y;
    }
    
}