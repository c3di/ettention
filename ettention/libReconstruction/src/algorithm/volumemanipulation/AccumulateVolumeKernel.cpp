#include "stdafx.h"
#include "AccumulateVolumeKernel.h"
#include "AccumulateVolume_bin2c.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{

    AccumulateVolumeKernel::AccumulateVolumeKernel(Framework* framework, GPUMapped<SubVolume>* totalVolume, GPUMapped<SubVolume>* deltaVolume, GPUMapped<SubVolume>* voxelIntersectionsSum)
        : ComputeKernel(framework, AccumulateVolume_kernel_SourceCode, "accumulate", "AccumulateVolumeKernel")
        , totalVolume(totalVolume)
        , deltaVolume(deltaVolume)
        , voxelIntersectionsSum(voxelIntersectionsSum)
    {
    }

    AccumulateVolumeKernel::~AccumulateVolumeKernel()
    {
    }

    void AccumulateVolumeKernel::prepareKernelArguments()
    {
        implementation->setArgument("totalVolume", totalVolume);
        deltaVolume->ensureIsUpToDateOnGPU();
        implementation->setArgument("deltaVolume", deltaVolume);
        voxelIntersectionsSum->ensureIsUpToDateOnGPU();
        implementation->setArgument("traversal_length", voxelIntersectionsSum);
        implementation->setArgument("volumeResolutionX", (int)totalVolume->getResolution().x);
        implementation->setArgument("volumeResolutionY", (int)totalVolume->getResolution().y);
        implementation->setArgument("volumeResolutionZ", (int)totalVolume->getResolution().z);
    }

    void AccumulateVolumeKernel::preRun()
    {
        implementation->setGlobalWorkSize(totalVolume->getResolution().xy());
    }
    
    void AccumulateVolumeKernel::postRun()
    {
        totalVolume->markAsChangedOnGPU();
    }

    GPUMapped<SubVolume>* AccumulateVolumeKernel::getOutput() const
    {
        return totalVolume;
    }

    GPUMapped<SubVolume>* AccumulateVolumeKernel::getVoxelIntersectionsSum() const
    {
        return voxelIntersectionsSum;
    }

    GPUMapped<SubVolume>* AccumulateVolumeKernel::getDeltaVolume() const
    {
        return deltaVolume;
    }
}