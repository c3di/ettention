#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class ErosionKernel : public ImageProcessingKernel
    {
    public:
        ErosionKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result);
        ErosionKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement);
        ~ErosionKernel();

        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getInput() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        GPUMapped<Image>* structureElement;
        Vec2ui projectionResolution;
        Vec2ui structureResolution;
    };
}