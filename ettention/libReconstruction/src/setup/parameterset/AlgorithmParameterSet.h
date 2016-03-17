#pragma once
#include <string>
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

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
        
    public:
        AlgorithmParameterSet(const ParameterSource* parameterSource);
        ~AlgorithmParameterSet();

        const std::string& getAlgorithm() const;
        unsigned int getProjectionBlockSize() const;
        unsigned int getNumberOfIterations() const;
        float getLambda() const;
        bool shouldUseLongObjectCompensation() const;
        BasisFunctionType getBasisFunctions() const;
    };
}