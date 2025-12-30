#pragma once 
#include "ui/Renderer.h"
#include <vector>
#include <functional>
#include <utility> 

#include "extra/vec2.h"
#include "extra/vec4.h"

namespace ui 
{
    class View
    {
    public:
        View ();
        ~View ();

        void setColor(vec4<float> color);

        void setSize(vec2<float> size);
        void setPosition(vec2<float> position);

        void setViewport(vec2<float> viewportSize);
        vec2<float> getViewport();
        
        virtual void draw();
        virtual void destroy();
        virtual void drawSelf();

        vec2<float> getAbsolutePosition(vec2<float> ownerPosition = vec2<float>{0,0});

        void onTouchDown(vec2<float> eventPosition);
        void onTouchMove(vec2<float> eventPosition);
        void onTouchUp(vec2<float> eventPosition);

        void setOnTouchUpListener(std::function<void(vec2<float>)> f);
        void setOnTouchDownListener(std::function<void(vec2<float>)> f);
        void setOnTouchMoveListener(std::function<void(vec2<float>)> f);

        virtual View* hitTest(vec2<float> position);
        bool isHittable() {return _hittable;}

        void setHittable(bool state);

        bool contains(vec2<float> hitPointPosition);
        bool isShownOnScreen();

        vec2<float> getPosition();

        vec2<float> getSize();

        View* getParent();
        void addChild(View* childView);
        std::vector<View*>& getChildren();

    protected:
        vec2<float> _size;
        vec2<float> _pos;

        vec4<float> _color;

        bool _hittable = true;

        std::function<void(vec2<float>)> _onTouchDownFunc,
            _onTouchUpFunc, _onTouchMoveFunc;
        
        View* _parent = nullptr;
        std::vector<View*> _children;

        vec2<float> _viewport;

    };
}