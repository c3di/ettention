#include "stdafx.h"
#include "Regularization_BackProjection_Operator.h"
#include "framework/math/Matrix4x4.h"
#include "model/geometry/GeometricSetup.h"
#include "reconstruction/GenerateCircularPSFKernel.h"
#include "reconstruction/operators/PreFilterOperator.h"
#include "reconstruction/projections/AdjointBackProjectionKernel.h"

namespace ettention
{
    namespace stem
    {
        Regularization_BackProjectionOperator::Regularization_BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMapped<Image>* residual, float lambda)
            : TFS_BackProjectionOperator(framework, geometricSetup, volume, residual, lambda)
        {
            backProjectionKernel = new RegularizationBackProjectionKernel(framework, geometricSetup, residual, computeRayLengthKernel->getOutput(), volume->getMappedSubVolume(), lambda, false);
        }

        Regularization_BackProjectionOperator::~Regularization_BackProjectionOperator()
        {
            delete backProjectionKernel;
        }

        void Regularization_BackProjectionOperator::run()
        {
            computeRayLengthKernel->run();
            residual->ensureIsUpToDateOnGPU();
            for(unsigned int i = 0; i < volume->getObjectOnCPU()->Properties().GetSubVolumeCount(); i++)
            {
                geometricSetup->setCurrentSubVolumeIndex(i);
                backProjectionKernel->setProjectionProperties(projectionProperties);
                backProjectionKernel->run();
            }
        }
    }
}