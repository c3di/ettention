#pragma once

#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"

#include "gpu/GPUMapped.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/parametersource/ManualParameterSource.h"

#include <vector>

namespace ettention
{
    class MarkerDetectorInterface
    {
    public:
        virtual void process(GPUMapped<Image> *source) = 0;
        virtual void process() = 0; // Process previous input
        virtual void adjustThreshold(float value) = 0; // Change threshold level and re-do only last steps of pipeline

        virtual Vec2ui getParticleSize() = 0;
        virtual GPUMapped<Image> *getParticlesMap() = 0;
        virtual std::vector<Vec2ui> *getParticlesList() = 0;

        virtual bool darkFieldSupported() = 0; // Only Bright Field mode is required

    protected:
        bool operatingInBrightFieldMode;
        bool resultCoordinateIsUpperLeftCorner;
        GPUMapped<Image> *resultImage;
        std::vector<Vec2ui> *resultVector;

    protected:
        CLAbstractionLayer *openCL;

    };
}