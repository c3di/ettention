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

        FloatVolume(const Vec3ui& resolution, boost::optional<float> initValue, boost::optional<unsigned int> subVolumeCount = 1U);
        FloatVolume(const Vec3ui& resolution, const float* initialData, boost::optional<unsigned int> subVolumeCount = 1U);
        FloatVolume(VolumeParameterSet* volumeParameterSet, boost::optional<unsigned int> subVolumeCount = 1U);
        ~FloatVolume();

        std::unique_ptr<float> convertToFloat() override;
        void setZSlice(unsigned int z, Image* image) override;

        void setVoxelToValue(const size_t index, float value) override;
        void setVolumeToValue(float value) override;

        float getVoxelValue(size_t index) const override;

    protected:
        void fillMemoryInNativeFormat(void* addr, float value, size_t count) override;
        void* getRawPointerStartingFrom(size_t offset = 0) const override;

        void init(float initialValue) override;
        void init(const float* initialData) override;

    private:
        float *data;
    };
};
