#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class ComputeKernelImplementation;
    class GeometricSetup;
    class GPUMappedVolume;
    class Image;
    class OptimizationParameterSet;
    template<typename _T> class GPUMapped;

    class ForwardProjectionKernel : public ComputeKernel
    {
    public:
        ForwardProjectionKernel(Framework* framework,
                                const std::string& sourceCode,
                                const std::string& kernelName,
                                const std::string& kernelCaption,
                                const GeometricSetup* geometricSetup,
                                GPUMappedVolume* volume,
                                GPUMappedVolume* priorKnowledgeMask,
                                unsigned int samples);
        ~ForwardProjectionKernel();

        GPUMappedVolume* getVolume();
        void SetOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength);
        const GeometricSetup* GetGeometricSetup() const;
        GPUMapped<Image>* GetVirtualProjection() const;
        GPUMapped<Image>* GetTraversalLength() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        const GeometricSetup* geometricSetup;
        GPUMapped<Image>* virtualProjection;
        GPUMapped<Image>* traversalLength;
        GPUMappedVolume* volume;
        GPUMappedVolume* priorKnowledgeMask;
        Vec2ui projectionResolution;
        bool useVolumeImage;
        unsigned int samples;
    };
}
