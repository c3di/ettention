#pragma once

namespace ettention
{
    class Framework;
    class GPUMappedVolume;
    class SubVolume;
    class VolumeConvolutionKernel;
    template<typename _T> class GPUMapped;

    class VolumeConvolutionOperator
    {
    public:
        VolumeConvolutionOperator(Framework* framework, GPUMappedVolume* input, GPUMappedVolume* output, GPUMapped<SubVolume>* kernelValues);
        ~VolumeConvolutionOperator();

        void run();

    private:
        GPUMappedVolume* input;
        GPUMappedVolume* output;
        GPUMapped<SubVolume>* kernelValues;
        VolumeConvolutionKernel* kernel;
    };
}