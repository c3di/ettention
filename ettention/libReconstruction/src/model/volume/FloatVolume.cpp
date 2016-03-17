#include "stdafx.h"
#include "FloatVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"

namespace ettention
{

    FloatVolume::FloatVolume(const Vec3ui& resolution, boost::optional<float> initValue, boost::optional<unsigned int> subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::FLOAT_32, resolution, initValue, subVolumeCount)
    {
        init(initValue == boost::none ? 0.0f : *initValue);
    }

    FloatVolume::FloatVolume(const Vec3ui& resolution, const float* initialData, boost::optional<unsigned int> subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::FLOAT_32, resolution, boost::none, subVolumeCount)
    {
        init(initialData);
    }

    FloatVolume::FloatVolume(VolumeParameterSet* volumeParameterSet, boost::optional<unsigned int> subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::FLOAT_32, volumeParameterSet, subVolumeCount)
    {
        init(volumeParameterSet->getInitValue());
    }

    FloatVolume::~FloatVolume()
    {
        delete data;
    }

    std::unique_ptr<float> FloatVolume::convertToFloat()
    {
        float* convertedData = new float[properties.getVolumeVoxelCount()];
        std::memcpy((void*)convertedData, (const void*)data, properties.getVolumeByteWidth());
        
        return std::unique_ptr<float>(convertedData);
    }

    void FloatVolume::setZSlice(unsigned int z, Image* image)
    {
        if(properties.getVolumeResolution().xy() != image->getResolution())
            ETTENTION_THROW_EXCEPTION("Image resolution (" << image->getResolution() << " does not match volume z slice resolution (" << properties.getVolumeResolution().xy() << ")!");

        size_t sizeOfSlice = image->getResolution().x * image->getResolution().y;
        void* sliceStart = getRawPointerOfVoxel(Vec3ui(0, 0, z));

        std::memcpy(sliceStart, (void*)image->getData(), sizeOfSlice * properties.getVoxelByteWidth());
    }

    void FloatVolume::setVoxelToValue(const size_t index, float value)
    {
        data[index] = value;
    }

    void FloatVolume::setVolumeToValue(float value)
    {
        for( size_t i = 0; i < properties.getVolumeVoxelCount(); i++ )
        {
            setVoxelToValue(i, value);
        }
    }

    float FloatVolume::getVoxelValue(size_t index) const
    {
        if( index == INVALID_VOXEL_INDEX )
            throw Exception("illegal voxel index");

        return data[index];
    }

    void FloatVolume::fillMemoryInNativeFormat(void* addr, float value, size_t count)
    {
        for( size_t i = 0; i < count; i++ )
        {
            ((float*)addr)[i] = value;
        }
    }

    void* FloatVolume::getRawPointerStartingFrom(size_t offset /*= 0*/) const
    {
        return &data[offset];
    }

    void FloatVolume::init(float initialValue)
    {
        data = new float[properties.getVolumeVoxelCount()];
        setVolumeToValue(initialValue);
    }

    void FloatVolume::init(const float* initialData)
    {
        data = new float[properties.getVolumeVoxelCount()];
        std::memcpy(data, initialData, properties.getVolumeByteWidth());
    }

}