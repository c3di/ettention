#include "stdafx.h"
#include "LocalMaximumInterface.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    LocalMaximumInterface::LocalMaximumInterface(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer, const Vec2ui& localAreaSize, const std::string& kernelCode, const std::string& kernelName, const std::string& kernelCaption)
        : ImageProcessingKernel(framework, outputContainer, kernelCode, kernelName, kernelCaption)
        , localAreaSize(localAreaSize)
    {
        setInput(source);
    }

    LocalMaximumInterface::LocalMaximumInterface(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaSize, const std::string& kernelCode, const std::string& kernelName, const std::string& kernelCaption)
        : ImageProcessingKernel(framework, source->getResolution(), kernelCode, kernelName, kernelCaption)
        , localAreaSize(localAreaSize)
    {
        setInput(source);
    }

    LocalMaximumInterface::~LocalMaximumInterface()
    {
    }

    void LocalMaximumInterface::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", getOutput());
        implementation->setArgument("sizeX", (int)source->getResolution().x);
        implementation->setArgument("sizeY", (int)source->getResolution().y);
        implementation->setArgument("width", (int)localAreaSize.x);
        implementation->setArgument("height", (int)localAreaSize.y);
    }

    void LocalMaximumInterface::setInput(GPUMapped<Image> *source)
    {
        this->source = source;
    }

    GPUMapped<Image> *LocalMaximumInterface::getSource() const
    {
        return source;
    }
}