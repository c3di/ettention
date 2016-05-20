#pragma once
#include "framework/math/Vec3.h"
#include "model/image/Image.h"
#include "model/volume/VolumeProperties.h"
#include "model/volume/Voxel.h"

namespace ettention
{
    class AlgebraicParameterSet;
    class GPUMappedVolume;
    class SubVolume;

    class Volume
    {
        friend class GPUMappedVolume;
        friend class SubVolume;

    protected:
        Volume(Voxel::DataType voxelType, const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount);
        Volume(Voxel::DataType voxelType, VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount);

        static const size_t INVALID_VOXEL_INDEX = (size_t)-1;

    public:
        enum BorderTreatment
        {
            TREAT_REPEAT,
            TREAT_MIRROR,
            TREAT_CLAMP,
            TREAT_EXCEPTION,
        };

        virtual ~Volume() = 0;

        virtual std::unique_ptr<float> convertToFloat() = 0;
        virtual void setZSlice(unsigned int z, Image* image) = 0;
        virtual void setVoxelToValue(const size_t index, float value) = 0;
        virtual float getVoxelValue(size_t index) const = 0;

        virtual void setVolumeToValue(float value);

        const VolumeProperties& getProperties() const;
        VolumeProperties& getProperties();

        void scaleVoxelValues(int numenator, int denominator = 1);
        virtual void setVoxelToValue(Vec3ui coords, float value);
        virtual float getVoxelValue(const Vec3i& coords, Volume::BorderTreatment borderTreatment = TREAT_EXCEPTION) const;
        virtual Image* extractSlice(unsigned int index, CoordinateOrder orientation = ORDER_XYZ) const;

        virtual size_t getSubVolumeOffset(unsigned int subVolumeIndex) const;
        virtual size_t getSubVolumeOffsetInBytes(unsigned int subVolumeIndex) const;
        virtual size_t getVoxelIndex(const Vec3i& coords, BorderTreatment borderTreatment = TREAT_EXCEPTION) const;
        virtual size_t getVoxelIndex(const Vec3ui& coords) const;

        virtual size_t computeDistanceBetweenVoxelsInMemory(const size_t indexOfFirst, const size_t indexOfSecond) const;
        inline std::size_t calculateVoxelIndex(const Vec3ui& coords) const
        {
            const size_t resX = (std::size_t)properties.getVolumeResolution().x;
            const size_t resY = (std::size_t)properties.getVolumeResolution().y;
            const size_t resZ = (std::size_t)properties.getVolumeResolution().z;
            const size_t x = (std::size_t)coords.x;
            const size_t y = (std::size_t)coords.y;
            const size_t z = (std::size_t)coords.z;
            return z * resY * resX + y * resX + x;
        }

    protected:
        virtual void allocateMemory() = 0;
        virtual void* getRawPointerStartingFrom(size_t offset = 0) const = 0;
        virtual void fillMemoryInNativeFormat(void* addr, float value, size_t count) = 0;

        virtual void init(float initialValue);

        virtual void* getRawPointerToSubvolume(unsigned int subVolumeIndex) const;
        virtual void* getRawPointerOfVoxel(const Vec3ui& voxel) const;
        virtual void* getRawPointerOfVoxel(size_t index) const;

    protected:
        VolumeProperties properties;
    };
};