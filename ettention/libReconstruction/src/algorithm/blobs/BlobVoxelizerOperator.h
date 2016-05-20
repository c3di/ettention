#pragma once
#include "algorithm/projections/back/Voxelizer.h"
#include "model/blobs/BlobParameters.h"

namespace ettention
{
    class VolumeConvolutionOperator;
    class Framework;
    class GPUMappedVolume;

    class BlobVoxelizerOperator : public Voxelizer
    {
    public:
        BlobVoxelizerOperator(Framework* framework, GPUMappedVolume* blobVolume, const BlobParameters& blobParameters);
        ~BlobVoxelizerOperator();

        GPUMappedVolume* getVoxelVolume() const override;
        void voxelize() override;

    private:
        BlobParameters blobParameters;
        VolumeConvolutionOperator* convolutionOperator;
        GPUMappedVolume* voxelVolume;
        GPUMappedVolume* kernelValues;
    };
}