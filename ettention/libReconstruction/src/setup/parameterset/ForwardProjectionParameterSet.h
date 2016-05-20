#pragma once
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class ForwardProjectionParameterSet : public ParameterSet
    {
    private:
        unsigned int samples;

    public:
        ForwardProjectionParameterSet(const ParameterSource* parameterSource);
        ~ForwardProjectionParameterSet();

        unsigned int getSamplesNumber() const;
    };
}
