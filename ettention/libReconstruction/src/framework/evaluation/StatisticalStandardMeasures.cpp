#include "stdafx.h"
#include "StatisticalStandardMeasures.h"
#include "framework/error/Exception.h"

namespace ettention
{
    StatisticalStandardMeasures::StatisticalStandardMeasures()
    {
    }

    StatisticalStandardMeasures::~StatisticalStandardMeasures()
    {
    }

    float StatisticalStandardMeasures::computeMean(const float* data, size_t numberOfElements)
    {
        float meanValue = 0.0f;

        for (unsigned int i = 0; i < numberOfElements; i++)
        {
            meanValue += data[i];
        }

        return (meanValue / numberOfElements);
    }

    float StatisticalStandardMeasures::computeMean(const std::vector<float> data)
    {
        return computeMean(&data[0], (unsigned int)data.size());
    }

    float StatisticalStandardMeasures::computeVariance(const float* data, size_t numberOfElements, bool unbiasedEstimate)
    {
        float sdValue = 0.0f;
        float meanValue = computeMean(data, numberOfElements);

        for (unsigned int i = 0; i < numberOfElements; i++)
        {
            sdValue += (data[i] - meanValue) * (data[i] - meanValue);
        }

        if (unbiasedEstimate)
        {
            sdValue /= (numberOfElements - 1);
        }
        else
        {
            sdValue /= numberOfElements;
        }

        return sdValue;
    }

    float StatisticalStandardMeasures::computeVariance(const std::vector<float> data, bool unbiasedEstimate)
    {
        return computeVariance(&data[0], (unsigned int)data.size(), unbiasedEstimate);
    }

    float StatisticalStandardMeasures::computeStandardDeviation(const float* data, size_t numberOfElements, bool unbiasedEstimate)
    {
        return std::sqrt(computeVariance(data, numberOfElements, unbiasedEstimate));
    }

    float StatisticalStandardMeasures::computeStandardDeviation(const std::vector<float> data, bool unbiasedEstimate)
    {
        return computeStandardDeviation(&data[0], (unsigned int)data.size(), unbiasedEstimate);
    }

    float StatisticalStandardMeasures::computeCovariance(const float* data1, const float* data2, size_t numberOfElements, bool unbiasedEstimate)
    {
        float covarianceValue = 0.0f;
        float meanValueData1 = computeMean(data1, numberOfElements);
        float meanValueData2 = computeMean(data2, numberOfElements);

        for (unsigned int i = 0; i < numberOfElements; i++)
        {
            covarianceValue += (data1[i] - meanValueData1) * (data2[i] - meanValueData2);
        }

        if (unbiasedEstimate)
        {
            return (covarianceValue / (numberOfElements - 1));
        }
        else
        {
            return (covarianceValue / numberOfElements);
        }
    }

    float StatisticalStandardMeasures::computeCovariance(const std::vector<float> data1, const std::vector<float> data2, bool unbiasedEstimate)
    {
        if (data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeCovariance(&data1[0], &data2[0], (unsigned int)data1.size(), unbiasedEstimate);
    }
}