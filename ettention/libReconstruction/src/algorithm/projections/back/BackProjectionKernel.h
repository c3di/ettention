#pragma once

#include "gpu/ComputeKernel.h"
#include "model/volume/VolumeProperties.h"
#include "VoxelSample.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/volume/ByteVolume.h"

namespace ettention
{
    class GeometricSetup;

    class BackProjectionKernel : public ComputeKernel
    {
    public:
        BackProjectionKernel(Framework* framework,
                             const std::string& sourceCode,
                             const std::string& kernelName,
                             const std::string& kernelCaption,
                             const GeometricSetup* geometricSetup,
                             GPUMappedVolume* volume,
                             GPUMappedVolume* priorKnowledgeMask,
                             float lambda,
                             unsigned int samples,
                             bool useLongObjectCompensation,
                             bool useProjectionsAsImages,
                             const std::string& additionalDefines = "");
        virtual ~BackProjectionKernel();

        void SetInput(GPUMapped<Image>* residual, GPUMapped<Image>* traversalLength);
        void setSubVolumeIndex(unsigned int newIndex);
        GPUMappedVolume* getOutput();
        virtual void setSamples(unsigned int samples);
        unsigned int getMinimumRequiredOversampling(const Vec2ui& projectionResolution, const Vec3ui& volumeResolution) const;
        void SetProjectionBlockSize(unsigned int projectionBlockSize);

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

        void allocateProjectionImages(const Vec2ui& projectionResolution);

        const GeometricSetup* geometricSetup;
        GPUMapped<Image>* residual;
        GPUMapped<Image>* rayLength;
        CLMemoryStructure* residualImage;
        CLMemoryStructure* rayLengthImage;
        GPUMappedVolume* volume;
        GPUMappedVolume* priorKnowledgeMask;
        unsigned int subVolumeIndex;
        float lambda;
        unsigned int projectionBlockSize;
        unsigned int samples;
        bool useProjectionsAsImages;

    private:
        ByteVolume* replacementMaskVolume;
        GPUMappedVolume* replacementMaskGPUVolume;
    };
}
