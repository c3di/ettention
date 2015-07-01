#pragma once
#include "model/image/Image.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class ConvolutionOperatorImplementation
    {
    public:
        ConvolutionOperatorImplementation();
        virtual ~ConvolutionOperatorImplementation();

        virtual void execute() = 0;

        virtual void setInputs(GPUMapped<Image>* kernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting);
        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) = 0;
        virtual void setInput(GPUMapped<Image>* buffer) = 0;
        virtual void setOutput(GPUMapped<Image>* buffer) = 0;

        virtual GPUMapped<Image>* getResult() = 0;
        virtual Image *getResultAsImage();
        virtual GPUMapped<Image>* getOutput();
    };
}

