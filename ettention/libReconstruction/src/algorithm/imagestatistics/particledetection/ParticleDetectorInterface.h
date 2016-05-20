#pragma once

#include "framework/Framework.h"
#include "framework/math/Vec2.h"
#include <vector>

namespace ettention
{
    class Image;
    template<class Image> class GPUMapped;

    class ParticleDetectorInterface
    {
    public:
        ParticleDetectorInterface(Vec2ui particleSize, bool isDarkFieldSupported, bool isResultCoordinateOfCenter);
        ~ParticleDetectorInterface();

        virtual void process(GPUMapped<Image> *source) = 0;
        virtual void process() = 0;                     // Process last input again
        virtual void adjustThreshold(float value) = 0;  // Change threshold level and re-do only last steps of pipeline

        virtual Vec2ui getParticleSize();
        virtual GPUMapped<Image> *getParticlesMap();
        virtual std::vector<Vec2ui> *getParticlesList();

        virtual bool operatingInBrightFieldMode();
        virtual bool darkFieldSupported();              // We can ignore absence of Dark Field mode, because only Bright Field mode is required
        virtual bool detectingMarkerCenter();

        virtual void dumpPercentOfParticlesInFile(std::string outputFilename, int percentage = 100);

    protected:
        bool isOperatingInBrightFieldMode;
        bool isDarkFieldSupported;
        bool isResultCoordinateOfCenter;                // otherwise UpperLeft corner of particle

        Vec2ui particleSize;
        GPUMapped<Image> *resultImage;
        std::vector<Vec2ui> *resultVector;
    };
}