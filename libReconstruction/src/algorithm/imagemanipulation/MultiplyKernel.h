#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class MultiplyKernel : public ComputeKernel
    {
    public:
        MultiplyKernel(Framework* framework, GPUMapped<Image>* imageA, GPUMapped<Image>* imageB);
        ~MultiplyKernel();
        
        GPUMapped<Image>* getOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMapped<Image>* imageA;
        GPUMapped<Image>* imageB;
        GPUMapped<Image>* result;
    };
}