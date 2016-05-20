#pragma once
#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include "algorithm/reconstruction/discrete/DiscreteReconstructionIterationImplementation.h"
#include "setup/parameterset/AlgorithmParameterSet.h"

namespace ettention
{
    class Framework;
    class GPUMappedVolume;
    class Visualizer;

    class DiscreteReconstructionAlgorithm : public ReconstructionAlgorithm
    {
    public:
        DiscreteReconstructionAlgorithm(Framework* framework, ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod);
        virtual ~DiscreteReconstructionAlgorithm();

        virtual void run() override;
        virtual void runSingleIteration() override;
        virtual GPUMappedVolume* getReconstructedVolume() const override;
        virtual void exportGeometryTo(Visualizer* visualizer) override;

        void takeOwnershipOfMethod();

        static const unsigned int ITERATIONS_LIMIT{90u};

    protected:
        Framework* framework;

    private:
        GPUMappedVolume* wholeVolumeMask;
        ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod;
        DiscreteReconstructionIterationImplementation* discreteReconstructionIteration;
        bool ownMethod;
        unsigned int currentIterationsLimit;

        DiscreteAlgorithmParameterSubSet parameters;
    };
}