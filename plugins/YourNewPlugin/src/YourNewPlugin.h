#include "framework/plugins/Plugin.h"

namespace ettention 
{
    namespace yournew 
    {
        class PLUGIN_API YourNewPlugin : public Plugin 
        {
        public:
            YourNewPlugin();
            ~YourNewPlugin();

            std::string getName() override;
        };
    }
}