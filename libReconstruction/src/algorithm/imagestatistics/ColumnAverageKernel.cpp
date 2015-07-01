#include "stdafx.h"
#include "ColumnAverageKernel.h"
#include "ColumnAverage_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    ColumnAverageKernel::ColumnAverageKernel(Framework* framework, GPUMapped<Image>* source)
        : ComputeKernel(framework, ColumnAverage_kernel_SourceCode, "computeColumnAverage", "ColumnAverageKernel")
    {
        setInput(source);
        createOutputBuffer();
    }

    ColumnAverageKernel::~ColumnAverageKernel()
    {
        deleteOutputBuffer();
    }

    void ColumnAverageKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("colSumm", temporalBuffer);
        implementation->setArgument("sizeX", (int)source->getResolution().x);
        implementation->setArgument("sizeY", (int)source->getResolution().y);
    }

    void ColumnAverageKernel::preRun()
    {
        unsigned int dimX = NumericalAlgorithms::nearestPowerOfTwo(source->getResolution().x);
        implementation->setGlobalWorkSize(dimX);
    }

    void ColumnAverageKernel::postRun()
    {
        temporalBuffer->markAsChangedOnGPU();
    }

    void ColumnAverageKernel::setInput(GPUMapped<Image>* buffer)
    {
        source = buffer;
    }

    GPUMappedVector *ColumnAverageKernel::getColumnAverage() const
    {
        return temporalBuffer;
    }

    void ColumnAverageKernel::createOutputBuffer()
    {
        temporalBuffer = new GPUMappedVector(abstractionLayer, source->getResolution().x);
    }

    void ColumnAverageKernel::deleteOutputBuffer()
    {
        delete temporalBuffer;
    }
}