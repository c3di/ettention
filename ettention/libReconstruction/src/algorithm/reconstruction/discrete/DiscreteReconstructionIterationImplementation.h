#pragma once

#include "framework/Framework.h"
#include "algorithm/reconstruction/ReconstructionAlgorithmUsingMask.h"

namespace ettention
{
    class Framework;
    class GPUMappedVolume;

    class DiscreteReconstructionIterationImplementation
    {
    public:
        DiscreteReconstructionIterationImplementation(Framework* framework, ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod);
        virtual ~DiscreteReconstructionIterationImplementation();

        virtual void run() = 0;

        virtual bool didReachStopCriterion() = 0;

        virtual GPUMappedVolume* getOutput() const = 0;
        
    protected:
        Framework* framework;
        ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod;
    };
}