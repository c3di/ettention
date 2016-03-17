#pragma once

namespace ettention
{
    class GPUMappedVolume;

    class Voxelizer
    {
    public:
        Voxelizer();
        virtual ~Voxelizer();

        virtual void voxelize() = 0;
        virtual GPUMappedVolume* getVoxelVolume() const = 0;
    };
}