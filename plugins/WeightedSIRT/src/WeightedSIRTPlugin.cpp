#include "stdafx.h"

#include "WeightedSIRTPlugin.h"
#include "reconstruction/WeightedSIRT.h"

namespace ettention
{
    namespace wsirt
    {
        WeightedSIRTPlugin::WeightedSIRTPlugin()
        {
        }

        WeightedSIRTPlugin::~WeightedSIRTPlugin()
        {
        }

        std::string WeightedSIRTPlugin::getName()
        {
            return "Weighted SIRT Plugin";
        }

        ReconstructionAlgorithm* WeightedSIRTPlugin::instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework)
        {
            return identifier == "weightedsirt" ? new WeightedSIRT(framework) : 0;
        }
    }
}

extern "C" 
PLUGIN_API ettention::Plugin* initializePlugin()
{
    return new ettention::wsirt::WeightedSIRTPlugin;
}
