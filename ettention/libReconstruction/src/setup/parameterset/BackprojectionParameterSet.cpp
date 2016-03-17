#include "stdafx.h"
#include "BackprojectionParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    BackProjectionParameterSet::BackProjectionParameterSet(const ParameterSource* parameterSource)
    {
        samples = 1;
        if(parameterSource->parameterExists("backProjection.samples"))
        {
            samples = parameterSource->getUIntParameter("backProjection.samples");
        }
    }

    BackProjectionParameterSet::~BackProjectionParameterSet()
    {
    }

    unsigned int BackProjectionParameterSet::getSamplesNumber() const
    {
        return samples;
    }

}