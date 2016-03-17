#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    ParameterSource::ParameterSource()
    {
    }

    ParameterSource::~ParameterSource()
    {
    }

    ParameterSource::LogLevel ParameterSource::getLogLevel() const
    {
        return logLevel;
    }

    void ParameterSource::setLogLevel(const LogLevel level)
    {
        this->logLevel = level;
    }

    float ParameterSource::getFloatParameter(std::string aName) const
    {
        std::string strValue = getStringParameter(aName);
        return boost::lexical_cast<float>(strValue);
    }

    bool ParameterSource::getBoolParameter(std::string aName) const
    {
        std::string strValue = getStringParameter(aName);
        return boost::lexical_cast<bool>(strValue);
    }

    int ParameterSource::getIntParameter(std::string aName) const
    {
        return boost::lexical_cast<int>(getStringParameter(aName));
    }

    unsigned int ParameterSource::getUIntParameter(std::string aName) const
    {
        return boost::lexical_cast<unsigned int>(getStringParameter(aName));
    }

    Vec2ui ParameterSource::getVec2uiParameter(std::string aName) const
    {
        return boost::lexical_cast<Vec2ui>(replaceCommasWithSpaces(getStringParameter(aName)));
    }

    Vec3ui ParameterSource::getVec3uiParameter(std::string aName) const
    {
        return boost::lexical_cast<Vec3ui>(replaceCommasWithSpaces(getStringParameter(aName)));
    }

    Vec3f ParameterSource::getVec3fParameter(std::string aName) const
    {
        return boost::lexical_cast<Vec3f>(replaceCommasWithSpaces(getStringParameter(aName)));
    }

    Vec2f ParameterSource::getVec2fParameter(std::string aName) const
    {
        return boost::lexical_cast<Vec2f>(replaceCommasWithSpaces(getStringParameter(aName)));
    }

    std::string ParameterSource::replaceCommasWithSpaces(std::string str) const
    {
        std::replace(str.begin(), str.end(), ',', ' ');
        return str;
    }
}