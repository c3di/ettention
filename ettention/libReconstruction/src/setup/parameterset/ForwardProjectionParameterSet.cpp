#include "stdafx.h"
#include "ForwardProjectionParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    ForwardProjectionParameterSet::ForwardProjectionParameterSet(const ParameterSource* parameterSource)
    {
        samples = 1;
        if(parameterSource->parameterExists("forwardProjection.samples"))
        {
            samples = parameterSource->getUIntParameter("forwardProjection.samples");
        }
    }

    ForwardProjectionParameterSet::~ForwardProjectionParameterSet()
    {
    }

    unsigned int ForwardProjectionParameterSet::getSamplesNumber() const
    {
        return samples;
    }
}