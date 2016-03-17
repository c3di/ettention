#include "stdafx.h"
#include "ParticleExtractor.h"

#include <iostream>
#include <fstream>
#include <iosfwd>
#include <cmath>

#include "framework/Framework.h"
#include "gpu/GPUMapped.h"
#include "model/image/Image.h"

namespace ettention
{
    ParticleExtractor::ParticleExtractor()
    {
    }


    ParticleExtractor::~ParticleExtractor()
    {
    }

    void ParticleExtractor::exportParticlesFromListToImage(const std::vector<Vec2ui> *inputVector, GPUMapped<Image> *outputContainer)
    {
        Image *img = outputContainer->getObjectOnCPU();
        for( auto it = inputVector->begin(); it != inputVector->end(); ++it )
        {
            img->setPixel(it->x, it->y, 1.0f);
        }
        outputContainer->markAsChangedOnCPU();
        outputContainer->ensureIsUpToDateOnGPU();
    }

    std::vector<Vec2ui> *ParticleExtractor::exportParticlesFromImageToListAll(GPUMapped<Image> *inputImage)
    {
        auto *result = new std::vector<Vec2ui>();

        inputImage->ensureIsUpToDateOnCPU();

        Vec2ui size = inputImage->getResolution();
        float value;
        for( unsigned int j = 0; j < size.y; ++j )
        {
            for( unsigned int i = 0; i < size.x; ++i )
            {
                value = inputImage->getObjectOnCPU()->getPixel((int)i, (int)j);
                if( value > 0.0f )
                {
                    result->push_back(Vec2ui(i, j));
                }
            }
        }
        std::sort(result->begin(), result->end(), [&](const Vec2ui& left, const Vec2ui& right)
        {
            float lVal = inputImage->getObjectOnCPU()->getPixel(left.x, left.y);
            float rVal = inputImage->getObjectOnCPU()->getPixel(right.x, right.y);
            return lVal > rVal;
        });
        result->shrink_to_fit();

        return result;
    }

    std::vector<Vec2ui> *ParticleExtractor::exportParticlesFromImageToList(GPUMapped<Image> *inputImage, size_t number)
    {
        auto *preResult = exportParticlesFromImageToListAll(inputImage);
        if( (number == 0) || (number > preResult->size()) )
            return preResult;

        preResult->resize(number);
        return preResult;
    }

    std::vector<Vec2ui> *ParticleExtractor::exportParticlesFromImageToListPercent(GPUMapped<Image> *inputImage, size_t percentage)
    {
        auto *preResult = exportParticlesFromImageToListAll(inputImage);
        if( percentage >= 100 )
            return preResult;

        preResult->resize(preResult->size() * percentage / 100);
        return preResult;
    }
    
    void ParticleExtractor::extractAndSaveToFile(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number)
    {
        auto *particles = ParticleExtractor::exportParticlesFromImageToList(inputImage, number);
        ParticleExtractor::saveToFileAll(particles, outputFilename, particleSize, particleCenter);
        delete particles;
    }

    void ParticleExtractor::extractAndSaveToFileAll(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter)
    {
        ParticleExtractor::extractAndSaveToFilePercent(inputImage, outputFilename, particleSize, particleCenter, 100);
    }

    void ParticleExtractor::extractAndSaveToFilePercent(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage)
    {
        auto *particles = ParticleExtractor::exportParticlesFromImageToListPercent(inputImage, percentage);
        ParticleExtractor::saveToFileAll(particles, outputFilename, particleSize, particleCenter);
        delete particles;
    }

    // Extracting all and saving just a small amount of particles from particles vector is more faster in most of the use cases 
    //      than extracting some percentage of particles and saving all of them
    //  We use ordered list so saving limited number if controlled by for-loop exit condition
    //      extracting in opposite requires memory (re)allocation at least.
    // Thats why we need all this "overloaded" functions.
    void ParticleExtractor::saveToFile(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number)
    {
        std::ofstream output(outputFilename, std::ofstream::out);
        output << particleSize.x << " " << particleSize.y << " ";
        if( particleCenter )
        {
            output << "1";
        } else {
            output << "0";
        }

        size_t limit = inputVector->size();
        if( (number > 0) && (number < limit) )
            limit = number;

        Vec2ui position;
        for( size_t i = 0; i < limit; ++i )
        {
            position = inputVector->at(i);
            output << std::endl << position.x << " " << position.y;
        }

        output.close();
    }

    void ParticleExtractor::saveToFileAll(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter)
    {
        ParticleExtractor::saveToFile(inputVector, outputFilename, particleSize, particleCenter, inputVector->size());
    }

    void ParticleExtractor::saveToFilePercent(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage)
    {
        ParticleExtractor::saveToFile(inputVector, outputFilename, particleSize, particleCenter, inputVector->size() * percentage / 100);
    }

    ParticleData ParticleExtractor::loadFromFile(std::string filename)
    {
        ParticleData result;
        result.data = new std::vector<Vec2ui>();

        std::ifstream input(filename, std::ifstream::in);
        if(!input.good())
        {
            std::cout << "Wrong particle file name " << filename << "!" << std::endl;
            return result;
        }

        input >> result.size.x >> result.size.y;

        unsigned int temp, temp2;
        input >> temp;
        if( temp )
            result.coordinateOfCenter = true;
        else
            result.coordinateOfCenter = false;

        while(input.good())
        {
            input >> temp >> temp2;
            result.data->push_back(Vec2ui(temp, temp2));
        }
        input.close();

        result.data->shrink_to_fit();
        return result;
    }

    DetectionQualityStructure ParticleExtractor::compareWithGroundTruth(ParticleData ground, ParticleData candidate, std::string outputFilename)
    {
        DetectionQualityStructure result;
        Vec2i shiftForGround = Vec2i(0, 0);
        Vec2i shiftForCandid = Vec2i(0, 0);

        Vec2i groundHalfSize = Vec2i(static_cast<int>(ground.size.x / 2), static_cast<int>(ground.size.y / 2));
        Vec2i candidHalfSize = Vec2i(static_cast<int>(candidate.size.x / 2), static_cast<int>(candidate.size.y / 2));

        if(!ground.coordinateOfCenter)
        {
            shiftForGround = shiftForGround + groundHalfSize;
        }
        if(!candidate.coordinateOfCenter)
        {
            shiftForCandid = shiftForCandid + candidHalfSize;
        }

        std::vector<Vec3ui> groundList;
        std::vector<Vec3ui> candidList;

        ParticleExtractor::createUniquePointsListFrom2DCoordinates(ground.data, groundList);
        ParticleExtractor::createUniquePointsListFrom2DCoordinates(candidate.data, candidList);

        std::vector<Vec3ui> correctFound;
        std::vector<Vec3ui> notFound;
        std::vector<Vec3ui> correctGuess;
        std::vector<Vec3ui> wrongGuess;

        unsigned int idGround;
        unsigned int idCandid;
        Vec2i tempGround;
        Vec2i tempCandid;
        bool found = false;
        double error = 0.0f;
        double errorSum = 0.0f;

        auto selectedGround = groundList.begin();
        std::ofstream output(outputFilename);
        output << "Pairs from (G)round and (C)andidate by IDs:" << std::endl;
        for( auto itc = candidList.begin(); itc != candidList.end(); ++itc )
        {
            idCandid = itc->z;
            tempCandid = Vec2i(static_cast<int>(itc->x) + shiftForCandid.x, static_cast<int>(itc->y) + shiftForCandid.y);

            for( auto itg = groundList.begin(); itg != groundList.end(); ++itg )
            {
                idGround = itg->z;

                tempGround = Vec2i(static_cast<int>(itg->x + shiftForGround.x), static_cast<int>(itg->y) + shiftForGround.y);

                if(   (tempCandid.x >= (tempGround.x - groundHalfSize.x)) && (tempCandid.x < (tempGround.x + groundHalfSize.x))
                   && (tempCandid.y >= (tempGround.y - groundHalfSize.y)) && (tempCandid.y < (tempGround.y + groundHalfSize.y))
                  )
                {
                    if(!idGround )   // We already marked this particle from ground truth as found
                    {
                        output << "Found correspondence between G" << itg->z << "(" << itg->x << "," << itg->y << ")" << " and "
                               << "C" << itc->z << "(" << itc->x << "," << itc->y << "), but this ground truth particle was already found once before." << std::endl;
                        continue;
                    }

                    Vec2i diff = tempGround - tempCandid;
                    if( found ) // this candidate particle overlapped another particle from ground truth before
                    {
                        // choose only one of them, with lesser error
                        if( (diff.x*diff.x + diff.y*diff.y) >= error )
                        {
                            continue;
                        }
                    }
                    error = diff.x*diff.x + diff.y*diff.y;
                    selectedGround = itg;
                    found = true;
                }
            }

            if( found )
            {
                correctFound.push_back(*selectedGround);
                correctGuess.push_back(*itc);
                errorSum += sqrt(error);
                output << "\tG" << selectedGround->z << "  \t(" << selectedGround->x << "," << selectedGround->y << ")" << " \t<=" << error << "=> "
                       << "\tC" << itc->z << "  \t(" << itc->x << "," << itc->y << ")" << std::endl;
                selectedGround->z = 0;
            } else {
                wrongGuess.push_back(*itc);
            }
            found = false;
            error = 0.0f;
        }

        int counterNotFoundParticlesFromGroundList = 0;
        for( auto itg = groundList.begin(); itg != groundList.end(); ++itg )
        {
            if( itg->z != 0 )
            {
                ++counterNotFoundParticlesFromGroundList;
                notFound.push_back(*itg);
            }
        }

        result.set(correctFound.size(), notFound.size(), wrongGuess.size(), errorSum);
        output << result << std::endl;

        output << "TruePositives[ground]:" << correctFound.size() << " FalseNegatives:" << notFound.size() << std::endl
               << " TruePositives[candidate]:" << correctGuess.size() << " FalsePositives:" << wrongGuess.size()  << std::endl
               << " Error:" << errorSum << " Ground:" << ground.data->size() << std::endl;
        ParticleExtractor::printParticleDistributionDetails(output, correctFound, notFound, correctGuess, wrongGuess);
        output.close();

        return result;
    }

    void ParticleExtractor::createUniquePointsListFrom2DCoordinates(const std::vector<Vec2ui> *points, std::vector<Vec3ui> &uniquePoints)
    {
        unsigned int nextId = 1;
        for( auto it = points->begin(); it != points->end(); ++it, ++nextId )
        {
            uniquePoints.push_back(Vec3ui(it->x, it->y, nextId));
        }
    }

    void ParticleExtractor::printParticleDistributionDetails(std::ofstream &output, 
                                                             const std::vector<Vec3ui> &found, 
                                                             const std::vector<Vec3ui> &notFound, 
                                                             const std::vector<Vec3ui> &correctGuess, 
                                                             const std::vector<Vec3ui> &incorrectGuess)
    {
        output << "Real particles which were found" << std::endl;
        for( auto it = found.begin(); it != found.end(); ++it )
        {
            output << "G" << it->z << " " << it->x << " " << it->y << std::endl;
        }
        output << "Real particles which were not found" << std::endl;
        for( auto it = notFound.begin(); it != notFound.end(); ++it )
        {
            output << "G" << it->z << " " << it->x << " " << it->y << std::endl;
        }
        output << "Particles candidates guessed correctly by Detector" << std::endl;
        for( auto it = correctGuess.begin(); it != correctGuess.end(); ++it )
        {
            output << "C" << it->z << " " << it->x << " " << it->y << std::endl;
        }
        output << "Particles candidates guessed incorrectly by Detector" << std::endl;
        for( auto it = incorrectGuess.begin(); it != incorrectGuess.end(); ++it )
        {
            output << "C" << it->z << " " << it->x << " " << it->y << std::endl;
        }
    }

}