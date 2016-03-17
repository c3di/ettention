#pragma once
#include "gpu/ComputeKernel.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    class NormalizedCrossCorrelationKernel : public ComputeKernel
    {
    public:
        NormalizedCrossCorrelationKernel(Framework* framework, GPUMappedVolume* volume, GPUMappedVolume* patch, GPUMappedVolume* normalizedCrossCorrelationVolume, float volumeMean);
        NormalizedCrossCorrelationKernel(Framework* framework, GPUMappedVolume* volume, Vec3ui patchCoords, GPUMappedVolume* patch, GPUMappedVolume* normalizedCrossCorrelationVolume, float volumeMean);
        ~NormalizedCrossCorrelationKernel();

        GPUMappedVolume* getNormalizedCrossCorrelationVolume();
        void setPatchCoord(Vec3ui patchCoord);

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMappedVolume* volume;
        GPUMappedVolume* patch;
        GPUMappedVolume* normalizedCrossCorrelationVolume;
        Vec3ui patchCoords = Vec3ui(0,0,0);
        float volumeMean = 0.0f;
    };
}
