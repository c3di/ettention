#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class CompareKernel : public ComputeKernel
    {
    public:
        CompareKernel(Framework* framework, const Vec2ui& projectionResolution, GPUMapped<Image>* realProjection, GPUMapped<Image>* virtualProjection);
        ~CompareKernel();

        GPUMapped<Image>* getOutput();
        void setInput(GPUMapped<Image>* realProjection, GPUMapped<Image>* virtualProjection);
        void setOutput(GPUMapped<Image>* residuals);

    public:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        Vec2ui projectionResolution;    
        GPUMapped<Image>* realProjection;
        GPUMapped<Image>* virtualProjection;
        GPUMapped<Image>* residuals;    
        bool ownsResiduals;
    };
}