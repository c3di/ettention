#include "stdafx.h"
#include "WBPParameterSet.h"
#include "framework/error/Exception.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    namespace wbp
    {
        WBPParameterSet::WBPParameterSet(const ParameterSource* parameterSource)
        {
            filterResolution = parameterSource->getVec3uiParameter("wbp.filterResolution").xy();
        }

        WBPParameterSet:: ~WBPParameterSet()
        {
        }

        Vec2ui WBPParameterSet::getFilterResolution() const
        {
            return filterResolution;
        }
    }
}