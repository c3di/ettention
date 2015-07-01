#pragma once
#include "framework/error/Exception.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    template<typename _ValType>
    class Vec3
    {
    public:
        _ValType x;
        _ValType y;
        _ValType z;

        Vec3();
        Vec3(_ValType x, _ValType y, _ValType z);
        Vec3(const Vec2<_ValType>& xy, _ValType z);
        Vec3(_ValType x, const Vec2<_ValType>& yz);
        ~Vec3();

        double Length() const;
        float LengthF() const;
        Vec3 Inverse() const;
        Vec2<_ValType> xy() const;
        Vec2<_ValType> yz() const;
        Vec2<_ValType> xz() const;

        const _ValType& operator[](unsigned int index) const;
        _ValType& operator[](unsigned int index);

        Vec3& operator+=(const Vec3& other);
        Vec3& operator-=(const Vec3& other);
        Vec3& operator*=(const Vec3& other);
        Vec3& operator/=(const Vec3& other);

        Vec3& operator*=(_ValType scalar);
        Vec3& operator/=(_ValType scalar);

        Vec3 operator+(const Vec3& other) const;
        Vec3 operator-(const Vec3& other) const;
        Vec3 operator*(const Vec3& other) const;
        Vec3 operator/(const Vec3& other) const;

        Vec3 operator-() const;

        Vec3 operator*(_ValType scalar) const;
        Vec3 operator/(_ValType scalar) const;

        bool operator==(const Vec3& other) const;
        bool operator!=(const Vec3& other) const;

        template<typename _CastValue>
        operator Vec3<_CastValue>() const
        {
            return Vec3<_CastValue>((_CastValue)x, (_CastValue)y, (_CastValue)z);
        }

        // should we really have these?
        // this implies that vectors can be ordered
        bool operator<(const Vec3& other) const;
        bool operator<=(const Vec3& other) const;
        bool operator>(const Vec3& other) const;
        bool operator>=(const Vec3& other) const;
    };

    template<typename _ValType>
    _ValType Dot(const Vec3<_ValType>& left, const Vec3<_ValType>& right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z;
    }

    template<typename _ValType>
    Vec3<_ValType> Cross(const Vec3<_ValType>& left, const Vec3<_ValType>& right)
    {
        return Vec3<_ValType>(left.y * right.z - left.z * right.y,
                              left.z * right.x - left.x * right.z,
                              left.x * right.y - left.y * right.x);
    }

    template<typename _ValType>
    Vec3<_ValType> Normalize(const Vec3<_ValType>& vec)
    {
        double length = vec.Length();
        if(length == 0.0)
        {
            throw Exception("Length of vector is 0!");
        }
        return Vec3<_ValType>((_ValType)((double)vec.x / length),
                              (_ValType)((double)vec.y / length),
                              (_ValType)((double)vec.z / length));
    }

    template<typename _ValType>
    Vec3<_ValType> ComponentWiseMin(const Vec3<_ValType>& left, const Vec3<_ValType>& right)
    {
        return Vec3<_ValType>(std::min(left.x, right.x),
                              std::min(left.y, right.y),
                              std::min(left.z, right.z));
    }

    template<typename _ValType>
    Vec3<_ValType> ComponentWiseMax(const Vec3<_ValType>& left, const Vec3<_ValType>& right)
    {
        return Vec3<_ValType>(std::max(left.x, right.x),
                              std::max(left.y, right.y),
                              std::max(left.z, right.z));
    }

    template<typename _ValType>
    Vec3<_ValType> operator*(_ValType left, const Vec3<_ValType>& right)
    {
        return Vec3<_ValType>(left * right.x, left * right.y, left * right.z);
    }

    template<typename _ValType>
    std::ostream& operator<<(std::ostream& stream, const Vec3<_ValType>& vec)
    {
        stream << vec.x << " " << vec.y << " " << vec.z;
        return stream;
    }

    template<typename _ValType>
    std::istream& operator>>(std::istream& stream, Vec3<_ValType>& vec)
    {
        stream >> vec.x >> vec.y >> vec.z;
        return stream;
    }

    typedef Vec3<float> Vec3f;
    typedef Vec3<unsigned int> Vec3ui;
    typedef Vec3<int> Vec3i;
    typedef Vec3<double> Vec3d;
}