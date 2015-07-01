#pragma once
#include "framework/math/Vec3.h"
#include "model/image/Image.h"
#include "model/volume/VolumeProperties.h"
#include "model/volume/VoxelType.h"

namespace ettention
{
    class AlgebraicParameterSet;

    class Volume
    {
    public:
        Volume(const Vec3ui& resolution);
        Volume(const VolumeProperties& properties, float initValue);
        Volume(const VolumeProperties& properties, const float* initialData);
        Volume(VoxelType voxelType, AlgebraicParameterSet* parameterSet);
        ~Volume();
        
        const VolumeProperties& Properties() const;
        VolumeProperties& Properties();

        void* getRawDataOfSubvolume(unsigned int subVolumeIndex);
        void* getRawDataOfOfVoxel(Vec3ui voxel);
        void* getRawDataOfOfVoxel(size_t index);

        std::unique_ptr<float> ConvertToFloat();

        void setVoxelToValue(const Vec3ui& index, float value);
        void setVoxelToValue(void* addr, float value, size_t count);
        float getVoxelValue(const Vec3ui& index) const;
        float getVoxelValue(size_t index) const;

        void setZSlice(unsigned int z, Image* image);

        size_t getSubVolumeOffsetInBytes(unsigned int subVolumeIndex) const;
        size_t GetVoxelIndex(const Vec3ui& voxel) const;

    protected:
        void init(const float initialValue);
        void init(const float* data);

        VolumeProperties properties;
        unsigned char* data;
    };
};
