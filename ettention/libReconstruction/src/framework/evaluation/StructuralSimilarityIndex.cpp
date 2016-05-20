#include "stdafx.h"
#include "RootMeanSquareError.h"
#include "framework/error/Exception.h"
#include "framework/NumericalAlgorithms.h"
#include "StructuralSimilarityIndex.h"
#include "framework/evaluation/StatisticalStandardMeasures.h"
#include <cmath>

namespace ettention
{
    StructuralSimilarityIndex::StructuralSimilarityIndex()
    {
    }

    StructuralSimilarityIndex::~StructuralSimilarityIndex()
    {
    }

    float StructuralSimilarityIndex::computeStructuralSimilarityIndex(const float* data1, const float* data2, size_t numberOfElements, float imageTypeInBit)
    {
        float ssim = 0.0f;
        float meanData1 = StatisticalStandardMeasures::computeMean(data1, numberOfElements);
        float meanData2 = StatisticalStandardMeasures::computeMean(data2, numberOfElements);
        float sdData1 = StatisticalStandardMeasures::computeStandardDeviation(data1, numberOfElements);
        float sdData2 = StatisticalStandardMeasures::computeStandardDeviation(data2, numberOfElements);
        float covarianceValue = StatisticalStandardMeasures::computeCovariance(data1, data2, numberOfElements);
        float dynamicPixelValueRange = std::pow(2.0f,imageTypeInBit);
        float c1 = 0.01f * (dynamicPixelValueRange - 1);
        float c2 = 0.03f * (dynamicPixelValueRange - 1);

        ssim = (2 * meanData1 * meanData2 + c1)*(2 * covarianceValue + c2) / (meanData1 * meanData1 + meanData2 * meanData2 + c1)*(sdData1 * sdData1 + sdData2 * sdData2 + c2);

        return ssim;
    }

    float StructuralSimilarityIndex::computeStructuralSimilarityIndex(std::vector<float>& data1, std::vector<float>& data2, float imageTypeInBit)
    {
        if (data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeStructuralSimilarityIndex(&data1[0], &data2[0], (unsigned int)data1.size(), imageTypeInBit);
    }
}