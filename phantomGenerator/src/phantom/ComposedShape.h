#pragma once
#include "PlottableShape.h"

namespace ettention
{
    class ComposedShape : public PlottableShape
    {
    public:
        ComposedShape(Volume* volume, const Matrix4x4& m);
        virtual ~ComposedShape();

        void plotToVolume(unsigned int sampleCount) const override;
        bool isPointInsideShape(Vec3f p) const override;
        BoundingBox3f getObjectSpaceBoundingBox() const override;

    protected:
        void addShape(PlottableShape* shape);

    private:
        std::vector<PlottableShape*> shapes;
    };
}