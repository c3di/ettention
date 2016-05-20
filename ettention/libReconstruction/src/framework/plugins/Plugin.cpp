#include "stdafx.h"
#include "Plugin.h"

namespace ettention
{
    Plugin::Plugin()
    {
    }

    Plugin::~Plugin()
    {
    }

    Framework* Plugin::getFramework()
    {
        return framework;
    }

    void Plugin::setFramework(Framework* val)
    {
        framework = val;
    }

    void Plugin::registerImageFileFormat(ImageStackDatasourceFactory* factory)
    {
    }

    void Plugin::registerVolumeFileFormat(VolumeSerializer* factory)
    {
    }

    void Plugin::registerForwardProjectionCreator(ForwardProjectionFactory* factory)
    {

    }

    void Plugin::registerBackProjectionCreator(BackProjectionFactory* factory)
    {

    }

    ReconstructionAlgorithm* Plugin::instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework)
    {
        return NULL;
    }

    ProjectionSet* Plugin::instantiateProjectionIterator(const std::string& identifier, ImageStackDatasource* source)
    {
        return NULL;
    }

    std::vector<ParameterSource*> Plugin::instantiateParameterSource()
    {
        std::vector<ParameterSource*> parameterSources;
        return parameterSources;
    }

}