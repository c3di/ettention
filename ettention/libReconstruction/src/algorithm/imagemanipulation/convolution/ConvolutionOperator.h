#pragma once
#include "algorithm/imagemanipulation/convolution/ConvolutionImplementation.h"

namespace ettention
{
    class Framework;
    class Image;
    template<typename _T> class GPUMapped;

    class ConvolutionOperator : public ConvolutionImplementation
    {
    public:
        ConvolutionOperator(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer, bool kernelRequiresOriginShifting = true);
        ConvolutionOperator(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting = true);
        virtual ~ConvolutionOperator();

        virtual void execute() override;

        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) override;
        virtual void setInput(GPUMapped<Image>* buffer) override;
        virtual void setOutput(GPUMapped<Image>* buffer) override;

        virtual GPUMapped<Image>* getOutput() const override;

    protected:
        bool shouldUseRealSpace(GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source);

    protected:
        ConvolutionImplementation* implementation;

    private:
        bool doUseRealSpaceImplementation;
    };
}

