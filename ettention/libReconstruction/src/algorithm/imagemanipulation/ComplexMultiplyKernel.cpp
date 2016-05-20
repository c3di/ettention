#include "stdafx.h"
#include "ComplexMultiplyKernel.h"
#include "ComplexMultiply_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    ComplexMultiplyKernel::ComplexMultiplyKernel(Framework* framework, GPUMapped<Image>* imageAReal, GPUMapped<Image>* imageAImaginary, GPUMapped<Image>* imageBReal, GPUMapped<Image>* imageBImaginary)
        : ComputeKernel(framework, ComplexMultiply_kernel_SourceCode, "multiply", "ComplexMultiplyKernel")
        , imageAReal(imageAReal)
        , imageAImaginary(imageAImaginary)
        , imageBReal(imageBReal)
        , imageBImaginary(imageBImaginary)
    {
        createOutputBuffer();
    }

    ComplexMultiplyKernel::~ComplexMultiplyKernel()
    {
        deleteOutputBuffer();
    }

    void ComplexMultiplyKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", imageAReal->getResolution());
        implementation->setArgument("imageAReal", imageAReal);
        implementation->setArgument("imageAImaginary", imageAImaginary);
        implementation->setArgument("imageBReal", imageBReal);
        implementation->setArgument("imageBImaginary", imageBImaginary);
        implementation->setArgument("resultReal", resultReal);
        implementation->setArgument("resultImaginary", resultImaginary);
    }

    void ComplexMultiplyKernel::preRun()
    {
        implementation->setGlobalWorkSize(imageAReal->getResolution());
    }

    void ComplexMultiplyKernel::postRun()
    {
        resultImaginary->markAsChangedOnGPU();
        resultReal->markAsChangedOnGPU();
        resultImaginary->ensureIsUpToDateOnCPU();
        resultReal->ensureIsUpToDateOnCPU();
    }

    GPUMapped<Image>* ComplexMultiplyKernel::getRealOutput() const
    {
        return resultReal;
    }

    GPUMapped<Image>* ComplexMultiplyKernel::getImaginaryOutput() const
    {
        return resultImaginary;
    }

    void ComplexMultiplyKernel::createOutputBuffer()
    {
        resultReal = new GPUMapped<Image>(abstractionLayer, imageAReal->getResolution().xy());
        resultImaginary = new GPUMapped<Image>(abstractionLayer, imageAImaginary->getResolution().xy());
    }

    void ComplexMultiplyKernel::deleteOutputBuffer()
    {
        delete resultImaginary;
        delete resultReal;
    }
}