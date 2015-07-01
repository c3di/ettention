#include "stdafx.h"
#include "PSFShiftingKernel.h"
#include "MoveOriginToCenter_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    PSFShiftingKernel::PSFShiftingKernel(Framework* framework, GPUMapped<Image>* source)
        : ComputeKernel(framework, MoveOriginToCenter_kernel_SourceCode, "computeShiftedToCenterOrigin", "PSFShiftingKernel")
    {
        setInput(source);
        createOutputBuffer();
    }

    PSFShiftingKernel::~PSFShiftingKernel()
    {
        deleteOutputBuffer();
    }

    void PSFShiftingKernel::createOutputBuffer()
    {
        result = new GPUMapped<Image>(abstractionLayer, source->getResolution());
    }

    void PSFShiftingKernel::deleteOutputBuffer()
    {
        delete result;
    }

    void PSFShiftingKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", result);
        implementation->setArgument("sizeX", (int)source->getResolution().x);
        implementation->setArgument("sizeY", (int)source->getResolution().y);
    }

    void PSFShiftingKernel::preRun()
    {
        unsigned int dimX = NumericalAlgorithms::nearestPowerOfTwo(source->getResolution().x);
        unsigned int dimY = NumericalAlgorithms::nearestPowerOfTwo(source->getResolution().y);
        implementation->setGlobalWorkSize(dimX, dimY);
    }

    void PSFShiftingKernel::postRun()
    {
        this->result->markAsChangedOnGPU();
    }

    void PSFShiftingKernel::setInput(GPUMapped<Image>* source)
    {
        this->source = source;
    }

    GPUMapped<Image> *PSFShiftingKernel::getResult() const
    {
        return result;
    }
}