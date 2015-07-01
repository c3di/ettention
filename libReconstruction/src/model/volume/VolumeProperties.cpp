#include "stdafx.h"
#include "model/volume/VolumeProperties.h"
#include "framework/Logger.h"
#include "framework/error/Exception.h"

namespace ettention
{
    VolumeProperties::VolumeProperties(VoxelType voxelType, const VolumeParameterSet& volumeOptions, boost::optional<unsigned int> subVolumeCount)
        : voxelType(voxelType)
        , resolution(volumeOptions.Resolution())
        , boundingBox(volumeOptions.GetBoundingBox())
    {
        voxelSize = boundingBox.GetDimension() / (Vec3f)resolution;
        this->SetSubVolumeCount(subVolumeCount == boost::none ? resolution.z : *subVolumeCount);
    }

    VolumeProperties::~VolumeProperties()
    {

    }


    VoxelType VolumeProperties::GetVoxelType() const
    {
        return voxelType;
    }

    size_t VolumeProperties::GetVoxelByteWidth() const
    {
        return VoxelTypeInformation::sizeOfVoxelType(voxelType);
    }

    const Vec3f& VolumeProperties::GetVoxelSize() const
    {
        return voxelSize;
    }


    const Vec3ui& VolumeProperties::GetVolumeResolution() const
    {
        return resolution;
    }

    const BoundingBox3f& VolumeProperties::GetVolumeBoundingBox() const
    {
        return boundingBox;
    }

    size_t VolumeProperties::GetVolumeVoxelCount() const
    {
        return (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
    }

    size_t VolumeProperties::GetVolumeByteWidth() const
    {
        return this->GetVolumeVoxelCount() * this->GetVoxelByteWidth();
    }


    unsigned int VolumeProperties::GetSubVolumeCount() const
    {
        return (unsigned int)subVolumeResolutions.size();
    }

    void VolumeProperties::SetSubVolumeCount(unsigned int count)
    {
        if(resolution.z < count)
        {
            ETTENTION_THROW_EXCEPTION("Volume of size " << resolution.z << " in z direction cannot be split into " << count << " subvolumes!");
        }
        unsigned int subVolumeResZ = resolution.z / count;
        unsigned int remaining = resolution.z - count * subVolumeResZ;
        subVolumeResolutions.clear();
        subVolumeBoundingBoxes.clear();
        auto nextMin = this->GetVolumeBoundingBox().GetMin();
        for(unsigned int i = 0; i < count; ++i)
        {
            Vec3ui subRes(resolution.xy(), i < remaining ? subVolumeResZ + 1 : subVolumeResZ);
            BoundingBox3f subBoundingBox(nextMin, nextMin + (Vec3f)subRes * this->GetVoxelSize());
            nextMin.z = subBoundingBox.GetMax().z;
            subVolumeResolutions.push_back(subRes);
            subVolumeBoundingBoxes.push_back(subBoundingBox);
        }
    }


    const Vec3ui& VolumeProperties::GetSubVolumeResolution(unsigned int index) const
    {
        if(index < this->GetSubVolumeCount())
        {
            return subVolumeResolutions[index];
        }
        else
        {
            ETTENTION_THROW_EXCEPTION("Tried to get resolution of subvolume " << index << " while there are only " << this->GetSubVolumeCount() << "!");
        }
    }

    const BoundingBox3f& VolumeProperties::GetSubVolumeBoundingBox(unsigned int index) const
    {
        if(index < this->GetSubVolumeCount())
        {
            return subVolumeBoundingBoxes[index];
        }
        else
        {
            ETTENTION_THROW_EXCEPTION("Tried to get bounding box of subvolume " << index << " while there are only " << this->GetSubVolumeCount() << "!");
        }
    }
    
    size_t VolumeProperties::GetSubVolumeVoxelCount(unsigned int index) const
    {
        const Vec3ui& res = this->GetSubVolumeResolution(index);
        return res.x * res.y * res.z;
    }

    size_t VolumeProperties::GetSubVolumeByteWidth(unsigned int index) const
    {
        return this->GetSubVolumeVoxelCount(index) * this->GetVoxelByteWidth();
    }
}
