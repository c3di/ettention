#pragma once
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    class Voxelizer
    {
    public:
        Voxelizer();
        virtual ~Voxelizer();

        virtual void Voxelize() = 0;
        virtual GPUMappedVolume* GetVoxelVolume() const = 0;
    };
}