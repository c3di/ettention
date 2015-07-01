#include "stdafx.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"

namespace ettention
{
    VolumeStatistics::VolumeStatistics()
        : min(std::numeric_limits<float>::quiet_NaN())
        , max(std::numeric_limits<float>::quiet_NaN())
        , mean(std::numeric_limits<float>::quiet_NaN())
    {
    }

    VolumeStatistics::VolumeStatistics(float min, float max, float mean)
        : min(min), max(max), mean(mean)
    {
    }

    VolumeStatistics VolumeStatistics::compute(Volume* volume)
    {
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        float mean = 0.0f;

        VolumeStatistics stat(min, max, mean);

        size_t test = volume->Properties().GetVolumeVoxelCount();

        for(size_t i = 0; i < volume->Properties().GetVolumeVoxelCount(); i++)
        {
            float voxelValue = volume->getVoxelValue(i);

            if(voxelValue < min)
                min = voxelValue;

            if(voxelValue > max)
                max = voxelValue;

            mean += voxelValue;
        }

        float scalingFactor = (1 / (float)volume->Properties().GetVolumeVoxelCount());

        mean *= scalingFactor;

        stat.min = min;
        stat.max = max;
        stat.mean = mean;

        return stat;
    }

    VolumeStatistics& VolumeStatistics::operator=(const VolumeStatistics& other)
    {
        min = other.min;
        max = other.max;
        mean = other.mean;
        return *this;
    }

    float VolumeStatistics::getMin()
    {
        return this->min;
    }

    float VolumeStatistics::getMax()
    {
        return this->max;
    }

    float VolumeStatistics::getMean()
    {
        return this->mean;
    }
}