#pragma once
#include "algorithm/projections/forward/factory/ForwardProjectionFactory.h"

namespace ettention
{
    class DefaultForwardProjectionCreator final : public ForwardProjectionFactory::InstanceCreator
    {
    public:
        DefaultForwardProjectionCreator();
        ~DefaultForwardProjectionCreator();

        ForwardProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const override;
    };
}