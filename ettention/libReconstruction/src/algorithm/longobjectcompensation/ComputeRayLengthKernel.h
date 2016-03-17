#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;
    class GeometricSetup;

    class ComputeRayLengthKernel : public ComputeKernel
    {
    public:
        ComputeRayLengthKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMapped<Image>* rayLengthImage);
        virtual ~ComputeRayLengthKernel();

        GPUMapped<Image>* getOutput() const;
        void setRayLengthImage(GPUMapped<Image>* rayLengthImage);

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        const GeometricSetup* geometricSetup;
        GPUMapped<Image>* rayLengthImage;
    };
}