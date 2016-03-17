#pragma once

#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetector.h"

namespace ettention
{
    class CrossCorrelationMarkerDetectorAveragingPattern : public CrossCorrelationMarkerDetector
    {
    public:
        CrossCorrelationMarkerDetectorAveragingPattern(Framework* framework, Image* src, std::vector<Vec2ui> initialGuess, int radius);
        virtual ~CrossCorrelationMarkerDetectorAveragingPattern();
    };
}