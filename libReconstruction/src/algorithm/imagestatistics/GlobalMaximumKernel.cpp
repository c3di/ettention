#include "stdafx.h"
#include "GlobalMaximumKernel.h"
#include "Maximum_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    GlobalMaximumKernel::GlobalMaximumKernel(Framework* framework, GPUMapped<Image>* source)
        : ComputeKernel(framework, Maximum_kernel_SourceCode, "computeMax", "GlobalMaximumKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
        createOutputBuffer();
    }

    GlobalMaximumKernel::~GlobalMaximumKernel()
    {
        deleteOutputBuffer();
    }

    void GlobalMaximumKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("colMaximumX", temporalBufferXpos);
        implementation->setArgument("colMaximumY", temporalBufferYpos);
        implementation->setArgument("maximumPosition", resultBufferPosition);
        implementation->setArgument("maximumValue", resultBufferValue);
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
    }

    void GlobalMaximumKernel::preRun()
    {
        unsigned int dimX = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution.x);
        implementation->setGlobalWorkSize(dimX);
    }

    void GlobalMaximumKernel::postRun()
    {
        resultBufferPosition->markAsChangedOnGPU();
        resultBufferValue->markAsChangedOnGPU();
        resultBufferPosition->ensureIsUpToDateOnCPU();
        resultBufferValue->ensureIsUpToDateOnCPU();
        maximumPosition.x = (unsigned int)resultBufferPosition->getObjectOnCPU()->at(0);
        maximumPosition.y = (unsigned int)resultBufferPosition->getObjectOnCPU()->at(1);
        maximum = (float)resultBufferValue->getObjectOnCPU()->at(0);
    }

    void GlobalMaximumKernel::setInput(GPUMapped<Image>* sourceImage)
    {
        if(sourceImage->getResolution().xy() != projectionResolution)
            throw Exception("Cannot assign as input Image of different resolution!");
        this->source = sourceImage;
    }

    const Vec2ui& GlobalMaximumKernel::getMaximumPosition() const
    {
        return maximumPosition;
    }

    float GlobalMaximumKernel::getMaximum() const
    {
        return maximum;
    }

    void GlobalMaximumKernel::createOutputBuffer()
    {
        temporalBufferXpos = new GPUMappedVector(abstractionLayer, projectionResolution.x);
        temporalBufferYpos = new GPUMappedVector(abstractionLayer, projectionResolution.x);
        resultBufferPosition = new GPUMappedIntVector(abstractionLayer, 2);
        resultBufferValue = new GPUMappedVector(abstractionLayer, 1);
    }

    void GlobalMaximumKernel::deleteOutputBuffer()
    {
        delete temporalBufferXpos;
        delete temporalBufferYpos;
        delete resultBufferPosition;
        delete resultBufferValue;
    }
}