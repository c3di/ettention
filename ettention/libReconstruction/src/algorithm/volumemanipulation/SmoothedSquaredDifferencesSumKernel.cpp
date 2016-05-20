#include "stdafx.h"
#include "SmoothedSquaredDifferencesSumKernel.h"
#include "SmoothedSquaredDifferencesSum_bin2c.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{

    SmoothedSquaredDifferencesSumKernel::SmoothedSquaredDifferencesSumKernel(Framework* framework, GPUMappedVolume* volume, GPUMappedVolume* patch, GPUMappedVolume* squaredDifferencesSumVolume, float volumeMean)
        : ComputeKernel(framework, SmoothedSquaredDifferencesSum_kernel_SourceCode, "SmoothedSquaredDifferencesSum", "SmoothedSquaredDifferencesSumKernel")
        , volume(volume)
        , patch(patch)
        , squaredDifferencesSumVolume(squaredDifferencesSumVolume)
        , volumeMean(volumeMean)
    {
    }

    SmoothedSquaredDifferencesSumKernel::SmoothedSquaredDifferencesSumKernel(Framework* framework, GPUMappedVolume* volume, Vec3ui patchCoords, GPUMappedVolume* patch, GPUMappedVolume* squaredDifferencesSumVolume, float volumeMean)
        : ComputeKernel(framework, SmoothedSquaredDifferencesSum_kernel_SourceCode, "SmoothedSquaredDifferencesSum", "SmoothedSquaredDifferencesSumKernel")
        , volume(volume)
        , patchCoords(patchCoords)
        , patch(patch)
        , squaredDifferencesSumVolume(squaredDifferencesSumVolume)
        , volumeMean(volumeMean)
    {
    }

    SmoothedSquaredDifferencesSumKernel::~SmoothedSquaredDifferencesSumKernel()
    {
    }

    void SmoothedSquaredDifferencesSumKernel::setPatchCoord(Vec3ui patchCoord)
    {
        patchCoords.x = patchCoord.x;
        patchCoords.y = patchCoord.y;
        patchCoords.z = patchCoord.z;
    }

    void SmoothedSquaredDifferencesSumKernel::prepareKernelArguments()
    {
        volume->getMappedSubVolume()->ensureIsUpToDateOnGPU();
        implementation->setArgument("volume", volume->getMappedSubVolume());
        implementation->setArgument("patchCoordX", (int)patchCoords.x);
        implementation->setArgument("patchCoordY", (int)patchCoords.y);
        implementation->setArgument("patchCoordZ", (int)patchCoords.z);
        implementation->allocateArgument("patch", patch->getProperties().getVolumeByteWidth());
        squaredDifferencesSumVolume->ensureIsUpToDateOnGPU();
        implementation->setArgument("ssdVolume", squaredDifferencesSumVolume->getMappedSubVolume());
        implementation->setArgument("patchResolutionX", (int)patch->getProperties().getVolumeResolution().x);
        implementation->setArgument("patchResolutionY", (int)patch->getProperties().getVolumeResolution().y);
        implementation->setArgument("patchResolutionZ", (int)patch->getProperties().getVolumeResolution().z);
        implementation->setArgument("volumeMean", (float)volumeMean);
    }

    void SmoothedSquaredDifferencesSumKernel::preRun()
    {
        implementation->setGlobalWorkSize(volume->getProperties().getVolumeResolution());
    }
    
    void SmoothedSquaredDifferencesSumKernel::postRun()
    {
        squaredDifferencesSumVolume->markAsChangedOnGPU();
    }

    GPUMappedVolume* SmoothedSquaredDifferencesSumKernel::getSmoothedSquaredDifferencesSumVolume()
    {
        return squaredDifferencesSumVolume;
    }
}