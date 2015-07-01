#include "stdafx.h"
#include "PluginManager.h"
#include "Plugin.h"
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/function.hpp>
#include "framework/error/Exception.h"

#ifdef WIN32
#include <windows.h>
#endif

namespace ettention
{
    PluginManager::PluginManager(Framework* framework)
        : framework(framework)
    {
    }

    PluginManager::PluginManager(Framework* framework, boost::filesystem::path pathToPluginDirectory)
        : framework(framework)
    {
        scanDirectory(pathToPluginDirectory);
    }

    PluginManager::~PluginManager()
    {
        for(auto it = autoLoadedPlugins.begin(); it != autoLoadedPlugins.end(); ++it)
        {
            delete *it;
        }
    }

    void PluginManager::scanDirectory(boost::filesystem::path pathToPluginDirectory)
    {
        if(!exists(pathToPluginDirectory))
            return;

        boost::filesystem::directory_iterator end_itr;

        for(boost::filesystem::directory_iterator fileIterator(pathToPluginDirectory); fileIterator != end_itr; ++fileIterator)
        {
            if(!boost::filesystem::is_regular_file(fileIterator->status()))
                continue;

            std::string filename = fileIterator->path().filename().string();
            if(filename.find("Plugin.dll") == std::string::npos)
                continue;

            loadPlugin(fileIterator->path());
        }
    }

    void PluginManager::loadPlugin(boost::filesystem::path pathToPlugin)
    {
#ifdef WIN32
        HINSTANCE hinstacePlugin = LoadLibrary(pathToPlugin.string().c_str());

        if(hinstacePlugin == NULL)
            return;

        typedef Plugin* (FAR *EntryFunction)();
        EntryFunction entryFunction = (EntryFunction)GetProcAddress(hinstacePlugin, "initializePlugin");

        if(entryFunction == NULL)
        {
            FreeLibrary(hinstacePlugin);
            return;
        }

        Plugin* plugin = (*entryFunction)();
        plugin->setFramework(framework);

        plugins[plugin->getName()] = plugin;

        LOGGER_IMP("loaded " << plugin->getName());

        registerPlugin(plugin);
        autoLoadedPlugins.push_back(plugin);
#else
        throw Exception("plugin loading not implemented on that platform");
#endif
    }

    ReconstructionAlgorithm* PluginManager::instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework)
    {
        for(auto it = plugins.begin(); it != plugins.end(); ++it)
        {
            auto algorithm = it->second->instantiateReconstructionAlgorithm(identifier, framework);
            if(algorithm)
            {
                return algorithm;
            }
        }

        throw Exception("Reconstruction type " + identifier + " not supported! Plugin missing?");
    }

    ProjectionSet* PluginManager::instantiateProjectionIterator(const std::string& identifier, ImageStackDatasource* source)
    {
        for(auto it = plugins.begin(); it != plugins.end(); ++it)
        {
            auto op = it->second->instantiateProjectionIterator(identifier, source);
            if(op != NULL)
            {
                return op;
            }
        }
        throw Exception("Instantiation of projection iterator failed! Plugin missing?");
    }

    void PluginManager::registerPlugin(Plugin* plugin)
    {
        plugin->registerImageFileFormat(framework->getImageStackDataSourceFactory());
        plugin->registerVolumeFileFormat(framework->getVolumeSerializer());
        plugin->registerForwardProjectionCreator(framework->getForwardProjectionFactory());
        plugin->registerBackProjectionCreator(framework->getBackProjectionFactory());
    }
}