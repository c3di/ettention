#pragma once

#include <vector>
#include <boost/optional.hpp>
#include "framework/geometry/BoundingBox.h"
#include "framework/math/Vec3.h"
#include "model/volume/VoxelType.h"
#include "setup/ParameterSet/VolumeParameterSet.h"

namespace ettention
{
    class VolumeProperties
    {
    public:
        VolumeProperties(VoxelType voxelType, const VolumeParameterSet& volumeOptions, boost::optional<unsigned int> subVolumeCount);
        ~VolumeProperties();

        VoxelType GetVoxelType() const;
        size_t GetVoxelByteWidth() const;
        const Vec3f& GetVoxelSize() const;

        const Vec3ui& GetVolumeResolution() const;
        const BoundingBox3f& GetVolumeBoundingBox() const;
        size_t GetVolumeVoxelCount() const;
        size_t GetVolumeByteWidth() const;
        
        unsigned int GetSubVolumeCount() const;
        void SetSubVolumeCount(unsigned int count);

        const Vec3ui& GetSubVolumeResolution(unsigned int index) const;
        const BoundingBox3f& GetSubVolumeBoundingBox(unsigned int index) const;
        size_t GetSubVolumeVoxelCount(unsigned int index) const;
        size_t GetSubVolumeByteWidth(unsigned int index) const;

    private:
        VoxelType voxelType;
        Vec3f voxelSize;
        Vec3ui resolution;
        BoundingBox3f boundingBox;
        std::vector<Vec3ui> subVolumeResolutions;
        std::vector<BoundingBox3f> subVolumeBoundingBoxes;
    };
}
