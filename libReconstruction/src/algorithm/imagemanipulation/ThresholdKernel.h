#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class ThresholdKernel : public ComputeKernel
    {
    public:
        ThresholdKernel(Framework* framework, GPUMapped<Image>* source);
        ~ThresholdKernel();

        void setThreshold(float value);
        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getResult() const;
        float getThreshold() const;

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
        float thresholdValue;
    };
}