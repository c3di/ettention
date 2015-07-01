#pragma once
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class ConvolutionRealSpaceImplementation : public ComputeKernel, public ConvolutionOperatorImplementation
    {
    public:
        ConvolutionRealSpaceImplementation(Framework* framework, GPUMapped<Image>* kernel, GPUMapped<Image>* source);
        ~ConvolutionRealSpaceImplementation();

        virtual void execute() override;

        void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) override;
        void setInput(GPUMapped<Image>* buffer) override;
        void setOutput(GPUMapped<Image>* buffer) override;

        float getWeightsSum() const;
        GPUMapped<Image>* getResult() override;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void allocateOutput();
        void releaseOutput();

        GPUMapped<Image>* kernel;
        GPUMapped<Image>* source;
        GPUMapped<Image>* result;
        Vec2ui kernelResolution;
        Vec2ui projectionResolution;
        float weightsSum;
        bool ownsOutput;
    };
}
