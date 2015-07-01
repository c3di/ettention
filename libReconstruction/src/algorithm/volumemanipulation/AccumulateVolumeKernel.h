#pragma once
#include "gpu/ComputeKernel.h"
#include "model/volume/Volume.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    class AccumulateVolumeKernel : public ComputeKernel
    {
    public:
        AccumulateVolumeKernel(Framework* framework, GPUMapped<SubVolume>* totalVolume, GPUMapped<SubVolume>* deltaVolume, GPUMapped<SubVolume>* voxelIntersectionsSum);
        ~AccumulateVolumeKernel();

        GPUMapped<SubVolume>* getOutput() const;
        GPUMapped<SubVolume>* getDeltaVolume() const;
        GPUMapped<SubVolume>* getVoxelIntersectionsSum() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        GPUMapped<SubVolume>* totalVolume;
        GPUMapped<SubVolume>* deltaVolume;
        GPUMapped<SubVolume>* voxelIntersectionsSum;
    };
}
