#include "stdafx.h"

#include "STEMPlugin.h"

#include "reconstruction/TF_ART.h"
#include "reconstruction/TF_SART.h"
#include "io/TF_Datasource.h"

namespace ettention
{
    namespace stem
    {
        static const std::string TF_ART_ID = "tf-art";
        static const std::string TF_SART_ID = "tf-sart";

        STEMPlugin::STEMPlugin()
        {
        }

        STEMPlugin::~STEMPlugin()
        {
        }

        std::string STEMPlugin::getName()
        {
            return "STEM Plugin";
        }

        ReconstructionAlgorithm* STEMPlugin::instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework)
        {
            if(identifier == TF_ART_ID)
            {
                return new TF_ART(framework);
            }

            if(identifier == TF_SART_ID)
            {
                return new TF_SART(framework);
            }

            return NULL;
        }

        void STEMPlugin::registerImageFileFormat(ImageStackDatasourceFactory* factory)
        {
            factory->addDataSourcePrototype(20, new TF_Datasource());
        }

    }
}

extern "C" 
PLUGIN_API ettention::Plugin* initializePlugin()
{
    return new ettention::stem::STEMPlugin();
}
