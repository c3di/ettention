#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;
    template<typename _T> class VectorAdaptor;
    typedef GPUMapped<VectorAdaptor<float>> GPUMappedVector;

    class RMSKernel : public ComputeKernel
    {
    public:
        RMSKernel(Framework* framework, GPUMapped<Image>* residual);
        ~RMSKernel();

        float getRootMeanSquare() const;
        float getMeanSquare() const;
        void setInput(GPUMapped<Image>* input);

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void ensureResultSizeMatchesInput();

        GPUMapped<Image>* residual;
        GPUMappedVector* result;
        float rootMeanSquare;
        float meanSquare;
    };
}
