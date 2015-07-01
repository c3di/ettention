#pragma once
#include "algorithm/imagestatistics/particledetection/ConvolutionBasedDetector.h"

namespace ettention
{
    class ConvolutionBasedDetectorWithGaussian : public ConvolutionBasedDetector
    {
    public:
        ConvolutionBasedDetectorWithGaussian(CLAbstractionLayer *openCL, Vec2ui resolution, unsigned int particleDiameter);
        virtual ~ConvolutionBasedDetectorWithGaussian();

        virtual GPUMapped<Image>* getParticlesMap(GPUMapped<Image> *source);
        virtual bool darkFieldSupported();

    protected:
        void prepareMarkerPattern();
        void releaseMarkerPattern();

    private:
        Image* activeMarkerPattern;
        CLMemBuffer* activeMarkerPatternBuffer;
        Vec2ui resolution;
        Vec2ui particleSize;
    };
}