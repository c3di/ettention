#include "stdafx.h"
#include "ManualParameterSource.h"

namespace ettention
{
    ManualParameterSource::ParameterTypeWrapper::ParameterTypeWrapper(boost::any value)
        : value(value)
    {
    }

    ParameterSource::LogLevelValues ManualParameterSource::LogLevel() const
    {
        return ParameterSource::ALL;
    }

    void ManualParameterSource::LogLevel(const LogLevelValues level)
    {
    }

    void ManualParameterSource::parse()
    {
    }

    bool ManualParameterSource::parameterExists(std::string aName) const
    {
        auto paramIterator = parameters.find(aName);
        return paramIterator != parameters.end();
    }

    ManualParameterSource::ParameterTypeWrapper ManualParameterSource::getParameter(const std::string name) const
    {
        auto paramIterator = parameters.find(name);
        if(paramIterator != parameters.end())
        {
            return ParameterTypeWrapper(paramIterator->second);
        }
        else
        {
            return boost::any();
        }
    }

    std::string ManualParameterSource::getStringParameter(std::string aName) const
    {
        return getParameter(aName).as<std::string>();
    }

    boost::filesystem::path ManualParameterSource::getPathParameter(std::string aName) const
    {
        return boost::filesystem::path(getParameter(aName).as<std::string>());
    }

    float ManualParameterSource::getFloatParameter(std::string aName) const
    {
        return getParameter(aName).as<float>();
    }

    bool ManualParameterSource::getBoolParameter(std::string aName) const
    {
        return getParameter(aName).as<bool>();
    }

    int ManualParameterSource::getIntParameter(std::string aName) const
    {
        return getParameter(aName).as<int>();
    }

    unsigned int ManualParameterSource::getUIntParameter(std::string aName) const
    {
        return getParameter(aName).as<unsigned int>();
    }

    Vec3ui ManualParameterSource::getVec3uiParameter(std::string aName) const
    {
        return getParameter(aName).as<Vec3ui>();
    }

    Vec3f ManualParameterSource::getVec3fParameter(std::string aName) const
    {
        return getParameter(aName).as<Vec3f>();
    }
}