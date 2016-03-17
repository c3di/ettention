#include "stdafx.h"
#include "RMSKernel.h"
#include "BufferRMS_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    RMSKernel::RMSKernel(Framework* framework, GPUMapped<Image>* residual)
        : ComputeKernel(framework, BufferRMS_kernel_SourceCode, "compute", "RMSKernel")
        , residual(residual)
        , result(0)
    {
        ensureResultSizeMatchesInput();
    }

    RMSKernel::~RMSKernel()
    {
        delete result;
    }

    void RMSKernel::setInput(GPUMapped<Image>* input)
    {
        residual = input;
        ensureResultSizeMatchesInput();
    }

    void RMSKernel::ensureResultSizeMatchesInput()
    {
        if(result && (!residual || result->getResolution() != residual->getResolution().x))
        {
            delete result;
            result = 0;
        }
        if(!result && residual)
        {
            result = new GPUMappedVector((CLAbstractionLayer*)abstractionLayer, residual->getResolution().x);
        }
    }

    void RMSKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", residual->getResolution());
        implementation->setArgument("residual", residual);
        implementation->setArgument("result", result);
    }

    void RMSKernel::preRun()
    {
        unsigned int dimX = residual->getResolution().x;
        implementation->setGlobalWorkSize(dimX);
    }

    void RMSKernel::postRun()
    {
        result->markAsChangedOnGPU();
        result->ensureIsUpToDateOnCPU();
        meanSquare = result->getObjectOnCPU()->at(0);
        meanSquare /= (float)(residual->getResolution().x * residual->getResolution().y);
        rootMeanSquare = sqrtf(meanSquare);
    }

    float RMSKernel::getRootMeanSquare() const
    {
        return rootMeanSquare;
    }

    float RMSKernel::getMeanSquare() const
    {
        return meanSquare;
    }
}