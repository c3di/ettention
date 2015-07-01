#include "stdafx.h"
#include "Vec4.h"

namespace ettention
{
    template<typename _ValType>
    Vec4<_ValType>::Vec4()
        : x((_ValType)0)
        , y((_ValType)0)
        , z((_ValType)0)
        , w((_ValType)0)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(_ValType x, _ValType y, _ValType z, _ValType w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(const Vec2<_ValType>& xy, _ValType z, _ValType w)
        : x(xy.x)
        , y(xy.y)
        , z(z)
        , w(w)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(_ValType x, const Vec2<_ValType>& yz, _ValType w)
        : x(x)
        , y(yz.x)
        , z(yz.y)
        , w(w)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(_ValType x, _ValType y, const Vec2<_ValType>& zw)
        : x(x)
        , y(y)
        , z(zw.x)
        , w(zw.y)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(const Vec2<_ValType>& xy, const Vec2<_ValType>& zw)
        : x(xy.x)
        , y(xy.y)
        , z(zw.x)
        , w(zw.y)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(const Vec3<_ValType>& xyz, _ValType w)
        : x(xyz.x)
        , y(xyz.y)
        , z(xyz.z)
        , w(w)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::Vec4(_ValType x, const Vec3<_ValType>& yzw)
        : x(x)
        , y(yzw.x)
        , z(yzw.y)
        , w(yzw.z)
    {

    }

    template<typename _ValType>
    Vec4<_ValType>::~Vec4()
    {

    }

    template<typename _ValType>
    Vec2<_ValType> Vec4<_ValType>::xy() const
    {
        return Vec2<_ValType>(x, y);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec4<_ValType>::xz() const
    {
        return Vec2<_ValType>(x, z);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec4<_ValType>::xw() const
    {
        return Vec2<_ValType>(x, w);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec4<_ValType>::yz() const
    {
        return Vec2<_ValType>(y, z);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec4<_ValType>::yw() const
    {
        return Vec2<_ValType>(y, w);
    }

    template<typename _ValType>
    Vec2<_ValType> Vec4<_ValType>::zw() const
    {
        return Vec2<_ValType>(z, w);
    }

    template<typename _ValType>
    double Vec4<_ValType>::Length() const
    {
        return std::sqrt((double)Dot(*this, *this));
    }

    template<typename _ValType>
    float Vec4<_ValType>::LengthF() const
    {
        return (float)this->Length();
    }

    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::Inverse() const
    {
        double x = 1.0 / (double)this->x;
        double y = 1.0 / (double)this->y;
        double z = 1.0 / (double)this->z;
        double w = 1.0 / (double)this->w;
        return Vec4((_ValType)x, (_ValType)y, (_ValType)z, (_ValType)w);
    }

    template<typename _ValType>
    const _ValType& Vec4<_ValType>::operator[](unsigned int index) const
    {
        switch(index)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        default: throw Exception("Invalid index for Vec4 component.");
        }
    }

    template<typename _ValType>
    _ValType& Vec4<_ValType>::operator[](unsigned int index)
    {
        switch(index)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        default: throw Exception("Invalid index for Vec4 component.");
        }
    }

    template<typename _ValType>
    Vec4<_ValType>& Vec4<_ValType>::operator+=(const Vec4<_ValType>& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    template<typename _ValType>
    Vec4<_ValType>& Vec4<_ValType>::operator-=(const Vec4<_ValType>& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    template<typename _ValType>
    Vec4<_ValType>& Vec4<_ValType>::operator*=(const Vec4<_ValType>& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    template<typename _ValType>
    Vec4<_ValType>& Vec4<_ValType>::operator/=(const Vec4<_ValType>& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }


    template<typename _ValType>
    Vec4<_ValType>& Vec4<_ValType>::operator*=(_ValType scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    template<typename _ValType>
    Vec4<_ValType>& Vec4<_ValType>::operator/=(_ValType scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }


    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator+(const Vec4<_ValType>& other) const
    {
        return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator-(const Vec4<_ValType>& other) const
    {
        return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator*(const Vec4<_ValType>& other) const
    {
        return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator/(const Vec4<_ValType>& other) const
    {
        return Vec4(x / other.x, y / other.y, z / other.z, w / other.w);
    }


    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator-() const
    {
        return Vec4(-x, -y, -z, -w);
    }

    template <>
    Vec4ui Vec4ui::operator-() const
    {
        throw Exception("Unary minus operator not applicable to Vec4ui!");
    }


    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator*(_ValType scalar) const
    {
        return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    template<typename _ValType>
    Vec4<_ValType> Vec4<_ValType>::operator/(_ValType scalar) const
    {
        return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
    }


    template<typename _ValType>
    bool Vec4<_ValType>::operator==(const Vec4<_ValType>& other) const
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    template<typename _ValType>
    bool Vec4<_ValType>::operator!=(const Vec4<_ValType>& other) const
    {
        return x != other.x || y != other.y || z != other.z || w != other.w;
    }


    template<typename _ValType>
    template<typename _CastValue>
    Vec4<_ValType>::operator Vec4<_CastValue>() const
    {
        return Vec4<_CastValue>((_CastValue)x, (_CastValue)y, (_CastValue)z, (_CastValue)w);
    }


    template<typename _ValType>
    bool Vec4<_ValType>::operator<(const Vec4<_ValType>& other) const
    {
        return x < other.x && y < other.y && z < other.z && w < other.w;
    }

    template<typename _ValType>
    bool Vec4<_ValType>::operator<=(const Vec4<_ValType>& other) const
    {
        return x <= other.x && y <= other.y && z <= other.z && w <= other.w;
    }

    template<typename _ValType>
    bool Vec4<_ValType>::operator>(const Vec4<_ValType>& other) const
    {
        return x > other.x && y > other.y && z > other.z && w > other.w;
    }

    template<typename _ValType>
    bool Vec4<_ValType>::operator>=(const Vec4<_ValType>& other) const
    {
        return x >= other.x && y >= other.y && z >= other.z && w >= other.w;
    }


    template<typename _ValType>
    _ValType Dot(const Vec4<_ValType>& left, const Vec4<_ValType>& right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
    }

    template class Vec4<float>;
    template class Vec4<unsigned int>;
    template class Vec4<int>;
    template class Vec4<double>;
}