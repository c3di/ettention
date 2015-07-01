#include "stdafx.h"
#include "FFTMagnitudeKernel.h"
#include "FFTMagnitude_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    FFTMagnitudeKernel::FFTMagnitudeKernel(Framework* framework, GPUMapped<Image>* imageReal, GPUMapped<Image>* imageImaginary)
        : ComputeKernel(framework, FFTMagnitude_kernel_SourceCode, "computeMagnitude", "FFTMagnitudeKernel")
        , imageReal(imageReal)
        , imageImaginary(imageImaginary)
    {
        createOutputBuffer();
    }

    FFTMagnitudeKernel::~FFTMagnitudeKernel()
    {
        deleteOutputBuffer();
    }

    void FFTMagnitudeKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", imageReal->getResolution());
        implementation->setArgument("imageReal", imageReal);
        implementation->setArgument("imageImaginary", imageImaginary);
        implementation->setArgument("resultMagnitude", resultMagnitude);
    }

    void FFTMagnitudeKernel::preRun()
    {
        implementation->setGlobalWorkSize(imageReal->getResolution());
    }

    void FFTMagnitudeKernel::postRun()
    {
        this->resultMagnitude->markAsChangedOnGPU();
    }

    GPUMapped<Image>* FFTMagnitudeKernel::getOutput() const
    {
        return resultMagnitude;
    }

    void FFTMagnitudeKernel::createOutputBuffer()
    {
        resultMagnitude = new GPUMapped<Image>(abstractionLayer, imageReal->getResolution().xy());
    }

    void FFTMagnitudeKernel::deleteOutputBuffer()
    {
        delete resultMagnitude;
    }
}