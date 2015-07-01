#include "stdafx.h"

#include "YourNewPlugin.h"

namespace ettention
{
    namespace yournew
    {

        YourNewPlugin::YourNewPlugin()
        {
        }

        YourNewPlugin::~YourNewPlugin()
        {
        }

        std::string YourNewPlugin::getName()
        {
            return "YourNew Plugin";
        }
    }
}

extern "C" 
PLUGIN_API ettention::Plugin* initializePlugin()
{
    return new ettention::yournew::YourNewPlugin();
}
