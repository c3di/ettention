#include "stdafx.h"
#include "Vec3.h"

namespace ettention
{
    template<typename _ValType>
    Vec3<_ValType>::Vec3()
        : x((_ValType)0)
        , y((_ValType)0)
        , z((_ValType)0)
    {

    }

    template<typename _ValType>
    Vec3<_ValType>::Vec3(_ValType x, _ValType y, _ValType z)
        : x(x)
        , y(y)
        , z(z)
    {

    }

    template<typename _ValType>
    Vec3<_ValType>::Vec3(const Vec2<_ValType>& xy, _ValType z)
        : x(xy.x)
        , y(xy.y)
        , z(z)
    {

    }

    template<typename _ValType>
    Vec3<_ValType>::Vec3(_ValType x, const Vec2<_ValType>& yz)
        : x(x)
        , y(yz.x)
        , z(yz.y)
    {

    }

    template<typename _ValType>
    Vec3<_ValType>::~Vec3()
    {

    }

    template<typename _ValType>
    Vec2<_ValType> Vec3<_ValType>::xy() const
    {
        return Vec2<_ValType>(x, y);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec3<_ValType>::yz() const
    {
        return Vec2<_ValType>(y, z);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec3<_ValType>::xz() const
    {
        return Vec2<_ValType>(x, z);
    }

    template<typename _ValType>
    double Vec3<_ValType>::Length() const
    {
        return std::sqrt((double)Dot(*this, *this));
    }

    template<typename _ValType>
    float Vec3<_ValType>::LengthF() const
    {
        return (float)this->Length();
    }

    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::Inverse() const
    {
        double x = 1.0 / (double)this->x;
        double y = 1.0 / (double)this->y;
        double z = 1.0 / (double)this->z;
        return Vec3((_ValType)x, (_ValType)y, (_ValType)z);
    }

    template<typename _ValType>
    const _ValType& Vec3<_ValType>::operator[](unsigned int index) const
    {
        switch(index)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw Exception("Invalid index for Vec3 component.");
        }
    }

    template<typename _ValType>
    _ValType& Vec3<_ValType>::operator[](unsigned int index)
    {
        switch(index)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw Exception("Invalid index for Vec3 component.");
        }
    }

    template<typename _ValType>
    Vec3<_ValType>& Vec3<_ValType>::operator+=(const Vec3<_ValType>& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    template<typename _ValType>
    Vec3<_ValType>& Vec3<_ValType>::operator-=(const Vec3<_ValType>& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    template<typename _ValType>
    Vec3<_ValType>& Vec3<_ValType>::operator*=(const Vec3<_ValType>& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    template<typename _ValType>
    Vec3<_ValType>& Vec3<_ValType>::operator/=(const Vec3<_ValType>& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }


    template<typename _ValType>
    Vec3<_ValType>& Vec3<_ValType>::operator*=(_ValType scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    template<typename _ValType>
    Vec3<_ValType>& Vec3<_ValType>::operator/=(_ValType scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }


    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator+(const Vec3<_ValType>& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator-(const Vec3<_ValType>& other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator*(const Vec3<_ValType>& other) const
    {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator/(const Vec3<_ValType>& other) const
    {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }


    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    template <>
    Vec3ui Vec3ui::operator-() const
    {
        throw Exception("Unary minus operator not applicable to Vec3ui!");
    }


    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator*(_ValType scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    template<typename _ValType>
    Vec3<_ValType> Vec3<_ValType>::operator/(_ValType scalar) const
    {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }


    template<typename _ValType>
    bool Vec3<_ValType>::operator==(const Vec3<_ValType>& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    template<typename _ValType>
    bool Vec3<_ValType>::operator!=(const Vec3<_ValType>& other) const
    {
        return x != other.x || y != other.y || z != other.z;
    }


    template<typename _ValType>
    bool Vec3<_ValType>::operator<(const Vec3<_ValType>& other) const
    {
        return x < other.x && y < other.y && z < other.z;
    }

    template<typename _ValType>
    bool Vec3<_ValType>::operator<=(const Vec3<_ValType>& other) const
    {
        return x <= other.x && y <= other.y && z <= other.z;
    }

    template<typename _ValType>
    bool Vec3<_ValType>::operator>(const Vec3<_ValType>& other) const
    {
        return x > other.x && y > other.y && z > other.z;
    }

    template<typename _ValType>
    bool Vec3<_ValType>::operator>=(const Vec3<_ValType>& other) const
    {
        return x >= other.x && y >= other.y && z >= other.z;
    }

    template class Vec3<float>;
    template class Vec3<unsigned int>;
    template class Vec3<int>;
    template class Vec3<double>;
}