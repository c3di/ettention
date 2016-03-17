#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class FFTPhaseKernel : public ComputeKernel
    {
    public:
        FFTPhaseKernel(Framework* framework, GPUMapped<Image>* imageReal, GPUMapped<Image>* imageImaginary);
        ~FFTPhaseKernel();

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
        GPUMapped<Image>* resultPhase;
    };
}