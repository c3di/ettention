#pragma once
#include "framework/math/Vec3.h"

namespace ettention
{
    class VoxelSample
    {
    public:
        VoxelSample();
        VoxelSample(Vec3f positionOffset, float rayDistanceInsideVoxel);
        ~VoxelSample();

        Vec3f PositionOffset() const;
        float RayDistanceInsideVoxel() const;

    protected:
        Vec3f positionOffset;
        float rayDistanceInsideVoxel;
    };
}