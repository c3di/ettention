#include "stdafx.h"
#include "MultiplyKernel.h"
#include "Multiply_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    MultiplyKernel::MultiplyKernel(Framework* framework, GPUMapped<Image>* imageA, GPUMapped<Image>* imageB)
        : ComputeKernel(framework, Multiply_kernel_SourceCode, "multiply", "MultiplyKernel")
        , imageA(imageA)
        , imageB(imageB)
    {
        if(imageA->getResolution() != imageB->getResolution())
            throw Exception("cannot multiply images of different resolution");
        result = new GPUMapped<Image>(abstractionLayer, imageA->getResolution());
    }

    MultiplyKernel::~MultiplyKernel()
    {
        delete result;
    }

    void MultiplyKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", imageA->getResolution());
        imageA->ensureIsUpToDateOnGPU();
        implementation->setArgument("realProjection", imageA);
        imageB->ensureIsUpToDateOnGPU();
        implementation->setArgument("virtualProjection", imageB);
        implementation->setArgument("residual", result);
    }

    void MultiplyKernel::preRun()
    {
        implementation->setGlobalWorkSize(imageA->getResolution());
    }

    void MultiplyKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    GPUMapped<Image>* MultiplyKernel::getOutput() const
    {
        return result;
    }
}