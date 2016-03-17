#pragma once
#include "algorithm/imagestatistics/globalmaximum/GlobalMaximumPositionInterface.h"
#include "gpu/ComputeKernel.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class SubVolume;
    template<typename _T> class GPUMapped;

    class GlobalMaximumPositionVolumeKernel : public GlobalMaximumPositionInterface
    {
    public:
        GlobalMaximumPositionVolumeKernel(Framework* framework, GPUMapped<SubVolume>* sourceVolume);
        ~GlobalMaximumPositionVolumeKernel();

        void setInputVolume(GPUMapped<SubVolume>* sourceVolume);
        Vec3ui getMaximumXYZ() const;

    private:
        GPUMapped<SubVolume>* sourceVolume;
        Vec3ui resolution;
    };
}