#include "stdafx.h"
#include <cfloat>
#include "algorithm/imagestatistics/OtsuThresholdDecider.h"
#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    float OtsuThresholdDecider::computeThreshold(Image *src, int numberOfIntervals)
    {
        float result = 0.0f;

        float min = src->findMinValue();
        float max = src->findMaxValue();
        float step = (max - min) / (numberOfIntervals + 1);

        int interval = computeInterval(src, numberOfIntervals);

        result = (interval + 1) * step;
        return result;
    }

    int OtsuThresholdDecider::computeInterval(Image *src, int numberOfIntervals)
    {
        std::vector<int> counter;
        std::vector<float> sumer;
        ThresholdDecider::computeHistogram(src, numberOfIntervals, counter, sumer);

        int totalCount = 0;
        float totalSum = 0.0f;
        for(int i = 0; i < numberOfIntervals; ++i)
        {
            totalCount += counter.at(i);
            totalSum += sumer.at(i);
        }

        float w;
        float a;
        float sigma;
        float maxSigma = FLT_MIN;
        int interval = -1;

        float tempCount = 0.0f;
        float tempSum = 0.0f;
        for(int i = 0; i < numberOfIntervals; ++i)
        {
            tempCount += (float)counter.at(i);
            tempSum += sumer.at(i);

            w = tempCount / tempCount;
            a = (tempSum / tempCount) - (totalSum - tempSum) / ((float)totalCount - tempCount);
            sigma = w * (1 - w) * a * a;
            if(maxSigma < sigma)
            {
                maxSigma = sigma;
                interval = i;
            }
        }

        return interval;
    }
}