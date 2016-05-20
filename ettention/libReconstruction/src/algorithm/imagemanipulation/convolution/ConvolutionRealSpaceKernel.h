#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    template<typename _T> class Vec2;

    class ConvolutionRealSpaceKernel : public ImageProcessingKernel
    {
    public:
        ConvolutionRealSpaceKernel(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer);
        ConvolutionRealSpaceKernel(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source);
        ~ConvolutionRealSpaceKernel();

        void setInputs(GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source);
        void setKernel(GPUMapped<Image>* convolutionKernel);
        void setInput(GPUMapped<Image>* source);

        float getWeightsSum() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* convolutionKernel;
        GPUMapped<Image>* source;
        Vec2ui kernelResolution;
        Vec2ui projectionResolution;
        float weightsSum;

    };
}