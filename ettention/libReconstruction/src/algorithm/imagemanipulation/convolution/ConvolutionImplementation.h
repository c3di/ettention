#pragma once

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class ConvolutionImplementation
    {
    public:
        ConvolutionImplementation();
        virtual ~ConvolutionImplementation();

        virtual void setInputs(GPUMapped<Image>* kernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting);

        virtual void execute() = 0;

        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) = 0;
        virtual void setInput(GPUMapped<Image>* buffer) = 0;
        virtual void setOutput(GPUMapped<Image>* buffer) = 0;

        virtual GPUMapped<Image>* getOutput() const = 0;
    };
}

