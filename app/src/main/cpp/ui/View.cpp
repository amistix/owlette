#include "ui/View.h"

namespace ui
{
    View::View() {}

    View::~View() {}

    View* View::getParent()
    {
        return _parent;
    }

    std::vector<View*> View::getChildren()
    {
        return _children;
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
}