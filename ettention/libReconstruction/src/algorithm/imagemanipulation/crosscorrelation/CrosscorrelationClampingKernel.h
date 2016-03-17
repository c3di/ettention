#pragma once
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationKernel.h"

namespace ettention
{
    class CrosscorrelationClampingKernel : public CrosscorrelationKernel
    {
    public:
        CrosscorrelationClampingKernel(Framework* framework, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, float candidateMean = 0.0f);
        ~CrosscorrelationClampingKernel();
    };
}