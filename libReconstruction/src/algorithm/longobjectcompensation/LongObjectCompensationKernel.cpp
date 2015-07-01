#include "stdafx.h"
#include "LongObjectCompensationKernel.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    LongObjectCompensationKernel::LongObjectCompensationKernel(Framework* framework, const std::string &kernelSource, const std::string& kernelName, const std::string& kernelCaption, GPUMapped<Image>* traversalLength)
        : ComputeKernel(framework, kernelSource, kernelName, kernelCaption)
        , projection(projection)
        , traversalLength(traversalLength)
    {
    }

    LongObjectCompensationKernel::~LongObjectCompensationKernel()
    {
    }

    void LongObjectCompensationKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", projection->getResolution());
        implementation->setArgument("projection", projection);
        implementation->setArgument("travel_length", traversalLength);
        implementation->setArgument("constantLength", constantLength);
    }

    void LongObjectCompensationKernel::preRun()
    {
        implementation->setGlobalWorkSize(projection->getResolution());
    }

    void LongObjectCompensationKernel::postRun()
    {
        projection->markAsChangedOnGPU();
        traversalLength->markAsChangedOnGPU();
    }

    void LongObjectCompensationKernel::setConstantLength(float constantLength)
    {
        this->constantLength = constantLength;
    }

    GPUMapped<Image>* LongObjectCompensationKernel::getProjection() const
    {
        return projection;
    }

    void LongObjectCompensationKernel::setProjection(GPUMapped<Image>* projection)
    {
        this->projection = projection;
    }

    void LongObjectCompensationKernel::setTraversalLength(GPUMapped<Image>* traversalLength)
    {
        this->traversalLength = traversalLength;
    }
}