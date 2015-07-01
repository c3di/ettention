#pragma once
#include <framework/geometry/BoundingBox.h>
#include <framework/math/Matrix4x4.h>
#include <framework/math/Vec3.h>

namespace ettention
{
    class Volume;

    class PlottableShape
    {
    public:
        PlottableShape(Volume* volume, const Matrix4x4& m, float intensityDelta);
        virtual ~PlottableShape();

        void writeShapeToPoint(Vec3ui position, float intensityScale) const;
        BoundingBox3f getBoundingBox() const;
        Volume* getVolume() const;

        virtual void plotToVolume(unsigned int sampleCount) const;
        virtual bool isPointInsideShape(Vec3f p) const = 0;
        virtual BoundingBox3f getObjectSpaceBoundingBox() const = 0;

    private:
        Matrix4x4 transform;
        Matrix4x4 inverseTransform;
        Volume* volume;
        float intensityDelta;
    };
}