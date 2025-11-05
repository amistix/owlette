#pragma once 
#include <GLES2/gl2.h>
#include <vector>

namespace ui 
{
    class View
    {
    public:
        View ();
        ~View ();

        void setSize(int width, int height);
        void addChild(View* childView);

        View* getParent();
        std::vector<View*> getChildren();

    private:
        int _width;
        int _height;
        View* _parent;
        std::vector<View*> _children;
    };
}