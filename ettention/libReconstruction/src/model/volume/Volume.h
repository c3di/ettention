#pragma once
#include "framework/math/Vec3.h"
#include "model/image/Image.h"
#include "model/volume/VolumeProperties.h"
#include "model/volume/Voxel.h"

#define INVALID_VOXEL_INDEX ((size_t)-1)

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
        Volume(Voxel::DataType voxelType, const Vec3ui& resolution, boost::optional<float> initValue, boost::optional<unsigned int> subVolumeCount);
        Volume(Voxel::DataType voxelType, VolumeParameterSet* volumeParameterSet, boost::optional<unsigned int> subVolumeCount);

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
        virtual void setVolumeToValue(float value) = 0;
        virtual float getVoxelValue(size_t index) const = 0;
        
        const VolumeProperties& getProperties() const;
        VolumeProperties& getProperties();

        void scaleVoxelValues(int numenator, int denominator = 1);
        virtual void setVoxelToValue(Vec3ui coords, float value);
        virtual float getVoxelValue(const Vec3i& coords, Volume::BorderTreatment borderTreatment = TREAT_EXCEPTION) const;
        virtual float getVoxelValue(const Vec3ui& coords) const;

        virtual size_t getSubVolumeOffset(unsigned int subVolumeIndex) const;
        virtual size_t getSubVolumeOffsetInBytes(unsigned int subVolumeIndex) const;
        virtual size_t getVoxelIndex(const Vec3i& coords, BorderTreatment borderTreatment = TREAT_EXCEPTION) const;
        virtual size_t getVoxelIndex(const Vec3ui& coords) const;

        virtual size_t computeDistanceBetweenVoxelsInMemory(const size_t indexOfFirst, const size_t indexOfSecond) const;

    protected:
        virtual void* getRawPointerStartingFrom(size_t offset = 0) const = 0;
        virtual void fillMemoryInNativeFormat(void* addr, float value, size_t count) = 0;

        std::size_t calculateVoxelIndex(const Vec3ui& coords) const;

        virtual void init(float initialValue) = 0;
        virtual void init(const float* initialData) = 0;

        virtual void* getRawPointerToSubvolume(unsigned int subVolumeIndex) const;
        virtual void* getRawPointerOfVoxel(const Vec3ui& voxel) const;
        virtual void* getRawPointerOfVoxel(size_t index) const;

    protected:
        VolumeProperties properties;
    };
};