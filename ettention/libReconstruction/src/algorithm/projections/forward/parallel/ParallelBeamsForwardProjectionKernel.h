#pragma once
#include "algorithm/projections/forward/ForwardProjectionKernel.h"

namespace ettention
{
    class ParallelBeamsForwardProjectionKernel : public ForwardProjectionKernel
    {
    public:
        ParallelBeamsForwardProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask, unsigned int samples);
        virtual ~ParallelBeamsForwardProjectionKernel();
    };
}