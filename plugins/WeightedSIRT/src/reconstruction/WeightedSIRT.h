#pragma once
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"

namespace ettention
{
    class ConvolutionOperator;
    class GenerateRamLakKernel;

    namespace wsirt
    {
        class PLUGIN_API WeightedSIRT : public BlockIterativeReconstructionOperator
        {
        public:
            WeightedSIRT(Framework* framework);
            ~WeightedSIRT();

        protected:
            virtual void ComputeResidual() override;
        
        private:
            GenerateRamLakKernel* ramLakGeneratorKernel;
            ConvolutionOperator* convolution;
        };
    }
}