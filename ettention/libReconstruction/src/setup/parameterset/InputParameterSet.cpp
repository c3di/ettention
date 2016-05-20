#include "stdafx.h"
#include "InputParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    InputParameterSet::InputParameterSet(const ParameterSource* parameterSource)
    {
        projectionsPath = parameterSource->getPathParameter("input.projections");
        
        tiltAnglesPath = "";
        if(parameterSource->parameterExists("input.tiltAngles"))
        {
            tiltAnglesPath = parameterSource->getPathParameter("input.tiltAngles");
        }

        xAxisTilt = 0.0f;
        if(parameterSource->parameterExists("input.xAxisTilt"))
        {
            xAxisTilt = parameterSource->getFloatParameter("input.xAxisTilt");
        }

        logarithmize = false;
        if(parameterSource->parameterExists("input.logarithmize"))
        {
            logarithmize = parameterSource->getBoolParameter("input.logarithmize");
        }

        if( parameterSource->parameterExists("input.skipProjections") )
        {
            parseSkipProjectionList(parameterSource->getStringParameter("input.skipProjections"));
        }
    }

    InputParameterSet::~InputParameterSet()
    {
    }

    const std::set<HyperStackIndex>& InputParameterSet::getSkipProjectionList() const
    {
        return skipProjectionList;
    }

    const boost::filesystem::path& InputParameterSet::getProjectionsPath() const
    {
        return projectionsPath;
    }

    const boost::filesystem::path& InputParameterSet::getTiltAnglesPath() const
    {
        return tiltAnglesPath;
    }

    float InputParameterSet::getXAxisTilt() const
    {
        return xAxisTilt;
    }

    bool InputParameterSet::getLogarithmize() const
    {
        return logarithmize;
    }

    void InputParameterSet::parseSkipProjectionList(const std::string &s)
    {
        if( s == "none" )
            return;
        boost::char_separator<char> sep(",");
        boost::tokenizer<boost::char_separator<char> >  tok(s, sep);
        boost::tokenizer<boost::char_separator<char>>::iterator i;

        for( i = tok.begin(); i != tok.end(); i++ )
        {
            std::string f(*i);
            boost::trim(f);

            if( f.find("-") != std::string::npos )
            {
                insertRangeIntoSkipProjectionsList(f);
            } else
            {
                unsigned int index = (boost::lexical_cast<unsigned int>(f)-1);
                skipProjectionList.insert(HyperStackIndex(index));
            }
        }
    }

    void InputParameterSet::insertRangeIntoSkipProjectionsList(const std::string &s)
    {
        boost::char_separator<char> sep("-");
        boost::tokenizer<boost::char_separator<char> >  tok(s, sep);
        boost::tokenizer<boost::char_separator<char>>::iterator i;

        unsigned int rangeBegin = boost::lexical_cast<unsigned int>((*tok.begin())) - 1;
        i = tok.begin();
        i++;
        unsigned int rangeEnd = boost::lexical_cast<unsigned int>((*i)) - 1;

        for( unsigned int j = rangeBegin; j <= rangeEnd; j++ )
            skipProjectionList.insert(HyperStackIndex(j));
    }
}
