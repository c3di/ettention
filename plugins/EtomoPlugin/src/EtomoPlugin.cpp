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
