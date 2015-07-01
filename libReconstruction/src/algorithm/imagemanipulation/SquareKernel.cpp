#include "stdafx.h"
#include "SquareKernel.h"
#include "Square_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    SquareKernel::SquareKernel(Framework* framework, GPUMapped<Image>* source)
        : ComputeKernel(framework, Square_kernel_SourceCode, "computeSquare", "SquareKernel")
        , projectionResolution(source->getResolution().xy())
    {
        createOutputBuffer();
        setInput(source);
    }

    SquareKernel::~SquareKernel()
    {
        deleteOutputBuffer();
    }

    void SquareKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", result);
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
    }

    void SquareKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution);
        implementation->setGlobalWorkSize(dim);
    }

    void SquareKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    void SquareKernel::createOutputBuffer()
    {
        result = new GPUMapped<Image>(abstractionLayer, projectionResolution);
    }

    void SquareKernel::deleteOutputBuffer()
    {
        delete result;
    }

    void SquareKernel::setInput(GPUMapped<Image>* buffer)
    {
        if(buffer->getResolution().xy() != projectionResolution)
            throw ImageResolutionDiffersException(projectionResolution, buffer->getResolution().xy(), "SquareKernel::setInput");

        source = buffer;
    }

    GPUMapped<Image>* SquareKernel::getResult() const
    {
        return result;
    }

    GPUMapped<Image>* SquareKernel::getInput() const
    {
        return source;
    }
}