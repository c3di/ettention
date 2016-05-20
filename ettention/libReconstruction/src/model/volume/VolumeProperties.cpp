#include "stdafx.h"
#include "VolumeProperties.h"
#include "framework/Logger.h"
#include "framework/error/Exception.h"
#include "setup/parameterset/HardwareParameterSet.h"

namespace ettention
{
    VolumeProperties::VolumeProperties(Voxel::DataType voxelType, const VolumeParameterSet& volumeParameterSet, unsigned int subVolumeCount)
        : voxelType(voxelType)
        , resolution(volumeParameterSet.getResolution())
        , boundingBox(volumeParameterSet.getBoundingBox())
    {
        init(subVolumeCount);
    }

    VolumeProperties::~VolumeProperties()
    {
    }

    void VolumeProperties::init(unsigned int subVolumeCount)
    {
        voxelCount    = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
        sizeInBytes   = getVolumeVoxelCount() * getVoxelByteWidth();
        voxelSize     = boundingBox.getDimension() / (Vec3f)resolution;
        setSubVolumeCount(subVolumeCount == HardwareParameterSet::SUBVOLUME_COUNT_AUTO ? resolution.z : subVolumeCount);
    }

    Voxel::DataType VolumeProperties::getVoxelType() const
    {
        return voxelType;
    }

    size_t VolumeProperties::getVoxelByteWidth() const
    {
        return Voxel::sizeOfVoxelType(voxelType);
    }

    const Vec3f& VolumeProperties::getVoxelSize() const
    {
        return voxelSize;
    }

    const Vec3ui& VolumeProperties::getVolumeResolution() const
    {
        return resolution;
    }

    const BoundingBox3f& VolumeProperties::getRealSpaceBoundingBox() const
    {
        return boundingBox;
    }

	const BoundingBox3i VolumeProperties::getMemoryBoundingBox() const
	{
		return BoundingBox3i(Vec3i(0, 0, 0), resolution - Vec3i(1, 1, 1));
	}

	size_t VolumeProperties::getVolumeVoxelCount() const
    {
        return voxelCount;
    }

    size_t VolumeProperties::getVolumeByteWidth() const
    {
        return sizeInBytes;
    }

    unsigned int VolumeProperties::getSubVolumeCount() const
    {
        return (unsigned int)subVolumeResolutions.size();
    }

    void VolumeProperties::setSubVolumeCount(unsigned int count)
    {
        if(resolution.z < count)
        {
            ETTENTION_THROW_EXCEPTION("Volume of size " << resolution.z << " in z direction cannot be split into " << count << " subvolumes!");
        }
        unsigned int subVolumeResZ = resolution.z / count;
        unsigned int remaining = resolution.z - count * subVolumeResZ;
        subVolumeResolutions.clear();
        subVolumeBaseCoords.clear();
        subVolumeBoundingBoxes.clear();
        auto nextMin = this->getRealSpaceBoundingBox().getMin();
        auto nextBaseCoordZ = 0U;
        for(unsigned int i = 0; i < count; ++i)
        {
            Vec3ui subBaseCoord(0, 0, nextBaseCoordZ);
            Vec3ui subRes(resolution.xy(), i < remaining ? subVolumeResZ + 1 : subVolumeResZ);
            BoundingBox3f subBoundingBox(nextMin, nextMin + (Vec3f)subRes * this->getVoxelSize());
            nextMin.z = subBoundingBox.getMax().z;
            nextBaseCoordZ += subRes.z;
            subVolumeResolutions.push_back(subRes);
            subVolumeBaseCoords.push_back(subBaseCoord);
            subVolumeBoundingBoxes.push_back(subBoundingBox);
        }
    }

    const Vec3ui& VolumeProperties::getSubVolumeResolution(unsigned int index) const
    {
        if(index < this->getSubVolumeCount())
        {
            return subVolumeResolutions[index];
        }
        else
        {
            ETTENTION_THROW_EXCEPTION("Tried to get resolution of subvolume " << index << " while there are only " << this->getSubVolumeCount() << "!");
        }
    }

    const Vec3ui& VolumeProperties::getSubVolumeBaseCoords(unsigned int index) const
    {
        if(index < this->getSubVolumeCount())
        {
            return subVolumeBaseCoords[index];
        }
        else
        {
            ETTENTION_THROW_EXCEPTION("Tried to get resolution of subvolume " << index << " while there are only " << this->getSubVolumeCount() << "!");
        }
    }

    const BoundingBox3f& VolumeProperties::getSubVolumeBoundingBox(unsigned int index) const
    {
        if(index < this->getSubVolumeCount())
        {
            return subVolumeBoundingBoxes[index];
        }
        else
        {
            ETTENTION_THROW_EXCEPTION("Tried to get bounding box of subvolume " << index << " while there are only " << this->getSubVolumeCount() << "!");
        }
    }
    
    size_t VolumeProperties::getSubVolumeVoxelCount(unsigned int index) const
    {
        const Vec3ui& res = this->getSubVolumeResolution(index);
        return res.x * res.y * res.z;
    }

    size_t VolumeProperties::getSubVolumeByteWidth(unsigned int index) const
    {
        return this->getSubVolumeVoxelCount(index) * this->getVoxelByteWidth();
    }
}