#include "framework/plugins/Plugin.h"
#include "setup/parametersource/ParameterSource.h"
#include "setup/EtomoParameterSource.h"

namespace ettention 
{
    namespace etomo 
    {
        class PLUGIN_API EtomoPlugin : public ettention::Plugin
        {
        public:
            EtomoPlugin();

            virtual ~EtomoPlugin();

            virtual std::string getName() override;
            virtual std::vector<ParameterSource*> instantiateParameterSource();

        private:
            ParameterSource* parameterSource;
        };
    }
}