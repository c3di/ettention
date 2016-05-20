#include "stdafx.h"
#include "SquareKernel.h"
#include "Square_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    SquareKernel::SquareKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, Square_kernel_SourceCode, "computeSquare", "SquareKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    SquareKernel::SquareKernel(Framework* framework, GPUMapped<Image>* source)
        : ImageProcessingKernel(framework, source->getResolution(), Square_kernel_SourceCode, "computeSquare", "SquareKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    SquareKernel::~SquareKernel()
    {
    }

    void SquareKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", getOutput());
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
    }

    void SquareKernel::setInput(GPUMapped<Image>* buffer)
    {
        if(buffer->getResolution().xy() != projectionResolution)
            throw ImageResolutionDiffersException(projectionResolution, buffer->getResolution().xy(), "SquareKernel::setInput");

        source = buffer;
    }

    GPUMapped<Image>* SquareKernel::getInput() const
    {
        return source;
    }
}