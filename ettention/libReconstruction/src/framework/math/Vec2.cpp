#include "stdafx.h"
#include "Vec2.h"

namespace ettention
{
    template<typename _ValType>
    Vec2<_ValType>::Vec2()
        : x((_ValType)0)
        , y((_ValType)0)
    {

    }

    template<typename _ValType>
    Vec2<_ValType>::Vec2(_ValType x, _ValType y)
        : x(x)
        , y(y)
    {

    }

    template<typename _ValType>
    Vec2<_ValType>::~Vec2()
    {

    }

    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::xy() const
    {
        return *this;
    }

    template<typename _ValType>
    double Vec2<_ValType>::getLength() const
    {
        return std::sqrt((double)dotProduct(*this, *this));
    }

    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::maleInverse() const
    {
        double x = 1.0 / (double)this->x;
        double y = 1.0 / (double)this->y;
        return Vec2((_ValType)x, (_ValType)y);
    }

    template<typename _ValType>
    const _ValType& Vec2<_ValType>::operator[](unsigned int index) const
    {
        switch(index)
        {
        case 0: return x;
        case 1: return y;
        default: throw Exception("Invalid index for Vec2 component.");
        }
    }

    template<typename _ValType>
    _ValType& Vec2<_ValType>::operator[](unsigned int index)
    {
        switch(index)
        {
        case 0: return x;
        case 1: return y;
        default: throw Exception("Invalid index for Vec2 component.");
        }
    }

    template<typename _ValType>
    Vec2<_ValType>& Vec2<_ValType>::operator+=(const Vec2<_ValType>& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    template<typename _ValType>
    Vec2<_ValType>& Vec2<_ValType>::operator-=(const Vec2<_ValType>& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template<typename _ValType>
    Vec2<_ValType>& Vec2<_ValType>::operator*=(const Vec2<_ValType>& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    template<typename _ValType>
    Vec2<_ValType>& Vec2<_ValType>::operator/=(const Vec2<_ValType>& other)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }


    template<typename _ValType>
    Vec2<_ValType>& Vec2<_ValType>::operator*=(_ValType scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template<typename _ValType>
    Vec2<_ValType>& Vec2<_ValType>::operator/=(_ValType scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }


    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::operator+(const Vec2<_ValType>& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::operator-(const Vec2<_ValType>& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::operator*(const Vec2<_ValType>& other) const
    {
        return Vec2(x * other.x, y * other.y);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::operator/(const Vec2<_ValType>& other) const
    {
        return Vec2(x / other.x, y / other.y);
    }


    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::operator*(_ValType scalar) const
    {
        return Vec2(x * scalar, y * scalar);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec2<_ValType>::operator/(_ValType scalar) const
    {
        return Vec2(x / scalar, y / scalar);
    }


    template<typename _ValType>
    bool Vec2<_ValType>::operator==(const Vec2<_ValType>& other) const
    {
        return x == other.x && y == other.y;
    }

    template<typename _ValType>
    bool Vec2<_ValType>::operator!=(const Vec2<_ValType>& other) const
    {
        return !(*this == other);
    }


    template<typename _ValType>
    _ValType dotProduct(const Vec2<_ValType>& left, const Vec2<_ValType>& right)
    {
        return left.x * right.x + left.y * right.y;
    }

    template class Vec2<float>;
    template class Vec2<unsigned int>;
    template class Vec2<int>;
    template class Vec2<double>;
}