#include "ui/View.h"
#include <GLES2/gl2.h>

static GLuint rectProgram = 0;
static GLint posLoc = -1;
static GLint colorLoc = -1;
static GLint sizeLoc = -1;
static GLint offsetLoc = -1;

using namespace ui;
static void initRectShader()
{
    if (rectProgram != 0) {
        glDeleteProgram(rectProgram);
        rectProgram = 0;
    }

    const char* vsSrc =
        "attribute vec2 aPos;"
        "uniform vec2 uOffset;"
        "uniform vec2 uSize;"
        "void main() {"
        "   vec2 pos = (aPos * uSize) + uOffset;"
        "   gl_Position = vec4(pos, 0.0, 1.0);"
        "}";

    const char* fsSrc =
        "precision mediump float;"
        "uniform vec4 uColor;"
        "void main() { gl_FragColor = uColor; }";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, nullptr);
    glCompileShader(fs);

    rectProgram = glCreateProgram();
    glAttachShader(rectProgram, vs);
    glAttachShader(rectProgram, fs);
    glLinkProgram(rectProgram);

    posLoc    = glGetAttribLocation(rectProgram, "aPos");
    offsetLoc = glGetUniformLocation(rectProgram, "uOffset");
    sizeLoc   = glGetUniformLocation(rectProgram, "uSize");
    colorLoc  = glGetUniformLocation(rectProgram, "uColor");
}

namespace ui
{
    View::View() {}

    View::~View() {}

    float View::getX(){return _x;}
    float View::getY(){return _y;}

    View* View::getParent()
    {
        return _parent;
    }

    std::vector<View*>& View::getChildren()
    {
        return _children;
    }

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

    void View::drawSelf()
    {
        initRectShader();
        glUseProgram(rectProgram);

        auto [vw, vh] = getViewport();

        // Convert pixel coordinates → OpenGL (-1 to 1)
        float xNorm = ( (float)_x      / vw ) * 2.0f - 1.0f;
        float yNorm = ( (float)_y      / vh ) * -2.0f + 1.0f;
        float wNorm = ( (float)_width  / vw ) * 2.0f;
        float hNorm = ( (float)_height / vh ) * -2.0f;

        GLfloat verts[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };

        glUniform2f(offsetLoc, xNorm, yNorm);
        glUniform2f(sizeLoc,   wNorm, hNorm);
        glUniform4f(colorLoc,  _r, _g, _b, _a);

        glEnableVertexAttribArray(posLoc);
        glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, verts);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(posLoc);
    }

    void View::draw()
    {
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

    void View::onTouchDown(float x, float y){
        if (_onTouchDownFunc) _onTouchDownFunc(x, y);
    };
    void View::onTouchMove(float x, float y){
        if (_onTouchMoveFunc) _onTouchMoveFunc(x, y);
    };
    void View::onTouchUp(float x, float y){
        if (_onTouchUpFunc) _onTouchUpFunc(x, y);
    };

    void View::setOnTouchDownListener(std::function<void(float, float)> f)
    { _onTouchDownFunc = f;}
    void View::setOnTouchUpListener(std::function<void(float, float)> f)
    { _onTouchUpFunc = f;}
    void View::setOnTouchMoveListener(std::function<void(float, float)> f)
    { _onTouchMoveFunc = f;}

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
        return x >= _x && x <= _x + _width &&
            y >= _y && y <= _y + _height;
    }
}