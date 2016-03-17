#pragma once
#include "gpu/ComputeKernel.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    class SetToValueKernel : public ComputeKernel
    {
    public:
        SetToValueKernel(Framework* framework, GPUMapped<SubVolume>* subVolume, float value);
        ~SetToValueKernel();

        void setOutputSubvolume(GPUMapped<SubVolume>* subVolume);
        GPUMapped<SubVolume>* getOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMapped<SubVolume>* subVolume;
        float value;
    };
}