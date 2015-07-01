#include "stdafx.h"
#include "LocalMaximumInterface.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    LocalMaximumInterface::LocalMaximumInterface(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaResolution, const std::string& kernelCode, const std::string& kernelName, const std::string& kernelCaption)
        : ComputeKernel(framework, kernelCode, kernelName, kernelCaption)
        , localResolution(localAreaResolution)
    {
        setInput(source);
        createOutputBuffer();
    }

    LocalMaximumInterface::~LocalMaximumInterface()
    {
        deleteOutputBuffer();
    }

    void LocalMaximumInterface::createOutputBuffer()
    {
        result = new GPUMapped<Image>(abstractionLayer, source->getResolution());
    }

    void LocalMaximumInterface::deleteOutputBuffer()
    {
        delete result;
    }

    void LocalMaximumInterface::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", result);
        implementation->setArgument("sizeX", (int)source->getResolution().x);
        implementation->setArgument("sizeY", (int)source->getResolution().y);
        implementation->setArgument("width", (int)localResolution.x);
        implementation->setArgument("height", (int)localResolution.y);
    }

    void LocalMaximumInterface::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(source->getResolution());
        implementation->setGlobalWorkSize(dim);
    }

    void LocalMaximumInterface::postRun()
    {
        this->result->markAsChangedOnGPU();
    }

    void LocalMaximumInterface::setInput(GPUMapped<Image> *source)
    {
        this->source = source;
    }

    GPUMapped<Image> *LocalMaximumInterface::getSource() const
    {
        return source;
    }

    GPUMapped<Image> *LocalMaximumInterface::getResult() const
    {
        return result;
    }
}