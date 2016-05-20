#include "stdafx.h"
#include "DilationKernel.h"
#include "Dilation_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include "framework/error/ImageResolutionDiffersException.h"

namespace ettention
{
    DilationKernel::DilationKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, Dilation_kernel_SourceCode, "computeDilation", "DilationKernel")
        , projectionResolution(source->getResolution().xy())
        , structureResolution(structureElement->getResolution().xy())
        , structureElement(structureElement)
    {
        setInput(source);
    }

    DilationKernel::DilationKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement)
        : ImageProcessingKernel(framework, source->getResolution(), Dilation_kernel_SourceCode, "computeDilation", "DilationKernel")
        , projectionResolution(source->getResolution().xy())
        , structureResolution(structureElement->getResolution().xy())
        , structureElement(structureElement)
    {
        setInput(source);
    }

    DilationKernel::~DilationKernel()
    {
    }

    void DilationKernel::prepareKernelArguments()
    {
        implementation->setArgument("src",               this->source);
        implementation->setArgument("structElement",     this->structureElement);
        implementation->setArgument("result",            getOutput());
        implementation->setArgument("sizeX",        (int)this->projectionResolution.x);
        implementation->setArgument("sizeY",        (int)this->projectionResolution.y);
        implementation->setArgument("structWidth",  (int)this->structureResolution.x);
        implementation->setArgument("structHeight", (int)this->structureResolution.y);
    }

    void DilationKernel::setInput(GPUMapped<Image>* source)
    {
        if( source->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, source->getResolution().xy(), "DilationKernel::setInput");

        this->source = source;
    }

    GPUMapped<Image> *DilationKernel::getInput() const
    {
        return source;
    }
}