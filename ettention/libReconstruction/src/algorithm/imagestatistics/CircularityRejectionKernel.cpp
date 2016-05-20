#include "stdafx.h"
#include "CircularityRejectionKernel.h"
#include "CircularityRejection_bin2c.h"

#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include "framework/error/ImageResolutionDiffersException.h"

namespace ettention
{
    CircularityRejectionKernel::CircularityRejectionKernel(Framework *framework, GPUMapped<Image> *particlesMap, GPUMapped<Image> *segmentationMap, GPUMapped<Image> *outputcontainer, Vec2ui particleSize)
        : ImageProcessingKernel(framework, outputcontainer, CircularityRejection_kernel_SourceCode, "filterOutNonRoundRegions", "CircularityRejectionKernel")
        , projectionResolution(particlesMap->getResolution().xy())
        , particleSize(particleSize)
    {
        setInput(particlesMap, segmentationMap);
    }

    CircularityRejectionKernel::CircularityRejectionKernel(Framework *framework, GPUMapped<Image> *particlesMap, GPUMapped<Image> *segmentationMap, Vec2ui particleSize)
        : ImageProcessingKernel(framework, particlesMap->getResolution(), CircularityRejection_kernel_SourceCode, "filterOutNonRoundRegions", "CircularityRejectionKernel")
        , projectionResolution(particlesMap->getResolution().xy())
        , particleSize(particleSize)
    {
        setInput(particlesMap, segmentationMap);
    }

        CircularityRejectionKernel::~CircularityRejectionKernel()
    {
    }

    void CircularityRejectionKernel::prepareKernelArguments()
    {
        implementation->setArgument("particlesMap",       particlesMap);
        implementation->setArgument("segmentationMap",    segmentationMap);
        implementation->setArgument("result",             getOutput());
        implementation->setArgument("sizeX",         (int)projectionResolution.x);
        implementation->setArgument("sizeY",         (int)projectionResolution.y);
        implementation->setArgument("particleSizeX", (int)particleSize.x);
        implementation->setArgument("particleSizeY", (int)particleSize.y);
    }

    void CircularityRejectionKernel::setInput(GPUMapped<Image> *particlesMap, GPUMapped<Image> *segmentationMap)
    {
        if( particlesMap->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, particlesMap->getResolution().xy(), "CircularityRejectionKernel::setInput:particlesMap");

        if( segmentationMap->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, segmentationMap->getResolution().xy(), "CircularityRejectionKernel::setInput:segmentationMap");

        this->particlesMap = particlesMap;
        this->segmentationMap = segmentationMap;
    }

    void CircularityRejectionKernel::setParticleSize(Vec2ui particleSize)
    {
        this->particleSize = particleSize;
    }

    GPUMapped<Image> *CircularityRejectionKernel::getInputParticlesMap() const
    {
        return particlesMap;
    }

    GPUMapped<Image> *CircularityRejectionKernel::getInputSegmentationMap() const
    {
        return segmentationMap;
    }
}