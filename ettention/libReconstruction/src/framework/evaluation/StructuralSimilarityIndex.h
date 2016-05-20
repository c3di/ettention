#pragma once

#include <vector>

namespace ettention
{
    class StructuralSimilarityIndex
    {
    public:
        StructuralSimilarityIndex();
        ~StructuralSimilarityIndex();

        static float computeStructuralSimilarityIndex(const float* data1, const float* data2, size_t numberOfElements, float imageTypeInBit);
        static float computeStructuralSimilarityIndex(std::vector<float>& data1, std::vector<float>& data2, float imageTypeInBit);
    };
}