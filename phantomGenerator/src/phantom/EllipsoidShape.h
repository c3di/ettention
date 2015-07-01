#pragma once
#include "PlottableShape.h"

namespace ettention
{
    class EllipsoidShape : public PlottableShape
    {
    public:
        EllipsoidShape(Volume* volume, const Matrix4x4& transform, float a, float b, float c, float intensityDelta);
        ~EllipsoidShape();

        bool isPointInsideShape(Vec3f p) const override;
        BoundingBox3f getObjectSpaceBoundingBox() const override;

    private:
        float a;
        float b;
        float c;
    };
}