#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class CaoDetectorKernel : public ImageProcessingKernel
    {
    public:
        CaoDetectorKernel(Framework* framework, GPUMapped<Image>* minLambdas, GPUMapped<Image>* maxLambdas, GPUMapped<Image>* result);
        CaoDetectorKernel(Framework* framework, GPUMapped<Image>* minLambdas, GPUMapped<Image>* maxLambdas);
        ~CaoDetectorKernel();

        void setInput(GPUMapped<Image>* mins, GPUMapped<Image>* maxs);

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* mins;
        GPUMapped<Image>* maxs;
        Vec2ui projectionResolution;
    };
}