#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class CaoDetectorKernel : public ComputeKernel
    {
    public:
        CaoDetectorKernel(Framework* framework, GPUMapped<Image>* minLambdas, GPUMapped<Image>* maxLambdas);
        ~CaoDetectorKernel();

        void setInput(GPUMapped<Image>* mins, GPUMapped<Image>* maxs);
        GPUMapped<Image>* getResult();
        Image* getResultAsImage();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* mins;
        GPUMapped<Image>* maxs;
        GPUMapped<Image>* result;
        Vec2ui projectionResolution;
    };
}