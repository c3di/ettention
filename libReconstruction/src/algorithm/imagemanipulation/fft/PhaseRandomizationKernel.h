#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class PhaseRandomizationKernel : public ComputeKernel
    {
    public:
        PhaseRandomizationKernel(Framework* framework, GPUMapped<Image>* imageMagnitude, GPUMapped<Image>* imageRandom);
        ~PhaseRandomizationKernel();

        GPUMapped<Image>* getRealOutput() const;
        GPUMapped<Image>* getImaginaryOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* imageMagnitude;
        GPUMapped<Image>* imageRandom;
        GPUMapped<Image>* imagePhaseShiftedReal;
        GPUMapped<Image>* imagePhaseShiftedImaginary;
    };
}