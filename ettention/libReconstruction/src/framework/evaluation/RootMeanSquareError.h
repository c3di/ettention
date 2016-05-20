#pragma once

#include <vector>

namespace ettention
{
    class RootMeanSquareError;

    class RootMeanSquareError
    {
    public:
        RootMeanSquareError();
        ~RootMeanSquareError();

        static float computeMSError(const float* data1, const float* data2, size_t numberOfElements);
        static float computeRMSError(const float* data1, const float* data2, size_t numberOfElements);

        static float computeMSError(const std::vector<float>& data1, const std::vector<float>& data2);
        static float computeRMSError(const std::vector<float>& data1, const std::vector<float>& data2);
    };
}