#ifndef __VEC2_HPP__
#define __VEC2_HPP__

#include <ostream>
#include <cmath>
#include <SFML/System/Vector2.hpp>

template <typename T>
struct Vec2
{
    //////////
    // DATA //
    //////////

    T x, y;

    //////////////////
    // CONSTRUCTORS //
    //////////////////

    Vec2(const T& x, const T& y) : 
        x {x}, y {y}
    {
    }

    Vec2(const T& xy) :
        Vec2(xy, xy)
    {
    }

    Vec2() :
        Vec2(T{0})
    {
    }

    template <typename U>
    static Vec2 from(const U& v) 
    {
        return Vec2( static_cast<T>(v.x), static_cast<T>(v.y) ); 
    }

    template <typename U>
    sf::Vector2<U> to() const
    {
        return U{ static_cast<U>(x), static_cast<U>(y) };
    }

    /////////////////
    // VECTOR MATH //
    /////////////////

    T dot(const Vec2& rhs)
    {
        return x * rhs.x + y * rhs.y;
    }

    T length() const
    {
        return std::hypot(x, y);
    }

    ///////////////
    // OPERATORS //
    ///////////////

    Vec2& operator+=(const Vec2& rhs)
    {
        x += rhs.x, y += rhs.y;
        return *this;
    }

    friend Vec2 operator+(Vec2 lhs, const Vec2& rhs)
    {
        return lhs += rhs;
    }

    Vec2& operator-=(const Vec2& rhs)
    {
        x -= rhs.x, y -= rhs.y;
        return *this;
    }

    friend Vec2 operator-(Vec2 lhs, const Vec2& rhs)
    {
        return lhs -= rhs;
    }

    Vec2& operator*=(const Vec2& rhs)
    {
        x *= rhs.x, y *= rhs.y;
        return *this;
    }

    friend Vec2 operator*(Vec2 lhs, const Vec2& rhs)
    {
        return lhs *= rhs;
    }

    Vec2& operator/=(const Vec2& rhs)
    {
        x /= rhs.x, y /= rhs.y;
        return *this;
    }

    friend Vec2 operator/(Vec2 lhs, const Vec2& rhs)
    {
        return lhs /= rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& rhs)
    {
        return os << "( " << rhs.x << " , " << rhs.y << " )";
    }

};

#endif