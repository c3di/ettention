#include "stdafx.h"
#include "ThresholdKernel.h"
#include "Threshold_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "model/image/Image.h"

namespace ettention
{
    ThresholdKernel::ThresholdKernel(Framework* framework, GPUMapped<Image>* source)
        : ComputeKernel(framework, Threshold_kernel_SourceCode, "computeThreshold", "ThresholdKernel")
        , thresholdValue(0.0f)
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
        createOutputBuffer();
    }

    ThresholdKernel::~ThresholdKernel()
    {
        deleteOutputBuffer();
    }

    void ThresholdKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", this->source);
        implementation->setArgument("result", this->result);
        implementation->setArgument("sizeX", (int)this->projectionResolution.x);
        implementation->setArgument("sizeY", (int)this->projectionResolution.y);
        implementation->setArgument("threshold", this->thresholdValue);
    }

    void ThresholdKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution);
        implementation->setGlobalWorkSize(dim);
    }

    void ThresholdKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    void ThresholdKernel::createOutputBuffer()
    {
        result = new GPUMapped<Image>(abstractionLayer, projectionResolution);
    }

    void ThresholdKernel::deleteOutputBuffer()
    {
        delete result;
    }

    void ThresholdKernel::setThreshold(float value)
    {
        thresholdValue = value;
    }

    void ThresholdKernel::setInput(GPUMapped<Image>* buffer)
    {
        if(buffer->getResolution().xy() != projectionResolution)
            throw ImageResolutionDiffersException(projectionResolution, buffer->getResolution().xy(), "ThresholdKernel::setInput");

        source = buffer;
    }

    GPUMapped<Image> *ThresholdKernel::getResult() const
    {
        return result;
    }

    float ThresholdKernel::getThreshold() const
    {
        return thresholdValue;
    }
}