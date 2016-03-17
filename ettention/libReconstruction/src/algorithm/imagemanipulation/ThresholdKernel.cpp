#include "stdafx.h"
#include "ThresholdKernel.h"
#include "Threshold_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "model/image/Image.h"

namespace ettention
{
    ThresholdKernel::ThresholdKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, Threshold_kernel_SourceCode, "computeThreshold", "ThresholdKernel")
        , thresholdValue(0.0f)
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    ThresholdKernel::ThresholdKernel(Framework* framework, GPUMapped<Image>* source)
        : ImageProcessingKernel(framework, source->getResolution(), Threshold_kernel_SourceCode, "computeThreshold", "ThresholdKernel")
        , thresholdValue(0.0f)
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    ThresholdKernel::~ThresholdKernel()
    {
    }

    void ThresholdKernel::prepareKernelArguments()
    {
        implementation->setArgument("src",        source);
        implementation->setArgument("result",     getOutput());
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
        implementation->setArgument("threshold",  thresholdValue);
    }

    void ThresholdKernel::setThreshold(float value)
    {
        thresholdValue = value;
    }

    void ThresholdKernel::setInput(GPUMapped<Image>* source)
    {
        if( source->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, source->getResolution().xy(), "ThresholdKernel::setInput");

        this->source = source;
    }

    GPUMapped<Image> *ThresholdKernel::getInput() const
    {
        return source;
    }

    float ThresholdKernel::getThreshold() const
    {
        return thresholdValue;
    }
}