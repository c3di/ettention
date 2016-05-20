#include "stdafx.h"
#include "SubVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/Voxel.h"
#include "framework/math/Float16Compressor.h"

namespace ettention
{ 
    SubVolume::SubVolume(Volume* volume)
        : volume(volume)
        , currentSubvolumeIndex(0)
        , ownsVolume(false)
    {
        resolution = Vec3ui(volume->getProperties().getSubVolumeResolution(currentSubvolumeIndex));
        setCurrentSubVolume(0);
    }

    SubVolume::SubVolume(const Vec3ui& resolution)
        : volume(nullptr)
        , currentSubvolumeIndex(0)
        , ownsVolume(true)
        , resolution(resolution)
    {
        throw Exception("SubVolume is not supposed to be used without Volume.");
    }

    SubVolume::~SubVolume()
    {
        if(ownsVolume)
        {
            delete volume;
        }
        if(currentSubvolumeIndex == ARBITRARY_SUBVOLUME_SET)
        {
            delete[] subvolumeData;
        }
    }

    const Vec3ui& SubVolume::getResolution() const
    {
        return resolution;
    }

    const Vec3i& SubVolume::getBaseCoords() const
    {
        return baseCoords;
    }

    size_t SubVolume::size() const
    {
        return (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
    }

    std::size_t SubVolume::getByteWidth() const
    {
        return size() * volume->getProperties().getVoxelByteWidth();
    }

    void SubVolume::fillMemory(float value)
    {
        volume->fillMemoryInNativeFormat(getData(), value, size());
    }

    void* SubVolume::getData()
    {
        return subvolumeData;
    }

    const void* SubVolume::getData() const
    {
        return subvolumeData;
    }

    std::unique_ptr<float> SubVolume::convertToFloat()
    {
        float* convertedData = new float[ size() ];

        if(volume->getProperties().getVoxelType() == Voxel::DataType::HALF_FLOAT_16)
        {
            Float16Compressor::decompress((uint16_t*) subvolumeData, convertedData, size());
        }
        else if(volume->getProperties().getVoxelType() == Voxel::DataType::FLOAT_32)
        {
            std::memcpy((void*)convertedData, (void*)subvolumeData, getByteWidth());
        }
        else
            throw Exception("illegal voxel type");

        return std::unique_ptr<float>(convertedData);
    }

    void SubVolume::setArbitrarySubVolume(const Vec3i& baseCoords, const Vec3ui& resolution, float outOfBoundsValue)
    {
        if(currentSubvolumeIndex == ARBITRARY_SUBVOLUME_SET)
            delete[](subvolumeData);

        currentSubvolumeIndex = ARBITRARY_SUBVOLUME_SET;
        this->resolution = resolution;
        this->baseCoords = baseCoords;

        size_t numberOfVoxelsInSubvolume = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
        subvolumeData = new unsigned char[numberOfVoxelsInSubvolume * volume->getProperties().getVoxelByteWidth()];

        Vec3i endCoordinate = baseCoords + (Vec3i)resolution;
        Vec3i volumeMax = (Vec3i)volume->getProperties().getVolumeResolution();
        Vec3i effectiveEndCoordinate = componentWiseMin(endCoordinate, volumeMax);
        Vec3i effectiveBaseCoords = componentWiseMax(baseCoords, Vec3i(0, 0, 0));

        if(baseCoords != effectiveBaseCoords || endCoordinate != effectiveEndCoordinate)
            volume->fillMemoryInNativeFormat(subvolumeData, outOfBoundsValue, size());

        copyToPrivateData(effectiveBaseCoords, effectiveEndCoordinate);
    }

    void SubVolume::copyPrivateDataToVolume()
    {
        if(currentSubvolumeIndex != ARBITRARY_SUBVOLUME_SET)
            return;

        Vec3i endCoordinate = baseCoords + (Vec3i)resolution;
        Vec3i volumeMax = (Vec3i)volume->getProperties().getVolumeResolution();
        Vec3i effectiveEndCoordinate = componentWiseMin(endCoordinate, volumeMax);
        Vec3i effectiveBaseCoords = componentWiseMax(baseCoords, Vec3i(0, 0, 0));

        Vec3i positionOffset = effectiveBaseCoords - baseCoords;
        Vec3i rectSize = effectiveEndCoordinate - effectiveBaseCoords;

        size_t resolutionX = (size_t)resolution.x;
        size_t resolutionY = (size_t)resolution.y;
        size_t resolutionZ = (size_t)resolution.z;
        size_t lineByteWidth = rectSize.x * volume->getProperties().getVoxelByteWidth();

        for(int z = 0; z < rectSize.z; ++z)
        {
            for(int y = 0; y < rectSize.y; ++y)
            {
                void* targetPtr = volume->getRawPointerOfVoxel(effectiveBaseCoords + Vec3i(0, y, z));
                size_t sourceIndex = (positionOffset.z + z) * resolutionY * resolutionX + (positionOffset.y + y) * resolutionX + positionOffset.x;
                void* sourcePtr = &subvolumeData[sourceIndex * volume->getProperties().getVoxelByteWidth()];
                std::memcpy(targetPtr, sourcePtr, lineByteWidth);
            }
        }
    }

    void SubVolume::takeOwnershipOfVolume()
    {
        ownsVolume = true;
    }

    void SubVolume::setCurrentSubVolume(unsigned int index)
    {
        if(index >= volume->getProperties().getSubVolumeCount())
            throw Exception("subvolume index out of bounds");

        if(currentSubvolumeIndex == ARBITRARY_SUBVOLUME_SET)
            delete[](subvolumeData);

        currentSubvolumeIndex = index;
        subvolumeData = (unsigned char*)volume->getRawPointerToSubvolume(currentSubvolumeIndex);
        resolution = volume->getProperties().getSubVolumeResolution(currentSubvolumeIndex);
        baseCoords = (Vec3i)volume->getProperties().getSubVolumeBaseCoords(currentSubvolumeIndex);
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
        Vec3i positionOffset = fromCoord - baseCoords;
        Vec3i rectSize = toCoord - fromCoord;

        size_t resolutionX = (size_t)resolution.x;
        size_t resolutionY = (size_t)resolution.y;
        size_t resolutionZ = (size_t)resolution.z;
        size_t memLineSize = rectSize.x * volume->getProperties().getVoxelByteWidth();

        for(int z = 0; z < rectSize.z; ++z)
        {
            for(int y = 0; y < rectSize.y; ++y)
            {
                void* sourcePtr = volume->getRawPointerOfVoxel(Vec3ui(fromCoord.x, y + fromCoord.y, z + fromCoord.z));
                size_t targetIndex = (positionOffset.z + z) * resolutionX*resolutionY + (positionOffset.y + y) * resolutionX + (positionOffset.x);
                void* targetPtr = &subvolumeData[targetIndex * volume->getProperties().getVoxelByteWidth()];
                std::memcpy(targetPtr, sourcePtr, memLineSize);
            }
        }
    }

};