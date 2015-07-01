#pragma once
#include "algorithm/projections/forward/ForwardProjectionKernel.h"

namespace ettention
{
    namespace contour
    {
        class PLUGIN_API ContourForwardProjectionKernel : public ForwardProjectionKernel
        {
        public:
            ContourForwardProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask);
            ~ContourForwardProjectionKernel();
        };
    }
}