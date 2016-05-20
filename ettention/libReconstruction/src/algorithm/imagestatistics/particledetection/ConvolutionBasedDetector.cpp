#include "stdafx.h"

#include "ConvolutionBasedDetector.h"

namespace ettention
{

    ConvolutionBasedDetector::ConvolutionBasedDetector(Framework *framework, Vec2ui particleSize, bool isDarkFieldSupported, bool isResultCoordinateOfCenter)
        : ParticleDetectorInterface(particleSize, isDarkFieldSupported, isResultCoordinateOfCenter)
    {
        isOperatingInBrightFieldMode = true;
    }

    ConvolutionBasedDetector::~ConvolutionBasedDetector()
    {
    }
}