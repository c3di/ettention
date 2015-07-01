#include "stdafx.h"
#include "CrossCorrelationMarkerDetector.h"

#include "framework/Framework.h"

#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"

namespace ettention
{
    CrossCorrelationMarkerDetector::CrossCorrelationMarkerDetector(Framework *framework, const Vec2ui &projectionResolution, GPUMapped<Image> *particlePattern)
        : activeMarkerPatternBuffer(particlePattern)
        , ownInputPattern(false)
        , projectionResolution(projectionResolution)
    {
        resultImage = nullptr;
        resultVector = nullptr;

        this->operatingInBrightFieldMode = true;

        this->stubSource = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        this->nccOperator = new CrosscorrelationOperator(framework, this->stubSource, this->activeMarkerPatternBuffer);
        this->thresholdOperator = new ThresholdKernel(framework, this->nccOperator->getResult());
        this->localMaxOperator = new LocalMaximumSimpleKernel(framework, this->thresholdOperator->getResult(), this->activeMarkerPatternBuffer->getResolution().xy());
    }

    CrossCorrelationMarkerDetector::~CrossCorrelationMarkerDetector()
    {
        releaseMarkerPattern();

        delete this->stubSource;

        delete this->localMaxOperator;
        delete this->thresholdOperator;
        delete this->nccOperator;

        delete resultVector;
    }

    void CrossCorrelationMarkerDetector::prepareMarkerPattern()
    {
    }

    void CrossCorrelationMarkerDetector::releaseMarkerPattern()
    {
        if(this->ownInputPattern)
        {
            delete activeMarkerPatternBuffer;
        }
    }

    void CrossCorrelationMarkerDetector::process(GPUMapped<Image> *source)
    {
        this->nccOperator->setInput(source, this->activeMarkerPatternBuffer);
        this->process();
    }

    void CrossCorrelationMarkerDetector::process()
    {
        delete this->resultVector;

        this->nccOperator->matchTemplate();
        //this->adjustThreshold(ThresholdDecider::optimalEntropyThreshold(this->nccOperator->getResultAsImage()));
        this->adjustThreshold(0.0f);
    }

    void CrossCorrelationMarkerDetector::adjustThreshold(float value)
    {
        this->thresholdOperator->setThreshold(value);

        this->thresholdOperator->run();
        this->localMaxOperator->run();

        this->resultImage = this->localMaxOperator->getResult();
        this->resultVector = ParticleExtractor::extractVector(this->resultImage);
    }

    Vec2ui CrossCorrelationMarkerDetector::getParticleSize()
    {
        return this->activeMarkerPatternBuffer->getResolution();
    }

    GPUMapped<Image> *CrossCorrelationMarkerDetector::getParticlesMap()
    {
        if(resultImage == nullptr)
            throw Exception("ConvolutionBasedDetector.getParticlesMap: Cannot get result (even as image) before execution!");

        return this->resultImage;
    }
    std::vector<Vec2ui> *CrossCorrelationMarkerDetector::getParticlesList()
    {
        if(resultVector == nullptr)
            throw Exception("ConvolutionBasedDetector.getParticlesMap: Cannot get result (even as vector) before execution!");

        return this->resultVector;
    }

    bool CrossCorrelationMarkerDetector::darkFieldSupported()
    {
        return true;
    }

    void CrossCorrelationMarkerDetector::setThreshold(float value)
    {
        this->thresholdOperator->setThreshold(value);
    }
}