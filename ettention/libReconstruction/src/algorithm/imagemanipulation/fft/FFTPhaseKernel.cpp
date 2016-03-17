#include "stdafx.h"
#include "FFTPhaseKernel.h"
#include "FFTPhase_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    FFTPhaseKernel::FFTPhaseKernel(Framework* framework, GPUMapped<Image>* imageReal, GPUMapped<Image>* imageImaginary)
        : ComputeKernel(framework, FFTPhase_kernel_SourceCode, "computePhase", "FFTPhaseKernel")
        , imageReal(imageReal)
        , imageImaginary(imageImaginary)
    {
        createOutputBuffer();
    }

    FFTPhaseKernel::~FFTPhaseKernel()
    {
        deleteOutputBuffer();
    }

    void FFTPhaseKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", imageReal->getResolution());
        implementation->setArgument("imageReal", imageReal);
        implementation->setArgument("imageImaginary", imageImaginary);
        implementation->setArgument("resultPhase", resultPhase);
    }

    void FFTPhaseKernel::preRun()
    {
        implementation->setGlobalWorkSize(imageReal->getResolution());
    }

    void FFTPhaseKernel::postRun()
    {
        this->resultPhase->markAsChangedOnGPU();
        this->resultPhase->ensureIsUpToDateOnCPU();
    }

    GPUMapped<Image>* FFTPhaseKernel::getOutput() const
    {
        return resultPhase;
    }

    void FFTPhaseKernel::createOutputBuffer()
    {
        resultPhase = new GPUMapped<Image>(abstractionLayer, imageReal->getResolution());
    }

    void FFTPhaseKernel::deleteOutputBuffer()
    {
        delete resultPhase;
    }
}