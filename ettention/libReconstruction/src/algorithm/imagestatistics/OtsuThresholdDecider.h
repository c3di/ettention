#pragma once

#include "model/image/Image.h"
#include <vector>

namespace ettention
{
    class OtsuThresholdDecider
    {
    public:
        static float computeThreshold(Image *src, int numberOfIntervals);
        static int computeInterval(Image *src, int numberOfIntervals);
    };
}