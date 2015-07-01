#include "stdafx.h"
#include "io/ImageStackDataSourceFactory.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/MRCStack.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"

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
        std::string filename = paramSet->get<IOParameterSet>()->InputStackFileName().string();

        for(auto it = protoTypes.rbegin(); it != protoTypes.rend(); ++it)
        {
            auto dataSource = it->second.get();
            if(dataSource->canHandleResource(paramSet))
            {
                return dataSource->instantiate(paramSet);
            }
        }

        throw Exception("unable to open image stack " + filename + ". incorrect filename or plugin missing?");
    }

    void ImageStackDatasourceFactory::addDataSourcePrototype(int priority, ImageStackDatasource* dataSource)
    {
        protoTypes.insert(std::pair< int, std::unique_ptr<ImageStackDatasource > >(priority, std::unique_ptr<ImageStackDatasource >(dataSource)));
    }

    void ImageStackDatasourceFactory::registerDefaultPrototypes()
    {
        addDataSourcePrototype(0, new MRCStack());
        addDataSourcePrototype(20, new ImageStackDirectoryDatasource());
    }
}