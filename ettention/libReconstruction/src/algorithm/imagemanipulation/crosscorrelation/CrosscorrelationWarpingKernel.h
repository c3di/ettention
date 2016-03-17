#pragma once

#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationKernel.h"

namespace ettention
{
    class CrosscorrelationWarpingKernel : public CrosscorrelationKernel
    {
    public:
        CrosscorrelationWarpingKernel(Framework* framework, GPUMapped<Image>* reference, GPUMapped<Image>* candidate, float candidateMean = 0.0f);
        virtual ~CrosscorrelationWarpingKernel();
    };
}
