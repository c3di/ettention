#pragma once
#include <algorithm/projections/forward/factory/ForwardProjectionFactory.h>

namespace ettention
{
    namespace contour
    {
        class PLUGIN_API ContourForwardProjectionCreator : public ForwardProjectionFactory::InstanceCreator
        {
        public:
            ContourForwardProjectionCreator();
            ~ContourForwardProjectionCreator();

            ForwardProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const override;
        };
    }
}