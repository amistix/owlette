#pragma once 
#include <vector>
#include <utility> 

namespace ui 
{
    class View
    {
    public:
        View ();
        ~View ();

        void setColor(float r, float g, float b, float a);
        void setSize(int width, int height);
        void setPosition(int x, int y);
        void setViewport(int w, int h);
        std::pair<int,int> getViewport();
        void addChild(View* childView);
        void draw();
        void destroy();

        View* getParent();
        std::vector<View*>& getChildren();

    protected:
        int _width, _height;
        int _y, _x;
        float _r, _g, _b, _a;
        
        View* _parent;
        std::vector<View*> _children;

        int _viewportW, _viewportH;

        virtual void drawSelf() ;
    };
}