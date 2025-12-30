#pragma once
#include <chrono>
#include <functional>
#include "ui/View.h"

namespace animation
{
    class Animator
    {
    public:
        static void animatePosition(ui::View* view, 
            float fromX, float fromY, 
            float toX, float toY, 
            float duration);    

    private:
        struct Animation
        {
            std::chrono::steady_clock::time_point startTime;
            float duration;

            std::function<void(float)> onUpdate;

            float startValue;
            float endValue;
        };
    };
}