#include "stdafx.h"
#include "ByteVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"

namespace ettention
{

    ByteVolume::ByteVolume(const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount /*= 1U*/, boost::optional<float> maximumInputFloatValue /*= 1.0f */)
        : Volume(Voxel::DataType::UCHAR_8, resolution, initValue, subVolumeCount)
        , maximumInputFloatValue(*maximumInputFloatValue)
    {
        Volume::init(initValue == boost::none ? 0.0f : *initValue);
    }

    ByteVolume::ByteVolume(const Vec3ui& resolution, const byte_t* initialData, unsigned int subVolumeCount /*= 1U*/, boost::optional<float> maximumInputFloatValue /*= 1.0f */)
        : Volume(Voxel::DataType::UCHAR_8, resolution, boost::none, subVolumeCount)
        , maximumInputFloatValue(*maximumInputFloatValue)
    {
        init(initialData);
    }

    ByteVolume::ByteVolume(VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount /*= 1U*/, boost::optional<float> maximumInputFloatValue /*= 1.0f */)
        : Volume(Voxel::DataType::UCHAR_8, volumeParameterSet, subVolumeCount)
        , maximumInputFloatValue(*maximumInputFloatValue)
    {
        Volume::init(volumeParameterSet->getInitValue());
    }

    ByteVolume::~ByteVolume()
    {
        delete data;
    }

    std::unique_ptr<float> ByteVolume::convertToFloat()
    {
        float* convertedData = new float[properties.getVolumeVoxelCount()];
        for( size_t i = 0; i < properties.getVolumeVoxelCount(); ++i )
        {
            convertedData[i] = convertValueToFloat(data[i]);
        }

        return std::unique_ptr<float>(convertedData);
    }

    float ByteVolume::convertValueToFloat(byte_t value) const
    {
        float result = static_cast<float>(value);
        return result;
    }

    byte_t ByteVolume::convertValueFromFloat(float value) const
    {
        byte_t result = static_cast<byte_t>(value);
        return result;
    }

    void ByteVolume::setZSlice(unsigned int z, Image* image)
    {
        if(properties.getVolumeResolution().xy() != image->getResolution())
            ETTENTION_THROW_EXCEPTION("Image resolution (" << image->getResolution() << " does not match volume z slice resolution (" << properties.getVolumeResolution().xy() << ")!");

        size_t sizeOfSlice = image->getResolution().x * image->getResolution().y;
        size_t indexOffset = calculateVoxelIndex(Vec3ui(0, 0, z));

        for( size_t i = 0; i < sizeOfSlice; i++ )
        {
            data[indexOffset + i] = convertValueFromFloat(image->getData()[i]);
        }
    }

    void ByteVolume::setVoxelToValue(const size_t index, float value)
    {
        data[index] = convertValueFromFloat(value);
    }

    float ByteVolume::getVoxelValue(size_t index) const
    {
        if( index == Volume::INVALID_VOXEL_INDEX )
            throw Exception("illegal voxel index");

        byte_t value = data[index];
        return convertValueToFloat(value);
    }

    void ByteVolume::allocateMemory()
    {
        data = new byte_t[properties.getVolumeVoxelCount()];
    }

    void ByteVolume::fillMemoryInNativeFormat(void* addr, float value, size_t count)
    {
        byte_t convertedInitValue = convertValueFromFloat(value);
        for( size_t i = 0; i < count; i++ )
        {
            ((byte_t*)addr)[i] = convertedInitValue;
        }
    }

    void* ByteVolume::getRawPointerStartingFrom(size_t offset /*= 0*/) const
    {
        return &data[offset];
    }

    void ByteVolume::init(const float* initialData)
    {
        allocateMemory();
        for( size_t i = 0; i < properties.getVolumeVoxelCount(); i++ )
        {
            data[i] = convertValueFromFloat(initialData[i]);
        }
    }

    void ByteVolume::init(const byte_t* initialData)
    {
        allocateMemory();
        std::memcpy(data, initialData, properties.getVolumeByteWidth());
    }

	void ByteVolume::setVoxelToByteValue(size_t index, byte_t value) const
	{
		data[index] = value;
	}

}