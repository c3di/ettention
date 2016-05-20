#pragma once

#include <vector>

namespace ettention
{
    class StructuralCorrelation
    {
    public:
        StructuralCorrelation();
        ~StructuralCorrelation();

        static float computeStructuralCorrelation(const float* data1, const float* data2, size_t numberOfElements);
        static float computeStructuralCorrelation(const std::vector<float>& data1, const std::vector<float>& data2);
    };
}