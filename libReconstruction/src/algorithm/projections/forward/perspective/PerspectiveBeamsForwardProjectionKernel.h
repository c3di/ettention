#pragma once
#include "algorithm/projections/forward/ForwardProjectionKernel.h"

namespace ettention
{
    class PerspectiveBeamsForwardProjectionKernel : public ForwardProjectionKernel
    {
    public:
        PerspectiveBeamsForwardProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, unsigned int samples);
        virtual ~PerspectiveBeamsForwardProjectionKernel();
    };
}