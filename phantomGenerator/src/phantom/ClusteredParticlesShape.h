#pragma once
#include "ComposedShape.h"

namespace ettention
{
    class ClusteredParticlesShape : public ComposedShape
    {
    public:
        ClusteredParticlesShape(Volume* volume, unsigned int clusterCount, unsigned int clusterSize, float particleSize, float particleIntensity);
        ~ClusteredParticlesShape();
    };
}