#include "stdafx.h"
#include "SetToValueKernel.h"
#include "SetToValue_bin2c.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    SetToValueKernel::SetToValueKernel(Framework* framework, GPUMapped<SubVolume>* subVolume, float value)
        : ComputeKernel(framework, SetToValue_kernel_SourceCode, "setToValue", "SetToValueKernel")
        , subVolume(subVolume)
        , value(value)
    {
    }

    SetToValueKernel::~SetToValueKernel()
    {
    }

    void SetToValueKernel::prepareKernelArguments()
    {
        subVolume->ensureIsUpToDateOnGPU();
        implementation->setArgument("volume", subVolume);
        implementation->setArgument("value", value);
        implementation->setArgument("volumeResolutionX", subVolume->getResolution().x);
        implementation->setArgument("volumeResolutionY", subVolume->getResolution().y);
        implementation->setArgument("volumeResolutionZ", subVolume->getResolution().z);
    }

    void SetToValueKernel::preRun()
    {
        implementation->setGlobalWorkSize(subVolume->getResolution());
    }

    void SetToValueKernel::postRun()
    {
        subVolume->markAsChangedOnGPU();
    }

    void SetToValueKernel::setOutputSubvolume(GPUMapped<SubVolume>* subVolume)
    {
        this->subVolume = subVolume;
    }

    GPUMapped<SubVolume>* SetToValueKernel::getOutput() const
    {
        return subVolume;
    }
}