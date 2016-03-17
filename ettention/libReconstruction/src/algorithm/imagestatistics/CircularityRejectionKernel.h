#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class CircularityRejectionKernel : public ImageProcessingKernel
    {
    public:
        CircularityRejectionKernel(Framework *framework, GPUMapped<Image> *particlesMap, GPUMapped<Image> *segmentationMap, GPUMapped<Image> *outputcontainer, Vec2ui particleSize);
        CircularityRejectionKernel(Framework *framework, GPUMapped<Image> *particlesMap, GPUMapped<Image> *segmentationMap, Vec2ui particleSize);
        ~CircularityRejectionKernel();

        void setInput(GPUMapped<Image> *particlesMap, GPUMapped<Image> *segmentationMap);
        void setParticleSize(Vec2ui particleSize);
        GPUMapped<Image> *getInputParticlesMap() const;
        GPUMapped<Image> *getInputSegmentationMap() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image> *particlesMap;
        GPUMapped<Image> *segmentationMap;
        Vec2ui projectionResolution;
        Vec2ui particleSize;
    };
}