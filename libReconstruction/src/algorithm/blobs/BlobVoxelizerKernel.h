#pragma once
#include "gpu/ComputeKernel.h"
#include "algorithm/blobs/BlobVoxelizerSampler.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    class CLMemBuffer;

    class BlobVoxelizerKernel : public ComputeKernel
    {
    public:
        BlobVoxelizerKernel(Framework* framework, const BlobParameters& blobParameters, GPUMappedVolume* blobVolume, GPUMappedVolume* voxelVolume);
        ~BlobVoxelizerKernel();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        BlobVoxelizerSampler sampler;
        GPUMappedVolume* blobVolume;
        GPUMappedVolume* voxelVolume;
        CLMemBuffer* samplerBuffer;
    };
}