#pragma once
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"
#include "model/TFProjectionProperties.h"
#include "model/volume/GPUMappedVolume.h"
#include "gpu/GPUMapped.h"
#include "reconstruction/projections/RegularizationBackprojectionKernel.h"
#include "reconstruction/operators/PreFilterOperator.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API TFS_BackProjectionOperator
        {
        public:
            TFS_BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMapped<Image>* residual, float lambda);
            virtual ~TFS_BackProjectionOperator();

            virtual void run() = 0;
            virtual GPUMappedVolume* getOutput();

            virtual void SetProjectionProperties(const TFProjectionProperties& projectionProperties);

        protected:
            CLAbstractionLayer* clContext;
            GeometricSetup* geometricSetup;
            TFProjectionProperties projectionProperties;
            GPUMappedVolume* volume;
            GPUMapped<Image>* residual;
            GPUMapped<Image>* rayLengthImage;
            float lambda;

            ComputeRayLengthKernel* computeRayLengthKernel;
        };
    }
}