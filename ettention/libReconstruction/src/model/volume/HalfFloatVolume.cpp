#include "stdafx.h"
#include "HalfFloatVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "framework/math/Float16Compressor.h"

namespace ettention
{

    HalfFloatVolume::HalfFloatVolume(const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::HALF_FLOAT_16, resolution, initValue, subVolumeCount)
    {
        Volume::init(initValue == boost::none ? 0.0f : *initValue);
    }

    HalfFloatVolume::HalfFloatVolume(const Vec3ui& resolution, const float* initialData, unsigned int subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::HALF_FLOAT_16, resolution, boost::none, subVolumeCount)
    {
        init(initialData);
    }

    HalfFloatVolume::HalfFloatVolume(VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::HALF_FLOAT_16, volumeParameterSet, subVolumeCount)
    {
        Volume::init(volumeParameterSet->getInitValue());
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

    float HalfFloatVolume::getVoxelValue(size_t index) const
    {
        if( index == Volume::INVALID_VOXEL_INDEX )
            throw Exception("illegal voxel index");

        return Float16Compressor::decompress(data[index]);
    }

    void HalfFloatVolume::allocateMemory()
    {
        data = new half_float_t[properties.getVolumeVoxelCount()];
    }

    void HalfFloatVolume::fillMemoryInNativeFormat(void* addr, float value, size_t count)
    {
        auto halfValue = Float16Compressor::compress(value);
        for( size_t i = 0; i < count; i++ )
        {
            ((half_float_t*)addr)[i] = halfValue;
        }
    }

    void* HalfFloatVolume::getRawPointerStartingFrom(size_t offset /*= 0*/) const
    {
        return &data[offset];
    }

    void HalfFloatVolume::init(const float* initialData)
    {
        allocateMemory();
        Float16Compressor::compress(initialData, data, properties.getVolumeVoxelCount());
    }

}