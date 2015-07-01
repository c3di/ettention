#include "stdafx.h"
#include "AlgorithmParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    AlgorithmParameterSet::AlgorithmParameterSet(const ParameterSource* parameterSource)
    {
        algorithm = parameterSource->getStringParameter("algorithm");
        if(algorithm == "sart")
        {
            projectionBlockSize = PROJECTION_BLOCK_SIZE_SART;
        }
        else if(algorithm == "sirt")
        {
            projectionBlockSize = PROJECTION_BLOCK_SIZE_SIRT;
        }
        else if(algorithm == "blockIterative")
        {
            projectionBlockSize = parameterSource->getUIntParameter("algorithm.blockSize");
        }

        numberOfIterations = parameterSource->getUIntParameter("algorithm.numberOfIterations");
        lambda = parameterSource->getFloatParameter("algorithm.lambda");

        useLongObjectCompensation = false;
        if(parameterSource->parameterExists("algorithm.useLongObjectCompensation"))
        {
            useLongObjectCompensation = parameterSource->getBoolParameter("algorithm.useLongObjectCompensation");
        }
    }

    AlgorithmParameterSet::~AlgorithmParameterSet()
    {
    }

    unsigned int AlgorithmParameterSet::ProjectionBlockSize() const
    {
        return projectionBlockSize;
    }

    const std::string& AlgorithmParameterSet::Algorithm() const
    {
        return algorithm;
    }

    float AlgorithmParameterSet::Lambda() const
    {
        return lambda;
    }

    bool AlgorithmParameterSet::UseLongObjectCompensation() const
    {
        return useLongObjectCompensation;
    }

    unsigned int AlgorithmParameterSet::NumberOfIterations() const
    {
        return numberOfIterations;
    }
}