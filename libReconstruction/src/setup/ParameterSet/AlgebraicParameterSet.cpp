#include "stdafx.h"
#include "AlgebraicParameterSet.h"
#include "setup/ParameterSet/ParameterSet.h"
#include "setup/parametersource/ParameterSource.h"
#include "framework/error/Exception.h"

namespace ettention
{
    AlgebraicParameterSet::AlgebraicParameterSet(const ParameterSource* parameterSource)
        : parameterSource(parameterSource)
    {
        projectionSetType = std::string("random");
        if(parameterSource->parameterExists("projectionIteration"))
            projectionSetType = parameterSource->getStringParameter("projectionIteration");

        if(parameterSource->parameterExists("skipProjections"))
            ParseSkipProjectionList(parameterSource->getStringParameter("skipProjections"));

        basisFunctions = BASIS_VOXELS;
        if(parameterSource->parameterExists("basisFunctions"))
        {
            if(parameterSource->getStringParameter("basisFunctions") == "blobs")
            {
                basisFunctions = BASIS_BLOBS;
            }
            else if(parameterSource->getStringParameter("basisFunctions") != "voxels")
            {
                throw Exception("Illegal value for parameter \"basisFunctions\": " + parameterSource->getStringParameter("basisFunctions") + "!");
            }
        }
    }

    AlgebraicParameterSet::~AlgebraicParameterSet()
    {
        while(!parameterSets.empty())
        {
            delete parameterSets.back();
            parameterSets.pop_back();
        }
    }

    const std::set<HyperStackIndex>& AlgebraicParameterSet::SkipProjectionList() const
    {
        return skipProjectionList;
    }

    void AlgebraicParameterSet::ParseSkipProjectionList(const std::string &s)
    {
        if(s == "none")
            return;
        boost::char_separator<char> sep(",");
        boost::tokenizer<boost::char_separator<char> >  tok(s, sep);
        boost::tokenizer<boost::char_separator<char>>::iterator i;

        for(i = tok.begin(); i != tok.end(); i++)
        {
            std::string f(*i);
            boost::trim(f);

            if(f.find("-") != std::string::npos)
            {
                InsertRangeIntoSkipProjectionsList(f);
            }
            else
            {
                unsigned int index = (boost::lexical_cast<unsigned int>(f) - 1);
                skipProjectionList.insert(HyperStackIndex(index));
            }
        }
    }

    void AlgebraicParameterSet::InsertRangeIntoSkipProjectionsList(const std::string &s)
    {
        boost::char_separator<char> sep("-");
        boost::tokenizer<boost::char_separator<char> >  tok(s, sep);
        boost::tokenizer<boost::char_separator<char>>::iterator i;

        unsigned int rangeBegin = boost::lexical_cast<unsigned int>((*tok.begin())) - 1;
        i = tok.begin();
        i++;
        unsigned int rangeEnd = boost::lexical_cast<unsigned int>((*i)) - 1;

        for(unsigned int j = rangeBegin; j <= rangeEnd; j++)
            skipProjectionList.insert(HyperStackIndex(j));
    }

    const std::string& AlgebraicParameterSet::ProjectionSetType() const
    {
        return projectionSetType;
    }

    AlgebraicParameterSet::BasisFunctionType AlgebraicParameterSet::GetBasisFunctions() const
    {
        return basisFunctions;
    }
}