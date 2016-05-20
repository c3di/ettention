#pragma once

namespace ettention
{
    class Framework;
    class GPUMappedVolume;
    class VolumeConvolutionKernel;
    template<typename _T> class GPUMapped;

    class VolumeConvolutionOperator
    {
    public:
        VolumeConvolutionOperator(Framework* framework, GPUMappedVolume* input, GPUMappedVolume* output, GPUMappedVolume* kernelValues, GPUMappedVolume* mask = nullptr);
        ~VolumeConvolutionOperator();

        void run();

    private:
        GPUMappedVolume* input;
        GPUMappedVolume* output;
        GPUMappedVolume* kernelValues;
        VolumeConvolutionKernel* kernel;
    };
}