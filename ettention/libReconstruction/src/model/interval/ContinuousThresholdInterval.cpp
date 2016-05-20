#include "stdafx.h"
#include "ContinuousThresholdInterval.h"

#include "framework/VectorAlgorithms.h"
#include "framework/error/Exception.h"
#include <boost/algorithm/string.hpp>

namespace ettention
{

    ContinuousThresholdInterval::ContinuousThresholdInterval()
        : Interval(1.0f)
    {
    }

    void ContinuousThresholdInterval::parseFromString(const std::string& input)
    {
        levelThresholdValues.clear();
        std::vector<std::string> tokens = VectorAlgorithms::splitString(input, "<");
        std::string temp;
        for( auto it = tokens.begin(); it < tokens.end(); ++it )
        {
            temp = boost::trim_copy(*it);
            if(temp.empty())
                continue;

            levelThresholdValues.push_back(boost::lexical_cast<float>(temp));
        }

        levelIntensityValues.clear();
        auto count = tokens.size();
        float valueStep = 1.0f / count;
        float currentValue = 0.0f;

        levelIntensityValues.push_back(currentValue);
        for( size_t i = 1; i < count; ++i )
        {
            currentValue += valueStep;
            levelIntensityValues.push_back(currentValue);
        }
    }

    std::string ContinuousThresholdInterval::convertToString() const
    {
        std::stringstream ss;
        for( auto it = levelThresholdValues.begin(); it < levelThresholdValues.end(); ++it )
        {
            ss << (*it);
            if( it < levelThresholdValues.end() )
                ss << " < ";
        }
        return ss.str();
    }

    void ContinuousThresholdInterval::parseIntensityValues(const std::string& input)
    {
        levelIntensityValues.clear();
        std::vector<std::string> tokens = VectorAlgorithms::splitString(boost::trim_copy(input), " ");
        if( tokens.size() != levelThresholdValues.size() )
            throw Exception( (boost::format("The amount of threshold levels (%1%) is different from number of suggested material intensity levels (%2%).") % levelThresholdValues.size() % tokens.size()).str() );

        std::string temp;
        for( auto it = tokens.begin(); it < tokens.end(); ++it )
        {
            temp = boost::trim_copy(*it);
            if( temp.empty() )
                continue;

            levelIntensityValues.push_back(boost::lexical_cast<float>(temp));
        }
    }

    const std::vector<float>& ContinuousThresholdInterval::getThresholdValues() const
    {
        return levelThresholdValues;
    }

    const std::vector<float>& ContinuousThresholdInterval::getIntensityValues() const
    {
        return levelIntensityValues;
    }

}