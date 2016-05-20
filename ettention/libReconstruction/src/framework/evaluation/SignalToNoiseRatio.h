#pragma once

#include <vector>

namespace ettention
{
    class RootMeanSquareError;

    class SignalToNoiseRatio
    {
    public:
        SignalToNoiseRatio();
        ~SignalToNoiseRatio();

        static float computeImprovedSignalToNoiseRatio(const float* data1, const float* data2, size_t numberOfElements);
        static float computeImprovedSignalToNoiseRatio(const std::vector<float>& data1, const std::vector<float>& data2);

        static float computePeakSignalToNoiseRatio(const float* data1, const float* data2, size_t numberOfElements, float imageTypeInBit);
        static float computePeakSignalToNoiseRatio(const std::vector<float>& data1, const std::vector<float>& data2, float imageTypeInBit);

        static float computeSignalToNoiseRatio(const float* data, size_t numberOfElements);
        static float computeSignalToNoiseRatio(const std::vector<float>& data);
    };
}