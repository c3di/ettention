#pragma once
#include "ComposedShape.h"

namespace ettention
{
    class SheppLoganShape : public ComposedShape
    {
    public:
        SheppLoganShape(Volume* volume, bool generateMaskOnly);
        ~SheppLoganShape();

    private:
        Vec3f scale;

        void addEllipsoid(float intensity, float a, float b, float c, float x0, float y0, float z0, float phiInDegree, float thetaInDegree, float psiInDegree);
    };
}