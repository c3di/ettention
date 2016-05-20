#pragma once
#include <string>
#include "setup/parameterset/ParameterSet.h"
#include "model/interval/ContinuousThresholdInterval.h"

namespace ettention
{
    class ParameterSource;

    struct DiscreteAlgorithmParameterSubSet
    {
        std::string algorithm;
        ContinuousThresholdInterval interval;
        unsigned int iterationsLimit;
        bool debugOutput;
        std::string debugOutputPath;
    };

    class AlgorithmParameterSet : public ParameterSet
    {
    public:
        enum class BasisFunctionType
        {
            BASIS_VOXELS,
            BASIS_BLOBS,
        };

    private:
        static const unsigned int PROJECTION_BLOCK_SIZE_SIRT = (unsigned int)-1;
        static const unsigned int PROJECTION_BLOCK_SIZE_SART = 1U;

        std::string algorithm;
        unsigned int projectionBlockSize;
        unsigned int numberOfIterations;
        float lambda;
        bool useLongObjectCompensation;
        BasisFunctionType basisFunctions;
        DiscreteAlgorithmParameterSubSet discreteParameters;
        
    public:
        AlgorithmParameterSet(const ParameterSource* parameterSource);
        ~AlgorithmParameterSet();

        const std::string& getAlgorithm() const;
        unsigned int getProjectionBlockSize() const;
        unsigned int getNumberOfIterations() const;
        float getLambda() const;
        bool shouldUseLongObjectCompensation() const;
        BasisFunctionType getBasisFunctions() const;

        bool isReconstructionDiscrete() const;
        const DiscreteAlgorithmParameterSubSet& getDiscreteReconstructionParameters() const;

    protected:
        void parseDiscreteParameters(const ParameterSource* parameterSource);
    };
}