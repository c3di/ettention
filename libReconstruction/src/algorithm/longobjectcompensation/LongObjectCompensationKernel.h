#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class LongObjectCompensationKernel : public ComputeKernel
    {
    public:
        LongObjectCompensationKernel(Framework* framework, const std::string &kernelSource, const std::string& kernelName, const std::string& kernelCaption, GPUMapped<Image>* traversalLength);
        virtual ~LongObjectCompensationKernel();

        GPUMapped<Image>* getProjection() const;
        void setConstantLength(float constantLength);
        void setProjection(GPUMapped<Image>* projection);
        void setTraversalLength(GPUMapped<Image>* traversalLength);

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMapped<Image>* projection;
        GPUMapped<Image>* traversalLength;
        float constantLength;
    };
}