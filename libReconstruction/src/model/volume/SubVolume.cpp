#include "stdafx.h"
#include "model/volume/SubVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/VoxelType.h"
#include "framework/math/Float16Compressor.h"

namespace ettention
{ 
    SubVolume::SubVolume(Volume* volume)
        : volume(volume)
        , currentSubvolumeIndex(0)
        , ownsVolume(false)
    {
        resolution = Vec3ui(volume->Properties().GetSubVolumeResolution(currentSubvolumeIndex));
        setCurrentSubVolume(0);
    }

    SubVolume::SubVolume(const Vec3ui& resolution)
        : volume(new Volume(resolution))
        , currentSubvolumeIndex(0)
        , ownsVolume(true)
        , resolution(resolution)
    {
        setCurrentSubVolume(0);
    }

    SubVolume::~SubVolume()
    {
        if(ownsVolume)
        {
            delete volume;
        }
        if(currentSubvolumeIndex == ARBITRARY_SUBVOLUME_SET)
            delete[](subvolumeData);
    }

    const Vec3ui& SubVolume::getResolution() const
    {
        return resolution;
    }

    size_t SubVolume::Size() const
    {
        return (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
    }

    std::size_t SubVolume::getByteWidth() const
    {
        return Size() * volume->Properties().GetVoxelByteWidth();
    }

    void* SubVolume::getData()
    {
        return subvolumeData;
    }

    const void* SubVolume::getData() const
    {
        return subvolumeData;
    }

    std::unique_ptr<float> SubVolume::ConvertToFloat()
    {
        float* convertedData = new float[ Size() ];

        if(volume->Properties().GetVoxelType() == VoxelType::HALF_FLOAT_16)
        {
            Float16Compressor::decompress((uint16_t*) subvolumeData, convertedData, Size());
        }
        else if(volume->Properties().GetVoxelType() == VoxelType::FLOAT_32)
        {
            std::memcpy((void*)convertedData, (void*)subvolumeData, getByteWidth());
        }
        else
            throw Exception("illegal voxel type");

        return std::unique_ptr<float>(convertedData);
    }

    void SubVolume::setArbitrarySubVolume(const Vec3i& origin, const Vec3ui& resolution, float outOfBoundsValue)
    {
        if(currentSubvolumeIndex == ARBITRARY_SUBVOLUME_SET)
            delete[](subvolumeData);

        currentSubvolumeIndex = ARBITRARY_SUBVOLUME_SET;
        this->resolution = resolution;
        this->origin = origin;

        size_t numberOfVoxelsInSubvolume = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
        subvolumeData = new unsigned char[numberOfVoxelsInSubvolume * volume->Properties().GetVoxelByteWidth()];

        Vec3i endCoordinate = origin + Vec3i(resolution.x, resolution.y, resolution.z);
        Vec3i volumeMax = Vec3i(volume->Properties().GetVolumeResolution().x, volume->Properties().GetVolumeResolution().y, volume->Properties().GetVolumeResolution().z);
        Vec3i effectiveEndCoordinate = Vec3i(std::min(endCoordinate.x, volumeMax.x), std::min(endCoordinate.y, volumeMax.y), std::min(endCoordinate.z, volumeMax.z));
        Vec3i effectiveOrigin = Vec3i(std::max(origin.x, 0), std::max(origin.y, 0), std::max(origin.z, 0));

        if(origin != effectiveOrigin || endCoordinate != effectiveEndCoordinate)
            volume->setVoxelToValue(subvolumeData, outOfBoundsValue, Size());

        copyToPrivateData(effectiveOrigin, effectiveEndCoordinate);
    }

    void SubVolume::copyPrivateDataToVolume()
    {
        if(currentSubvolumeIndex != ARBITRARY_SUBVOLUME_SET)
            return;

        Vec3i endCoordinate = origin + Vec3i(resolution.x, resolution.y, resolution.z);
        Vec3i volumeMax = Vec3i(volume->Properties().GetVolumeResolution().x, volume->Properties().GetVolumeResolution().y, volume->Properties().GetVolumeResolution().z);
        Vec3i effectiveEndCoordinate = Vec3i(std::min(endCoordinate.x, volumeMax.x), std::min(endCoordinate.y, volumeMax.y), std::min(endCoordinate.z, volumeMax.z));
        Vec3i effectiveOrigin = Vec3i(std::max(origin.x, 0), std::max(origin.y, 0), std::max(origin.z, 0));

        Vec3i positionOffset = effectiveOrigin - origin;
        Vec3i rectSize = effectiveEndCoordinate - effectiveOrigin;

        size_t resolutionX = (size_t)resolution.x;
        size_t resolutionY = (size_t)resolution.y;
        size_t resolutionZ = (size_t)resolution.z;
        size_t memLineSize = rectSize.x* volume->Properties().GetVoxelByteWidth();

        for(int z = 0; z < rectSize.z; ++z)
        {
            for(int y = 0; y < rectSize.y; ++y)
            {
                void* targetPtr = volume->getRawDataOfOfVoxel(Vec3ui(effectiveOrigin.x, y + effectiveOrigin.y, z + effectiveOrigin.z));
                size_t sourceIndex = (positionOffset.z + z) * resolutionX*resolutionY + (positionOffset.y + y) * resolutionX + (positionOffset.x);
                void* sourcePtr = &subvolumeData[sourceIndex * volume->Properties().GetVoxelByteWidth()];
                std::memcpy(targetPtr, sourcePtr, memLineSize);
            }
        }
    }

    void SubVolume::setCurrentSubVolume(unsigned int index)
    {
        if(index >= volume->Properties().GetSubVolumeCount())
            throw Exception("subvolume index out of bounds");

        if(currentSubvolumeIndex == ARBITRARY_SUBVOLUME_SET)
            delete[](subvolumeData);

        currentSubvolumeIndex = index;
        subvolumeData = (unsigned char*) volume->getRawDataOfSubvolume(currentSubvolumeIndex);
        resolution = volume->Properties().GetSubVolumeResolution(currentSubvolumeIndex);
    }

    unsigned int SubVolume::getCurrentSubVolume() const
    {
        return currentSubvolumeIndex;
    }

    Volume* SubVolume::getParentVolume() const
    {
        return volume;
    }

    void SubVolume::resetSubvolumePartitioning()
    {
        setCurrentSubVolume(0);
    }

    void SubVolume::copyToPrivateData(const Vec3i& fromCoord, const Vec3i& toCoord)
    {
        Vec3i positionOffset = fromCoord - origin;
        Vec3i rectSize = toCoord - fromCoord;

        size_t resolutionX = (size_t)resolution.x;
        size_t resolutionY = (size_t)resolution.y;
        size_t resolutionZ = (size_t)resolution.z;
        size_t memLineSize = rectSize.x* volume->Properties().GetVoxelByteWidth();

        for(int z = 0; z < rectSize.z; ++z)
        {
            for(int y = 0; y < rectSize.y; ++y)
            {
                void* sourcePtr = volume->getRawDataOfOfVoxel(Vec3ui(fromCoord.x, y + fromCoord.y, z + fromCoord.z));
                size_t targetIndex = (positionOffset.z + z) * resolutionX*resolutionY + (positionOffset.y + y) * resolutionX + (positionOffset.x);
                void* targetPtr = &subvolumeData[targetIndex * volume->Properties().GetVoxelByteWidth()];
                std::memcpy(targetPtr, sourcePtr, memLineSize);
            }
        }
    }

};