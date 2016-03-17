#pragma once
#include "gpu/ComputeKernel.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    class SmoothedSquaredDifferencesSumKernel : public ComputeKernel
    {
    public:
        SmoothedSquaredDifferencesSumKernel(Framework* framework, GPUMappedVolume* volume, GPUMappedVolume* patch, GPUMappedVolume* squaredDifferencesSumVolume, float volumeMean);
        SmoothedSquaredDifferencesSumKernel(Framework* framework, GPUMappedVolume* volume, Vec3ui patchCoords, GPUMappedVolume* patch, GPUMappedVolume* squaredDifferencesSumVolume, float volumeMean);
        ~SmoothedSquaredDifferencesSumKernel();

        GPUMappedVolume* getSmoothedSquaredDifferencesSumVolume();
        void setPatchCoord(Vec3ui patchCoord);

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMappedVolume* volume;
        GPUMappedVolume* patch;
        GPUMappedVolume* squaredDifferencesSumVolume;
        Vec3ui patchCoords = Vec3ui(0,0,0);
        float volumeMean = 0.0f;
    };
}
