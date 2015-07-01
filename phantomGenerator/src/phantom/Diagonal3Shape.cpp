#include "stdafx.h"
#include "Diagonal3Shape.h"
#include "EllipsoidShape.h"
#include <model/volume/Volume.h>

namespace ettention
{
    Diagonal3Shape::Diagonal3Shape(Volume* volume)
        : ComposedShape(volume, Matrix4x4::identity), volume(volume)
    {
        for(int x = 1; x <= 3; x++)
        {
            for(int y = 1; y <= 3; y++)
            {
                for(int z = 1; z <= 3; z++)
                {
                    if(x != y || y != z)
                    {
                        addSphere(16.0f, Vec3f(((float)x) * 0.25f, ((float)y) * 0.25f, ((float)z) * 0.25f), 0.02f);
                    }
                }
            }
        }
        addSphere(16.0f, Vec3f(0.25f, 0.25f, 0.25f), 0.06f);
        addSphere(16.0f, Vec3f(0.50f, 0.50f, 0.50f), 0.06f);
        addSphere(16.0f, Vec3f(0.75f, 0.75f, 0.75f), 0.06f);
    }

    Diagonal3Shape::~Diagonal3Shape()
    {
    }

    void Diagonal3Shape::addSphere(float intensity, Vec3f particlePosition, float size)
    {
        Vec3f scale = Vec3f((float)volume->Properties().GetVolumeResolution().x, (float)volume->Properties().GetVolumeResolution().y, (float)volume->Properties().GetVolumeResolution().z);
        particlePosition = particlePosition * scale;
        size *= scale.x;
        Matrix4x4 transform = Matrix4x4::translationMatrix(particlePosition);
        addShape(new EllipsoidShape(volume, transform, size, size, size, intensity));
    }
}