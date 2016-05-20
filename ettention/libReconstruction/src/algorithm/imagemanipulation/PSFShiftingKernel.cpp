#include "stdafx.h"
#include "PSFShiftingKernel.h"
#include "MoveOriginToCenter_bin2c.h"

#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    PSFShiftingKernel::PSFShiftingKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, MoveOriginToCenter_kernel_SourceCode, "computeShiftedToCenterOrigin", "PSFShiftingKernel")
    {
        setInput(source);
    }

    PSFShiftingKernel::PSFShiftingKernel(Framework* framework, GPUMapped<Image>* source)
        : ImageProcessingKernel(framework, source->getResolution(), MoveOriginToCenter_kernel_SourceCode, "computeShiftedToCenterOrigin", "PSFShiftingKernel")
    {
        setInput(source);
    }

    PSFShiftingKernel::~PSFShiftingKernel()
    {
    }

    void PSFShiftingKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", getOutput());
        implementation->setArgument("sizeX", (int)source->getResolution().x);
        implementation->setArgument("sizeY", (int)source->getResolution().y);
    }

    void PSFShiftingKernel::setInput(GPUMapped<Image>* source)
    {
        this->source = source;
    }
}