#include "stdafx.h"
#include "RootMeanSquareError.h"
#include "framework/error/Exception.h"

namespace ettention
{
    RootMeanSquareError::RootMeanSquareError()
    {
    }

    RootMeanSquareError::~RootMeanSquareError()
    {
    }

    float RootMeanSquareError::computeRMSError(const float* data1, const float* data2, size_t numberOfElements)
    {
        float rmsError = 0.0f;
        for(unsigned int i = 0; i < numberOfElements; i++)
        {
            assertValueIsFinite(data1[i]);
            assertValueIsFinite(data2[i]);
            rmsError += (data1[i] - data2[i]) * (data1[i] - data2[i]);
        }
        rmsError = sqrt(rmsError / numberOfElements);
        return rmsError;
    }

    float RootMeanSquareError::computeRMSError(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if(data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeRMSError(&data1[0], &data2[0], (unsigned int)data1.size());
    }


    float RootMeanSquareError::computeNormalizedRMSError(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if(data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");

        float rmsError = 0.f;
        float minValue = FLT_MAX;
        float maxValue = -FLT_MAX;
        for(unsigned int i = 0; i < data1.size(); i++)
        {
            assertValueIsFinite(data1[i]);
            assertValueIsFinite(data2[i]);
            minValue = std::min(minValue, data2[i]);
            maxValue = std::max(maxValue, data2[i]);
            rmsError += (data1[i] - data2[i]) * (data1[i] - data2[i]);
        }
        rmsError = sqrt(rmsError / (float)data1.size());
        if((maxValue - minValue) != 0.0f)
        {
            return rmsError / (maxValue - minValue);
        }
        else
        {
            return 0.0f;
        }
    }

    float RootMeanSquareError::computeRelativeRMSError(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if(data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");

        float rmsError = 0.f;
        float referenceValuesSquared = 0.f;
        for(unsigned int i = 0; i < data1.size(); i++)
        {
            assertValueIsFinite(data1[i]);
            assertValueIsFinite(data2[i]);
            rmsError += (data1[i] - data2[i]) * (data1[i] - data2[i]);
            referenceValuesSquared += data1[i] * data1[i];
        }
        return sqrt(rmsError / referenceValuesSquared);
    }

    void RootMeanSquareError::assertValueIsFinite(float value)
    {
        if(!boost::math::isfinite(value))
        {
            throw Exception("Infinite value in data!");
        }
    }
}