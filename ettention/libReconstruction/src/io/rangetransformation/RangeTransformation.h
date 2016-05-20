#pragma once
#include "algorithm/volumestatistics/VolumeStatistics.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class RangeTransformation
    {
    public:
        RangeTransformation(Volume* volume);

        virtual float transformRange(float value) = 0;
        virtual void transformRange(float* data, size_t size) = 0;

        VolumeStatistics getTransformedStatistics();

    protected:
        VolumeStatistics statistics;
    };
}
