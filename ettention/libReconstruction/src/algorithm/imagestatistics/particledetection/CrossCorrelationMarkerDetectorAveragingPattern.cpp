#include "stdafx.h"
#include "CrossCorrelationMarkerDetectorAveragingPattern.h"
#include "algorithm/imagestatistics/particlepattern/PatternByAveraging.h"

namespace ettention 
{

    CrossCorrelationMarkerDetectorAveragingPattern::CrossCorrelationMarkerDetectorAveragingPattern(Framework* framework, Image* src, std::vector<Vec2ui> initialGuess, int radius)
        : CrossCorrelationMarkerDetector(framework, src->getResolution(), new GPUMapped<Image>(framework->getOpenCLStack(), PatternByAveraging::computePattern(src, initialGuess, radius)))
    {
        this->isOperatingInBrightFieldMode = true;
        this->isDarkFieldSupported = true;
        this->ownInputPattern = true;
    }
    
    CrossCorrelationMarkerDetectorAveragingPattern::~CrossCorrelationMarkerDetectorAveragingPattern()
    {
    }
}