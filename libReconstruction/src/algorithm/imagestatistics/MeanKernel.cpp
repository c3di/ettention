#include "stdafx.h"
#include "MeanKernel.h"
#include "Mean_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    MeanKernel::MeanKernel(Framework* framework, GPUMappedVector* source)
        : ComputeKernel(framework, Mean_kernel_SourceCode, "computeMean", "MeanKernel")
    {
        setInput(source);
        createOutputBuffer();
    }

    MeanKernel::~MeanKernel()
    {
        deleteOutputBuffer();
    }

    void MeanKernel::setInput(GPUMappedVector* buffer)
    {
        source = buffer;
    }

    void MeanKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", result);
        implementation->setArgument("size", (int)source->getObjectOnCPU()->size());
    }

    void MeanKernel::preRun()
    {
        implementation->setGlobalWorkSize(1);
    }

    void MeanKernel::postRun()
    {
        result->markAsChangedOnGPU();
        result->ensureIsUpToDateOnCPU();
        mean = result->getObjectOnCPU()->at(0);
    }

    float MeanKernel::getMean() const
    {
        return mean;
    }

    void MeanKernel::createOutputBuffer()
    {
        result = new GPUMappedVector(this->abstractionLayer, (unsigned int)source->getObjectOnCPU()->size());
    }

    void MeanKernel::deleteOutputBuffer()
    {
        delete result;
    }
}