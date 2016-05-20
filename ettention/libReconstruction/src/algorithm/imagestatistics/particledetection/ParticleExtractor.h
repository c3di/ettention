#pragma once
#include <iostream>
#include "algorithm/imagestatistics/particledetection/DetectionQualityStructure.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class Framework;
    class Image;
    template<typename _T> class GPUMapped;
    
    class ParticleExtractor
    {
    public:
        ParticleExtractor();
        ~ParticleExtractor();

        static void exportParticlesFromListToImage(const std::vector<Vec2ui> *inputVector, GPUMapped<Image> *outputContainer);

        static std::vector<Vec2ui> *exportParticlesFromImageToList(GPUMapped<Image> *inputImage, size_t number);
        static std::vector<Vec2ui> *exportParticlesFromImageToListAll(GPUMapped<Image> *inputImage);
        static std::vector<Vec2ui> *exportParticlesFromImageToListPercent(GPUMapped<Image> *inputImage, size_t percentage);

        static void extractAndSaveToFile(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number);
        static void extractAndSaveToFileAll(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter);
        static void extractAndSaveToFilePercent(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage);

        static void saveToFile(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter , size_t number);
        static void saveToFileAll(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter);
        static void saveToFilePercent(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage);

        static ParticleData loadFromFile(std::string filename);
        static DetectionQualityStructure compareWithGroundTruth(ParticleData ground, ParticleData candidate, std::string outputFilename);

    private:
        static void createUniquePointsListFrom2DCoordinates(const std::vector<Vec2ui> *points, std::vector<Vec3ui> &uniquePoints);
        static void printParticleDistributionDetails(std::ofstream &output, 
                                                     const std::vector<Vec3ui> &correctFound, 
                                                     const std::vector<Vec3ui> &notFound, 
                                                     const std::vector<Vec3ui> &correctGuess, 
                                                     const std::vector<Vec3ui> &wrongGuess);
    };
}