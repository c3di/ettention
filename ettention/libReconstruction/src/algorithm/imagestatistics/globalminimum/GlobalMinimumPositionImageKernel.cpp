#include "stdafx.h"
#include "GlobalMinimumPositionImageKernel.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    GlobalMinimumPositionImageKernel::GlobalMinimumPositionImageKernel(Framework* framework, GPUMapped<Image>* sourceImage)
        : GlobalMinimumPositionInterface(framework, sourceImage->getBufferOnGPU(), sourceImage->getResolution().x * sourceImage->getResolution().y)
        , resolution(sourceImage->getResolution())
    {
        setInputImage(sourceImage);
        sourceImage->ensureIsUpToDateOnGPU();
    }

    GlobalMinimumPositionImageKernel::~GlobalMinimumPositionImageKernel()
    {
    }

    void GlobalMinimumPositionImageKernel::setInputImage(GPUMapped<Image>* sourceImage)
    {
        if( sourceImage->getResolution() != resolution )
            throw Exception("Cannot assign as input Image of different resolution!");
        this->sourceImage = sourceImage;
    }

    Vec2ui GlobalMinimumPositionImageKernel::getMinimumXY() const
    {
        return Vec2ui(minimumPosition % resolution.x, minimumPosition / resolution.x);
    }
}