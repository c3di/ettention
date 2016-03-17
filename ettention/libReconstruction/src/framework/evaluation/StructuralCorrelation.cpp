#include "stdafx.h"
#include "RootMeanSquareError.h"
#include "framework/error/Exception.h"
#include "framework/NumericalAlgorithms.h"
#include "StructuralCorrelation.h"

namespace ettention
{
    StructuralCorrelation::StructuralCorrelation()
    {
    }

    StructuralCorrelation::~StructuralCorrelation()
    {
    }

    float StructuralCorrelation::computeStructuralCorrelation(const float* data1, const float* data2, size_t numberOfElements)
    {
        float scValue = 0.0f;
        float sumData1 = 0.0f;
        float sumData2 = 0.0f;
        float sumAbsDifference = 0.0f;

        for (unsigned int i = 0; i < numberOfElements; i++)
        {
            sumData1 += data1[i];
            sumData2 += data2[i];
            sumAbsDifference += std::abs(data1[i] - data2[i]);
        }

        scValue = 1 - (2 * sumAbsDifference / (sumData1 + sumData2));
        return scValue;
    }

    float StructuralCorrelation::computeStructuralCorrelation(const std::vector<float>& data1, const std::vector<float>& data2)
    {
        if(data1.size() != data2.size())
            throw std::runtime_error("illegal comparison of stacks with different size");
        return computeStructuralCorrelation(&data1[0], &data2[0], (unsigned int)data1.size());
    }
}