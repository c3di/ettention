#include "stdafx.h"
#include "CascadingParameterSource.h"
#include "framework/error/ParameterNotFoundException.h"

namespace ettention
{
    CascadingParameterSource::CascadingParameterSource()
    {
    }

    CascadingParameterSource::~CascadingParameterSource()
    {
    }

    bool CascadingParameterSource::parameterExists(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return true;
            }
        }
        return false;
    };

    boost::filesystem::path CascadingParameterSource::getPathParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getPathParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    };

    std::string CascadingParameterSource::getStringParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getStringParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    };

    float CascadingParameterSource::getFloatParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getFloatParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    }

    bool CascadingParameterSource::getBoolParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getBoolParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    }

    int CascadingParameterSource::getIntParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getIntParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    }

    unsigned int CascadingParameterSource::getUIntParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getUIntParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    }

    Vec3ui CascadingParameterSource::getVec3uiParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getVec3uiParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    }

    Vec3f CascadingParameterSource::getVec3fParameter(std::string aName) const
    {
        for(auto it = sources.begin(); it != sources.end(); ++it)
        {
            if((*it)->parameterExists(aName))
            {
                return (*it)->getVec3fParameter(aName);
            }
        }
        throw ParameterNotFoundException(aName);
    }

    void CascadingParameterSource::parseAndAddSource(ParameterSource* source)
    {
        source->parse();
        sources.push_front(source);
    }

    void CascadingParameterSource::parse()
    {

    }

    ParameterSource::LogLevelValues CascadingParameterSource::LogLevel() const
    {
        return ParameterSource::LogLevelValues::OVERALL_PROGRESS;
    }

    void CascadingParameterSource::LogLevel(const ParameterSource::LogLevelValues level)
    {
        //
    }
}