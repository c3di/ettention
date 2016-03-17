#include "stdafx.h"

#include "model/image/Image.h"
#include "framework/error/Exception.h"
#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "algorithm/imagestatistics/OtsuThresholdDecider.h"

namespace ettention
{
    float ThresholdDecider::otsuThreshold(Image *src)
    {
        return OtsuThresholdDecider::computeThreshold(src, ThresholdDecider::NUMBER_OF_INTERVALS_FOR_OTSU_DECIDER);
    }

    float ThresholdDecider::avg(Image *src)
    {
        return src->computeAverageValue();
    }

    float ThresholdDecider::halfMax(Image *src)
    {
        float result = 0.0f;
        float max = src->findMaxValue();

        result = max / 2;
        return result;
    }

    float ThresholdDecider::halfMaxMin(Image *src)
    {
        float result = 0.0f;

        float min = src->findMinValue();
        float max = src->findMaxValue();

        result = min + ((max - min) / 2);
        return result;
    }

    float ThresholdDecider::ninetyfive(Image *src)
    {
        float result = 0.0f;

        float min = src->findMinValue();
        float max = src->findMaxValue();

        result = min + ((max - min) * 0.95f);
        return result;
    }

    float ThresholdDecider::optimalEntropyThreshold(Image *src)
    {
        float result = 0.0f;

        std::vector<int> counter;
        std::vector<float> sumer;

        std::vector<float> hist;

        std::vector<float> blacks;
        std::vector<float> whites;
        std::vector<float> summ;

        ThresholdDecider::computeHistogram(src, ThresholdDecider::NUMBER_OF_INTERVALS_FOR_ENTROPY_DECIDER, counter, sumer);
        ThresholdDecider::normalizeHistogram(counter, (int)src->getPixelCount(), hist);
        ThresholdDecider::computeEntropies(hist, blacks, whites);

        summ.resize(hist.size());
        for(unsigned int i = 0; i < hist.size(); ++i)
        {
            summ[i] = blacks.at(i) + whites.at(i);
        }

        unsigned int maxIndex = 0;
        float maxValue = summ[maxIndex];
        for(unsigned int i = 1; i < hist.size(); ++i)
        {
            if(maxValue < summ[i])
            {
                maxValue = summ[i];
                maxIndex = i;
            }
        }

        float min = src->findMinValue();
        float max = src->findMaxValue();
        float step = (max - min) / hist.size();

        result = (maxIndex + 1) * step;

        return result;
    }

    void ThresholdDecider::computeEntropies(std::vector<float> normalizedHistogram, std::vector<float> &entropyOfBlack, std::vector<float> &entropyOfWhite)
    {
        entropyOfBlack.clear();
        entropyOfBlack.resize(normalizedHistogram.size());

        entropyOfWhite.clear();
        entropyOfWhite.resize(normalizedHistogram.size());

        for(size_t i = 0; i < normalizedHistogram.size(); ++i)
        {
            entropyOfBlack[i] = entropyFunction(normalizedHistogram, 0, i + 1);
            entropyOfWhite[i] = entropyFunction(normalizedHistogram, i + 1, normalizedHistogram.size());
        }
    }

    float ThresholdDecider::entropyFunction(std::vector<float> normalizedHistogram, size_t from, size_t to)
    {
        float result = 0.0f;

        float summ = 0.0f;
        for(size_t i = from; i < to; ++i)
        {
            summ += normalizedHistogram.at(i);
        }

        float temp = 0.0f;
        for(size_t i = from; i < to; ++i)
        {
            temp = normalizedHistogram.at(i) / summ;
            result += temp * log(temp);
        }
        result *= -1;

        return result;
    }

    void ThresholdDecider::computeHistogram(Image *src, int numberOfIntervals, std::vector<int> &count, std::vector<float> &sum)
    {
        float min = src->findMinValue();
        float max = src->findMaxValue();
        float step = (max - min) / (numberOfIntervals);
        auto resolution = src->getResolution();

        count.clear();
        count.resize(numberOfIntervals + 1);

        sum.clear();
        sum.resize(numberOfIntervals + 1);

        if(step == 0.0f)
        {
            return;
        }

        float value;
        int histPosition;
        for(int j = 0; j < (int)resolution.y; ++j)
        {
            for(int i = 0; i < (int)resolution.x; ++i)
            {
                value = src->getPixel(i, j);
                histPosition = (int)((value - min) / step);
                if((histPosition < 0) || (histPosition > numberOfIntervals))
                {
                    throw Exception("Threshold Decider: wrong input data, possibly NaN image.");
                }
                ++count[histPosition];
                sum[histPosition] += value;
            }
        }

        count.shrink_to_fit();
        sum.shrink_to_fit();
    }

    void ThresholdDecider::normalizeHistogram(std::vector<int> &count, int size, std::vector<float> &histogram)
    {
        histogram.clear();
        histogram.resize(count.size());

        for(unsigned int i = 0; i < count.size(); ++i)
        {
            histogram[i] = (float)count.at(i) / (float)size;
        }

        histogram.shrink_to_fit();
    }
}