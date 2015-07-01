#include "stdafx.h"
#include "ContourPlugin.h"
#include "algorithm/projections/ContourBackProjectionCreator.h"
#include "algorithm/projections/ContourForwardProjectionCreator.h"

namespace ettention
{
    namespace contour
    {
        ContourPlugin::ContourPlugin()
        {
        }

        ContourPlugin::~ContourPlugin()
        {
        }

        std::string ContourPlugin::getName()
        {
            return "Contour Prior Knowledge Plugin";
        }

        ReconstructionAlgorithm* ContourPlugin::instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework)
        {
            return 0;
        }

        void ContourPlugin::registerBackProjectionCreator(BackProjectionFactory* factory)
        {
            factory->RegisterCreator(std::unique_ptr<BackProjectionFactory::InstanceCreator>(new ContourBackProjectionCreator));
        }

        void ContourPlugin::registerForwardProjectionCreator(ForwardProjectionFactory* factory)
        {
            factory->RegisterCreator(std::unique_ptr<ForwardProjectionFactory::InstanceCreator>(new ContourForwardProjectionCreator));
        }
    }
}

extern "C" 
PLUGIN_API ettention::Plugin* initializePlugin()
{
    return new ettention::contour::ContourPlugin();
}
