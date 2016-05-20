#pragma once

#include "model/volume/Volume.h"
#include "model/volume/VolumeProperties.h"

namespace ettention
{
    #define ARBITRARY_SUBVOLUME_SET (unsigned int) -1

    class SubVolume
    {
    public:
        typedef Vec3ui ResolutionType;

        SubVolume(Volume* volume);
        SubVolume(const Vec3ui& resolution);
        ~SubVolume();

        const Vec3ui& getResolution() const;
        const Vec3i& getBaseCoords() const;
        size_t size() const;
        std::size_t getByteWidth() const;

        void fillMemory(float value);
        void* getData();
        const void* getData() const;
        std::unique_ptr<float> convertToFloat();

        void setArbitrarySubVolume(const Vec3i& baseCoords, const Vec3ui& resolution, float outOfBoundsValue = 0.0f);
        void setCurrentSubVolume(unsigned int index);

        Volume* getParentVolume() const;

        void resetSubvolumePartitioning();
        unsigned int getCurrentSubVolume() const;

        void copyToPrivateData(const Vec3i& fromCoord, const Vec3i& toCoord);
        void copyPrivateDataToVolume();

        void takeOwnershipOfVolume();

    protected:
        Volume* volume;
        bool ownsVolume;
        unsigned int currentSubvolumeIndex;
        unsigned char* subvolumeData;
        Vec3ui resolution;
        Vec3i baseCoords;

    };
};
