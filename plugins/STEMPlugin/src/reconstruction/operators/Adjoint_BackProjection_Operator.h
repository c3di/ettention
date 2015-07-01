#pragma once
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"
#include "model/volume/Volume.h"
#include "reconstruction/projections/AdjointBackProjectionKernel.h"
#include "reconstruction/operators/PreFilterOperator.h"
#include "TFS_BackProjection_Operator.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API Adjoint_BackProjectionOperator : public TFS_BackProjectionOperator
        {
        public:
            Adjoint_BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMapped<Image>* residual, float lambda);
            virtual ~Adjoint_BackProjectionOperator();

            virtual void run();

            virtual void SetProjectionProperties(const TFProjectionProperties& projectionProperties) override;

        protected:
            float getMaximalBlurrRadius();

        protected:
            AdjointBackProjectionKernel* backProjectionKernel;
            PrefilterOperator* prefilterOperator;
        };

    }
}