#include "stdafx.h"

#include "EtomoPlugin.h"

namespace ettention
{
    namespace etomo
    {
        EtomoPlugin::EtomoPlugin()
        {
            parameterSource = new EtomoParameterSource("", false);
            parameterSource->parse();
        }

        EtomoPlugin::~EtomoPlugin()
        {
            delete parameterSource;
        }

        std::vector<ParameterSource*> EtomoPlugin::instantiateParameterSource()
        {
            std::vector<ParameterSource*> parameterSources;
            parameterSources.push_back(parameterSource);
            return parameterSources;
        }

        std::string EtomoPlugin::getName()
        {
            return "Etomo Plugin";
        }
    }
}

extern "C" 
PLUGIN_API ettention::Plugin* initializePlugin()
{
    return new ettention::etomo::EtomoPlugin();
}
