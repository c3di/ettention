#pragma once

#include "model/image/Image.h"

namespace ettention
{
    class ThresholdDecider
    {
    public:

        static float avg(Image *src);
        static float halfMax(Image *src);
        static float halfMaxMin(Image *src);
        static float ninetyfive(Image *src);

        static float otsuThreshold(Image *src);

        static float optimalEntropyThreshold(Image *src);

        static void computeHistogram(Image *src, int numberOfIntervals, std::vector<int> &count, std::vector<float> &sum);

    protected:
        static void computeEntropies(std::vector<float> normalizedHistogram, std::vector<float> &entropyOfBlack, std::vector<float> &entropyOfWhite);

        static float entropyFunction(std::vector<float> normalizedHistogram, size_t from, size_t to);

        static void normalizeHistogram(std::vector<int> &count, int size, std::vector<float> &histogram);

    private:
        static const int NUMBER_OF_INTERVALS_FOR_OTSU_DECIDER = 255;    // 255 intervals == 256 values.
        static const int NUMBER_OF_INTERVALS_FOR_ENTROPY_DECIDER = 255;
    };
}