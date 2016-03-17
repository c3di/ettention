#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    template<typename _T> class GPUMapped;
    template<typename _T> class VectorAdaptor;
    typedef GPUMapped<VectorAdaptor<float>> GPUMappedVector;

    class MeanKernel : public ComputeKernel
    {
    public:
        MeanKernel(Framework* framework, GPUMappedVector* source);
        ~MeanKernel();

        void setInput(GPUMappedVector* source);
        float getMean() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMappedVector* source;
        GPUMappedVector* result;
        float mean;
    };
}