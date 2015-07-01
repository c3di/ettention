#include "stdafx.h"
#include "ParallelBeamsForwardProjectionKernel.h"
#include "ParallelBeamsForwardProjection_bin2c.h"

namespace ettention
{
    ParallelBeamsForwardProjectionKernel::ParallelBeamsForwardProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, unsigned int samples)
        : ForwardProjectionKernel(framework,
                                  ParallelBeamsForwardProjection_kernel_SourceCode,
                                  "execute",
                                  "ParallelBeamsForwardProjectionKernel",
                                  geometricSetup,
                                  volume,
                                  0,
                                  samples)
    {
    }

    ParallelBeamsForwardProjectionKernel::~ParallelBeamsForwardProjectionKernel()
    {
    }
}