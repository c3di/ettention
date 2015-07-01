#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class SquareKernel : public ComputeKernel
    {
    public:
        SquareKernel(Framework* framework, GPUMapped<Image>* source);
        ~SquareKernel();

        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getResult() const;
        GPUMapped<Image>* getInput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
        GPUMapped<Image>* result;
        Vec2ui projectionResolution;
    };
}

