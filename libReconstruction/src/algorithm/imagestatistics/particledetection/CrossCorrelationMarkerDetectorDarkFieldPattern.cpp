#include "stdafx.h"
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetectorDarkFieldPattern.h"

namespace ettention
{
    CrossCorrelationMarkerDetectorDarkFieldPattern::CrossCorrelationMarkerDetectorDarkFieldPattern(Framework* framework, const Vec2ui& projectionResolution)
        : CrossCorrelationMarkerDetector(framework, projectionResolution, new GPUMapped<Image>(openCL, ImageDeserializer::readImage(CrossCorrelationMarkerDetectorDarkFieldPattern::filename)))
    {
        this->operatingInBrightFieldMode = false;
        this->ownInputPattern = true;
    }

    CrossCorrelationMarkerDetectorDarkFieldPattern::~CrossCorrelationMarkerDetectorDarkFieldPattern()
    {
    }

    bool CrossCorrelationMarkerDetectorDarkFieldPattern::darkFieldSupported()
    {
        return true;
    }

    const std::string CrossCorrelationMarkerDetectorDarkFieldPattern::filename = "/data/alignment/marker_pattern_white_11.tif"; // temp
    const unsigned int CrossCorrelationMarkerDetectorDarkFieldPattern::width = 11;
    const unsigned int CrossCorrelationMarkerDetectorDarkFieldPattern::height = 11;
    const float *CrossCorrelationMarkerDetectorDarkFieldPattern::pattern = new float[width * height](); // will be used later
}