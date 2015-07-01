#pragma once
#include "framework/math/Matrix4x4.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    template<typename _T>
    class BoundingBox
    {
    public:
        BoundingBox(const _T& min, const _T& max);
        ~BoundingBox();

        void SetMin(const _T& min);
        const _T& GetMin() const;
        void SetMax(const _T& max);
        const _T& GetMax() const;
        void Set(const _T& min, const _T& max);
        _T GetDimension() const;
        _T GetCenter() const;

    private:
        _T min;
        _T max;
    };

    template<typename _T>
    std::ostream& operator<<(std::ostream& stream, const BoundingBox<_T>& boundingBox);

    typedef BoundingBox<Vec3f> BoundingBox3f;

    BoundingBox3f Transform(const BoundingBox3f& base, const Matrix4x4& transform);
}

