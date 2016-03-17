#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    template<typename _T> class GPUMapped;
    template<typename _T> class VectorAdaptor;
    typedef GPUMapped<VectorAdaptor<unsigned short>> GPUMappedUShortVector;

    class HalfFloatTestKernel : public ComputeKernel
    {
    public:
        HalfFloatTestKernel(Framework* framework, GPUMappedUShortVector* data);
        ~HalfFloatTestKernel();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMappedUShortVector* data;
    };
}