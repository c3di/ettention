#include "stdafx.h"
#include "DiscreteReconstructionIterationImplementation.h"

namespace ettention
{
    DiscreteReconstructionIterationImplementation::DiscreteReconstructionIterationImplementation(Framework* framework, ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod)
        : internalAlgebraicReconstructionMethod(internalAlgebraicReconstructionMethod)
        , framework(framework)
    {
    }

    DiscreteReconstructionIterationImplementation::~DiscreteReconstructionIterationImplementation()
    {
    }

}