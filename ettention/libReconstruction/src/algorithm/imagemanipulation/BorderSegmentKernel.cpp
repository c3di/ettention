#include "stdafx.h"
#include "BorderSegmentKernel.h"
#include "BorderSegmentation_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include "framework/error/ImageResolutionDiffersException.h"

namespace ettention
{
    BorderSegmentKernel::BorderSegmentKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, BorderSegmentation_kernel_SourceCode, "segmentBorderAndInternalArea", "BorderSegmentKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    BorderSegmentKernel::BorderSegmentKernel(Framework* framework, GPUMapped<Image>* source)
        : ImageProcessingKernel(framework, source->getResolution(), BorderSegmentation_kernel_SourceCode, "segmentBorderAndInternalArea", "BorderSegmentKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInput(source);
    }

    BorderSegmentKernel::~BorderSegmentKernel()
    {
    }

    void BorderSegmentKernel::prepareKernelArguments()
    {
        implementation->setArgument("src",        this->source);
        implementation->setArgument("result",     getOutput());
        implementation->setArgument("sizeX", (int)this->projectionResolution.x);
        implementation->setArgument("sizeY", (int)this->projectionResolution.y);
        implementation->setArgument("useAllEightNeighbors", this->useAllEightNeighbors ? 1 : 0);
    }

    void BorderSegmentKernel::setUsingEightNeighbors(bool useAllEightNeighbors)
    {
        this->useAllEightNeighbors = useAllEightNeighbors;
    }

    void BorderSegmentKernel::setInput(GPUMapped<Image>* source)
    {
        if( source->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, source->getResolution().xy(), "BorderSegmentKernel::setInput");

        this->source = source;
    }

    GPUMapped<Image> *BorderSegmentKernel::getInput() const
    {
        return source;
    }
}