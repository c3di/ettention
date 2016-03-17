#include "stdafx.h"
#include "ComputeRayLengthKernel.h"
#include "ComputeRayLength_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    ComputeRayLengthKernel::ComputeRayLengthKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMapped<Image>* rayLengthImage)
        : ComputeKernel(framework, ComputeRayLength_kernel_SourceCode, "execute", "ComputeRayLengthKernel")
        , geometricSetup(geometricSetup)
        , rayLengthImage(rayLengthImage)
    {
    }

    ComputeRayLengthKernel::~ComputeRayLengthKernel()
    {
    }

    void ComputeRayLengthKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", geometricSetup->getResolution());
        implementation->setArgument("ray_length", rayLengthImage);
        implementation->setArgument("projectionGeometry", geometricSetup->getProjectionVariableContainer());
        implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
    }

    void ComputeRayLengthKernel::preRun()
    {
        implementation->setGlobalWorkSize(geometricSetup->getResolution());
    }

    void ComputeRayLengthKernel::postRun()
    {
        rayLengthImage->markAsChangedOnGPU();
    }

    GPUMapped<Image>* ComputeRayLengthKernel::getOutput() const
    {
        return rayLengthImage;
    }

    void ComputeRayLengthKernel::setRayLengthImage(GPUMapped<Image>* rayLengthImage)
    {
        this->rayLengthImage = rayLengthImage;
    }
}