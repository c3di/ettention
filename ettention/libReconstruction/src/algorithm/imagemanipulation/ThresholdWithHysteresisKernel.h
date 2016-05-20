#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class ThresholdWithHysteresisKernel : public ImageProcessingKernel
    {
    public:
        ThresholdWithHysteresisKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result);
        ThresholdWithHysteresisKernel(Framework* framework, GPUMapped<Image>* source);
        ~ThresholdWithHysteresisKernel();

        void setThreshold(float floorLevel, float ceilingLevel);
        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getInput() const;
        float getFloorThreshold() const;
        float getCeilingThreshold() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        Vec2ui projectionResolution;
        float floorLevel;
        float ceilingLevel;
    };
}