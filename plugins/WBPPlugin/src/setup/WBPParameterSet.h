#pragma once
#include "setup/ParameterSet/ParameterSet.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class ParameterSource;

    namespace wbp
    {
        class PLUGIN_API WBPParameterSet : public ParameterSet
        {
        public:
            WBPParameterSet(const ParameterSource* parameterSource);
            ~WBPParameterSet();

            Vec2ui getFilterResolution() const;

        protected:
            Vec2ui filterResolution;
        };
    }
}