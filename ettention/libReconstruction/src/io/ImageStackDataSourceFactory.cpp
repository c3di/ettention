#include "stdafx.h"
#include "io/ImageStackDataSourceFactory.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/MRCStack.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/InputParameterSet.h"

namespace ettention
{
    ImageStackDatasourceFactory::ImageStackDatasourceFactory()
    {
        registerDefaultPrototypes();
    }

    ImageStackDatasourceFactory::~ImageStackDatasourceFactory()
    {
    }

    ImageStackDatasource* ImageStackDatasourceFactory::instantiate(AlgebraicParameterSet* paramSet)
    {
        for(auto it = protoTypes.rbegin(); it != protoTypes.rend(); ++it)
        {
            auto dataSource = it->second.get();
            if(dataSource->canHandleResource(paramSet))
            {
                LOGGER("Chosen ImageStackDatasource: " << dataSource->getName() << ".");
                return dataSource->instantiate(paramSet);
            }
        }

        std::string filename = paramSet->get<InputParameterSet>()->getProjectionsPath().string();
        throw Exception("Unable to open image stack " + filename + "! Incorrect filename or plugin missing?");
    }

    void ImageStackDatasourceFactory::addDataSourcePrototype(int priority, ImageStackDatasource* dataSource)
    {
        protoTypes.insert(std::pair< int, std::unique_ptr<ImageStackDatasource > >(priority, std::unique_ptr<ImageStackDatasource >(dataSource)));
    }

    void ImageStackDatasourceFactory::registerDefaultPrototypes()
    {
        addDataSourcePrototype(0, new MRCStack());
        addDataSourcePrototype(1, new ImageStackDirectoryDatasource());
    }
}