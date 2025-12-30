#pragma once
#include <chrono>
#include <vector>
#include <functional>
#include "ui/View.h"

namespace animation
{
    class Animator
    {
    public:
        static void animatePosition(ui::View* view, 
            vec2<float> startPos, 
            vec2<float> endPos,
            std::function<float(float)> timingCurveFunc,
            float duration);

        static void animateSize(ui::View* view, 
            vec2<float> startSize, 
            vec2<float> endSize,
            std::function<float(float)> timingCurveFunc,
            float duration);     

        static void animateColor(ui::View* view, 
            vec4<float> startColor, 
            vec4<float> endColor,
            std::function<float(float)> timingCurveFunc,
            float duration);    
            
        static void update(float deltaTime);

    private:
        struct Animation
        {
            float startTime;
            float duration;

            std::function<void(float)> onUpdate;
            bool isActive;
        };

        static std::vector<Animation> _animations;
        static float _currentTime;
        Animator() = delete;
    };
}