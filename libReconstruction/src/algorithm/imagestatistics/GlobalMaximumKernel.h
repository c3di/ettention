#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;
    template<typename _T> class VectorAdaptor;
    typedef GPUMapped<VectorAdaptor<float>> GPUMappedVector;
    typedef GPUMapped<VectorAdaptor<int>> GPUMappedIntVector;

    class GlobalMaximumKernel : public ComputeKernel
    {
    public:
        GlobalMaximumKernel(Framework* framework, GPUMapped<Image>* source);
        ~GlobalMaximumKernel();

        void setInput(GPUMapped<Image>* sourceImage);
        const Vec2ui& getMaximumPosition() const;
        float getMaximum() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
        GPUMappedVector* temporalBufferXpos;
        GPUMappedVector* temporalBufferYpos;
        GPUMappedIntVector* resultBufferPosition;
        GPUMappedVector* resultBufferValue;
        Vec2ui projectionResolution;
        Vec2ui maximumPosition;
        float maximum;
    };
}