#include "stdafx.h"
#include "AlgorithmParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    AlgorithmParameterSet::AlgorithmParameterSet(const ParameterSource* parameterSource)
    {
        algorithm = parameterSource->getStringParameter("algorithm");
        projectionBlockSize = PROJECTION_BLOCK_SIZE_SIRT;
        if(algorithm == "sart")
        {
            projectionBlockSize = PROJECTION_BLOCK_SIZE_SART;
        }
        else if(algorithm == "sirt")
        {
            projectionBlockSize = PROJECTION_BLOCK_SIZE_SIRT;
        }
        else if (parameterSource->parameterExists("algorithm.blockSize"))
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

        basisFunctions = BasisFunctionType::BASIS_VOXELS;
        if( parameterSource->parameterExists("algorithm.basisFunctions") )
        {
            auto basisFunctionParameter = parameterSource->getStringParameter("algorithm.basisFunctions");
            if( basisFunctionParameter == "blobs" )
            {
                basisFunctions = BasisFunctionType::BASIS_BLOBS;
            } else
            if( basisFunctionParameter != "voxels" )
            {
                throw Exception("Illegal value for parameter \"algorithm.basisFunctions\": " + basisFunctionParameter + "!");
            }
        }
    }

    AlgorithmParameterSet::~AlgorithmParameterSet()
    {
    }

    unsigned int AlgorithmParameterSet::getProjectionBlockSize() const
    {
        return projectionBlockSize;
    }

    const std::string& AlgorithmParameterSet::getAlgorithm() const
    {
        return algorithm;
    }

    float AlgorithmParameterSet::getLambda() const
    {
        return lambda;
    }

    bool AlgorithmParameterSet::shouldUseLongObjectCompensation() const
    {
        return useLongObjectCompensation;
    }

    AlgorithmParameterSet::BasisFunctionType AlgorithmParameterSet::getBasisFunctions() const
    {
        return basisFunctions;
    }

    unsigned int AlgorithmParameterSet::getNumberOfIterations() const
    {
        return numberOfIterations;
    }
}