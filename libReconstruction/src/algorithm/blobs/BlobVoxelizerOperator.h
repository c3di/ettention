#pragma once
#include "algorithm/projections/back/Voxelizer.h"
#include "model/blobs/BlobParameters.h"

namespace ettention
{
    class BlobVoxelizerKernel;
    class Framework;
    class GPUMappedVolume;

    class BlobVoxelizerOperator : public Voxelizer
    {
    public:
        BlobVoxelizerOperator(Framework* framework, const BlobParameters& blobParameters, GPUMappedVolume* blobVolume);
        ~BlobVoxelizerOperator();

        GPUMappedVolume* GetVoxelVolume() const override;
        void Voxelize() override;

    private:
        BlobParameters parameters;
        BlobVoxelizerKernel* kernel;
        GPUMappedVolume* blobVolume;
        Volume* voxelVolume;
        GPUMappedVolume* mappedVoxelVolume;
    };
}