#pragma once
#include <string>
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class AlgorithmParameterSet : public ParameterSet
    {
    private:
        static const unsigned int PROJECTION_BLOCK_SIZE_SIRT = (unsigned int)-1;
        static const unsigned int PROJECTION_BLOCK_SIZE_SART = 1U;

        std::string algorithm;
        unsigned int projectionBlockSize;
        unsigned int numberOfIterations;
        float lambda;
        bool useLongObjectCompensation;
        
    public:
        AlgorithmParameterSet(const ParameterSource* parameterSource);
        ~AlgorithmParameterSet();

        const std::string& Algorithm() const;
        unsigned int ProjectionBlockSize() const;
        unsigned int NumberOfIterations() const;
        float Lambda() const;
        bool UseLongObjectCompensation() const;
    };
}