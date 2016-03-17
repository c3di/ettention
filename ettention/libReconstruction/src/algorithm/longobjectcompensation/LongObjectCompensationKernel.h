#pragma once

#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class LongObjectCompensationKernel : public ComputeKernel
    {
    public:
        LongObjectCompensationKernel(Framework* framework, const std::string &kernelSource, const std::string& kernelName, const std::string& kernelCaption, GPUMapped<Image>* projection, GPUMapped<Image>* traversalLength, boost::optional<float> constantLength = boost::none);
        virtual ~LongObjectCompensationKernel();

        GPUMapped<Image>* getProjection() const;
        void setProjection(GPUMapped<Image>* projection);
        void setTraversalLength(GPUMapped<Image>* traversalLength);
        void setConstantLength(float constantLength);

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