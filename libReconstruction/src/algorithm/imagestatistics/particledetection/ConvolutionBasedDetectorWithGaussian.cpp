#include "stdafx.h"

#include "algorithm/imagestatistics/particledetection/ConvolutionBasedDetectorWithGaussian.h"

#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationOperator.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"

#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"

namespace ettention
{
    ConvolutionBasedDetectorWithGaussian::ConvolutionBasedDetectorWithGaussian(CLAbstractionLayer *openCL, Vec2ui resolution, unsigned int particleDiameter)
        : ConvolutionBasedDetector(openCL)
        , resolution(resolution)
        , particleSize(Vec2ui(particleDiameter, particleDiameter))
    {
        operatingInBrightFieldMode = true;
    }

    ConvolutionBasedDetectorWithGaussian::~ConvolutionBasedDetectorWithGaussian()
    {
    }

    GPUMapped<Image> * ConvolutionBasedDetectorWithGaussian::getParticlesMap(GPUMapped<Image> *source)
    {
        // Stub
        return source;
    }

    bool ConvolutionBasedDetectorWithGaussian::darkFieldSupported()
    {
        return false;
    }
}