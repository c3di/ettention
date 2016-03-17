#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class SubVolume;
    template<typename _T> class GPUMapped;

    class VolumeConvolutionKernel : public ComputeKernel
    {
    public:
        VolumeConvolutionKernel(Framework* framework, GPUMapped<SubVolume>* input, GPUMapped<SubVolume>* output, GPUMapped<SubVolume>* convolutionKernel);
        ~VolumeConvolutionKernel();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMapped<SubVolume>* input;
        GPUMapped<SubVolume>* output;
        GPUMapped<SubVolume>* convolutionKernel;
    };
}