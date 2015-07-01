#pragma once
#include "algorithm/imagestatistics/particledetection/MarkerDetectorInterface.h"
#include "framework/math/Vec2.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class ConvolutionBasedDetector : public MarkerDetectorInterface
    {
    public:
        ConvolutionBasedDetector(CLAbstractionLayer *openCL);
        virtual ~ConvolutionBasedDetector();

        virtual GPUMapped<Image> *getParticlesMap();
        virtual std::vector<Vec2ui> *getParticlesList();
        virtual bool darkFieldSupported();
    };
}
