#include "ui/View.h"
#include <GLES2/gl2.h>

namespace ui
{
    View::View() {}

    View::~View() {}

    float View::getX(){return _x;}
    float View::getY(){return _y;}

    std::pair<float, float> View::getSize() {return {_width, _height};}

    View* View::getParent(){return _parent;}

    std::vector<View*>& View::getChildren(){return _children;}

    std::pair<int, int> View::getViewport()
    {
        if (_parent) return _parent->getViewport();
        return {_viewportW, _viewportH};   
    }

    void View::addChild(View* childView)
    {
        _children.push_back(childView);
        childView->_parent = this;
    }

    void View::setSize(int width, int height)
    {
        _width = width;
        _height = height;
    }

    void View::setColor(float r, float g, float b, float a)
    {
        _r = r; _g = g; _b = b; _a = a;
    }

    void View::setPosition(int x, int y)
    {
        _x = x; _y = y;
    }

    void View::setViewport(int w, int h)
    {
        _viewportW = w; _viewportH = h;
    }

    std::pair<int,int> View::getAbsolutePosition(int ownerX, int ownerY)
    {
        int absX = ownerX + _x;
        int absY = ownerY + _y;

        if (_parent)
        {
            return _parent->getAbsolutePosition(absX, absY);
        }
        else
        {
            return {absX, absY};
        }
    }

    void View::drawSelf()
    {
        glUseProgram(getRectProgram());

        auto [vw, vh] = getViewport();

        auto [absX, absY] = getAbsolutePosition();

        // Convert pixel coordinates → OpenGL (-1 to 1)
        float xNorm = ( (float)absX    / vw ) * 2.0f - 1.0f;
        float yNorm = ( (float)absY    / vh ) * -2.0f + 1.0f;
        float wNorm = ( (float)_width  / vw ) * 2.0f;
        float hNorm = ( (float)_height / vh ) * -2.0f;

        GLfloat verts[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };

        glUniform2f(getOffsetLoc(), xNorm, yNorm);
        glUniform2f(getSizeLoc(),   wNorm, hNorm);
        glUniform4f(getColorLoc(),  _r, _g, _b, _a);

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

    void View::onTouchDown(float x, float y)
    {if (_onTouchDownFunc) _onTouchDownFunc(x, y);};
    void View::onTouchMove(float x, float y)
    {if (_onTouchMoveFunc) _onTouchMoveFunc(x, y);};
    void View::onTouchUp(float x, float y)
    {if (_onTouchUpFunc) _onTouchUpFunc(x, y);};

    void View::setOnTouchDownListener(std::function<void(float, float)> f)
    { _onTouchDownFunc = f;}
    void View::setOnTouchUpListener(std::function<void(float, float)> f)
    { _onTouchUpFunc = f;}
    void View::setOnTouchMoveListener(std::function<void(float, float)> f)
    { _onTouchMoveFunc = f;}

    bool View::isShownOnScreen()
    {
        auto [vw, vh] = getViewport();
        auto [absX, absY] = getAbsolutePosition();

        return absX + _width > 0  && absX < vw &&
            absY + _height > 0 && absY < vh;
    }

    View* View::hitTest(float x, float y) {
        for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
            View* child = *it;
            if (child->contains(x, y)) {
                View* deeper = child->hitTest(x, y);
                return deeper ? deeper : child;
            }
        }

        return contains(x, y) ? this : nullptr;
    }

    bool View::contains(float x, float y) {
        auto [absX, absY] = getAbsolutePosition();
        return x >= absX && x <= absX + _width &&
            y >= absY && y <= absY + _height;
    }
    
}