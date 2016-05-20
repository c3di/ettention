#pragma once
#include "algorithm/imagestatistics/localmaximum/LocalMaximumInterface.h"

namespace ettention
{
    class LocalMaximumSpiralKernel : public LocalMaximumInterface
    {
    public:
        LocalMaximumSpiralKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaResolution);
        ~LocalMaximumSpiralKernel();
    };
}