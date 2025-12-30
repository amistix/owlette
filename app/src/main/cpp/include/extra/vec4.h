#pragma once

template <typename T>
struct vec4
{
    T x;
    T y;
    T z;
    T w;

    vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    template<typename U>
    vec4(U x, U y, U z, U w) 
        :x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)), w(static_cast<T>(w)) {}

    inline vec2<T> operator+(vec2<T> a) {
        return vec2<T>(x + a.x, y + a.y, z + a.z, w + a.w);
    }
    inline vec2<T> operator-(vec2<T> a) {
        return vec2<T>(x - a.x, y - a.y, z - a.z, w - a.w);
    }
};