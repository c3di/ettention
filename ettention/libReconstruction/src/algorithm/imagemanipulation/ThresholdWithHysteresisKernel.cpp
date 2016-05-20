#include "stdafx.h"
#include "ThresholdWithHysteresisKernel.h"
#include "ThresholdWithHysteresis_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "model/image/Image.h"

namespace ettention
{
    ThresholdWithHysteresisKernel::ThresholdWithHysteresisKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, ThresholdWithHysteresis_kernel_SourceCode, "computeThresholdWithHysteresis", "ThresholdWithHysteresisKernel", "-D THRESHOLDING_FLOORVALUE=0.0f -D THRESHOLDING_CEILVALUE=1.0f")
        , floorLevel(0.0f)
        , ceilingLevel(1.0f)
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    ThresholdWithHysteresisKernel::ThresholdWithHysteresisKernel(Framework* framework, GPUMapped<Image>* source)
        : ImageProcessingKernel(framework, source->getResolution(), ThresholdWithHysteresis_kernel_SourceCode, "computeThresholdWithHysteresis", "ThresholdWithHysteresisKernel", "-D THRESHOLDING_FLOORVALUE=0.0f -D THRESHOLDING_CEILVALUE=1.0f")
        , floorLevel(0.0f)
        , ceilingLevel(1.0f)
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    ThresholdWithHysteresisKernel::~ThresholdWithHysteresisKernel()
    {
    }

    void ThresholdWithHysteresisKernel::prepareKernelArguments()
    {
        implementation->setArgument("src",          source);
        implementation->setArgument("result",       getOutput());
        implementation->setArgument("sizeX",   (int)projectionResolution.x);
        implementation->setArgument("sizeY",   (int)projectionResolution.y);
        implementation->setArgument("floorLevel",   floorLevel);
        implementation->setArgument("ceilingLevel", ceilingLevel);
    }

    void ThresholdWithHysteresisKernel::setThreshold(float floorLevel, float ceilingLevel)
    {
        this->floorLevel = floorLevel;
        this->ceilingLevel = ceilingLevel;
    }

    void ThresholdWithHysteresisKernel::setInput(GPUMapped<Image>* source)
    {
        if( source->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, source->getResolution().xy(), "ThresholdWithHysteresisKernel::setInput");

        this->source = source;
    }

    GPUMapped<Image> *ThresholdWithHysteresisKernel::getInput() const
    {
        return source;
    }

    float ThresholdWithHysteresisKernel::getFloorThreshold() const
    {
        return floorLevel;
    }

    float ThresholdWithHysteresisKernel::getCeilingThreshold() const
    {
        return ceilingLevel;
    }
}