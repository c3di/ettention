#include "stdafx.h"
#include "framework/Framework.h"
#include "framework/math/Matrix4x4.h"
#include "reconstruction/projections/AdjointBackProjectionKernel.h"
#include "reconstruction/GenerateCircularPSFKernel.h"
#include "reconstruction/operators/PreFilterOperator.h"
#include "Regularization_BackProjection_Operator.h"

namespace ettention
{
    namespace stem
    {
        TFS_BackProjectionOperator::TFS_BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMapped<Image>* residual, float lambda)
            : clContext(framework->getOpenCLStack())
            , geometricSetup(geometricSetup)
            , volume(volume)
            , residual(residual)
            , lambda(lambda)
        {
            rayLengthImage = new GPUMapped<Image>(clContext, residual->getResolution());
            computeRayLengthKernel = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);
        }

        TFS_BackProjectionOperator::~TFS_BackProjectionOperator()
        {
            delete computeRayLengthKernel;
            delete rayLengthImage;
        }

        void TFS_BackProjectionOperator::setScannerGeometry(STEMScannerGeometry* scannerGeometry)
        {
            this->scannerGeometry = scannerGeometry;
        }

        GPUMappedVolume* TFS_BackProjectionOperator::getOutput()
        {
            return volume;
        }
    }
}