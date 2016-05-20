#include "stdafx.h"
#include "CrosscorrelationClampingKernel.h"
#include "CrosscorrelationClampingFunction_bin2c.h"

namespace ettention
{
    CrosscorrelationClampingKernel::CrosscorrelationClampingKernel(Framework* framework, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, float candidateMean)
        : CrosscorrelationKernel(framework, CrosscorrelationClampingFunction_kernel_SourceCode, "computeAreaNCC", "CrosscorrelationClampingKernel", reference, candidate, candidateMean)
    {
    }

    CrosscorrelationClampingKernel::~CrosscorrelationClampingKernel()
    {
    }
}