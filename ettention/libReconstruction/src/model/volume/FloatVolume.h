#pragma once
#include "framework/math/Vec3.h"
#include "model/volume/Volume.h"
#include "model/volume/VolumeProperties.h"
#include "model/volume/Voxel.h"

namespace ettention
{
    class AlgebraicParameterSet;

    class FloatVolume : public Volume
    {
    public:

        FloatVolume(const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount = 1U);
        FloatVolume(const Vec3ui& resolution, const float* initialData, unsigned int subVolumeCount = 1U);
        FloatVolume(VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount = 1U);
        ~FloatVolume();

        std::unique_ptr<float> convertToFloat() override;

        void setZSlice(unsigned int z, Image* image) override;
        void setVoxelToValue(const size_t index, float value) override;
        float getVoxelValue(size_t index) const override;

		inline float getVoxelValueAsFloat(size_t index) const { return data[index]; }

        Image* extractSlice(unsigned int index, CoordinateOrder orientation = ORDER_XYZ) const override;

    protected:
        void allocateMemory() override;
        void fillMemoryInNativeFormat(void* addr, float value, size_t count) override;
        void* getRawPointerStartingFrom(size_t offset = 0) const override;

        void init(const float* initialData);

    private:
        float *data;
    };
};
