#include "stdafx.h"
#include "LocalMaximumSimpleKernel.h"
#include "LocalMaximumSimple_bin2c.h"

namespace ettention
{
    LocalMaximumSimpleKernel::LocalMaximumSimpleKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaResolution)
        : LocalMaximumInterface(framework, source, localAreaResolution, LocalMaximumSimple_kernel_SourceCode, "computeLocalMax", "LocalMaximumSimpleKernel")
    {
    }

    LocalMaximumSimpleKernel::~LocalMaximumSimpleKernel()
    {
    }
}