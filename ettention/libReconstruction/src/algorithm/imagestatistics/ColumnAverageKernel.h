#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;
    template<typename _T> class VectorAdaptor;
    typedef GPUMapped<VectorAdaptor<float>> GPUMappedVector;

    class ColumnAverageKernel : public ComputeKernel
    {
    public:
        ColumnAverageKernel(Framework* framework, GPUMapped<Image>* source);
        ~ColumnAverageKernel();

        void setInput(GPUMapped<Image>* source);
        GPUMappedVector* getColumnAverage() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
        GPUMappedVector* temporalBuffer;
    };
}

