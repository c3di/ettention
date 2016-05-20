#pragma once
#include "model/volume/VolumeProperties.h"
#include "model/volume/Volume.h"
#include "model/volume/SubVolume.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class GPUMappedVolume
    {
    public:
        GPUMappedVolume(CLAbstractionLayer* clStack, Volume* volume, bool useImage = true);
        virtual ~GPUMappedVolume();

        void changeSubVolumeSizeToPartOfTotalMemory(float maxPercentageOfGPUMemory = 1.0f);
        void changeSubVolumeCountToFixedNumber(unsigned int subVolumeCount);
        unsigned int getCurrentSubvolumeIndex() const;
        void setCurrentSubvolumeIndex(unsigned int val);
        void setArbitrarySubVolume(const Vec3i& origin, const Vec3ui& resolution, float outOfBoundsValue = 0.0f);

        VolumeProperties& getProperties();

        void markAsChangedOnCPU();
        void markAsChangedOnGPU();
        void updateRegionOnGPU(const Vec3ui& baseCorner, const Vec3ui& resolution);

        void ensureIsUpToDateOnGPU();
        void ensureIsUpToDateOnCPU();

        void takeOwnershipOfObjectOnCPU();
        Volume* getObjectOnCPU();
        CLMemBuffer* getBufferOnGPU();
        GPUMapped<SubVolume>* getMappedSubVolume();

    protected:
        CLAbstractionLayer* clStack;
        Volume* volume;
        GPUMapped<SubVolume>* mappedSubVolume;
        SubVolume* subVolume;
        unsigned int currentSubvolumeIndex;
    };
};
