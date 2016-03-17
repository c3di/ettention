#include "stdafx.h"
#include "ProjectionIteratorFactory.h"
#include "framework/plugins/PluginManager.h"
#include "model/projectionset/ProjectionSetIdentity.h"
#include "model/projectionset/ProjectionSetRandom.h"
#include "model/projectionset/ProjectionSetMaxAngle.h"
#include "model/projectionset/ProjectionSetBuckets.h"

namespace ettention
{
    ProjectionSet* ProjectionIteratorFactory::instantiate(PluginManager* pluginManager, std::string id, ImageStackDatasource* datasource)
    {
        if(id == "identity")
            return new ProjectionSetIdentity();
        if(id == "random")
            return new ProjectionSetRandom();
        if(id == "maxangle")
            return new ProjectionSetMaxAngle();
        if(id == "buckets")
            return new ProjectionSetBuckets();

        return pluginManager->instantiateProjectionIterator(id, datasource);
    }
}