#pragma once 
#include <vector>
#include <functional>
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
        
        virtual void draw();
        virtual void destroy();

        void onTouchDown(float x, float y);
        void onTouchMove(float x, float y);
        void onTouchUp(float x, float y);

        void setOnTouchUpListener(std::function<void(float, float)> f);
        void setOnTouchDownListener(std::function<void(float, float)> f);
        void setOnTouchMoveListener(std::function<void(float, float)> f);

        View* hitTest(float x, float y);
        bool contains(float x, float y);

        float getX();
        float getY();

        View* getParent();
        void addChild(View* childView);
        std::vector<View*>& getChildren();

    protected:
        int _width, _height;
        int _y, _x;
        float _r, _g, _b, _a;

        std::function<void(float, float)> _onTouchDownFunc,
            _onTouchUpFunc, _onTouchMoveFunc;
        
        View* _parent;
        std::vector<View*> _children;

        int _viewportW, _viewportH;

        virtual void drawSelf() ;
    };
}