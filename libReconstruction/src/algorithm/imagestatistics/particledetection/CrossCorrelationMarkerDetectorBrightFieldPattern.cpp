#include "stdafx.h"

#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetectorBrightFieldPattern.h"

namespace ettention
{
    CrossCorrelationMarkerDetectorBrightFieldPattern::CrossCorrelationMarkerDetectorBrightFieldPattern(Framework* framework, const Vec2ui& projectionResolution)
        : CrossCorrelationMarkerDetector(framework, projectionResolution, new GPUMapped<Image>(openCL, ImageDeserializer::readImage(CrossCorrelationMarkerDetectorBrightFieldPattern::filename)))
    {
        this->operatingInBrightFieldMode = true;
        this->ownInputPattern = true;
    }

    CrossCorrelationMarkerDetectorBrightFieldPattern::~CrossCorrelationMarkerDetectorBrightFieldPattern()
    {
    }

    bool CrossCorrelationMarkerDetectorBrightFieldPattern::darkFieldSupported()
    {
        return false;
    }

    const std::string CrossCorrelationMarkerDetectorBrightFieldPattern::filename = "/data/alignment/marker_pattern_black_11.tif"; // temp
    const unsigned int CrossCorrelationMarkerDetectorBrightFieldPattern::width = 11;
    const unsigned int CrossCorrelationMarkerDetectorBrightFieldPattern::height = 11;
    const float *CrossCorrelationMarkerDetectorBrightFieldPattern::pattern = new float[width * height](); // will be used later
}