#pragma once
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class BackProjectionParameterSet : public ParameterSet
    {
    private:
        unsigned int samples;

    public:
        BackProjectionParameterSet(const ParameterSource* parameterSource);
        ~BackProjectionParameterSet();

        unsigned int getSamplesNumber() const;
    };
}