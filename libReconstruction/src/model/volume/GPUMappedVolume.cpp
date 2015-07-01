#include "stdafx.h"
#include "model/volume/GPUMappedVolume.h"
#include "gpu/opencl/CLAbstractionLayer.h"

namespace ettention
{
    GPUMappedVolume::GPUMappedVolume(CLAbstractionLayer* clStack, Volume* volume)
        : clStack(clStack)
        , volume(volume)
        , currentSubvolumeIndex(0)
    {
        subVolume = new SubVolume(volume);
        mappedSubVolume = new GPUMapped<SubVolume>(clStack, subVolume, true, volume->Properties().GetVoxelType());
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

        // if(mappedSubVolume->doesUseImage())
        //     subVolumeSize /= 2; // requires double memory in this case

        size_t maxChunkSize = clStack->getMaxSizeOfIndividualMemoryObjectInBytes();
        if(maxChunkSize < subVolumeSize)
            subVolumeSize = maxChunkSize;

        size_t volumeSizeInBytes = volume->Properties().GetVolumeByteWidth();
        int subVolumeCount = (int) ((volumeSizeInBytes - 1) / subVolumeSize) + 1;

        changeSubVolumeCountToFixedNumber(subVolumeCount);
    }

    void GPUMappedVolume::changeSubVolumeCountToFixedNumber(unsigned int subVolumeCount)
    {
        volume->Properties().SetSubVolumeCount(subVolumeCount);
        mappedSubVolume->getObjectOnCPU()->resetSubvolumePartitioning();
        mappedSubVolume->reallocateGPUBuffer();
    }

    void GPUMappedVolume::setCurrentSubvolumeIndex(unsigned int val)
    {
        if(mappedSubVolume->getObjectOnCPU()->getCurrentSubVolume() != val)
        {
            this->ensureIsUpToDateOnCPU();
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
        this->ensureIsUpToDateOnCPU();
        mappedSubVolume->getObjectOnCPU()->setArbitrarySubVolume(origin, resolution, outOfBoundsValue);
        mappedSubVolume->markAsChangedOnCPU();
        currentSubvolumeIndex = (unsigned int)-1;
    }

    void GPUMappedVolume::markAsChangedOnCPU()
    {
        mappedSubVolume->markAsChangedOnCPU();
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

    VolumeProperties& GPUMappedVolume::Properties()
    {
        return volume->Properties();
    }
}