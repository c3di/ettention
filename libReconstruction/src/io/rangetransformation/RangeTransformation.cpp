#include "stdafx.h"
#include "RangeTransformation.h"

namespace ettention
{
    RangeTransformation::RangeTransformation(Volume* volume)
    {
        statistics = VolumeStatistics::compute(volume);
    }

    VolumeStatistics RangeTransformation::getTransformedStatistics()
    {
        return VolumeStatistics(transformRange(statistics.getMin()), transformRange(statistics.getMax()), transformRange(statistics.getMean()));
    }
}