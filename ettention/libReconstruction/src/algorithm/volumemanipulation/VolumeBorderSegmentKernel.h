#pragma once
#include "gpu/ComputeKernel.h"
#include "gpu/GPUMapped.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class VolumeBorderSegmentKernel : public ComputeKernel
    {
    public:
        static const unsigned int BORDER_VALUE;

        VolumeBorderSegmentKernel(Framework *framework, GPUMappedVolume *sourceVolume);
        VolumeBorderSegmentKernel(Framework *framework, GPUMappedVolume *sourceVolume, GPUMappedVolume *resultVolume);
        ~VolumeBorderSegmentKernel();

        void setInput(GPUMappedVolume *sourceVolume);
        void setOutput(GPUMappedVolume *resultVolume);
        GPUMappedVolume *getOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMappedVolume *sourceVolume;
        GPUMappedVolume *resultVolume;

        bool ownResultVolume;
    };
}