#pragma once
#include "setup/ParameterSet/ParameterSet.h"

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

        unsigned int Samples() const;
    };
}