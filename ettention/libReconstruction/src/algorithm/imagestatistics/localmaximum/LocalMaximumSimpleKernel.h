#pragma once
#include "algorithm/imagestatistics/localmaximum/LocalMaximumInterface.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class LocalMaximumSimpleKernel : public LocalMaximumInterface
    {
    public:
        LocalMaximumSimpleKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaResolution);
        virtual ~LocalMaximumSimpleKernel();
    };
}