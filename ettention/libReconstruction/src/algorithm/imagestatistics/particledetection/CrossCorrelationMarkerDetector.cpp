#include "stdafx.h"
#include "CrossCorrelationMarkerDetector.h"

#include "framework/Framework.h"
#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"
#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "algorithm/imagestatistics/CircularityRejectionKernel.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationOperator.h"
#include "algorithm/imagemanipulation/morphology/ClosingOperator.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"
#include "algorithm/imagemanipulation/BorderSegmentKernel.h"
#include "algorithm/imagemanipulation/morphology/StructuringElementGenerator.h"
#include "io/serializer/ImageSerializer.h"

namespace ettention
{
    CrossCorrelationMarkerDetector::CrossCorrelationMarkerDetector(Framework *framework, const Vec2ui &projectionResolution, GPUMapped<Image> *particlePattern)
        : ParticleDetectorInterface(particlePattern->getResolution(), true, false)
        , activeMarkerPatternBuffer(particlePattern)
        , ownInputPattern(false)
        , projectionResolution(projectionResolution)
    {
        this->isOperatingInBrightFieldMode = true;

        this->stubSource            = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        this->nccOperator           = new CrosscorrelationOperator(framework, stubSource, activeMarkerPatternBuffer);
        this->thresholdOperator     = new ThresholdKernel(framework, nccOperator->getOutput());
        this->localMaxOperator      = new LocalMaximumSimpleKernel(framework, thresholdOperator->getOutput(), activeMarkerPatternBuffer->getResolution());

        structElement               = new GPUMapped<Image>(framework->getOpenCLStack(), StructuringElementGenerator::generateDiamondElement());
        structElement->takeOwnershipOfObjectOnCPU();

        this->closingOperator       = new ClosingOperator(framework, thresholdOperator->getOutput(), structElement);
        this->borderSegmentator     = new BorderSegmentKernel(framework, closingOperator->getOutput());
        this->circularityRejector   = new CircularityRejectionKernel(framework, localMaxOperator->getOutput(), borderSegmentator->getOutput(), getParticleSize());

        this->borderSegmentator->setUsingEightNeighbors(true);
    }

    CrossCorrelationMarkerDetector::~CrossCorrelationMarkerDetector()
    {
        releaseMarkerPattern();

        delete circularityRejector;
        delete borderSegmentator;
        delete closingOperator;

        delete structElement;

        delete localMaxOperator;
        delete thresholdOperator;
        delete nccOperator;
        delete stubSource;

        delete refinedPL;
        delete refinedFivePercentPL;
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
        this->adjustThreshold(0.0f);
    }

    void CrossCorrelationMarkerDetector::adjustThreshold(float value)
    {
        this->thresholdOperator->setThreshold(value);

        this->thresholdOperator->run();
        this->localMaxOperator->run();

        this->resultImage = this->localMaxOperator->getOutput();
        this->resultVector = ParticleExtractor::exportParticlesFromImageToListAll(this->resultImage);

        // // // Refinement experiment
        this->closingOperator->execute();
        this->borderSegmentator->run();
        this->circularityRejector->run();

        refinedPL = ParticleExtractor::exportParticlesFromImageToListPercent(this->circularityRejector->getOutput(), 100);
        refinedFivePercentPL = ParticleExtractor::exportParticlesFromImageToListPercent(this->circularityRejector->getOutput(), 5);
    }

    Vec2ui CrossCorrelationMarkerDetector::getParticleSize()
    {
        return this->activeMarkerPatternBuffer->getResolution();
    }

    GPUMapped<Image> *CrossCorrelationMarkerDetector::getRefinedByCircularScoreParticlesMap()
    {
        if( resultImage == nullptr )
            throw Exception("CrossCorrelationMarkerDetector.getRefinedByCircularScoreParticlesMap: Cannot get result (even from circularRejector) before execution!");

        return circularityRejector->getOutput();
    }

    void CrossCorrelationMarkerDetector::setThreshold(float value)
    {
        this->thresholdOperator->setThreshold(value);
    }
}