#pragma once

#include "io/datasource/ImageStackDatasource.h"


namespace ettention
{
    class PluginManager;
    class ProjectionSet;

    class ProjectionIteratorFactory final
    {
    public:
        static ProjectionSet* instantiate(PluginManager* pluginManager, std::string id, ImageStackDatasource* datasource);
    };
}