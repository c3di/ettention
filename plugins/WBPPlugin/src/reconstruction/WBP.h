#pragma once
#include <algorithm/reconstruction/ReconstructionAlgorithm.h>

namespace ettention
{
    class BackProjectionOperator;
    class ComputeRayLengthKernel;
    class ConvolutionOperator;
    class GenerateRamLakKernel;

    namespace wbp
    {
        class PLUGIN_API WBP : public ReconstructionAlgorithm
        {
        public:
            WBP(Framework* framework);
            ~WBP();

            void run() override;
            GPUMappedVolume* getReconstructedVolume() const override;

        private:
            void optimizeMemoryUsage();
            
            // Data structures
            GPUMappedVolume* volume;
            ImageStackDatasource* input;
            GeometricSetup* geometricSetup;
            // GPUMapped<Image>* ramLakKernel;
            GPUMapped<Image>* projection;
            GPUMapped<Image>* rayLength;

            unsigned int nProjections;

            // Compute Kernels
            unsigned int filterSize;
            BackProjectionOperator* backprojection;
            GenerateRamLakKernel* ramLakKernel;
            ComputeRayLengthKernel* rayLengthKernel;
            ConvolutionOperator* convolution;
        };
    }
}