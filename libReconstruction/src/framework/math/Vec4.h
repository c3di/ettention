#pragma once
#include "framework/error/Exception.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    template<typename _ValType>
    class Vec4
    {
    public:
        _ValType x;
        _ValType y;
        _ValType z;
        _ValType w;

        Vec4();
        Vec4(_ValType x, _ValType y, _ValType z, _ValType w);
        Vec4(const Vec2<_ValType>& xy, _ValType z, _ValType w);
        Vec4(_ValType x, const Vec2<_ValType>& yz, _ValType w);
        Vec4(_ValType x, _ValType y, const Vec2<_ValType>& zw);
        Vec4(const Vec2<_ValType>& xy, const Vec2<_ValType>& zw);
        Vec4(const Vec3<_ValType>& xyz, _ValType w);
        Vec4(_ValType x, const Vec3<_ValType>& yzw);
        ~Vec4();

        double Length() const;
        float LengthF() const;
        Vec4 Inverse() const;
        Vec2<_ValType> xy() const;
        Vec2<_ValType> xz() const;
        Vec2<_ValType> xw() const;
        Vec2<_ValType> yz() const;
        Vec2<_ValType> yw() const;
        Vec2<_ValType> zw() const;
        

        const _ValType& operator[](unsigned int index) const;
        _ValType& operator[](unsigned int index);

        Vec4& operator+=(const Vec4& other);
        Vec4& operator-=(const Vec4& other);
        Vec4& operator*=(const Vec4& other);
        Vec4& operator/=(const Vec4& other);

        Vec4& operator*=(_ValType scalar);
        Vec4& operator/=(_ValType scalar);

        Vec4 operator+(const Vec4& other) const;
        Vec4 operator-(const Vec4& other) const;
        Vec4 operator*(const Vec4& other) const;
        Vec4 operator/(const Vec4& other) const;

        Vec4 operator-() const;

        Vec4 operator*(_ValType scalar) const;
        Vec4 operator/(_ValType scalar) const;

        bool operator==(const Vec4& other) const;
        bool operator!=(const Vec4& other) const;

        template<typename _CastValue>
        operator Vec4<_CastValue>() const;

        // should we really have these?
        // this implies that vectors can be ordered
        bool operator<(const Vec4& other) const;
        bool operator<=(const Vec4& other) const;
        bool operator>(const Vec4& other) const;
        bool operator>=(const Vec4& other) const;
    };

    template<typename _ValType>
    _ValType Dot(const Vec4<_ValType>& left, const Vec4<_ValType>& right);

    template<typename _ValType>
    Vec4<_ValType> Normalize(const Vec4<_ValType>& vec)
    {
        double length = vec.Length();
        if(length == 0.0)
        {
            throw Exception("Length of vector is 0!");
        }
        return Vec4<_ValType>((_ValType)((double)vec.x / length),
                              (_ValType)((double)vec.y / length),
                              (_ValType)((double)vec.z / length),
                              (_ValType)((double)vec.w / length));
    }

    template<typename _ValType>
    Vec4<_ValType> Homogenize(const Vec4<_ValType>& vec)
    {
        if(vec.w == (_ValType)0)
        {
            throw Exception("Homogneneous coordinate is 0!");
        }
        return Vec4<_ValType>(vec.x / vec.w,
                              vec.y / vec.w,
                              vec.z / vec.w,
                              (_ValType)1);
    }

    template<typename _ValType>
    Vec4<_ValType> ComponentWiseMin(const Vec4<_ValType>& left, const Vec4<_ValType>& right)
    {
        return Vec4<_ValType>(std::min(left.x, right.x),
                              std::min(left.y, right.y),
                              std::min(left.z, right.z),
                              std::min(left.w, right.w));
    }

    template<typename _ValType>
    Vec4<_ValType> ComponentWiseMax(const Vec4<_ValType>& left, const Vec4<_ValType>& right)
    {
        return Vec4<_ValType>(std::max(left.x, right.x),
                              std::max(left.y, right.y),
                              std::max(left.z, right.z),
                              std::max(left.w, right.w));
    }

    template<typename _ValType>
    Vec4<_ValType> operator*(_ValType left, const Vec4<_ValType>& right)
    {
        return Vec4<_ValType>(left * right.x, left * right.y, left * right.z, left * right.w);
    }

    template<typename _ValType>
    std::ostream& operator<<(std::ostream& stream, const Vec4<_ValType>& vec)
    {
        stream << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
        return stream;
    }

    template<typename _ValType>
    std::istream& operator>>(std::istream& stream, Vec4<_ValType>& vec)
    {
        stream >> vec.x >> vec.y >> vec.z >> vec.w;
        return stream;
    }

    typedef Vec4<float> Vec4f;
    typedef Vec4<unsigned int> Vec4ui;
    typedef Vec4<int> Vec4i;
    typedef Vec4<double> Vec4d;
}