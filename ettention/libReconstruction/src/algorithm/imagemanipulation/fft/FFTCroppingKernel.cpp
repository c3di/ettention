#include "stdafx.h"
#include "FFTCroppingKernel.h"
#include "CroppingForFFT_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    FFTCroppingKernel::FFTCroppingKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer)
        : ImageProcessingKernel(framework, outputContainer, CroppingForFFT_kernel_SourceCode, "computeCropFromFourierSpace", "FFTCroppingKernel")
        , outputResolution(outputContainer->getResolution())
    {
        setInput(source);
    }

    FFTCroppingKernel::FFTCroppingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& outputResolution)
        : ImageProcessingKernel(framework, outputResolution, CroppingForFFT_kernel_SourceCode, "computeCropFromFourierSpace", "FFTCroppingKernel")
        , outputResolution(outputResolution)
    {
        setInput(source);
    }

    FFTCroppingKernel::~FFTCroppingKernel()
    {
    }

    void FFTCroppingKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", getOutput());
        implementation->setArgument("currentSizeX", (int)source->getResolution().x);
        implementation->setArgument("currentSizeY", (int)source->getResolution().y);
        implementation->setArgument("originalSizeX", (int)outputResolution.x);
        implementation->setArgument("originalSizeY", (int)outputResolution.y);
    }

    void FFTCroppingKernel::setInput(GPUMapped<Image>* source)
    {
        this->source = source;
    }
}