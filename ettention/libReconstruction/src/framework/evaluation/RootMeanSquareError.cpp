#include "stdafx.h"
#include "RootMeanSquareError.h"
#include "framework/error/Exception.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    RootMeanSquareError::RootMeanSquareError()
    {
    }

    RootMeanSquareError::~RootMeanSquareError()
    {
    }

    float RootMeanSquareError::computeMSError(const float* data1, const float* data2, size_t numberOfElements)
    {
        float rmsError = 0.0f;
        std::size_t realNumberOfElements = 0;
        for(unsigned int i = 0; i < numberOfElements; i++)
        {
            if(boost::math::isfinite(data1[i]) ^ boost::math::isfinite(data2[i]))
            {
                throw Exception("Found infinite value which corresponding value is finite!");
            }
            else if(boost::math::isfinite(data1[i]))
            {
                ++realNumberOfElements;
                rmsError += (data1[i] - data2[i]) * (data1[i] - data2[i]);
            }
        }
        return realNumberOfElements == 0 ? 0.0f : rmsError / (float)realNumberOfElements;
    }

    float RootMeanSquareError::computeRMSError(const float* data1, const float* data2, size_t numberOfElements)
    {
        return std::sqrt(computeMSError(data1, data2, numberOfElements));
    }

    float RootMeanSquareError::computeMSError(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if (data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeMSError(&data1[0], &data2[0], (unsigned int)data1.size());
    }

    float RootMeanSquareError::computeRMSError(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if(data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeRMSError(&data1[0], &data2[0], (unsigned int)data1.size());
    }
}