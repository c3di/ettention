#pragma once
#include "algorithm/imagestatistics/particledetection/ParticleDetectorInterface.h"
#include "io/deserializer/ImageDeserializer.h"

namespace ettention
{
    class CrosscorrelationOperator;
    class ThresholdKernel;
    class LocalMaximumSimpleKernel;
    class ClosingOperator;
    class BorderSegmentKernel;
    class CircularityRejectionKernel;

    class CrossCorrelationMarkerDetector : public ParticleDetectorInterface
    {
    public:
        CrossCorrelationMarkerDetector(Framework* framework, const Vec2ui &projectionResolution, GPUMapped<Image> *particlePattern);
        virtual ~CrossCorrelationMarkerDetector();

        virtual void process(GPUMapped<Image> *source) override;
        virtual void process() override;
        virtual void adjustThreshold(float value) override;

        virtual Vec2ui getParticleSize() override;
        GPUMapped<Image> *getRefinedByCircularScoreParticlesMap();

        void setThreshold(float value);

        std::vector<Vec2ui> *refinedPL;
        std::vector<Vec2ui> *refinedFivePercentPL;

    protected:
        void releaseMarkerPattern();

    protected:
        bool ownInputPattern;

    private:
        Vec2ui projectionResolution;
        GPUMapped<Image>* stubSource;
        GPUMapped<Image>* structElement;
        GPUMapped<Image>* activeMarkerPatternBuffer;

        CrosscorrelationOperator* nccOperator;
        ThresholdKernel* thresholdOperator;
        LocalMaximumSimpleKernel* localMaxOperator;
        ClosingOperator* closingOperator;
        BorderSegmentKernel* borderSegmentator;
        CircularityRejectionKernel* circularityRejector;
    };
}