#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "vec2.hpp"
#include <ostream>

namespace util
{

template <typename T>
T map(const T& v, const T& a, const T& b, const T& c, const T& d)
{
    return c + (v-a) * (d - c) / (b - a);
}

template <typename T>
class camera
{   
// private: 
public:
    T _zero, _size; // window 1
    T _bottom, _top; // window 2

public:

    //////////////////
    // CONSTRUCTORS //
    //////////////////

    camera(
        const T& old_bottom, const T& old_top, 
        const T& new_bottom, const T& new_top
    ) :
        _zero { old_bottom }, _size { old_top },
        _bottom { new_bottom }, _top { new_top }
    {
    }

    /////////////////////
    // TRANSFORMATIONS //
    /////////////////////

    camera& operator+=(const T& t)
    {
        _bottom += t, _top  += t;
        return *this;
    }

    camera& operator-=(const T& t)
    {
        _bottom -= t, _top -= t;
        return *this;
    }

    camera& operator*=(const T& t)
    {
        _bottom *= t, _top *= t;
        return *this;
    }

    camera& operator/=(const T& t)
    {
        _bottom /= t, _top /= t;
        return *this;
    }

    /////////////////
    // CONVERSIONS //
    /////////////////

    T pixel_to_world(const T& pt) const
    {
        return map(pt, _zero, _size, _bottom, _top);
    }

    T world_to_pixel(const T& pt) const
    {
        return map(pt, _bottom, _top, _zero, _size);
    }

    ///////////
    // DEBUG //
    ///////////

    friend std::ostream& operator<<(std::ostream& os, const camera& rhs)
    {
        return os << 
            "(\n" <<
                "\tOriginal Window: " << rhs._zero << ", " << rhs._size << "\n" <<
                "\tScale Window: " << rhs._bottom << ", " << rhs._top << "\n" <<
            ")";
    }

};

}

#endif