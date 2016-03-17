#include "stdafx.h"
#include "HalfFloatVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "framework/math/Float16Compressor.h"

namespace ettention
{

    HalfFloatVolume::HalfFloatVolume(const Vec3ui& resolution, boost::optional<float> initValue, boost::optional<unsigned int> subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::HALF_FLOAT_16, resolution, initValue, subVolumeCount)
    {
        init(initValue == boost::none ? 0.0f : *initValue);
    }

    HalfFloatVolume::HalfFloatVolume(const Vec3ui& resolution, const float* initialData, boost::optional<unsigned int> subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::HALF_FLOAT_16, resolution, boost::none, subVolumeCount)
    {
        init(initialData);
    }

    HalfFloatVolume::HalfFloatVolume(VolumeParameterSet* volumeParameterSet, boost::optional<unsigned int> subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::HALF_FLOAT_16, volumeParameterSet, subVolumeCount)
    {
        init(volumeParameterSet->getInitValue());
    }

    HalfFloatVolume::~HalfFloatVolume()
    {
        delete data;
    }

    std::unique_ptr<float> HalfFloatVolume::convertToFloat()
    {
        float* convertedData = new float[properties.getVolumeVoxelCount()];
        Float16Compressor::decompress(data, convertedData, properties.getVolumeVoxelCount());
        
        return std::unique_ptr<float>(convertedData);
    }

    void HalfFloatVolume::setZSlice(unsigned int z, Image* image)
    {
        if(properties.getVolumeResolution().xy() != image->getResolution())
            ETTENTION_THROW_EXCEPTION("Image resolution (" << image->getResolution() << " does not match volume z slice resolution (" << properties.getVolumeResolution().xy() << ")!");

        size_t sizeOfSlice = image->getResolution().x * image->getResolution().y;
        void* sliceStart = getRawPointerOfVoxel(Vec3ui(0, 0, z));

        Float16Compressor::compress(image->getData(), (half_float_t*)sliceStart, sizeOfSlice);
    }

    void HalfFloatVolume::setVoxelToValue(const size_t index, float value)
    {
        data[index] = Float16Compressor::compress(value);
    }

    void HalfFloatVolume::setVolumeToValue(float value)
    {
        half_float_t convertedInitValue = Float16Compressor::compress(value);
        for( size_t i = 0; i < properties.getVolumeVoxelCount(); i++ )
        {
            data[i] = convertedInitValue;
        }
    }

    float HalfFloatVolume::getVoxelValue(size_t index) const
    {
        if( index == INVALID_VOXEL_INDEX )
            throw Exception("illegal voxel index");

        return Float16Compressor::decompress(data[index]);
    }

    void HalfFloatVolume::fillMemoryInNativeFormat(void* addr, float value, size_t count)
    {
        for( size_t i = 0; i < count; i++ )
        {
            ((half_float_t*)addr)[i] = Float16Compressor::compress(value);
        }
    }

    void* HalfFloatVolume::getRawPointerStartingFrom(size_t offset /*= 0*/) const
    {
        return &data[offset];
    }

    void HalfFloatVolume::init(float initialValue)
    {
        data = new half_float_t[properties.getVolumeVoxelCount()];
        setVolumeToValue(initialValue);
    }

    void HalfFloatVolume::init(const float* initialData)
    {
        data = new half_float_t[properties.getVolumeVoxelCount()];
        Float16Compressor::compress(initialData, data, properties.getVolumeVoxelCount());
    }

}