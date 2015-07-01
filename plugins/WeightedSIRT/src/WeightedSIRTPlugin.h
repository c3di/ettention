#include "framework/plugins/Plugin.h"

namespace ettention 
{
    namespace wsirt 
    {
        class PLUGIN_API WeightedSIRTPlugin : public Plugin
        {
        public:
            WeightedSIRTPlugin();
            ~WeightedSIRTPlugin();

            std::string getName() override;
            ReconstructionAlgorithm* instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework) override;
        };
    }
}