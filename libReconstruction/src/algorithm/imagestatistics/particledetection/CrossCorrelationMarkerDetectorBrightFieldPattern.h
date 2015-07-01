#pragma once
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetector.h"

namespace ettention
{
    class CrossCorrelationMarkerDetectorBrightFieldPattern : public CrossCorrelationMarkerDetector
    {
    public:
        CrossCorrelationMarkerDetectorBrightFieldPattern(Framework* framework, const Vec2ui& projectionResolution);
        virtual ~CrossCorrelationMarkerDetectorBrightFieldPattern();
        virtual bool darkFieldSupported();

    private:
        static const std::string filename;
        static const unsigned int width;
        static const unsigned int height;
        static const float *pattern;
    };
}