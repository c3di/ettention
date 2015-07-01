#include "stdafx.h"
#include "EllipsoidShape.h"

namespace ettention
{
    EllipsoidShape::EllipsoidShape(Volume* volume, const Matrix4x4& transform, float a, float b, float c, float intensityDelta)
        : PlottableShape(volume, transform, intensityDelta)
        , a(a)
        , b(b)
        , c(c)
    {
    }

    EllipsoidShape::~EllipsoidShape()
    {
    }

    bool EllipsoidShape::isPointInsideShape(Vec3f p) const
    {
        const float dx = (p.x * p.x) / (a * a);
        const float dy = (p.y * p.y) / (b * b);
        const float dz = (p.z * p.z) / (c * c);
        return dx + dy + dz < 1.0f;
    }

    BoundingBox3f EllipsoidShape::getObjectSpaceBoundingBox() const
    {
        return BoundingBox3f(Vec3f(-a, -b, -c), Vec3f(a, b, c));
    }
}