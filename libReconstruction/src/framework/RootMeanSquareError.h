#pragma once

#include <vector>

namespace ettention
{
    class RootMeanSquareError
    {
    public:
        RootMeanSquareError();
        ~RootMeanSquareError();

        static float computeRMSError(const float* data1, const float* data2, size_t numberOfElements);
        static float computeRMSError(const std::vector<float>& data1, const std::vector<float>& data2);
        static float computeNormalizedRMSError(const std::vector<float>& data1, const std::vector<float>& data2);
        static float computeRelativeRMSError(const std::vector<float>& data1, const std::vector<float>& data2);

    private:
        static void assertValueIsFinite(float value);
    };
}