#pragma once
#include "ComposedShape.h"

namespace ettention
{
    class Diagonal3Shape : public ComposedShape
    {
    public:
        Diagonal3Shape(Volume* volume);
        ~Diagonal3Shape();

    private:
        Volume* volume;
        Vec3f scale;

        void addSphere(float intensity, Vec3f particlePosition, float size);
    };
}