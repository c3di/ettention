#include "stdafx.h"
#include "FFTCroppingKernel.h"
#include "CroppingForFFT_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    FFTCroppingKernel::FFTCroppingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& outputResolution)
        : ComputeKernel(framework, CroppingForFFT_kernel_SourceCode, "computeCropFromFourierSpace", "FFTCroppingKernel")
        , outputResolution(outputResolution)
    {
        setInput(source);
        createOutputBuffer();
    }

    FFTCroppingKernel::~FFTCroppingKernel()
    {
        deleteOutputBuffer();
    }

    void FFTCroppingKernel::createOutputBuffer()
    {
        result = new GPUMapped<Image>(abstractionLayer, outputResolution);
        ownsOutput = true;
    }

    void FFTCroppingKernel::deleteOutputBuffer()
    {
        if(ownsOutput)
        {
            delete result;
        }
    }

    void FFTCroppingKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", result);
        implementation->setArgument("currentSizeX", (int)source->getResolution().x);
        implementation->setArgument("currentSizeY", (int)source->getResolution().y);
        implementation->setArgument("originalSizeX", (int)outputResolution.x);
        implementation->setArgument("originalSizeY", (int)outputResolution.y);
    }

    void FFTCroppingKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(source->getResolution());
        implementation->setGlobalWorkSize(dim);
    }

    void FFTCroppingKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    void FFTCroppingKernel::setInput(GPUMapped<Image>* input)
    {
        source = input;
    }

    void FFTCroppingKernel::setOutput(GPUMapped<Image>* output)
    {
        if(ownsOutput) 
        {
            delete result;
            ownsOutput = false;
        }
        result = output;
    }

    GPUMapped<Image> *FFTCroppingKernel::getResult() const
    {
        return result;
    }
}