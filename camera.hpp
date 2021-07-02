#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "vec2.hpp"

template <typename T>
T map(const T& v, const T& a, const T& b, const T& c, const T& d)
{
    return c + (v-a) * (d - c) / (b - a);
}

template <typename T>
class Camera
{   

    /// camera is really just a vector with 2 extra functions 
    // (i could extend from vector lmaolmaolmao)

    Vec2<T> zero, size;
    Vec2<T> bottom_left, top_right;

public:

    Camera(
        const Vec2<T>& zero, const Vec2<T>& size, 
        const Vec2<T>& bl, const Vec2<T>& tr) :
        zero { zero }, size { size },
        bottom_left { bl }, top_right { tr }
    {
    }

    Camera& operator+=(const Vec2<T>& t)
    {
        bottom_left += t;
        top_right += t;

        return *this;
    }

    Camera& operator-=(const Vec2<T>& t)
    {
        bottom_left -= t;
        top_right -= t;
        
        return *this;
    }

    Camera& operator*=(const Vec2<T>& t)
    {
        bottom_left *= t;
        top_right *= t;

        return *this;
    }

    Camera& operator/=(const Vec2<T>& t)
    {
        bottom_left /= t;
        top_right /= t;

        return *this;
    }

    Vec2<T> pixel_to_world(const Vec2<T>& pt) const
    {
        return map(pt, zero, size, bottom_left, top_right);
    }

    Vec2<T> world_to_pixel(const Vec2<T>& pt) const
    {
        return map(pt, bottom_left, top_right, zero, size);
    }

};

#endif