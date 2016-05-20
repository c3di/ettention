#pragma once
#include "algorithm/imagemanipulation/convolution/ConvolutionImplementation.h"

namespace ettention
{
    class Framework;
    class ConvolutionRealSpaceKernel;

    class ConvolutionRealSpaceImplementation : public ConvolutionImplementation
    {
    public:
        ConvolutionRealSpaceImplementation(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer);
        ConvolutionRealSpaceImplementation(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source);
        ~ConvolutionRealSpaceImplementation();

        virtual void execute() override;

        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) override;
        virtual void setInput(GPUMapped<Image>* buffer) override;
        virtual void setOutput(GPUMapped<Image>* buffer) override;

        virtual GPUMapped<Image>* getOutput() const override;

        float getWeightsSum() const;

    private:
        ConvolutionRealSpaceKernel *convolutionInRealSpaceCLKernel;

    };
}