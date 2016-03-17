#include "stdafx.h"
#include "AlgebraicParameterSet.h"
#include "setup/parameterset/ParameterSet.h"
#include "setup/parametersource/ParameterSource.h"
#include "framework/error/Exception.h"

namespace ettention
{
    AlgebraicParameterSet::AlgebraicParameterSet(const ParameterSource* parameterSource)
        : parameterSource(parameterSource)
    {
    }

    AlgebraicParameterSet::~AlgebraicParameterSet()
    {
        while(!parameterSets.empty())
        {
            delete parameterSets.back();
            parameterSets.pop_back();
        }
    }
}