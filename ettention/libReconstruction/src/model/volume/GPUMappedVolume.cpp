#include "stdafx.h"
#include "GPUMappedVolume.h"
#include "framework/Logger.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"

namespace ettention
{
    GPUMappedVolume::GPUMappedVolume(CLAbstractionLayer* clStack, Volume* volume, bool useImage)
        : clStack(clStack)
        , volume(volume)
        , currentSubvolumeIndex(0)
    {
        subVolume = new SubVolume(volume);
        mappedSubVolume = new GPUMapped<SubVolume>(clStack, subVolume, useImage, volume->getProperties().getVoxelType());
    }

    GPUMappedVolume::~GPUMappedVolume()
    {
        delete mappedSubVolume;
        delete subVolume;
    }

    void GPUMappedVolume::changeSubVolumeSizeToPartOfTotalMemory(float maxPercentageOfGPUMemory)
    {
        if(maxPercentageOfGPUMemory > 1.0f)
            throw Exception("volume can not take more than 100% of available HPC memory");

        mappedSubVolume->ensureIsUpToDateOnCPU();

        size_t subVolumeSize = size_t((double)clStack->getRemainingMemorySizeInBytes() * (double)maxPercentageOfGPUMemory);

        size_t maxChunkSize = clStack->getMaxSizeOfIndividualMemoryObjectInBytes();
        if(maxChunkSize < subVolumeSize)
            subVolumeSize = maxChunkSize;

        size_t volumeSizeInBytes = volume->getProperties().getVolumeByteWidth();
        int subVolumeCount = (int) ((volumeSizeInBytes - 1) / subVolumeSize) + 1;

        changeSubVolumeCountToFixedNumber(subVolumeCount);
    }

    void GPUMappedVolume::changeSubVolumeCountToFixedNumber(unsigned int subVolumeCount)
    {
        volume->getProperties().setSubVolumeCount(subVolumeCount);
        mappedSubVolume->getObjectOnCPU()->resetSubvolumePartitioning();
        mappedSubVolume->reallocateGPUBuffer();
    }

    void GPUMappedVolume::setCurrentSubvolumeIndex(unsigned int val)
    {
        if(mappedSubVolume->getObjectOnCPU()->getCurrentSubVolume() != val)
        {
            ensureIsUpToDateOnCPU();
            mappedSubVolume->getObjectOnCPU()->setCurrentSubVolume(val);
            mappedSubVolume->markAsChangedOnCPU();
            currentSubvolumeIndex = val;
        }        
    }

    unsigned int GPUMappedVolume::getCurrentSubvolumeIndex() const
    {
        return currentSubvolumeIndex;
    }

    void GPUMappedVolume::setArbitrarySubVolume(const Vec3i& origin, const Vec3ui& resolution, float outOfBoundsValue)
    {
        ensureIsUpToDateOnCPU();
        mappedSubVolume->getObjectOnCPU()->setArbitrarySubVolume(origin, resolution, outOfBoundsValue);
        mappedSubVolume->markAsChangedOnCPU();
        currentSubvolumeIndex = ARBITRARY_SUBVOLUME_SET;
    }

    void GPUMappedVolume::markAsChangedOnCPU()
    {
        mappedSubVolume->markAsChangedOnCPU();
    }

    void GPUMappedVolume::updateRegionOnGPU(const Vec3ui& baseCorner, const Vec3ui& resolution)
    {
        if(mappedSubVolume->hasChangedOnCPU())
        {
            LOGGER_ERROR("updateRegionOnGPU() was called but the objectOnCPU was marked as changed so it will be completely updated anyway.");
        }
        auto updateRegionMin = componentWiseMax((Vec3i)baseCorner, subVolume->getBaseCoords());
        auto updateRegionMax = componentWiseMin((Vec3i)(baseCorner + resolution), subVolume->getBaseCoords() + (Vec3i)subVolume->getResolution());
        if(updateRegionMin == subVolume->getBaseCoords() && updateRegionMax == subVolume->getBaseCoords() + subVolume->getResolution())
        {
            LOGGER_IMP("updateRegionOnGPU() was called for a region that completely spans the current sub volume.");
            mappedSubVolume->markAsChangedOnCPU();
            mappedSubVolume->ensureIsUpToDateOnGPU();
        }
        else
        {
            for(int z = updateRegionMin.z; z < updateRegionMax.z; ++z)
            {
                for(int y = updateRegionMin.y; y < updateRegionMax.y; ++y)
                {
                    Vec3ui rowVolumeCoords(updateRegionMin.x, y, z);
                    auto rowVolumeIndex = volume->getVoxelIndex(rowVolumeCoords);
                    Vec3ui rowSubVolumeCoords = rowVolumeCoords - subVolume->getBaseCoords();
                    auto rowSubVolumeIndex = rowSubVolumeCoords.z * subVolume->getResolution().y * subVolume->getResolution().x + rowSubVolumeCoords.y * subVolume->getResolution().x + rowSubVolumeCoords.x;

                    auto byteWidth = (updateRegionMax.x - updateRegionMin.x) * volume->getProperties().getVoxelByteWidth();
                    const void* rowPtr = volume->getRawPointerOfVoxel(rowVolumeIndex);
                    mappedSubVolume->getBufferOnGPU()->transferFrom(rowPtr, rowSubVolumeIndex * volume->getProperties().getVoxelByteWidth(), byteWidth);
                }
            }
        }
    }

    void GPUMappedVolume::markAsChangedOnGPU()
    {
        mappedSubVolume->markAsChangedOnGPU();
    }

    void GPUMappedVolume::ensureIsUpToDateOnGPU()
    {
        mappedSubVolume->ensureIsUpToDateOnGPU();
    }

    void GPUMappedVolume::ensureIsUpToDateOnCPU()
    {
        if(mappedSubVolume->hasChangedOnGPU())
        {
            mappedSubVolume->ensureIsUpToDateOnCPU();
            mappedSubVolume->getObjectOnCPU()->copyPrivateDataToVolume();
        }
    }

    void GPUMappedVolume::takeOwnershipOfObjectOnCPU()
    {
        subVolume->takeOwnershipOfVolume();
    }

    Volume* GPUMappedVolume::getObjectOnCPU()
    {
        return volume;
    }

    CLMemBuffer* GPUMappedVolume::getBufferOnGPU()
    {
        return mappedSubVolume->getBufferOnGPU();
    }

    GPUMapped<SubVolume>* GPUMappedVolume::getMappedSubVolume()
    {
        return mappedSubVolume;
    }

    VolumeProperties& GPUMappedVolume::getProperties()
    {
        return volume->getProperties();
    }
}