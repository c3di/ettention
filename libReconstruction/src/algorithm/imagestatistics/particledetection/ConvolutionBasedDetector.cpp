#include "stdafx.h"

#include "algorithm/imagestatistics/particledetection/ConvolutionBasedDetector.h"

#include "algorithm/imagemanipulation/ThresholdKernel.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationOperator.h"

#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"

namespace ettention
{

    ConvolutionBasedDetector::ConvolutionBasedDetector(CLAbstractionLayer *openCL)
    {
        resultImage = nullptr;
        resultVector = nullptr;

        this->openCL = openCL;

        operatingInBrightFieldMode = true;
    }

    ConvolutionBasedDetector::~ConvolutionBasedDetector()
    {
        delete resultVector;
    }

    GPUMapped<Image> *ConvolutionBasedDetector::getParticlesMap()
    {
        if(resultImage == nullptr)
            throw Exception("ConvolutionBasedDetector.getParticlesMap: Cannot get result as image before execution!");

        return this->resultImage;
    }
    std::vector<Vec2ui> *ConvolutionBasedDetector::getParticlesList()
    {
        if(resultVector == nullptr)
            throw Exception("ConvolutionBasedDetector.getParticlesMap: Cannot get result as vector before execution!");

        return this->resultVector;
    }

    bool ConvolutionBasedDetector::darkFieldSupported()
    {
        return false;
    }
}