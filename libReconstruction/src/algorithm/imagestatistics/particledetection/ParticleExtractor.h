#pragma once
#include <iostream>
#include "framework/math/Vec2.h"
#include "gpu/GPUMapped.h"
#include "model/image/Image.h"

namespace ettention
{
    struct ParticleData
    {
        Vec2ui size;
        bool coordinateOfCenter;
        std::vector<Vec2ui> *data;
    };

    struct DetectionQuality
    {
        unsigned int positives;
        unsigned int falseNegatives;
        unsigned int falsePositives;
        double placementError;

        void set(size_t positivesSize, size_t falseNegativesSize, size_t falsePositivesSize, double error)
        {
            this->positives = static_cast<unsigned int>(positivesSize);
            this->falseNegatives = static_cast<unsigned int>(falseNegativesSize);
            this->falsePositives = static_cast<unsigned int>(falsePositivesSize);
            this->placementError = error;
        }

        double getErrorPerHit()
        {
            return placementError / positives;
        }
        float getQuality()
        {
            return this->getWeightedQuality(0);
        }
        float getPurity()
        {
            return this->getWeightedQuality(1);
        }
        float getStrictPurity()
        {
            return this->getWeightedQuality(10);
        }
        float getWeightedQuality(int conservationRate)
        {
            return static_cast<float>(positives) / (positives + falseNegatives + falsePositives * conservationRate);
        }
        float getStrictQuality()
        {
            return static_cast<float>(positives) / (positives + falseNegatives*falseNegatives + falsePositives*falsePositives);
        }
    };

    class ParticleExtractor
    {
    public:
        ParticleExtractor();
        ~ParticleExtractor();

        static std::vector<Vec2ui> *extractVector(GPUMapped<Image> *inputImage);

        static void saveToFile(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number);
        static void saveToFileAll(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter);
        static void saveToFilePercent(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage);

        static void extractAndSaveToFile(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number);
        static void extractAndSaveToFileAll(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter);
        static void extractAndSaveToFilePercent(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage);

        static ParticleData loadFromFile(std::string filename);
        static DetectionQuality compareWithGroundTruth(ParticleData ground, ParticleData candidate, std::string outputFilename);
    };
}