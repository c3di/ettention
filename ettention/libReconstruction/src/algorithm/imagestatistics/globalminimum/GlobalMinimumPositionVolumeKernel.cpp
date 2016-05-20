#include "stdafx.h"
#include "GlobalMinimumPositionVolumeKernel.h"
#include "gpu/opencl/CLKernel.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    GlobalMinimumPositionVolumeKernel::GlobalMinimumPositionVolumeKernel(Framework* framework, GPUMapped<SubVolume>* sourceVolume)
        : GlobalMinimumPositionInterface(framework, sourceVolume->getBufferOnGPU(), sourceVolume->getResolution().x * sourceVolume->getResolution().y * sourceVolume->getResolution().z)
        , resolution(sourceVolume->getResolution())
    {
        setInputVolume(sourceVolume);
        sourceVolume->ensureIsUpToDateOnGPU();
    }

    GlobalMinimumPositionVolumeKernel::~GlobalMinimumPositionVolumeKernel()
    {
    }

    void GlobalMinimumPositionVolumeKernel::setInputVolume(GPUMapped<SubVolume>* sourceVolume)
    {
        if( sourceVolume->getResolution() != resolution )
            throw Exception("Cannot assign as input Volume of different resolution!");
        this->sourceVolume = sourceVolume;
    }

    Vec3ui GlobalMinimumPositionVolumeKernel::getMinimumXYZ() const
    {
        return Vec3ui(minimumPosition % resolution.x, (minimumPosition / resolution.x) % resolution.y, minimumPosition / (resolution.x * resolution.y) );
    }
}