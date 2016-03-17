#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class GenerateRamLakKernel : public ComputeKernel
    {
    public:
        GenerateRamLakKernel(Framework* framework, const unsigned int resolution);
        ~GenerateRamLakKernel();

        GPUMapped<Image>* getOutput();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMapped<Image> *result;
        unsigned int resolution;
    };
}

