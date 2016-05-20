#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class SubVolume;
    class GPUMappedVolume;
    template<typename _T> class GPUMapped;

    class VolumeConvolutionKernel : public ComputeKernel
    {
    public:
        VolumeConvolutionKernel(Framework* framework, GPUMappedVolume* input, GPUMappedVolume* output, GPUMappedVolume* convolutionKernel, GPUMappedVolume* mask = nullptr);
        ~VolumeConvolutionKernel();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMappedVolume* input;
        GPUMappedVolume* output;
        GPUMappedVolume* convolutionKernel;
        GPUMappedVolume* mask;

        GPUMappedVolume* replacementMaskGPUVolume;
    };
}