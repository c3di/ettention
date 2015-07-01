#include "stdafx.h"
#include "WeightedSIRT.h"
#include <algorithm/imagemanipulation/convolution/ConvolutionOperator.h>
#include <algorithm/imagemanipulation/GenerateRamLakFilterKernel.h>

namespace ettention
{
    namespace wsirt
    {
        WeightedSIRT::WeightedSIRT(Framework* framework)
            : BlockIterativeReconstructionOperator(framework)
        {
            ramLakGeneratorKernel = new GenerateRamLakKernel(framework, 128);
            ramLakGeneratorKernel->run();
            convolution = new ConvolutionOperator(framework, ramLakGeneratorKernel->getOutput(), compareKernel->getOutput(), true);
        }

        WeightedSIRT::~WeightedSIRT()
        {
            delete convolution;
            delete ramLakGeneratorKernel;
        }

        void WeightedSIRT::ComputeResidual()
        {
            logger.OnResidualComputationStart();

            compareKernel->setInput(state->getCurrentRealProjection(), state->getVirtualProjection());
            compareKernel->run();

            convolution->setInput(compareKernel->getOutput());
            convolution->setOutput(state->getResidual());
            convolution->execute();

            logger.OnResidualComputationEnd();
        }
    }
}
