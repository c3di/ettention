#include "stdafx.h"
#include "PerspectiveBeamsForwardProjectionKernel.h"
#include "PerspectiveBeamsForwardProjection_bin2c.h"

namespace ettention
{
    PerspectiveBeamsForwardProjectionKernel::PerspectiveBeamsForwardProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask, unsigned int samples)
        : ForwardProjectionKernel(framework,
                                  PerspectiveBeamsForwardProjection_kernel_SourceCode,
                                  "execute",
                                  "PerspectiveBeamsForwardProjectionKernel",
                                  geometricSetup,
                                  volume,
                                  priorKnowledgeMask,
                                  samples)
    {        
    }

    PerspectiveBeamsForwardProjectionKernel::~PerspectiveBeamsForwardProjectionKernel()
    {
    }
}