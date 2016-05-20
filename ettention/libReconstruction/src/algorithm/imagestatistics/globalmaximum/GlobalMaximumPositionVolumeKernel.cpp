#include "stdafx.h"
#include "GlobalMaximumPositionVolumeKernel.h"
#include "gpu/opencl/CLKernel.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    GlobalMaximumPositionVolumeKernel::GlobalMaximumPositionVolumeKernel(Framework* framework, GPUMapped<SubVolume>* sourceVolume)
        : GlobalMaximumPositionInterface(framework, sourceVolume->getBufferOnGPU(), sourceVolume->getResolution().x * sourceVolume->getResolution().y * sourceVolume->getResolution().z)
        , resolution(sourceVolume->getResolution())
    {
        setInputVolume(sourceVolume);
        sourceVolume->ensureIsUpToDateOnGPU();
    }

    GlobalMaximumPositionVolumeKernel::~GlobalMaximumPositionVolumeKernel()
    {
    }

    void GlobalMaximumPositionVolumeKernel::setInputVolume(GPUMapped<SubVolume>* sourceVolume)
    {
        if( sourceVolume->getResolution() != resolution )
            throw Exception("Cannot assign as input Volume of different resolution!");
        this->sourceVolume = sourceVolume;
    }

    Vec3ui GlobalMaximumPositionVolumeKernel::getMaximumXYZ() const
    {
        return Vec3ui(maximumPosition % resolution.x, (maximumPosition / resolution.x) % resolution.y, maximumPosition / (resolution.x * resolution.y) );
    }
}