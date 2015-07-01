#include "stdafx.h"
#include "CrossCorrelationMarkerDetectorAveragingPattern.h"
#include "algorithm/imagestatistics/particlepattern/PatternByAveraging.h"

namespace ettention 
{

    CrossCorrelationMarkerDetectorAveragingPattern::CrossCorrelationMarkerDetectorAveragingPattern(Framework* framework, Image* src, std::vector<Vec2ui> initialGuess, int radius)
        : CrossCorrelationMarkerDetector(framework, src->getResolution(), new GPUMapped<Image>(openCL, PatternByAveraging::computePattern(src, initialGuess, radius)))
    {
        this->operatingInBrightFieldMode = true;
        this->ownInputPattern = true;
    }
    
    CrossCorrelationMarkerDetectorAveragingPattern::~CrossCorrelationMarkerDetectorAveragingPattern()
    {
    }
    
    bool CrossCorrelationMarkerDetectorAveragingPattern::darkFieldSupported()
    {
        return true;
    }
}