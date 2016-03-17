#include "stdafx.h"
#include "ParallelBeamsForwardProjectionKernel.h"
#include "ParallelBeamsForwardProjection_bin2c.h"

namespace ettention
{
    ParallelBeamsForwardProjectionKernel::ParallelBeamsForwardProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask, unsigned int samples)
        : ForwardProjectionKernel(framework,
                                  ParallelBeamsForwardProjection_kernel_SourceCode,
                                  "execute",
                                  "ParallelBeamsForwardProjectionKernel",
                                  geometricSetup,
                                  volume,
                                  priorKnowledgeMask,
                                  samples)
    {
    }

    ParallelBeamsForwardProjectionKernel::~ParallelBeamsForwardProjectionKernel()
    {
    }
}