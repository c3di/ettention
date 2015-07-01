#pragma once

#include "model/volume/Volume.h"

namespace ettention
{
    class VolumeStatistics
    {
    public:
        VolumeStatistics();
        VolumeStatistics(float min, float max, float mean);

        static VolumeStatistics compute(Volume* volume);
        VolumeStatistics& operator=(const VolumeStatistics& other);

        float getMin();
        float getMax();
        float getMean();

    protected:
        float min, max, mean;
    };
}

