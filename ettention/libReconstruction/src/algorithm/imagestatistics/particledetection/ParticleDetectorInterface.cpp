#include "stdafx.h"
#include "ParticleDetectorInterface.h"

#include "framework/Framework.h"
#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"
#include "io/serializer/ImageSerializer.h"

namespace ettention
{
    ParticleDetectorInterface::ParticleDetectorInterface(Vec2ui particleSize, bool isDarkFieldSupported, bool isResultCoordinateOfCenter)
        : particleSize(particleSize)
        , isOperatingInBrightFieldMode(true)
        , isDarkFieldSupported(isDarkFieldSupported)
        , isResultCoordinateOfCenter(isResultCoordinateOfCenter)
        , resultImage(nullptr)
        , resultVector(nullptr)
    {
    }

    ParticleDetectorInterface::~ParticleDetectorInterface()
    {
        delete resultVector;
    }

    Vec2ui ParticleDetectorInterface::getParticleSize()
    {
        return particleSize;
    }

    GPUMapped<Image> *ParticleDetectorInterface::getParticlesMap()
    {
        if(resultImage == nullptr)
            throw Exception("ParticleDetectorInterface.getParticlesMap: Cannot get result (even as image) before execution!");

        return resultImage;
    }

    std::vector<Vec2ui> *ParticleDetectorInterface::getParticlesList()
    {
        if(resultVector == nullptr)
            throw Exception("ParticleDetectorInterface.getParticlesList: Cannot get result (even as vector) before execution!");

        return resultVector;
    }

    bool ParticleDetectorInterface::operatingInBrightFieldMode()
    {
        return isOperatingInBrightFieldMode;
    }

    bool ParticleDetectorInterface::darkFieldSupported()
    {
        return isDarkFieldSupported;
    }

    bool ParticleDetectorInterface::detectingMarkerCenter()
    {
        return isResultCoordinateOfCenter;
    }

    void ParticleDetectorInterface::dumpPercentOfParticlesInFile(std::string outputFilename, int percentage)
    {
        ParticleExtractor::saveToFilePercent(getParticlesList(), outputFilename, getParticleSize(), detectingMarkerCenter(), percentage);
    }
}