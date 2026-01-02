#pragma once
#include <chrono>
#include <vector>
#include <functional>
#include <type_traits>

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

        template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
        static void animateCustom(
            std::function<void(T)> customFunc,
            T startValue,
            T endValue,
            std::function<float(float)> timingCurveFunc,
            float duration)
        
            {
                //f(0) = 0, f(1) = 1
                if (std::abs(timingCurveFunc(0.0f) - 0.0f) > 0.001f ||
                    std::abs(timingCurveFunc(1.0f) - 1.0f) > 0.001f) 
                {timingCurveFunc = [](float t){return t;};}

                Animation anim;
                anim.startTime = _currentTime;
                anim.duration = duration;
                anim.isActive = true;
                anim.onUpdate = [customFunc, startValue, endValue, timingCurveFunc](float t) 
                {
                    customFunc(startValue * (1 - t) + endValue * t);
                };
                _animations.push_back(anim);
            }
        

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