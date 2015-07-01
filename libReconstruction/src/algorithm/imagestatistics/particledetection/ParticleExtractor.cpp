#include "stdafx.h"
#include "ParticleExtractor.h"

#include <fstream>
#include <cmath>

namespace ettention
{
    ParticleExtractor::ParticleExtractor()
    {
    }


    ParticleExtractor::~ParticleExtractor()
    {
    }

    std::vector<Vec2ui> *ParticleExtractor::extractVector(GPUMapped<Image> *inputImage)
    {
        auto *result = new std::vector<Vec2ui>();

        inputImage->ensureIsUpToDateOnCPU();

        Vec2ui size = inputImage->getResolution();
        float value;
        for(unsigned int j = 0; j < size.y; ++j)
        {
            for(unsigned int i = 0; i < size.x; ++i)
            {
                value = inputImage->getObjectOnCPU()->getPixel((int)i, (int)j);
                if(value > 0.0f)
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

    void ParticleExtractor::saveToFile(std::vector<Vec2ui> *inputVector, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number)
    {
        std::ofstream output(outputFilename, std::ofstream::out);
        output << particleSize.x << " " << particleSize.y << " ";
        if(particleCenter)
        {
            output << "1";
        }
        else
        {
            output << "0";
        }

        size_t countLimit = inputVector->size();
        if((number > 0) && (number < countLimit))
        {
            countLimit = number;
        }

        Vec2ui position;
        for(size_t i = 0; i < countLimit; ++i)
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


    void ParticleExtractor::extractAndSaveToFile(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t number)
    {
        auto *particles = ParticleExtractor::extractVector(inputImage);
        ParticleExtractor::saveToFile(particles, outputFilename, particleSize, particleCenter, number);
        delete particles;
    }

    void ParticleExtractor::extractAndSaveToFileAll(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter)
    {
        ParticleExtractor::extractAndSaveToFilePercent(inputImage, outputFilename, particleSize, particleCenter, 100);
    }

    void ParticleExtractor::extractAndSaveToFilePercent(GPUMapped<Image> *inputImage, std::string outputFilename, Vec2ui particleSize, bool particleCenter, size_t percentage)
    {
        auto *particles = ParticleExtractor::extractVector(inputImage);
        ParticleExtractor::saveToFilePercent(particles, outputFilename, particleSize, particleCenter, percentage);
        delete particles;
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
        if(temp)
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

    DetectionQuality ParticleExtractor::compareWithGroundTruth(ParticleData ground, ParticleData candidate, std::string outputFilename)
    {
        DetectionQuality result;
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

        std::vector<Vec2ui> correctFound;
        std::vector<Vec2ui> notFound;
        std::vector<Vec2ui> correctGuess;
        std::vector<Vec2ui> wrongGuess;
        Vec2ui temp;
        Vec2i tempGround;
        Vec2i tempCandid;
        bool found = false;
        double errorSum = 0.0f;
        for(auto itc = candidate.data->begin(); itc != candidate.data->end(); ++itc)
        {
            found = false;
            tempCandid = Vec2i(static_cast<int>(itc->x) + shiftForCandid.x, static_cast<int>(itc->y) + shiftForCandid.y);
            for(auto itg = ground.data->begin(); itg != ground.data->end(); ++itg)
            {
                tempGround = Vec2i(static_cast<int>(itg->x + shiftForGround.x), static_cast<int>(itg->y) + shiftForGround.y);

                if(tempCandid.x >= (tempGround.x - groundHalfSize.x) &&
                   tempCandid.x < (tempGround.x + groundHalfSize.x) && 
                   tempCandid.y >= (tempGround.y - groundHalfSize.y) &&
                   tempCandid.y < (tempGround.y + groundHalfSize.y))
                {
                    Vec2i diff = tempGround - tempCandid;
                    errorSum += sqrt(diff.x*diff.x + diff.y*diff.y);
                    correctFound.push_back(*itg);
                    correctGuess.push_back(*itc);
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                wrongGuess.push_back(*itc);
            }
        }

        bool checked = false;
        for(auto itg = ground.data->begin(); itg != ground.data->end(); ++itg)
        {
            checked = false;
            for(auto itf = correctFound.begin(); itf != correctFound.end(); ++itf)
            {
                if((*itg) == (*itf))
                {
                    checked = true;
                    break;
                }
            }
            if(!checked)
            {
                notFound.push_back(*itg);
            }
        }

        std::ofstream output(outputFilename);
        output << correctFound.size() << " " << notFound.size() << " " << correctGuess.size() << " " << wrongGuess.size() << " " << errorSum << std::endl;
        output << "Real particles which were found" << std::endl;
        for(auto it = correctFound.begin(); it != correctFound.end(); ++it)
        {
            output << it->x << " " << it->y << std::endl;
        }
        output << "Real particles which were not found" << std::endl;
        for(auto it = notFound.begin(); it != notFound.end(); ++it)
        {
            output << it->x << " " << it->y << std::endl;
        }
        output << "Particles guessed correctly by Detector" << std::endl;
        for(auto it = correctGuess.begin(); it != correctGuess.end(); ++it)
        {
            output << it->x << " " << it->y << std::endl;
        }
        output << "Particles guessed incorrectly by Detector" << std::endl;
        for(auto it = wrongGuess.begin(); it != wrongGuess.end(); ++it)
        {
            output << it->x << " " << it->y << std::endl;
        }
        output.close();

        result.set(correctFound.size(), notFound.size(), wrongGuess.size(), errorSum);
        return result;
    }

}