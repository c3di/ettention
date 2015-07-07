#include "stdafx.h"

#include "EtomoPlugin.h"

namespace ettention
{
    namespace etomo
    {
        EtomoPlugin::EtomoPlugin()
        {
        }

        EtomoPlugin::~EtomoPlugin()
        {
        }

        std::vector<ParameterSource*> EtomoPlugin::instantiateParameterSource()
        {
            std::vector<ParameterSource*> parameterSources;
            ParameterSource* parameters = new EtomoParameterSource("", false);
            parameters->parse();
            parameterSources.push_back(parameters);
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
