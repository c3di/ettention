#pragma once

#include <vector>
#include <boost/optional.hpp>
#include "framework/geometry/BoundingBox.h"
#include "framework/math/Vec3.h"
#include "model/volume/Voxel.h"
#include "setup/parameterset/VolumeParameterSet.h"

namespace ettention
{
    class VolumeProperties
    {
    public:
        VolumeProperties(Voxel::DataType voxelType, const VolumeParameterSet& volumeParameterSet, unsigned int subVolumeCount);
        ~VolumeProperties();

        void init(unsigned int subVolumeCount);

        Voxel::DataType getVoxelType() const;
        size_t getVoxelByteWidth() const;
        const Vec3f& getVoxelSize() const;

        const Vec3ui& getVolumeResolution() const;
        const BoundingBox3f& getRealSpaceBoundingBox() const;
		const BoundingBox3i getMemoryBoundingBox() const;
        size_t getVolumeVoxelCount() const;
        size_t getVolumeByteWidth() const;
        
        unsigned int getSubVolumeCount() const;
        void setSubVolumeCount(unsigned int count);

        const Vec3ui& getSubVolumeResolution(unsigned int index) const;
        const Vec3ui& getSubVolumeBaseCoords(unsigned int index) const;
        const BoundingBox3f& getSubVolumeBoundingBox(unsigned int index) const;
        size_t getSubVolumeVoxelCount(unsigned int index) const;
        size_t getSubVolumeByteWidth(unsigned int index) const;

    private:
        Voxel::DataType voxelType;
        Vec3f voxelSize;
        Vec3ui resolution;
        size_t voxelCount;
        size_t sizeInBytes;
        BoundingBox3f boundingBox;
        std::vector<Vec3ui> subVolumeResolutions;
        std::vector<Vec3ui> subVolumeBaseCoords;
        std::vector<BoundingBox3f> subVolumeBoundingBoxes;
    };
}
