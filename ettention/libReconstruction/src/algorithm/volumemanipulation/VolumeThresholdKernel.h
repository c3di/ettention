#pragma once
#include "gpu/ComputeKernel.h"
#include "gpu/GPUMapped.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class VolumeThresholdKernel : public ComputeKernel
    {
    public:
        VolumeThresholdKernel(Framework *framework, GPUMappedVolume *sourceVolume, GPUMappedVolume *resultVolume, GPUMappedVector *floorLevels, GPUMappedVector *floorValues, GPUMappedVolume* maskVolume = nullptr);
        ~VolumeThresholdKernel();

        void setInput(GPUMappedVolume *sourceVolume);
        GPUMappedVolume *getOutput() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMappedVolume *sourceVolume;
        GPUMappedVolume *resultVolume;
        GPUMappedVolume *maskVolume;
        GPUMappedVector *thresholdFloorLevels;
        GPUMappedVector *thresholdFloorValues;

        unsigned int thresholdLevelCount;
    };
}