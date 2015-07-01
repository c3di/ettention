#include "stdafx.h"
#include "Volume.h"
#include "framework/error/Exception.h"
#include "framework/math/Float16Compressor.h"
#include "model/volume/VoxelType.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/HardwareParameterSet.h"

#define INVALID_VOXEL_INDEX ((size_t)-1)

namespace ettention
{

    Volume::Volume(const VolumeProperties& properties, float initValue)
        : properties(properties)
    {
        init(initValue);
    }

    Volume::Volume(const Vec3ui& resolution)
        : properties(VoxelType::FLOAT_32, VolumeParameterSet(resolution), 1)
    {      
        init(0.0f);
    }

    Volume::Volume(const VolumeProperties& properties, const float* initialData)
        : properties(properties)
    {
        init(initialData);
    }

    Volume::Volume(VoxelType voxelType, AlgebraicParameterSet* parameterSet)
        : properties(voxelType, *parameterSet->get<VolumeParameterSet>(), parameterSet->get<HardwareParameterSet>()->SubVolumeCount())
    {
        init(parameterSet->get<VolumeParameterSet>()->InitValue());
    }

    Volume::~Volume()
    {
        delete data;
    }

    const VolumeProperties& Volume::Properties() const
    {
        return properties;
    }

    VolumeProperties& Volume::Properties()
    {
        return properties;
    }

    void* Volume::getRawDataOfSubvolume(unsigned int subVolumeIndex)
    {
        return &data[getSubVolumeOffsetInBytes(subVolumeIndex)];
    }

    void* Volume::getRawDataOfOfVoxel(Vec3ui voxel)
    {
        return &data[GetVoxelIndex(voxel) * properties.GetVoxelByteWidth()];
    }

    void* Volume::getRawDataOfOfVoxel(size_t index)
    {
        return &data[index * properties.GetVoxelByteWidth()];
    }

    std::unique_ptr<float> Volume::ConvertToFloat()
    {
        float* convertedData = new float[properties.GetVolumeVoxelCount()];

        if(properties.GetVoxelType() == VoxelType::HALF_FLOAT_16)
        {
            Float16Compressor::decompress((uint16_t*)data, convertedData, properties.GetVolumeVoxelCount());
        }
        else if(properties.GetVoxelType() == VoxelType::FLOAT_32)
        {
            std::memcpy((void*)convertedData, (void*)data, properties.GetVolumeByteWidth());
        }
        else
            throw Exception("illegal voxel type");

        return std::unique_ptr<float>(convertedData);
    }

    void Volume::setVoxelToValue(const Vec3ui& voxel, float value)
    {
        auto index = this->GetVoxelIndex(voxel);

        void* voxelPtr = data + index * properties.GetVoxelByteWidth();
        setVoxelToValue(voxelPtr, value, 1);
    }

    void Volume::setVoxelToValue(void* addr, float value, size_t count)
    {
        for(size_t i = 0; i < count; i++)
        {
            if(properties.GetVoxelType() == VoxelType::HALF_FLOAT_16)
                ((uint16_t*)addr)[i] = Float16Compressor::compress(value);
            else if(properties.GetVoxelType() == VoxelType::FLOAT_32)
                ((float*)addr)[i] = value;
            else
                throw Exception("illegal voxel type");
        }
    }

    float Volume::getVoxelValue(const Vec3ui& voxel) const
    {
        size_t index = this->GetVoxelIndex(voxel);
        return getVoxelValue(index);
    }

    float Volume::getVoxelValue(size_t index) const
    {
        if(index == INVALID_VOXEL_INDEX)
            throw Exception("illegal voxel index");

        if(properties.GetVoxelType() == VoxelType::HALF_FLOAT_16)
            return Float16Compressor::decompress(((uint16_t*)data)[index]);
        else if(properties.GetVoxelType() == VoxelType::FLOAT_32)
            return ((float*)data)[index];
        throw Exception("illegal voxel type");
    }

    void Volume::setZSlice(unsigned int z, Image* image)
    {
        if(properties.GetVolumeResolution().xy() != image->getResolution())
            ETTENTION_THROW_EXCEPTION("Image resolution (" << image->getResolution() << " does not match volume z slice resolution (" << properties.GetVolumeResolution().xy() << ")!");

        size_t sizeOfSlice = image->getResolution().x * image->getResolution().y;
        void* sliceStart = getRawDataOfOfVoxel(Vec3ui(0, 0, z));

        if(properties.GetVoxelType() == VoxelType::HALF_FLOAT_16)
            Float16Compressor::compress(image->getData(), (uint16_t*)sliceStart, sizeOfSlice);
        else if(properties.GetVoxelType() == VoxelType::FLOAT_32)
            std::memcpy(sliceStart, (void*)image->getData(), sizeOfSlice * properties.GetVoxelByteWidth());
        else
            throw Exception("illegal voxel type");
    }

    void Volume::init(const float initialValue)
    {
        data = new unsigned char[properties.GetVolumeByteWidth()];

        if(properties.GetVoxelType() == VoxelType::HALF_FLOAT_16)
        {
            uint16_t convertedInitValue = Float16Compressor::compress(initialValue);
            uint16_t* dataPtr = (uint16_t*) data;
            for(size_t i = 0; i < properties.GetVolumeVoxelCount(); i++)
                dataPtr[i] = convertedInitValue;
        }
        else if(properties.GetVoxelType() == VoxelType::FLOAT_32)
        {
            float* floatDataPtr = (float*)data;
            for(size_t i = 0; i < properties.GetVolumeVoxelCount(); i++)
                floatDataPtr[i] = initialValue;
        }
        else
            throw Exception("illegal voxel type");
    }

    void Volume::init(const float* initialValue)
    {
        data = new unsigned char[properties.GetVolumeByteWidth()];

        if(properties.GetVoxelType() == VoxelType::HALF_FLOAT_16)
            Float16Compressor::compress(initialValue, (uint16_t*) data, properties.GetVolumeVoxelCount());
        else if(properties.GetVoxelType() == VoxelType::FLOAT_32)
            std::memcpy(data, initialValue, properties.GetVolumeByteWidth());
        else
            throw Exception("illegal voxel type");
    }

    size_t Volume::GetVoxelIndex(const Vec3ui& voxel) const
    {
        if(!(voxel < properties.GetVolumeResolution()))
            throw Exception("Volume Access Exception: index out of bounds");

        size_t resX = (size_t)properties.GetVolumeResolution().x;
        size_t resY = (size_t)properties.GetVolumeResolution().y;
        size_t x = (size_t)voxel.x;
        size_t y = (size_t)voxel.y;
        size_t z = (size_t)voxel.z;
        return z * resY * resX + y * resX + x;
    }

    size_t Volume::getSubVolumeOffsetInBytes(unsigned int subVolumeIndex) const
    {
        size_t offset = 0;
        for(unsigned int i = 0; i < subVolumeIndex; ++i)
        {
            offset += properties.GetSubVolumeVoxelCount(i) * properties.GetVoxelByteWidth();
        }
        return offset;
    }

};