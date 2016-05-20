#pragma once

#include <vector>

namespace ettention
{
    class StatisticalStandardMeasures
    {
    public:
        StatisticalStandardMeasures();
        ~StatisticalStandardMeasures();

        static float computeMean(const float* data, size_t numberOfElements);
        static float computeMean(const std::vector<float> data);

        static float computeVariance(const float* data, size_t numberOfElements, bool unbiasedEstimate = true);
        static float computeVariance(const std::vector<float> data, bool unbiasedEstimate = true);

        static float computeStandardDeviation(const float* data, size_t numberOfElements, bool unbiasedEstimate = true);
        static float computeStandardDeviation(const std::vector<float> data, bool unbiasedEstimate = true);

        static float computeCovariance(const float* data1, const float* data2, size_t numberOfElements, bool unbiasedEstimate = true);
        static float computeCovariance(const std::vector<float> data1, const std::vector<float> data2, bool unbiasedEstimate = true);
    };
}