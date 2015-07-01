#include "stdafx.h"
#include "ConvolutionRealSpaceImplementation.h"
#include "Convolution_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    ConvolutionRealSpaceImplementation::ConvolutionRealSpaceImplementation(Framework* framework, GPUMapped<Image>* kernel, GPUMapped<Image>* source)
        : ComputeKernel(framework, Convolution_kernel_SourceCode, "computeConvolution", "ConvolutionRealSpaceImplementation")
        , projectionResolution(source->getResolution().xy())
    {
        allocateOutput();
        setInputs(kernel, source, false);
    }

    ConvolutionRealSpaceImplementation::~ConvolutionRealSpaceImplementation()
    {
        releaseOutput();
    }

    void ConvolutionRealSpaceImplementation::allocateOutput()
    {
        ownsOutput = true;
        result = new GPUMapped<Image>(abstractionLayer, projectionResolution);
    }

    void ConvolutionRealSpaceImplementation::releaseOutput()
    {
        if(ownsOutput)
        {
            delete result;
        }
    }

    void ConvolutionRealSpaceImplementation::prepareKernelArguments()
    {
        implementation->setArgument("convolutionKernel", this->kernel);
        implementation->setArgument("source", this->source);
        implementation->setArgument("result", this->result);
        implementation->setArgument("kernelSizeX", (int)this->kernelResolution.x);
        implementation->setArgument("kernelSizeY", (int)this->kernelResolution.y);
        implementation->setArgument("sourceSizeX", (int)this->projectionResolution.x);
        implementation->setArgument("sourceSizeY", (int)this->projectionResolution.y);
        implementation->setArgument("weightSum", this->weightsSum);
    }

    void ConvolutionRealSpaceImplementation::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution);
        implementation->setGlobalWorkSize(dim);
    }

    void ConvolutionRealSpaceImplementation::postRun()
    {
        this->result->markAsChangedOnGPU();
    }

    void ConvolutionRealSpaceImplementation::setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting)
    {
        kernel = buffer;
        kernelResolution = buffer->getResolution().xy();

        float sum = 0.0f;
        for(unsigned int j = 0; j < kernelResolution.y; ++j)
        {
            for(unsigned int i = 0; i < kernelResolution.x; ++i)
            {
                sum += kernel->getObjectOnCPU()->getPixel(i, j);
            }
        }
        weightsSum = sum;
    }

    void ConvolutionRealSpaceImplementation::setInput(GPUMapped<Image>* buffer)
    {
        if(buffer->getResolution().xy() != projectionResolution)
            throw ImageResolutionDiffersException(projectionResolution, buffer->getResolution(), "ConvolutionRealSpaceImplementation::setInputSource");

        source = buffer;
    }

    void ConvolutionRealSpaceImplementation::setOutput(GPUMapped<Image>* buffer)
    {
        releaseOutput();
        result = buffer;
        ownsOutput = false;
    }

    float ConvolutionRealSpaceImplementation::getWeightsSum() const
    {
        return weightsSum;
    }

    GPUMapped<Image>* ConvolutionRealSpaceImplementation::getResult()
    {
        return result;
    }

    void ConvolutionRealSpaceImplementation::execute()
    {
        run();
    }

}