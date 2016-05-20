#include "stdafx.h"
#include "Volume.h"
#include "framework/error/Exception.h"
#include "framework/math/Float16Compressor.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"

namespace ettention
{
    Volume::Volume(Voxel::DataType voxelType, const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount)
        : properties(voxelType, VolumeParameterSet(resolution, initValue == boost::none ? 0.0f : *initValue), subVolumeCount)
    {
    }
    Volume::Volume(Voxel::DataType voxelType, VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount)
        : properties(voxelType, *volumeParameterSet, subVolumeCount)
    {
    }

    Volume::~Volume()
    {
    }

    const VolumeProperties& Volume::getProperties() const
    {
        return properties;
    }

    VolumeProperties& Volume::getProperties()
    {
        return properties;
    }

    void Volume::scaleVoxelValues(int numenator, int denominator /*= 1*/)
    {
        float scaledValue;
        for( size_t i = 0; i < properties.getVolumeVoxelCount(); ++i )
        {
            scaledValue = getVoxelValue(i) * numenator / denominator;
            setVoxelToValue(i, scaledValue);
        }
    }

    void* Volume::getRawPointerToSubvolume(unsigned int subVolumeIndex) const
    {
        return getRawPointerStartingFrom(getSubVolumeOffset(subVolumeIndex));
    }

    void* Volume::getRawPointerOfVoxel(const Vec3ui& voxel) const
    {
        return getRawPointerOfVoxel(getVoxelIndex(voxel));
    }

    void* Volume::getRawPointerOfVoxel(size_t index) const
    {
        return getRawPointerStartingFrom(index);
    }

    void Volume::setVoxelToValue(Vec3ui coords, float value)
    {
        size_t index = getVoxelIndex(coords);
        setVoxelToValue(index, value);
    }

    float Volume::getVoxelValue(const Vec3i& coords, BorderTreatment borderTreatment) const
    {
        return getVoxelValue(getVoxelIndex(coords, borderTreatment));
    }

    void Volume::setVolumeToValue(float value)
    {
        for( size_t i = 0; i < properties.getVolumeVoxelCount(); i++ )
        {
            setVoxelToValue(i, value);
        }
    }

	Image* Volume::extractSlice(unsigned int index, CoordinateOrder orientation /*= ORDER_XYZ*/) const
	{
		unsigned int orderDependentIndices[3];
		fillOrderDependentIndices(orderDependentIndices, orientation);
		Vec3ui outputResolution;
		outputResolution.z = properties.getVolumeResolution()[orderDependentIndices[0]];
		outputResolution.y = properties.getVolumeResolution()[orderDependentIndices[1]];
		outputResolution.x = properties.getVolumeResolution()[orderDependentIndices[2]];

		auto *result = new Image(outputResolution.xy());

		Vec3ui coords;
		coords[orderDependentIndices[0]] = index;
		for (unsigned int j = 0; j < result->getResolution().y; ++j)
		{
			for (unsigned int i = 0; i < result->getResolution().x; ++i)
			{
				coords[orderDependentIndices[1]] = j;
				coords[orderDependentIndices[2]] = i;
				result->setPixel(i, j, getVoxelValue(coords));
			}
		}

		return result;
	}

    size_t Volume::getVoxelIndex(const Vec3i& coords, BorderTreatment borderTreatment) const
    {
        Vec3i treatedCoords = coords;

		switch (borderTreatment)
        {
        case TREAT_EXCEPTION:
			if (!properties.getMemoryBoundingBox().isInside(coords))
			{
				std::stringstream ss;
				ss << "Volume coordinates (" << coords << ") out of bounds (" << properties.getVolumeResolution() << ")!";
				throw Exception( ss.str() );
			}
            break;
        case TREAT_CLAMP:
            treatedCoords = clamp(coords, Vec3i(0, 0, 0), (Vec3i)properties.getVolumeResolution());
            break;
        case TREAT_MIRROR:
            throw Exception("Mirror border treatment not yet implemented!");
            break;
        case TREAT_REPEAT:
            treatedCoords = Vec3i(coords.x % (int)properties.getVolumeResolution().x,
                                  coords.y % (int)properties.getVolumeResolution().y,
                                  coords.z % (int)properties.getVolumeResolution().z);
            break;
        default:
            throw Exception("Illegal border treatment given!");
        }
        return calculateVoxelIndex((Vec3ui)treatedCoords);
    }

    size_t Volume::computeDistanceBetweenVoxelsInMemory(const size_t indexOfFirst, const size_t indexOfSecond) const
    {
        return static_cast<unsigned char*>(getRawPointerOfVoxel(indexOfFirst)) - static_cast<unsigned char*>(getRawPointerOfVoxel(indexOfFirst));
    }

    void Volume::init(float initialValue)
    {
        allocateMemory();
        setVolumeToValue(initialValue);
    }

    size_t Volume::getVoxelIndex(const Vec3ui& coords) const
    {
        return this->getVoxelIndex((Vec3i)coords, TREAT_EXCEPTION);
    }

    size_t Volume::getSubVolumeOffset(unsigned int subVolumeIndex) const
    {
        size_t offset = 0;
        for(unsigned int i = 0; i < subVolumeIndex; ++i)
        {
            offset += properties.getSubVolumeVoxelCount(i);
        }
        return offset;
    }

    size_t Volume::getSubVolumeOffsetInBytes(unsigned int subVolumeIndex) const
    {
        size_t offset = 0;
        for( unsigned int i = 0; i < subVolumeIndex; ++i )
        {
            offset += properties.getSubVolumeVoxelCount(i) * properties.getVoxelByteWidth();
        }
        return offset;
    }
};