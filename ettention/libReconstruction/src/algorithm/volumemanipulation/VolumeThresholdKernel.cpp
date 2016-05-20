#include "stdafx.h"
#include "VolumeThresholdKernel.h"
#include "ThresholdMultiLevel_bin2c.h"

#include "framework/Framework.h"
#include "gpu/opencl/CLKernel.h"
#include "gpu/GPUMapped.h"
#include "gpu/VectorAdaptor.h"

#include <algorithm>

namespace ettention
{

    VolumeThresholdKernel::VolumeThresholdKernel(Framework *framework, GPUMappedVolume *sourceVolume, GPUMappedVolume *resultVolume, GPUMappedVector *floorLevels, GPUMappedVector *floorValues, GPUMappedVolume* maskVolume /*= nullptr*/)
        : ComputeKernel(framework, ThresholdMultiLevel_kernel_SourceCode, "thresholdMultiLevel", "VolumeThresholdKernel", "-D THRESHOLDING_CEILVALUE=1.0f")
        , sourceVolume(sourceVolume)
        , resultVolume(resultVolume)
        , maskVolume(maskVolume)
        , thresholdFloorLevels(floorLevels)
        , thresholdFloorValues(floorValues)
        , thresholdLevelCount(floorLevels->getResolution())
    {
    }

    VolumeThresholdKernel::~VolumeThresholdKernel()
    {
    }

    void VolumeThresholdKernel::prepareKernelArguments()
    {
        sourceVolume->getMappedSubVolume()->ensureImageIsUpToDate();
        thresholdFloorLevels->ensureIsUpToDateOnGPU();
        thresholdFloorValues->ensureIsUpToDateOnGPU();
    }

    void VolumeThresholdKernel::preRun()
    {
        implementation->setGlobalWorkSize(sourceVolume->getMappedSubVolume()->getResolution());
        implementation->setArgument("source", sourceVolume->getMappedSubVolume()->getImageOnGPU());         // passed as texture (image)
        implementation->setArgument("result", resultVolume->getMappedSubVolume()->getBufferOnGPU());        // passed as floats buffer
        implementation->setArgument("floorLevels", thresholdFloorLevels);
        implementation->setArgument("floorValues", thresholdFloorValues);
        implementation->setArgument("numberOfLevels", thresholdLevelCount);
    }

    void VolumeThresholdKernel::postRun()
    {
        resultVolume->getMappedSubVolume()->markAsChangedOnGPU();
    }

    void VolumeThresholdKernel::setInput(GPUMappedVolume *sourceVolume)
    {
        this->sourceVolume = sourceVolume;
    }

    GPUMappedVolume* VolumeThresholdKernel::getOutput() const
    {
        return resultVolume;
    }
}