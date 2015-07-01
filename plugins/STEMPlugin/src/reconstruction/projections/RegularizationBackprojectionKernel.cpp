#include "stdafx.h"
#include "RegularizationBackprojectionKernel.h"
#include "Voxel_by_Voxel_Regularization_Backprojection_bin2c.h"
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "framework/math/Matrix4x4.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    namespace stem
    {
        RegularizationBackProjectionKernel::RegularizationBackProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMapped<Image>* residual, GPUMapped<Image>* rayLength, GPUMapped<SubVolume>* subvolume, float lambda, bool useLongObjectCompensation)
            : ComputeKernel(framework, Voxel_by_Voxel_Regularization_Backprojection_kernel_SourceCode, "correct", "RegularizationBackProjectionKernel")
            , projectionResolution(geometricSetup->getResolution())
            , geometricSetup(geometricSetup)
            , residual(residual)
            , rayLength(rayLength)
            , subvolume(subvolume)
            , lambda(lambda)
        {
            transformMatrix = new GPUMapped<Matrix4x4>(abstractionLayer, Vec2ui(4, 4));
        }

        RegularizationBackProjectionKernel::~RegularizationBackProjectionKernel()
        {
            delete transformMatrix;
        }

        void RegularizationBackProjectionKernel::prepareKernelArguments()
        {
            implementation->setArgument("projectionGeometry", geometricSetup->getProjectionVariableContainer());
            implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
            implementation->setArgument("convergentBeamsParameter", projectionProperties.getVariableContainer());
            implementation->setArgument("projectionResolution", projectionResolution);
            implementation->setArgument("vol", subvolume);
            implementation->setArgument("projection", residual);
            implementation->setArgument("ray_length_inside_volume", rayLength);
            transformMatrix->setObjectOnCPU(new Matrix4x4(geometricSetup->getScannerGeometry().getProjectionMatrix()));
            transformMatrix->takeOwnershipOfObjectOnCPU();
            implementation->setArgument("transform", transformMatrix);
            implementation->setArgument("lambda", lambda);
        }

        void RegularizationBackProjectionKernel::preRun()
        {
            implementation->setGlobalWorkSize(subvolume->getResolution());
        }

        void RegularizationBackProjectionKernel::postRun()
        {
            subvolume->markAsChangedOnGPU();
        }

        void RegularizationBackProjectionKernel::setProjectionProperties(const TFProjectionProperties& projectionProperties)
        {
            this->projectionProperties = projectionProperties;
        }
    }
}