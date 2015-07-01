#include "stdafx.h"
#include "LocalMaximumSpiralKernel.h"
#include "LocalMaximumSpiral_bin2c.h"

namespace ettention
{
    LocalMaximumSpiralKernel::LocalMaximumSpiralKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaResolution)
        : LocalMaximumInterface(framework, source, Vec2ui(localAreaResolution.x, localAreaResolution.x), LocalMaximumSpiral_kernel_SourceCode, "computeLocalMax", "LocalMaximumSpiralKernel")
    {
    }

    LocalMaximumSpiralKernel::~LocalMaximumSpiralKernel()
    {
    }
}