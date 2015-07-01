#include "stdafx.h"
#include "BlobVoxelizerOperator.h"
#include "algorithm/blobs/BlobVoxelizerKernel.h"
#include "framework/Framework.h"
#include "io/serializer/VolumeSerializer.h"

namespace ettention
{
    BlobVoxelizerOperator::BlobVoxelizerOperator(Framework* framework, const BlobParameters& blobParameters, GPUMappedVolume* blobVolume)
        : blobVolume(blobVolume)
        , parameters(blobParameters)
    {
        voxelVolume = new Volume(blobVolume->getObjectOnCPU()->Properties(), 0.0f);
        mappedVoxelVolume = new GPUMappedVolume(framework->getOpenCLStack(), voxelVolume);        
        kernel = new BlobVoxelizerKernel(framework, blobParameters, blobVolume, mappedVoxelVolume);
    }

    BlobVoxelizerOperator::~BlobVoxelizerOperator()
    {
        delete kernel;
        delete mappedVoxelVolume;
        delete voxelVolume;
    }

    GPUMappedVolume* BlobVoxelizerOperator::GetVoxelVolume() const
    {
        return mappedVoxelVolume;
    }

    void BlobVoxelizerOperator::Voxelize()
    {
        static const int MAX_SUBVOLUME_RESOLUTION = 512;
        const Vec3i subVolumeResolution(std::min(MAX_SUBVOLUME_RESOLUTION, (int)blobVolume->Properties().GetVolumeResolution().x),
                                        std::min(MAX_SUBVOLUME_RESOLUTION, (int)blobVolume->Properties().GetVolumeResolution().y),
                                        std::min(MAX_SUBVOLUME_RESOLUTION, (int)blobVolume->Properties().GetVolumeResolution().z));

        const unsigned int blocksX = (unsigned int)ceil((float)blobVolume->Properties().GetVolumeResolution().x / (float)subVolumeResolution.x);
        const unsigned int blocksY = (unsigned int)ceil((float)blobVolume->Properties().GetVolumeResolution().y / (float)subVolumeResolution.y);
        const unsigned int blocksZ = (unsigned int)ceil((float)blobVolume->Properties().GetVolumeResolution().z / (float)subVolumeResolution.z);
        const unsigned int radius = (unsigned int)ceil(parameters.GetSupportRadius());
        for(unsigned int blockZ = 0; blockZ < blocksZ; ++blockZ)
        {
            for(unsigned int blockY = 0; blockY < blocksY; ++blockY)
            {
                for(unsigned int blockX = 0; blockX < blocksX; ++blockX)
                {
                    Vec3i origin = subVolumeResolution * Vec3i(blockX, blockY, blockZ);
                    Vec3ui resolution(subVolumeResolution.x, subVolumeResolution.y, subVolumeResolution.z);
                    blobVolume->setArbitrarySubVolume(origin - Vec3i(radius, radius, radius), resolution + 2U * Vec3ui(radius, radius, radius), 0.0f);
                    mappedVoxelVolume->setArbitrarySubVolume(origin, resolution, 0.0f);
                    blobVolume->ensureIsUpToDateOnGPU();
                    mappedVoxelVolume->ensureIsUpToDateOnGPU();
                    kernel->run();
                }
            }
        }
    }
}