#pragma once
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"
#include "model/volume/Volume.h"
#include "reconstruction/operators/PreFilterOperator.h"
#include "TFS_BackProjection_Operator.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API Regularization_BackProjectionOperator : public TFS_BackProjectionOperator
        {
        public:
            Regularization_BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMapped<Image>* residual, float lambda);
            virtual ~Regularization_BackProjectionOperator();

            virtual void run();

        protected:
            RegularizationBackProjectionKernel* backProjectionKernel;
        };
    }
}