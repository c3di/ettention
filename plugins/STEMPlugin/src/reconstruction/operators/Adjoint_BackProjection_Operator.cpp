#include "stdafx.h"
#include "Adjoint_BackProjection_Operator.h"
#include "framework/math/Matrix4x4.h"
#include "model/geometry/GeometricSetup.h"
#include "reconstruction/projections/AdjointBackProjectionKernel.h"
#include "reconstruction/GenerateCircularPSFKernel.h"
#include "reconstruction/operators/PreFilterOperator.h"

namespace ettention
{
    namespace stem
    {
        Adjoint_BackProjectionOperator::Adjoint_BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMapped<Image>* residual, float lambda)
            : TFS_BackProjectionOperator(framework, geometricSetup, volume, residual, lambda)
        {
            prefilterOperator = new PrefilterOperator(framework, residual, 1.0f);
            backProjectionKernel = new AdjointBackProjectionKernel(framework, geometricSetup, prefilterOperator->getOutput(), computeRayLengthKernel->getOutput(), volume->getMappedSubVolume(), lambda, false);
        }

        Adjoint_BackProjectionOperator::~Adjoint_BackProjectionOperator()
        {
            delete backProjectionKernel;
            delete prefilterOperator;
        }

        void Adjoint_BackProjectionOperator::run()
        {
            computeRayLengthKernel->run();
            prefilterOperator->run();

            for(unsigned int i = 0; i < volume->getObjectOnCPU()->Properties().GetSubVolumeCount(); i++)
            {
                geometricSetup->setCurrentSubVolumeIndex(i);
                volume->setCurrentSubvolumeIndex(i);
                backProjectionKernel->run();
            }
        }

        void Adjoint_BackProjectionOperator::SetProjectionProperties(const TFProjectionProperties& projectionProperties)
        {
            TFS_BackProjectionOperator::SetProjectionProperties(projectionProperties);
            prefilterOperator->setMaxRadius(getMaximalBlurrRadius());
            backProjectionKernel->setProjectionProperties(projectionProperties);
        }

        float Adjoint_BackProjectionOperator::getMaximalBlurrRadius()
        {
            Vec3ui resolution = volume->getObjectOnCPU()->Properties().GetVolumeResolution();
            Vec3f  volumeSize = Vec3f((float)resolution.x, (float)resolution.y, (float)resolution.z);
            float diagonal = volumeSize.LengthF();
            return diagonal * tanf(projectionProperties.GetConfocalOpeningHalfAngle());
        }
    }
}