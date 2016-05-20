#include "stdafx.h"
#include "FloatVolume.h"
#include "framework/error/Exception.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"

namespace ettention
{

    FloatVolume::FloatVolume(const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::FLOAT_32, resolution, initValue, subVolumeCount)
    {
        Volume::init(initValue == boost::none ? 0.0f : *initValue);
    }

    FloatVolume::FloatVolume(const Vec3ui& resolution, const float* initialData, unsigned int subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::FLOAT_32, resolution, boost::none, subVolumeCount)
    {
        init(initialData);
    }

    FloatVolume::FloatVolume(VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount /*= 1U*/)
        : Volume(Voxel::DataType::FLOAT_32, volumeParameterSet, subVolumeCount)
    {
        Volume::init(volumeParameterSet->getInitValue());
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

    float FloatVolume::getVoxelValue(size_t index) const
    {
        if( index == Volume::INVALID_VOXEL_INDEX )
            throw Exception("illegal voxel index");

        return data[index];
    }

	Image* FloatVolume::extractSlice(unsigned int index, CoordinateOrder orientation /*= ORDER_XYZ*/) const
    {
        if(orientation == ORDER_XYZ)
        {
            auto *result = new Image(properties.getVolumeResolution().xy());
            size_t sizeOfSlice = result->getResolution().x * result->getResolution().y;
            void* sliceStart = getRawPointerOfVoxel(Vec3ui(0, 0, index));
            std::memcpy((void*)result->getData(), sliceStart, sizeOfSlice * properties.getVoxelByteWidth());
            return result;
        } else {
            return Volume::extractSlice(index, orientation);
        }
    }

    void FloatVolume::allocateMemory()
    {
        data = new float[properties.getVolumeVoxelCount()];
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

    void FloatVolume::init(const float* initialData)
    {
        allocateMemory();
        std::memcpy(data, initialData, properties.getVolumeByteWidth());
    }

}