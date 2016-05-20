#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class ComplexMultiplyKernel : public ComputeKernel
    {
    public:
        ComplexMultiplyKernel(Framework* framework, GPUMapped<Image>* imageAReal, GPUMapped<Image>* imageAImaginary, GPUMapped<Image>* imageBReal, GPUMapped<Image>* imageBImaginary);
        ~ComplexMultiplyKernel();

        GPUMapped<Image>* getRealOutput() const;
        GPUMapped<Image>* getImaginaryOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* imageAReal;
        GPUMapped<Image>* imageBReal;
        GPUMapped<Image>* imageAImaginary;
        GPUMapped<Image>* imageBImaginary;
        GPUMapped<Image>* resultReal;
        GPUMapped<Image>* resultImaginary;
    };
}