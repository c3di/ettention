#include "framework/plugins/Plugin.h"

namespace ettention 
{
    namespace stem 
    {
        class PLUGIN_API STEMPlugin : public Plugin
        {
        public:
            STEMPlugin();
            ~STEMPlugin();

            std::string getName() override;
            ReconstructionAlgorithm* instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework) override;
            void registerImageFileFormat(ImageStackDatasourceFactory* factory) override;
        };
    }
}