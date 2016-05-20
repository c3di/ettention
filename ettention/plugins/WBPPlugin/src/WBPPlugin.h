#include "framework/plugins/Plugin.h"

namespace ettention 
{
    namespace wbp 
    {
        class PLUGIN_API WBPPlugin : public Plugin
        {
        public:
            WBPPlugin();
            virtual ~WBPPlugin();

            std::string getName() override;
            ReconstructionAlgorithm* instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework) override;
        };
    }
}