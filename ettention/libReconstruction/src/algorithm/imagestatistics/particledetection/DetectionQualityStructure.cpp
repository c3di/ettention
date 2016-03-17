#include "stdafx.h"
#include "DetectionQualityStructure.h"

#include <iostream>
#include <fstream>
#include <cmath>

namespace ettention
{
    std::ostream &operator<<(std::ostream &os, DetectionQualityStructure stats)
    {
        os  << stats.positives << " "
            << stats.falseNegatives << " "
            << stats.falsePositives << " "
            << stats.getQuality() << " "
            << stats.getPurity() << " "
            << stats.getStrictPurity() << " "
            << stats.getBalancedQuality() << " "
            << stats.placementError << " "
            << stats.getErrorPerHit();
        return os;
    }

    DetectionQualityStructure::DetectionQualityStructure(size_t positivesSize, size_t falseNegativesSize, size_t falsePositivesSize, double error)
    {
        set(positivesSize, falseNegativesSize, falsePositivesSize, error);
    }

    DetectionQualityStructure::DetectionQualityStructure()
    {
        set(0, 0, 0, 0.0f);
    }

    DetectionQualityStructure::~DetectionQualityStructure()
    {    
    }

    void DetectionQualityStructure::set(size_t positivesSize, size_t falseNegativesSize, size_t falsePositivesSize, double error)
    {
        this->positives = static_cast<unsigned int>(positivesSize);
        this->falseNegatives = static_cast<unsigned int>(falseNegativesSize);
        this->falsePositives = static_cast<unsigned int>(falsePositivesSize);
        this->placementError = error;
    }

    double DetectionQualityStructure::getErrorPerHit()
    {
        if( positives == 0 )
            return -1.0f;

        return placementError / positives;
    }

    float DetectionQualityStructure::getQuality()
    {
        return this->getWeightedQuality(0);
    }

    float DetectionQualityStructure::getPurity()
    {
        return this->getWeightedQuality(1);
    }

    float DetectionQualityStructure::getStrictPurity()
    {
        return this->getWeightedQuality(10);
    }

    float DetectionQualityStructure::getWeightedQuality(int conservationRate)
    {
        if( (positives + falseNegatives + falsePositives) == 0 )
            return -1.0f;

        return static_cast<float>(positives) / (positives + falseNegatives + falsePositives * conservationRate);
    }

    float DetectionQualityStructure::getBalancedQuality()
    {
        if( (positives + falseNegatives + falsePositives) == 0 )
            return -1.0f;

        return static_cast<float>(positives) / (positives + falseNegatives*falseNegatives + falsePositives*falsePositives);
    }
}