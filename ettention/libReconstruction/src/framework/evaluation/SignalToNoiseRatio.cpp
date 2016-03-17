#include "stdafx.h"
#include "framework/evaluation/RootMeanSquareError.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/Exception.h"
#include "SignalToNoiseRatio.h"
#include "StatisticalStandardMeasures.h"
#include <cmath>

namespace ettention
{
    SignalToNoiseRatio::SignalToNoiseRatio()
    {
    }

    SignalToNoiseRatio::~SignalToNoiseRatio()
    {
    }

    float SignalToNoiseRatio::computeImprovedSignalToNoiseRatio(const float* data1, const float* data2, size_t numberOfElements)
    {
        float isnr = 0.0f;
        float* zeroData = new float[numberOfElements];
        for (unsigned int i = 0; i < numberOfElements; i++)
        {
            zeroData[i] = 0.0f;
        }
        float mseZero = RootMeanSquareError::computeMSError(zeroData, data2, numberOfElements);
        float mse = RootMeanSquareError::computeMSError(data1, data2, numberOfElements);
        isnr = 10.0f * (std::log10(mseZero) - std::log10(mse));
        delete zeroData;
        return isnr;
    }

    float SignalToNoiseRatio::computeImprovedSignalToNoiseRatio(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if (data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeImprovedSignalToNoiseRatio(&data1[0], &data2[0], (unsigned int)data1.size());
    }

    float SignalToNoiseRatio::computePeakSignalToNoiseRatio(const float* data1, const float* data2, size_t numberOfElements, float imageTypeInBit)
    {
        float mse = RootMeanSquareError::computeMSError(data1, data2, numberOfElements);
        float dynamicPixelValueRange = std::pow(2.0f, imageTypeInBit);
        return 20.0f * std::log10(dynamicPixelValueRange - 1) - 10.0f * std::log10(mse);
    }

    float SignalToNoiseRatio::computePeakSignalToNoiseRatio(const std::vector<float>& data1, const std::vector<float>& data2, float imageTypeInBit)
    {
        if(data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computePeakSignalToNoiseRatio(&data1[0], &data2[0], (unsigned int)data1.size(), imageTypeInBit);
    }


    float SignalToNoiseRatio::computeSignalToNoiseRatio(const float* data, size_t numberOfElements)
    {
        float meanValue = StatisticalStandardMeasures::computeMean(data, numberOfElements);
        float sdValue = StatisticalStandardMeasures::computeStandardDeviation(data, numberOfElements);

        return (meanValue / sdValue);
    }

    float SignalToNoiseRatio::computeSignalToNoiseRatio(const std::vector<float>& data)
    {
        return computeSignalToNoiseRatio(&data[0], (unsigned int)data.size());
    }
}