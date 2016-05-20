#pragma once
#include "algorithm/imagestatistics/globalminimum/GlobalMinimumPositionInterface.h"
#include "gpu/ComputeKernel.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class SubVolume;
    template<typename _T> class GPUMapped;

    class GlobalMinimumPositionVolumeKernel : public GlobalMinimumPositionInterface
    {
    public:
        GlobalMinimumPositionVolumeKernel(Framework* framework, GPUMapped<SubVolume>* sourceVolume);
        ~GlobalMinimumPositionVolumeKernel();

        void setInputVolume(GPUMapped<SubVolume>* sourceVolume);
        Vec3ui getMinimumXYZ() const;

    private:
        GPUMapped<SubVolume>* sourceVolume;
        Vec3ui resolution;
    };
}