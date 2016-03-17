#include "stdafx.h"
#include "NormalizedCrossCorrelationKernel.h"
#include "NormalizedCrossCorrelation_bin2c.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{

    NormalizedCrossCorrelationKernel::NormalizedCrossCorrelationKernel(Framework* framework, GPUMappedVolume* volume, GPUMappedVolume* patch, GPUMappedVolume* normalizedCrossCorrelationVolume, float volumeMean)
        : ComputeKernel(framework, NormalizedCrossCorrelation_kernel_SourceCode, "NormalizedCrossCorrelation", "NormalizedCrossCorrelationKernel")
        , volume(volume)
        , patch(patch)
        , normalizedCrossCorrelationVolume(normalizedCrossCorrelationVolume)
        , volumeMean(volumeMean)
    {
    }

    NormalizedCrossCorrelationKernel::NormalizedCrossCorrelationKernel(Framework* framework, GPUMappedVolume* volume, Vec3ui patchCoords, GPUMappedVolume* patch, GPUMappedVolume* normalizedCrossCorrelationVolume, float volumeMean)
        : ComputeKernel(framework, NormalizedCrossCorrelation_kernel_SourceCode, "NormalizedCrossCorrelation", "NormalizedCrossCorrelationKernel")
        , volume(volume)
        , patchCoords(patchCoords)
        , patch(patch)
        , normalizedCrossCorrelationVolume(normalizedCrossCorrelationVolume)
        , volumeMean(volumeMean)
    {
    }

    NormalizedCrossCorrelationKernel::~NormalizedCrossCorrelationKernel()
    {
    }

    void NormalizedCrossCorrelationKernel::setPatchCoord(Vec3ui patchCoord)
    {
        patchCoords.x = patchCoord.x;
        patchCoords.y = patchCoord.y;
        patchCoords.z = patchCoord.z;
    }

    void NormalizedCrossCorrelationKernel::prepareKernelArguments()
    {
        volume->getMappedSubVolume()->ensureIsUpToDateOnGPU();
        implementation->setArgument("volume", volume->getMappedSubVolume());
        implementation->setArgument("patchCoordX", (int)patchCoords.x);
        implementation->setArgument("patchCoordY", (int)patchCoords.y);
        implementation->setArgument("patchCoordZ", (int)patchCoords.z);
        implementation->allocateArgument("patch", patch->getProperties().getVolumeVoxelCount() * patch->getProperties().getVoxelByteWidth());
        normalizedCrossCorrelationVolume->ensureIsUpToDateOnGPU();
        implementation->setArgument("nccVolume", normalizedCrossCorrelationVolume->getMappedSubVolume());
        implementation->setArgument("patchResolutionX", (int)patch->getProperties().getVolumeResolution().x);
        implementation->setArgument("patchResolutionY", (int)patch->getProperties().getVolumeResolution().y);
        implementation->setArgument("patchResolutionZ", (int)patch->getProperties().getVolumeResolution().z);
        implementation->setArgument("volumeMean", (float)volumeMean);
    }

    void NormalizedCrossCorrelationKernel::preRun()
    {
        implementation->setGlobalWorkSize(volume->getProperties().getVolumeResolution());
    }
    
    void NormalizedCrossCorrelationKernel::postRun()
    {
        normalizedCrossCorrelationVolume->markAsChangedOnGPU();
    }

    GPUMappedVolume* NormalizedCrossCorrelationKernel::getNormalizedCrossCorrelationVolume()
    {
        return normalizedCrossCorrelationVolume;
    }
}