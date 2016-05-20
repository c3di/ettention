#include "stdafx.h"
#include "ErosionKernel.h"
#include "Erosion_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include "framework/error/ImageResolutionDiffersException.h"

namespace ettention
{
    ErosionKernel::ErosionKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, Erosion_kernel_SourceCode, "computeErosion", "ErosionKernel")
        , projectionResolution(source->getResolution().xy())
        , structureResolution(structureElement->getResolution().xy())
        , structureElement(structureElement)
    {
        setInput(source);
    }

    ErosionKernel::ErosionKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement)
        : ImageProcessingKernel(framework, source->getResolution(), Erosion_kernel_SourceCode, "computeErosion", "ErosionKernel")
        , projectionResolution(source->getResolution().xy())
        , structureResolution(structureElement->getResolution().xy())
        , structureElement(structureElement)
    {
        setInput(source);
    }

    ErosionKernel::~ErosionKernel()
    {
    }

    void ErosionKernel::prepareKernelArguments()
    {
        implementation->setArgument("src",               this->source);
        implementation->setArgument("structElement",     this->structureElement);
        implementation->setArgument("result",            getOutput());
        implementation->setArgument("sizeX",        (int)this->projectionResolution.x);
        implementation->setArgument("sizeY",        (int)this->projectionResolution.y);
        implementation->setArgument("structWidth",  (int)this->structureResolution.x);
        implementation->setArgument("structHeight", (int)this->structureResolution.y);
    }

    void ErosionKernel::setInput(GPUMapped<Image>* source)
    {
        if( source->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, source->getResolution().xy(), "ErosionKernel::setInput");

        this->source = source;
    }

    GPUMapped<Image> *ErosionKernel::getInput() const
    {
        return source;
    }
}