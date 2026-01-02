#include "animation/Animator.h"


namespace animation
{
    std::vector<Animator::Animation> Animator::_animations;
    float Animator::_currentTime = 0.0f;

    // template <typename T, typename>
    // void Animator::animateCustom(
    //     std::function<void(T)> customFunc,
    //     T startValue,
    //     T endValue,
    //     std::function<float(float)> timingCurveFunc,
    //     float duration)
    // {

    //     //f(0) = 0, f(1) = 1
    //     if (std::abs(timingCurveFunc(0.0f) - 0.0f) > 0.001f ||
    //         std::abs(timingCurveFunc(1.0f) - 1.0f) > 0.001f) 
    //     {timingCurveFunc = [](float t){return t;};}

    //     Animation anim;
    //     anim.startTime = _currentTime;
    //     anim.duration = duration;
    //     anim.isActive = true;
    //     anim.onUpdate = [customFunc, startValue, endValue, timingCurveFunc](float t) 
    //     {
    //         customFunc(startValue * (1 - t) + endValue * t);
    //     };
    //     _animations.push_back(anim);
    // }

    void Animator::animatePosition(ui::View* view, 
        vec2<float> startPos, 
        vec2<float> endPos,
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
        anim.onUpdate = [view, startPos, endPos, timingCurveFunc](float t) 
        {
            view->setPosition(vec2<float>{
                (1.0f - timingCurveFunc(t)) * startPos.x + (timingCurveFunc(t)) * endPos.x,
                (1.0f - timingCurveFunc(t)) * startPos.y + (timingCurveFunc(t)) * endPos.y
            });
        };
        _animations.push_back(anim);
    }

    void Animator::animateSize(ui::View* view, 
        vec2<float> startSize, 
        vec2<float> endSize,
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
        anim.onUpdate = [view, startSize, endSize, timingCurveFunc](float t) 
        {
            view->setSize(vec2<float>{
                (1.0f - timingCurveFunc(t)) * startSize.x + (timingCurveFunc(t)) * endSize.x,
                (1.0f - timingCurveFunc(t)) * startSize.y + (timingCurveFunc(t)) * endSize.y
            });
        };
        _animations.push_back(anim);
    }

    void Animator::animateColor(ui::View* view, 
        vec4<float> startColor, 
        vec4<float> endColor,
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
        anim.onUpdate = [view, startColor, endColor, timingCurveFunc](float t) 
        {
            view->setColor(vec4<float>{
                (1.0f - timingCurveFunc(t)) * startColor.x + (timingCurveFunc(t)) * endColor.x,
                (1.0f - timingCurveFunc(t)) * startColor.y + (timingCurveFunc(t)) * endColor.y,
                (1.0f - timingCurveFunc(t)) * startColor.z + (timingCurveFunc(t)) * endColor.z,
                (1.0f - timingCurveFunc(t)) * startColor.w + (timingCurveFunc(t)) * endColor.w,
            });
        };
        _animations.push_back(anim);
    }

    void Animator::update(float deltaTime)
    {
        _currentTime += deltaTime;

        for (auto& anim : _animations)
        {
            if(!anim.isActive) continue;
            float elapsed = _currentTime - anim.startTime;

            float t = std::min(1.0f, elapsed / anim.duration);
            if (anim.onUpdate) anim.onUpdate(t);

            if (t >= 1.0f) anim.isActive = false;

            _animations.erase(
                std::remove_if(_animations.begin(), _animations.end(),
                            [](const Animation& a) { return !a.isActive; }),
                _animations.end()
            );
        }
    }
}