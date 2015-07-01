#include "stdafx.h"
#include "ClusteredParticlesShape.h"
#include "EllipsoidShape.h"
#include <framework/RandomAlgorithms.h>
#include <model/volume/Volume.h>

namespace ettention
{
    ClusteredParticlesShape::ClusteredParticlesShape(Volume* volume, unsigned int clusterCount, unsigned int clusterSize, float particleSize, float particleIntensity)
        : ComposedShape(volume, Matrix4x4::identity)
    {
        auto clusterRandom = RandomAlgorithms::generateRandomUnitVectors(clusterCount);
        for(unsigned int cluster = 0; cluster < clusterCount; cluster++)
        {
            Vec3f clusterPosition = clusterRandom[cluster];
            Vec3f dimensions = Vec3f((float)volume->Properties().GetVolumeResolution().x, (float)volume->Properties().GetVolumeResolution().y, (float)volume->Properties().GetVolumeResolution().z);
            clusterPosition = clusterPosition * dimensions;


            auto particleRandom = RandomAlgorithms::generateRandomUnitVectors(clusterSize);
            for(unsigned int i = 0; i < clusterSize; i++)
            {
                Vec3f particlePosition = particleRandom[i];
                Vec3f dimensions = Vec3f((float)volume->Properties().GetVolumeResolution().x, (float)volume->Properties().GetVolumeResolution().y, (float)volume->Properties().GetVolumeResolution().z) / 8.0f;
                particlePosition = particlePosition * dimensions;
                particlePosition += clusterPosition;

                Matrix4x4 transform = Matrix4x4::translationMatrix(particlePosition);
                addShape(new EllipsoidShape(volume, transform, particleSize, particleSize, particleSize, particleIntensity));
            }
        }
    }

    ClusteredParticlesShape::~ClusteredParticlesShape()
    {
    }
}