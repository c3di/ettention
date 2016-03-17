#include "stdafx.h"
#include "AdjointBackProjectionKernel.h"
#include "Voxel_by_Voxel_Adjoint_Backprojection_bin2c.h"
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "framework/math/Matrix4x4.h"
#include "gpu/opencl/CLKernel.h"
#include "io/serializer/ImageSerializer.h"
#include "model/geometry/GeometricSetup.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    namespace stem
    {
        AdjointBackProjectionKernel::AdjointBackProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMapped<Image>* preFilteredResidual, GPUMapped<Image>* rayLength, GPUMapped<SubVolume>* subvolume, float lambda, bool useLongObjectCompensation)
            : ComputeKernel(framework, Voxel_by_Voxel_Adjoint_Backprojection_kernel_SourceCode, "correct", "AdjointBackProjectionKernel")
            , projectionResolution(geometricSetup->getResolution())
            , geometricSetup(geometricSetup)
            , preFilteredResidual(preFilteredResidual)
            , rayLength(rayLength)
            , subvolume(subvolume)
            , lambda(lambda)
        {
            transformMatrix = new GPUMapped<Matrix4x4>(abstractionLayer, Vec2ui(4, 4));
        }

        AdjointBackProjectionKernel::~AdjointBackProjectionKernel()
        {
            delete transformMatrix;
        }

        void AdjointBackProjectionKernel::prepareKernelArguments()
        {
            VariableContainer* container = geometricSetup->getProjectionVariableContainer();
            implementation->setArgument("projectionGeometry", container);
            implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
            implementation->setArgument("projectionResolution", (Vec2i)projectionResolution);
            implementation->setArgument("vol", subvolume);
            implementation->setArgument("projection", preFilteredResidual);
            implementation->setArgument("ray_length_inside_volume", rayLength);
            transformMatrix->setObjectOnCPU(new Matrix4x4(geometricSetup->getScannerGeometry()->getProjectionMatrix()));
            transformMatrix->takeOwnershipOfObjectOnCPU();
            implementation->setArgument("transform", transformMatrix);
            implementation->setArgument("lambda", lambda);
            unsigned int numberOfPrefilteredResiduals = preFilteredResidual->getResolution().y / preFilteredResidual->getResolution().x;
            implementation->setArgument("numberOfPrecomputedResiduals", numberOfPrefilteredResiduals);
        }

        void AdjointBackProjectionKernel::preRun()
        {
            implementation->setGlobalWorkSize(subvolume->getResolution());
        }

        void AdjointBackProjectionKernel::postRun()
        {
            subvolume->markAsChangedOnGPU();
        }

        void AdjointBackProjectionKernel::setScannerGeometry(STEMScannerGeometry* geometry)
        {
            this->scannerGeometry = geometry;
        }
    }
}
