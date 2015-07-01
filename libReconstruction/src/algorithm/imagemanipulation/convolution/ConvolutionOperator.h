#pragma once
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"

namespace ettention
{
    class Framework;

    class ConvolutionOperator : public ConvolutionOperatorImplementation
    {
    public:
        ConvolutionOperator(Framework* framework, GPUMapped<Image>* kernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting = true);
        virtual ~ConvolutionOperator();

        virtual void execute() override;

        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting);
        virtual void setInput(GPUMapped<Image>* buffer);
        virtual void setOutput(GPUMapped<Image>* buffer);

        GPUMapped<Image>* getPaddedFilter();
        GPUMapped<Image>* getPaddedSource();

        virtual GPUMapped<Image>* getResult();

    protected:
        bool shouldUseRealSpace(GPUMapped<Image>* kernel, GPUMapped<Image>* source);

    protected:
        CLAbstractionLayer* abstractionLayer;
        ConvolutionOperatorImplementation* implementation;

    private:
        bool doUseRealSpaceImplementation;
    };
}

