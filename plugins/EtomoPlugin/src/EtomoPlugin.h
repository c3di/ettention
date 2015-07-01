#include "framework/plugins/Plugin.h"
#include "setup/parametersource/ParameterSource.h"
#include "setup/EtomoParameterSource.h"

namespace ettention 
{
    namespace etomo 
    {
        class PLUGIN_API EtomoPlugin : public Plugin
        {
        public:
            EtomoPlugin();
            ~EtomoPlugin();

            std::string getName() override;
        };
    }
}