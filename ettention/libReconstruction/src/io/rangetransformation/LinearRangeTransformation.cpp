#include "stdafx.h"
#include "LinearRangeTransformation.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"

namespace ettention
{
    LinearRangeTransformation::LinearRangeTransformation(Volume* volume, float outputRangeMinimum, float outputRangeMaximum)
        : RangeTransformation(volume)
        , outMin(outputRangeMinimum)
        , outMax(outputRangeMaximum)
    {
    }

    inline float LinearRangeTransformation::transformRange(float value)
    {
        const float valueZeroToMax = value - statistics.getMin();
        const float valueZeroToOne = valueZeroToMax / (statistics.getMax() - statistics.getMin());
        const float valueZeroToNewMax = valueZeroToOne * (outMax - outMin);
        const float valueNewMinToNewMax = valueZeroToNewMax + outMin;
        return valueNewMinToNewMax;
    }    

    void LinearRangeTransformation::transformRange(float* data, size_t size)
    {
        for(size_t i = 0; i < size; i++)
        {
            data[i] = transformRange(data[i]);
        }
    };
};
