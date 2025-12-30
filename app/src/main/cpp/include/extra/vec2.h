#pragma once

template <typename T>
struct vec2
{
    T x;
    T y;

    vec2() : x(0.0f), y(0.0f) {}
    template<typename U>
    vec2(U x, U y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

    inline vec2<T> operator+(vec2<T> a) {return vec2<T>(x + a.x, y + a.y);}
    inline vec2<T> operator-(vec2<T> a) {return vec2<T>(x - a.x, y - a.y);}
};