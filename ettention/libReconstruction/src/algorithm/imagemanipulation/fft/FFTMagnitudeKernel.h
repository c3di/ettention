#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class FFTMagnitudeKernel : public ComputeKernel
    {
    public:
        FFTMagnitudeKernel(Framework* framework, GPUMapped<Image>* imageReal, GPUMapped<Image>* imageImaginary);
        ~FFTMagnitudeKernel();

        GPUMapped<Image>* getOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* imageReal;
        GPUMapped<Image>* imageImaginary;
        GPUMapped<Image>* resultMagnitude;
    };
}