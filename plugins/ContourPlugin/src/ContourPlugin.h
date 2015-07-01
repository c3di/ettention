#include "framework/plugins/Plugin.h"

namespace ettention 
{
    namespace contour 
    {
        class PLUGIN_API ContourPlugin : public Plugin
        {
        public:
            ContourPlugin(void);
            ~ContourPlugin(void);

            std::string getName() override;
            ReconstructionAlgorithm* instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework) override;
            void registerForwardProjectionCreator(ForwardProjectionFactory* factory) override;
            void registerBackProjectionCreator(BackProjectionFactory* factory) override;
        };
    }
}