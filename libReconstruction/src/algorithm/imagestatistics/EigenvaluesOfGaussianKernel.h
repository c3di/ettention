#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class EigenvaluesOfGaussianKernel : public ComputeKernel
    {
    public:
        EigenvaluesOfGaussianKernel(Framework* framework, GPUMapped<Image>* sourceA, GPUMapped<Image>* sourceB, GPUMapped<Image>* sourceC);
        ~EigenvaluesOfGaussianKernel();

        void setInput(GPUMapped<Image>* sourceA, GPUMapped<Image>* sourceB, GPUMapped<Image>* sourceC);
        GPUMapped<Image>* getResultMins() const;
        GPUMapped<Image>* getResultMaxs() const;

    protected:
        void prepareKernelArguments();
        void preRun();
        void postRun();

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* sourceA;
        GPUMapped<Image>* sourceB;
        GPUMapped<Image>* sourceC;
        GPUMapped<Image>* resultMins;
        GPUMapped<Image>* resultMaxs;
    };
}