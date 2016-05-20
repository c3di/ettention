#include "stdafx.h"
#include "VolumeBorderSegmentKernel.h"
#include "BorderSegmentVolume_bin2c.h"

#include "framework/Framework.h"
#include "framework/error/VolumeResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "gpu/GPUMapped.h"
#include "gpu/VectorAdaptor.h"

#include <algorithm>

namespace ettention
{
    const unsigned int VolumeBorderSegmentKernel::BORDER_VALUE{1};

    VolumeBorderSegmentKernel::VolumeBorderSegmentKernel(Framework* framework, GPUMappedVolume* sourceVolume)
        : ComputeKernel(framework, BorderSegmentVolume_kernel_SourceCode, "segmentBorderOnVolume", "VolumeBorderSegmentKernel", (boost::format("-D BORDERSEGMENTVOLUME_BORDER_VALUE=%1%") % VolumeBorderSegmentKernel::BORDER_VALUE).str())
        , sourceVolume(sourceVolume)
        , ownResultVolume(true)
    {
        resultVolume = new GPUMappedVolume(framework->getOpenCLStack(), new ByteVolume(sourceVolume->getProperties().getVolumeResolution(), 0.0f), true);
        resultVolume->takeOwnershipOfObjectOnCPU();
    }

    VolumeBorderSegmentKernel::VolumeBorderSegmentKernel(Framework *framework, GPUMappedVolume *sourceVolume, GPUMappedVolume *resultVolume)
        : ComputeKernel(framework, BorderSegmentVolume_kernel_SourceCode, "segmentBorderOnVolume", "VolumeBorderSegmentKernel", (boost::format("-D BORDERSEGMENTVOLUME_BORDER_VALUE=%1%") % VolumeBorderSegmentKernel::BORDER_VALUE).str())
        , sourceVolume(sourceVolume)
        , resultVolume(resultVolume)
        , ownResultVolume(false)
    {
    }

    VolumeBorderSegmentKernel::~VolumeBorderSegmentKernel()
    {
        if( ownResultVolume )
            delete resultVolume;
    }

    void VolumeBorderSegmentKernel::prepareKernelArguments()
    {
        sourceVolume->ensureIsUpToDateOnGPU();
        implementation->setArgument("source", sourceVolume);
        implementation->setArgument("result", resultVolume->getMappedSubVolume());
    }

    void VolumeBorderSegmentKernel::preRun()
    {
        implementation->setGlobalWorkSize(sourceVolume->getMappedSubVolume()->getResolution());
    }

    void VolumeBorderSegmentKernel::postRun()
    {
        resultVolume->markAsChangedOnGPU();
    }

    void VolumeBorderSegmentKernel::setInput(GPUMappedVolume *sourceVolume)
    {
        if( this->sourceVolume->getProperties().getVolumeResolution() != sourceVolume->getProperties().getVolumeResolution() )
            throw VolumeResolutionDiffersException(this->sourceVolume->getProperties().getVolumeResolution(), sourceVolume->getProperties().getVolumeResolution(), "VolumeBorderSegmentKernel::setInput");

        this->sourceVolume = sourceVolume;
    }

    void VolumeBorderSegmentKernel::setOutput(GPUMappedVolume* volume)
    {
        if( ownResultVolume && (resultVolume != volume) )
            delete resultVolume;

        this->resultVolume = volume;
    }

    GPUMappedVolume* VolumeBorderSegmentKernel::getOutput() const
    {
        return resultVolume;
    }
}