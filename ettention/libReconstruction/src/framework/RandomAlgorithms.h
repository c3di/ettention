#pragma once

#include <vector>
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class Image;

    class RandomAlgorithms
    {
    public:
        static void generateRandomNumbersFromMinusOneToOne(unsigned int amount, std::vector<float>&, unsigned int seed = 42);
        static void generateRandomNumbersFromZeroToMax(unsigned int amount, unsigned int maxValue, std::vector<unsigned int>&, unsigned int seed = 42);
        static void generateRandomPointsFromZeroToMax(unsigned int amount, Vec2ui maxValue, std::vector<Vec2ui>&, unsigned int seed = 42);

        static std::vector<float> generateRandomNumbersFromMinusOneToOne(unsigned int amount, unsigned int seed = 42);
        static std::vector<Vec3f> generateRandomUnitVectors(unsigned int amount, unsigned int seed = 42);
        static std::vector<unsigned int> generateRandomPermutationSequence(unsigned int sequenceLength, unsigned int seed = 42);

        static Image* generateRandomImage(unsigned int width, unsigned int height, unsigned int seed = 42, float scalingFactor = 1.0);

    protected:
        unsigned int seed;
    };
}