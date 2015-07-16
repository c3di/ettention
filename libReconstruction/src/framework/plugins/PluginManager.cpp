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
#else
#include <dlfcn.h>
#include <unistd.h>
#include <limits.h>
#endif

namespace ettention
{
    PluginManager::PluginManager(Framework* framework)
        : framework(framework)
    {
		auto pluginPath = getPathToPluginDirectory();
		scanDirectory( pluginPath );
	}

    PluginManager::~PluginManager()
    {
        for(auto it = autoLoadedPlugins.begin(); it != autoLoadedPlugins.end(); ++it)
        {
            delete *it;
        }
    }

	boost::filesystem::path PluginManager::getPathToPluginDirectory()
	{
#ifdef WIN32
		char buffer[1024];
		size_t length = GetModuleFileName(NULL, buffer, sizeof(buffer) );
		if ( length != 0 ) 
		{
			buffer[length] = '\0';
			boost::filesystem::path exeFullPath = boost::filesystem::path( std::string( buffer ) );
			return exeFullPath.remove_filename();
		} else {
			throw Exception("unable to detect path of executable");
		}
#else
		char buffer[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
		if (len != -1) {
			buffer[len] = '\0';
			boost::filesystem::path exeFullPath = boost::filesystem::path( std::string( buffer ) );
			return exeFullPath.remove_filename();
		}
		throw Exception("unable to detect path of executable");
#endif
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

#ifdef WIN32
            if(filename.find("Plugin.dll") == std::string::npos)
                continue;
#else
            if (filename.find("Plugin.so") == std::string::npos)
                continue;
#endif
            loadPlugin(fileIterator->path());
        }
    }

    void PluginManager::loadPlugin(boost::filesystem::path pathToPlugin)
    {
#ifdef WIN32
        HINSTANCE hinstacePlugin = LoadLibrary(pathToPlugin.string().c_str());

        if (hinstacePlugin == NULL)
            return;

        typedef Plugin* (FAR *EntryFunction)();
        EntryFunction entryFunction = (EntryFunction)GetProcAddress(hinstacePlugin, "initializePlugin");

        if (entryFunction == NULL)
        {
            FreeLibrary(hinstacePlugin);
            return;
        }
#else
        void* libHandle = dlopen( pathToPlugin.string().c_str(), RTLD_LAZY );
        if (!libHandle) 
            return;

	    typedef Plugin* ( *EntryFunction )();

        EntryFunction entryFunction = (EntryFunction) dlsym(libHandle, "initializePlugin");
        if (dlerror() != NULL )
        {
            dlclose(libHandle);
            return;
        }
#endif
        Plugin* plugin = (*entryFunction)();
        plugin->setFramework(framework);

        plugins[plugin->getName()] = plugin;

        LOGGER_IMP("loaded " << plugin->getName());

        registerPlugin(plugin);
        autoLoadedPlugins.push_back(plugin);
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

    std::vector<ParameterSource*> PluginManager::instantiateParameterSource()
    {
        std::vector<ParameterSource*> parameterSources;
        for (auto it = plugins.begin(); it != plugins.end(); ++it)
        {
            std::vector<ParameterSource*> op = it->second->instantiateParameterSource();
            parameterSources.insert(parameterSources.end(), op.begin(), op.end());
        }
        return parameterSources;
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
