#pragma once
#include "Plugin.h"
#include <map>
#include <string>
#include <boost/filesystem/path.hpp>
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"

namespace ettention
{
    class Framework;
    class ReconstructionAlgorithm;

    class PluginManager
    {
    public:
        PluginManager(Framework* framework);
        PluginManager(Framework* framework, boost::filesystem::path pathToPluginDirectory);
        ~PluginManager();

        ReconstructionAlgorithm* instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework);
        ForwardProjectionOperator* instantiateForwardProjectionOperator(ImageStackDatasource* source, Framework* framework);
        ProjectionSet* instantiateProjectionIterator(const std::string& identifier, ImageStackDatasource* source);
        void registerPlugin(Plugin* plugin);

    protected:
        void scanDirectory(boost::filesystem::path pathToPluginDirectory);
        void loadPlugin(boost::filesystem::path pathToPlugin);

        std::map<std::string, Plugin*> plugins;
        std::vector<Plugin*> autoLoadedPlugins;
        Framework* framework;
    };
}