#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class SquareKernel : public ImageProcessingKernel
    {
    public:
        SquareKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result);
        SquareKernel(Framework* framework, GPUMapped<Image>* source);
        ~SquareKernel();

        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getInput() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        Vec2ui projectionResolution;
    };
}
