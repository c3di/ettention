#include "stdafx.h"
#include "BoundingBox.h"

namespace ettention 
{
    template<typename _T>
    BoundingBox<_T>::BoundingBox(const _T& min, const _T& max) 
        : min(min)
        , max(max) 
    {

    }

    template<typename _T>
    BoundingBox<_T>::~BoundingBox()
    {

    }

    template<typename _T>
    void BoundingBox<_T>::SetMin(const _T& min)
    {
        this->min = min;
    }

    template<typename _T>
    const _T& BoundingBox<_T>::GetMin() const
    {
        return min;
    }

    template<typename _T>
    void BoundingBox<_T>::SetMax(const _T& max)
    {
        this->max = max;
    }

    template<typename _T>
    const _T& BoundingBox<_T>::GetMax() const
    {
        return max;
    }

    template<typename _T>
    void BoundingBox<_T>::Set(const _T& min, const _T& max)
    {
        this->SetMin(min);
        this->SetMax(max);
    }

    template<typename _T>
    _T BoundingBox<_T>::GetDimension() const
    {
        return max - min;
    }

    template <>
    Vec3f BoundingBox<Vec3f>::GetCenter() const
    {
        return 0.5f * (min + max);
    }

    template <>
    Vec2f BoundingBox<Vec2f>::GetCenter() const
    {
        return 0.5f * (min + max);
    }

    template <>
    Vec3d BoundingBox<Vec3d>::GetCenter() const
    {
        return 0.5 * (min + max);
    }

    template <>
    Vec2d BoundingBox<Vec2d>::GetCenter() const
    {
        return 0.5 * (min + max);
    }

    template<typename _T>
    _T BoundingBox<_T>::GetCenter() const
    {
        return (min + max) / 2;
    }

    template<typename _T>
    std::ostream& operator<<(std::ostream& stream, const BoundingBox<_T>& boundingBox)
    {
        stream << "Bounding Box (" << boundingBox.GetMin() << " - " << boundingBox.GetMax() << ")";
        return stream;
    }

    BoundingBox3f Transform(const BoundingBox3f& base, const Matrix4x4& transform)
    {
        Vec3f corners[8] =
        {
            Vec3f(base.GetMin().x, base.GetMin().y, base.GetMin().z),
            Vec3f(base.GetMax().x, base.GetMin().y, base.GetMin().z),
            Vec3f(base.GetMin().x, base.GetMax().y, base.GetMin().z),
            Vec3f(base.GetMax().x, base.GetMax().y, base.GetMin().z),
            Vec3f(base.GetMin().x, base.GetMin().y, base.GetMax().z),
            Vec3f(base.GetMax().x, base.GetMin().y, base.GetMax().z),
            Vec3f(base.GetMin().x, base.GetMax().y, base.GetMax().z),
            Vec3f(base.GetMax().x, base.GetMax().y, base.GetMax().z),
        };
        TransformCoordStream(transform, corners, corners, 8);
        Vec3f transformedMin(std::min(std::min(std::min(corners[0].x, corners[1].x), std::min(corners[2].x, corners[3].x)), std::min(std::min(corners[4].x, corners[5].x), std::min(corners[6].x, corners[7].x))),
                             std::min(std::min(std::min(corners[0].y, corners[1].y), std::min(corners[2].y, corners[3].y)), std::min(std::min(corners[4].y, corners[5].y), std::min(corners[6].y, corners[7].y))),
                             std::min(std::min(std::min(corners[0].z, corners[1].z), std::min(corners[2].z, corners[3].z)), std::min(std::min(corners[4].z, corners[5].z), std::min(corners[6].z, corners[7].z))));
        Vec3f transformedMax(std::max(std::max(std::max(corners[0].x, corners[1].x), std::max(corners[2].x, corners[3].x)), std::max(std::max(corners[4].x, corners[5].x), std::max(corners[6].x, corners[7].x))),
                             std::max(std::max(std::max(corners[0].y, corners[1].y), std::max(corners[2].y, corners[3].y)), std::max(std::max(corners[4].y, corners[5].y), std::max(corners[6].y, corners[7].y))),
                             std::max(std::max(std::max(corners[0].z, corners[1].z), std::max(corners[2].z, corners[3].z)), std::max(std::max(corners[4].z, corners[5].z), std::max(corners[6].z, corners[7].z))));
        return BoundingBox3f(transformedMin, transformedMax);
    }

    template class BoundingBox<Vec3f>;
    template class BoundingBox<Vec3ui>;
    template class BoundingBox<Vec3i>;
    template class BoundingBox<Vec3d>;
    template class BoundingBox<Vec2f>;
    template class BoundingBox<Vec2ui>;
    template class BoundingBox<Vec2i>;
    template class BoundingBox<Vec2d>;
}