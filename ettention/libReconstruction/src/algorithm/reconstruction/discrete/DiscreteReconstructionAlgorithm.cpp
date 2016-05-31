#include "stdafx.h"
#include "DiscreteReconstructionAlgorithm.h"

#include "algorithm/reconstruction/discrete/DARTIteration.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{
    DiscreteReconstructionAlgorithm::DiscreteReconstructionAlgorithm(Framework* framework, ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod)
        : ReconstructionAlgorithm(framework)
        , framework(framework)
        , internalAlgebraicReconstructionMethod(internalAlgebraicReconstructionMethod)
        , ownMethod(false)
        , currentIterationsLimit(ITERATIONS_LIMIT)
    {
        if(DiscreteReconstructionAlgorithm *alg = dynamic_cast<DiscreteReconstructionAlgorithm *>(internalAlgebraicReconstructionMethod))
        {
            throw Exception("Discrete reconstruction MUST NOT be instantiated with another discrete reconstruction algorithm as internal");
        }

        this->parameters = framework->getParameterSet()->get<AlgorithmParameterSet>()->getDiscreteReconstructionParameters();
        this->currentIterationsLimit = this->parameters.iterationsLimit;
        
        /*wholeVolumeMask = new GPUMappedVolume(framework->getOpenCLStack(), new ByteVolume(internalAlgebraicReconstructionMethod->getReconstructedVolume()->getProperties().getVolumeResolution(), 1.0f));
        wholeVolumeMask->takeOwnershipOfObjectOnCPU();
        internalAlgebraicReconstructionMethod->setMaskVolume(wholeVolumeMask);  // now ARM is responsible for Mask volume*/
        
        // so far it's the only discrete iteration we have, later switch to factory or smth else
        discreteReconstructionIteration = new DARTIteration(framework, internalAlgebraicReconstructionMethod, internalAlgebraicReconstructionMethod->getReconstructedVolume(), nullptr );//wholeVolumeMask);
    }

    DiscreteReconstructionAlgorithm::~DiscreteReconstructionAlgorithm()
    {
        delete discreteReconstructionIteration;

        if( ownMethod )
            delete internalAlgebraicReconstructionMethod;
    }

    void DiscreteReconstructionAlgorithm::run()
    {
        std::string outputPrefix = parameters.debugOutputPath + "/iter%1%";
        internalAlgebraicReconstructionMethod->run();

        OutputParameterSet outParamset(IO_VOXEL_TYPE_FLOAT_32);
        std::string outputOfARMDumpPath = (boost::format(outputPrefix) % 0).str() + ".mrc";
        framework->getVolumeSerializer()->write(internalAlgebraicReconstructionMethod->getReconstructedVolume(), outputOfARMDumpPath, "mrc", &outParamset);

        bool stopCriterionReached = false;
        unsigned int currentIteration = 0;
        while( !stopCriterionReached )
        {
            discreteReconstructionIteration->run();

            if(parameters.debugOutput)
                ((DARTIteration*)discreteReconstructionIteration)->dumpIntermediateVolumes( (boost::format(outputPrefix) % currentIteration).str() );

            if( currentIteration++ >= currentIterationsLimit )
            {
                stopCriterionReached = true;
                LOGGER("Reached iterations limit in DiscreteReconstructionAlgorithm::Run");
                std::cout << "Reached iterations limit in DiscreteReconstructionAlgorithm::Run" << std::endl;
            } else {
                stopCriterionReached = discreteReconstructionIteration->didReachStopCriterion();
            }
        }
        std::cout << "Stopped after " << currentIteration << " iterations." << std::endl;
    }

    void DiscreteReconstructionAlgorithm::runSingleIteration()
    {
        internalAlgebraicReconstructionMethod->run();
        discreteReconstructionIteration->run();
    }

    GPUMappedVolume* DiscreteReconstructionAlgorithm::getReconstructedVolume() const
    {
        //return internalAlgebraicReconstructionMethod->getReconstructedVolume();
        return discreteReconstructionIteration->getOutput();
    }

    void DiscreteReconstructionAlgorithm::exportGeometryTo(Visualizer* visualizer)
    {
        internalAlgebraicReconstructionMethod->exportGeometryTo(visualizer);
    }

    void DiscreteReconstructionAlgorithm::takeOwnershipOfMethod()
    {
        ownMethod = true;
    }

}