#pragma once
#include "framework/error/Exception.h"

namespace ettention
{
    template<typename _ValType>
    class Vec2
    {
    public:
        _ValType x;
        _ValType y;

        Vec2();
        Vec2(_ValType x, _ValType y);
        ~Vec2();

        double getLength() const;
        Vec2 maleInverse() const;
        Vec2 xy() const;
        
        const _ValType& operator[](unsigned int index) const;
        _ValType& operator[](unsigned int index);

        Vec2& operator+=(const Vec2& other);
        Vec2& operator-=(const Vec2& other);
        Vec2& operator*=(const Vec2& other);
        Vec2& operator/=(const Vec2& other);

        Vec2& operator*=(_ValType scalar);
        Vec2& operator/=(_ValType scalar);

        Vec2 operator+(const Vec2& other) const;
        Vec2 operator-(const Vec2& other) const;
        Vec2 operator*(const Vec2& other) const;
        Vec2 operator/(const Vec2& other) const;

        Vec2 operator*(_ValType scalar) const;
        Vec2 operator/(_ValType scalar) const;

        bool operator==(const Vec2& other) const;
        bool operator!=(const Vec2& other) const;

        template<typename _CastValue>
        operator Vec2<_CastValue>() const
        {
            return Vec2<_CastValue>((_CastValue)x, (_CastValue)y);
        }
    };

    template<typename _ValType>
    _ValType dotProduct(const Vec2<_ValType>& left, const Vec2<_ValType>& right);

    template<typename _ValType>
    Vec2<_ValType> doNormalize(const Vec2<_ValType>& vec)
    {
        double length = vec.getLength();
        if(length == 0.0)
        {
            throw Exception("Length of vector is 0!");
        }
        return Vec2<_ValType>((_ValType)((double)vec.x / length),
                              (_ValType)((double)vec.y / length));
    }

	template<typename _ValType>
	Vec2<_ValType> componentWiseMax(const Vec2<_ValType>& one, const Vec2<_ValType>& other)
	{
		return Vec2<_ValType>(std::max(one.x, other.x), std::max(one.y, other.y));
	}

	template<typename _ValType>
	Vec2<_ValType> componentWiseMin(const Vec2<_ValType>& one, const Vec2<_ValType>& other)
	{
		return Vec2<_ValType>(std::min(one.x, other.x), std::min(one.y, other.y));
	}

	template<typename _ValType>
	Vec2<_ValType> clamp(const Vec2<_ValType>& v, const Vec2<_ValType>& min, const Vec2<_ValType>& max)
	{
		return Vec2<_ValType>(std::max(min.x, std::min(v.x, max.x)),
			std::max(min.y, std::min(v.y, max.y)));
	}

    template<typename _ValType>
    Vec2<_ValType> operator*(_ValType left, const Vec2<_ValType>& right)
    {
        return Vec2<_ValType>(left * right.x, left * right.y);
    }

    template<typename _ValType>
    std::ostream& operator<<(std::ostream& stream, const Vec2<_ValType>& vec)
    {
        stream << vec.x << " " << vec.y;
        return stream;
    }

    template<typename _ValType>
    std::istream& operator>>(std::istream& stream, Vec2<_ValType>& vec)
    {
        stream >> vec.x >> vec.y;
        return stream;
    }

    typedef Vec2<float> Vec2f;
    typedef Vec2<unsigned int> Vec2ui;
    typedef Vec2<int> Vec2i;
    typedef Vec2<double> Vec2d;
}