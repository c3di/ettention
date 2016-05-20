#include "stdafx.h"
#include "AlgorithmParameterSet.h"
#include "algorithm/reconstruction/discrete/DiscreteReconstructionAlgorithm.h"
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
        
        discreteParameters.algorithm = "";
        if( parameterSource->parameterExists("algorithm.discreteReconstruction") )
        {
            parseDiscreteParameters(parameterSource);
        }
    }

    AlgorithmParameterSet::~AlgorithmParameterSet()
    {
    }

    const std::string& AlgorithmParameterSet::getAlgorithm() const
    {
        return algorithm;
    }

    unsigned int AlgorithmParameterSet::getProjectionBlockSize() const
    {
        return projectionBlockSize;
    }

    unsigned int AlgorithmParameterSet::getNumberOfIterations() const
    {
        return numberOfIterations;
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

    bool AlgorithmParameterSet::isReconstructionDiscrete() const
    {
        return !discreteParameters.algorithm.empty();
    }

    const DiscreteAlgorithmParameterSubSet& AlgorithmParameterSet::getDiscreteReconstructionParameters() const
    {
        if( !isReconstructionDiscrete() )
            throw Exception("Can not get discrete reconstruction parameters for non discrete reconstruction algorithm " + algorithm);
        return discreteParameters;
    }

    void AlgorithmParameterSet::parseDiscreteParameters(const ParameterSource* parameterSource)
    {
        discreteParameters.algorithm = parameterSource->getStringParameter("algorithm.discreteReconstruction");

        if( parameterSource->parameterExists("algorithm.discreteReconstruction.threshold") )
        {
            discreteParameters.interval.parseFromString(parameterSource->getStringParameter("algorithm.discreteReconstruction.threshold"));

            if( parameterSource->parameterExists("algorithm.discreteReconstruction.intensity") )
            {
                discreteParameters.interval.parseIntensityValues(parameterSource->getStringParameter("algorithm.discreteReconstruction.intensity"));
            }
        } else {
            // TODO: Automatically detect levels and values (see TVR-DART);
        }

        discreteParameters.iterationsLimit = DiscreteReconstructionAlgorithm::ITERATIONS_LIMIT;
        if( parameterSource->parameterExists("algorithm.discreteReconstruction.iterationsLimit") )
        {
            discreteParameters.iterationsLimit = parameterSource->getUIntParameter("algorithm.discreteReconstruction.iterationsLimit");
        }

        discreteParameters.debugOutput = false;
        if( parameterSource->parameterExists("algorithm.discreteReconstruction.debugOutput") )
        {
            discreteParameters.debugOutput = true;

            if(parameterSource->parameterExists("algorithm.discreteReconstruction.debugOutput.path") )
            {
                discreteParameters.debugOutputPath = parameterSource->getStringParameter("algorithm.discreteReconstruction.debugOutput.path");
            } else {
                discreteParameters.debugOutputPath = "";
            }
        }
    }
}