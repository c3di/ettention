#include "stdafx.h"
#include "GlobalMaximumPositionImageKernel.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    GlobalMaximumPositionImageKernel::GlobalMaximumPositionImageKernel(Framework* framework, GPUMapped<Image>* sourceImage)
        : GlobalMaximumPositionInterface(framework, sourceImage->getBufferOnGPU(), sourceImage->getResolution().x * sourceImage->getResolution().y)
        , resolution(sourceImage->getResolution())
    {
        setInputImage(sourceImage);
        sourceImage->ensureIsUpToDateOnGPU();
    }

    GlobalMaximumPositionImageKernel::~GlobalMaximumPositionImageKernel()
    {
    }

    void GlobalMaximumPositionImageKernel::setInputImage(GPUMapped<Image>* sourceImage)
    {
        if( sourceImage->getResolution() != resolution )
            throw Exception("Cannot assign as input Image of different resolution!");
        this->sourceImage = sourceImage;
    }

    Vec2ui GlobalMaximumPositionImageKernel::getMaximumXY() const
    {
        return Vec2ui(maximumPosition % resolution.x, maximumPosition / resolution.x);
    }
}