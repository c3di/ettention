#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class CLMemBuffer;
    class Image;
    template<typename _T> class GPUMapped;
    template<typename _T> class VectorAdaptor;
    typedef GPUMapped<VectorAdaptor<float>> GPUMappedVector;
    typedef GPUMapped<VectorAdaptor<int>> GPUMappedIntVector;

    class GlobalMinimumPositionInterface : public ComputeKernel
    {
    protected:
        GlobalMinimumPositionInterface(Framework* framework, CLMemBuffer* source, int taskSize);
    public:
        ~GlobalMinimumPositionInterface();
        int getMinimumPosition() const;
        float getMinimumValue() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

        int minimumPosition;
        float minimumValue;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        CLMemBuffer* source;
        GPUMappedVector* resultValues;
        GPUMappedIntVector* resultPositions;
        int taskSize;

        int numberOfWorkgroups;
        unsigned int workersInWorkgroup;
        unsigned int workersTotal;
    };
}