#include "stdafx.h"
#include "WBPPlugin.h"
#include "reconstruction/WBP.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"

namespace ettention
{
    namespace wbp
    {
        WBPPlugin::WBPPlugin()
        {
        }

        WBPPlugin::~WBPPlugin()
        {
        }

        std::string WBPPlugin::getName()
        {
            return "WBP Plugin";
        }

        ReconstructionAlgorithm* WBPPlugin::instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework)
        {
            return identifier == "wbp" ? new WBP(framework) : 0;
        }
    }
}

extern "C" 
PLUGIN_API ettention::Plugin* initializePlugin()
{
    return new ettention::wbp::WBPPlugin;
}
