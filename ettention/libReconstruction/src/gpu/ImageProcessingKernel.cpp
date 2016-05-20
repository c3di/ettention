#include "stdafx.h"
#include "ImageProcessingKernel.h"

#include "gpu/opencl/CLKernel.h"
#include "framework/Framework.h"
#include "framework/math/Vec2.h"
#include "framework/error/NoMemoryAllocatedForOutputException.h"

namespace ettention
{
    ImageProcessingKernel::ImageProcessingKernel(Framework* framework, GPUMapped<Image> *outputContainer, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, const std::string& additionalKernelDefines)
        : ComputeKernel(framework, sourceCode, kernelName, kernelCaption, additionalKernelDefines)
    {
        setResultContainer(outputContainer);
    }

    ImageProcessingKernel::ImageProcessingKernel(Framework* framework, const Vec2ui& outputSize, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, const std::string& additionalKernelDefines)
        : ComputeKernel(framework, sourceCode, kernelName, kernelCaption, additionalKernelDefines)
    {
        allocateResultContainer(outputSize);
    }

    ImageProcessingKernel::~ImageProcessingKernel()
    {
        releaseResultContainer();
    }

    void ImageProcessingKernel::setResultContainer(GPUMapped<Image>* resultContainer)
    {
        if(resultContainer != this->resultContainer)
        {
            releaseResultContainer();
            this->resultContainer = resultContainer;
            ownResultContainer = false;
        }
    }

    void ImageProcessingKernel::allocateResultContainer(Vec2ui size)
    {
        resultContainer = new GPUMapped<Image>(abstractionLayer, size);
        ownResultContainer = true;
    }

    void ImageProcessingKernel::releaseResultContainer()
    {
        if( ownResultContainer )
            delete resultContainer;

        ownResultContainer = false;
    }

    void ImageProcessingKernel::preRun()
    {
        if( resultContainer == nullptr )
        {
            throw NoMemoryAllocatedForOutputException("ImageProcessingKernel::preRun");
        }

        implementation->setGlobalWorkSize(resultContainer->getResolution());
    }

    void ImageProcessingKernel::postRun()
    {
        resultContainer->markAsChangedOnGPU();
    }

    GPUMapped<Image> *ImageProcessingKernel::getOutput() const
    {
        return resultContainer;
    }
}