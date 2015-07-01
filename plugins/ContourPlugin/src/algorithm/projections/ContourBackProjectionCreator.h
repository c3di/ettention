#pragma once
#include <algorithm/projections/back/factory/BackProjectionFactory.h>

namespace ettention
{
    namespace contour
    {
        class PLUGIN_API ContourBackProjectionCreator : public BackProjectionFactory::InstanceCreator
        {
        public:
            ContourBackProjectionCreator();
            ~ContourBackProjectionCreator();

            BackProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const override;
        };
    }
}