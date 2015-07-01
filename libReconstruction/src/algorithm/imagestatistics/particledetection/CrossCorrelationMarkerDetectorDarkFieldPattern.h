#pragma once

#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetector.h"

namespace ettention
{
    class CrossCorrelationMarkerDetectorDarkFieldPattern : public CrossCorrelationMarkerDetector
    {
    public:
        CrossCorrelationMarkerDetectorDarkFieldPattern(Framework* framework, const Vec2ui& projectionResolution);
        virtual ~CrossCorrelationMarkerDetectorDarkFieldPattern();
        virtual bool darkFieldSupported();

    private:
        static const std::string filename;
        static const unsigned int width;
        static const unsigned int height;
        static const float *pattern;
    };
}