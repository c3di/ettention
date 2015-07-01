#pragma once
#include "algorithm/imagestatistics/particledetection/MarkerDetectorInterface.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"
#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationOperator.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"

namespace ettention
{
    class CrossCorrelationMarkerDetector : public MarkerDetectorInterface
    {
    public:
        CrossCorrelationMarkerDetector(Framework* framework, const Vec2ui &projectionResolution, GPUMapped<Image> *particlePattern);
        virtual ~CrossCorrelationMarkerDetector();

        virtual void process(GPUMapped<Image> *source) override;
        virtual void process() override;
        virtual void adjustThreshold(float value) override;

        virtual Vec2ui getParticleSize() override;
        virtual GPUMapped<Image> *getParticlesMap() override;
        virtual std::vector<Vec2ui> *getParticlesList() override;

        virtual bool darkFieldSupported() override;

        void setThreshold(float value);

    protected:
        void prepareMarkerPattern();
        void releaseMarkerPattern();

    protected:
        bool ownInputPattern;

    private:
        Vec2ui projectionResolution;
        GPUMapped<Image>* stubSource;

        GPUMapped<Image>* activeMarkerPatternBuffer;
        CrosscorrelationOperator* nccOperator;
        ThresholdKernel* thresholdOperator;
        LocalMaximumSimpleKernel* localMaxOperator;
    };
}