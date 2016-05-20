#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class ThresholdKernel : public ImageProcessingKernel
    {
    public:
        ThresholdKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result);
        ThresholdKernel(Framework* framework, GPUMapped<Image>* source);
        ~ThresholdKernel();

        void setThreshold(float value);
        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getInput() const;
        float getThreshold() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        Vec2ui projectionResolution;
        float thresholdValue;
    };
}