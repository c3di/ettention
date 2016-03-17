#pragma once
#include "algorithm/projections/back/factory/BackProjectionFactory.h"

namespace ettention
{
    class DefaultBackProjectionCreator final : public BackProjectionFactory::InstanceCreator
    {
    public:
        explicit DefaultBackProjectionCreator();
        ~DefaultBackProjectionCreator();

        BackProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const override;
    };
}