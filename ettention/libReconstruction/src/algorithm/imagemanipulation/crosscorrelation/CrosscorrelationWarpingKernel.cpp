#include "stdafx.h"
#include "CrosscorrelationWarpingKernel.h"
#include "CrosscorrelationWarpingFunction_bin2c.h"

namespace ettention
{
    CrosscorrelationWarpingKernel::CrosscorrelationWarpingKernel(Framework* framework, GPUMapped<Image>* reference, GPUMapped<Image>* candidate, float candidateMean)
        : CrosscorrelationKernel(framework, CrosscorrelationWarpingFunction_kernel_SourceCode, "computeCycledNCC", "CrosscorrelationWarpingKernel", reference, candidate, candidateMean)
    {
    }

    CrosscorrelationWarpingKernel::~CrosscorrelationWarpingKernel()
    {
    }
}