#pragma once
#include "algorithm/imagestatistics/particledetection/ParticleDetectorInterface.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class ConvolutionBasedDetector : public ParticleDetectorInterface
    {
    public:
        ConvolutionBasedDetector(Framework *framework, Vec2ui particleSize, bool isDarkFieldSupported, bool isResultCoordinateOfCenter);
        virtual ~ConvolutionBasedDetector();
    };
}
