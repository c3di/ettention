#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class PSFShiftingKernel : public ComputeKernel
    {
    public:
        PSFShiftingKernel(Framework* framework, GPUMapped<Image>* source);
        ~PSFShiftingKernel();

        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getResult() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
        GPUMapped<Image>* result;
    };
}