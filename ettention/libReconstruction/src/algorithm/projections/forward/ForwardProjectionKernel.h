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
                                unsigned int samples,
                                const std::string& additionalDefines = "");
        ~ForwardProjectionKernel();

        GPUMappedVolume* getVolume();
        void setOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength);
		const GeometricSetup* getGeometricSetup() const;
		GPUMapped<Image>* getVirtualProjection() const;
        GPUMapped<Image>* getTraversalLength() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    protected:
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
