#pragma once
#include "algorithm/projections/back/BackProjectionKernel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"

namespace ettention
{
    class BackProjectionFactory final
    {
    public:
        class InstanceCreator
        {
        public:
            explicit InstanceCreator();
            virtual ~InstanceCreator();

            virtual BackProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const = 0;
        };

    private:
        std::vector<std::unique_ptr<InstanceCreator>> creators;

    public:
        explicit BackProjectionFactory();
        ~BackProjectionFactory();

        void RegisterCreator(std::unique_ptr<InstanceCreator> creator);
        BackProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const;
    };
}