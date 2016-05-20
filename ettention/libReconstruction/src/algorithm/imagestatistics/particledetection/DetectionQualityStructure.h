#pragma once
#include <iostream>
#include "framework/math/Vec2.h"

namespace ettention
{
    struct ParticleData
    {
        Vec2ui size;
        bool coordinateOfCenter;
        std::vector<Vec2ui> *data;

        Vec2ui getParticleCenter(Vec2ui coordinate)
        {
            if( coordinateOfCenter )
                return coordinate;

            Vec2ui result(coordinate.x + (size.x / 2), coordinate.y + (size.y / 2));
            if( result.x > size.x )
                result.x = size.x;
            if( result.y > size.y )
                result.y = size.y;

            return result;
        }

        Vec2ui getParticleUpperLeftCorner(Vec2ui coordinate)
        {
            if(!coordinateOfCenter )
                return coordinate;

            Vec2ui result(coordinate.x - (size.x / 2), coordinate.y - (size.y / 2));
            if( result.x  < (size.x / 2) )
                result.x = 0;

            if( result.y  < (size.y / 2) )
                result.y = 0;

            return result;
        }
    };

    class DetectionQualityStructure
    {
    public:
        DetectionQualityStructure(size_t positivesSize, size_t falseNegativesSize, size_t falsePositivesSize, double error);
        DetectionQualityStructure();
        ~DetectionQualityStructure();

        void set(size_t positivesSize, size_t falseNegativesSize, size_t falsePositivesSize, double error);
        double getErrorPerHit();
        float getQuality();
        float getPurity();
        float getStrictPurity();
        float getWeightedQuality(int conservationRate);
        float getBalancedQuality();

        unsigned int positives;
        unsigned int falseNegatives;
        unsigned int falsePositives;
        double placementError;
    };

    std::ostream &operator<<(std::ostream &os, DetectionQualityStructure stats);
}