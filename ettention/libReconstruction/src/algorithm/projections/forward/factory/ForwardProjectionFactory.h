#pragma once

namespace ettention
{
    class ForwardProjectionKernel;
    class Framework;
    class GeometricSetup;
    class GPUMappedVolume;

    class ForwardProjectionFactory final
    {
    public:
        class InstanceCreator
        {
        public:
            explicit InstanceCreator();
            virtual ~InstanceCreator();

            virtual ForwardProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const = 0;
        };

    private:
        std::vector<std::unique_ptr<InstanceCreator>> creators;

    public:
        explicit ForwardProjectionFactory();
        ~ForwardProjectionFactory() {}

        void RegisterCreator(std::unique_ptr<InstanceCreator> creator);
        ForwardProjectionKernel* CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const;
    };
}