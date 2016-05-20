#pragma once
#include "framework/math/Vec3.h"
#include "model/volume/Voxel.h"
#include "model/volume/Volume.h"
#include "model/volume/VolumeProperties.h"
#include "io/rangetransformation/RangeTransformation.h"

namespace ettention
{
    class AlgebraicParameterSet;

    class ByteVolume : public Volume
    {
    public:

        ByteVolume(const Vec3ui& resolution, boost::optional<float> initValue, unsigned int subVolumeCount = 1U, boost::optional<float> maximumInputFloatValue = 1.0f);
        ByteVolume(const Vec3ui& resolution, const byte_t* initialData, unsigned int subVolumeCount = 1U, boost::optional<float> maximumInputFloatValue = 1.0f);
        ByteVolume(VolumeParameterSet* volumeParameterSet, unsigned int subVolumeCount = 1U, boost::optional<float> maximumInputFloatValue = 1.0f);
        ~ByteVolume();

        std::unique_ptr<float> convertToFloat() override;

        void setZSlice(unsigned int z, Image* image) override;
        void setVoxelToValue(const size_t index, float value) override;
		float getVoxelValue(size_t index) const override;

		inline byte_t getVoxelValueAsByte(size_t index) const { return data[index]; }

		void setVoxelToByteValue(size_t index, byte_t value) const;

    protected:
        void allocateMemory() override;
        void fillMemoryInNativeFormat(void* addr, float value, size_t count) override;
        void* getRawPointerStartingFrom(size_t offset = 0) const override;

        void init(const float* initialData);
        void init(const byte_t* initialData);

    private:
        float convertValueToFloat(byte_t value) const;
        byte_t convertValueFromFloat(float value) const;

    private:
        byte_t *data;
        float maximumInputFloatValue;
    };
};
