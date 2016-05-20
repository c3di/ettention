#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class PSFShiftingKernel : public ImageProcessingKernel
    {
    public:
        PSFShiftingKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result);
        PSFShiftingKernel(Framework* framework, GPUMapped<Image>* source);
        ~PSFShiftingKernel();

        void setInput(GPUMapped<Image>* source);

    protected:
        void prepareKernelArguments() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
    };
}