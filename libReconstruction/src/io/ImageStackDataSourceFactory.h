#pragma once

#include <memory>

namespace ettention
{
    class AlgebraicParameterSet;
    class ImageStackDatasource;

    /*
        The ImageStackDatasourceFactory is used to open a image stack location, for example a directory containing images or a mrc stack.
        The Factory has a number of ImageStackDatasource objects, which represent the different data formats.

        A program or plugin that wants to register a new data format uses the addDataSourcePrototype method. The factory will always
        instantiate a ImageStackDatasource that works with the given image location. In case of ambiguity, the priority given when registering
        the DataSource is used to resolve (DataSource with highest priority is instantiated).
        */
    class ImageStackDatasourceFactory
    {
    private:
        std::multimap<int, std::unique_ptr<ImageStackDatasource>> protoTypes;

        void registerDefaultPrototypes();

    public:
        ImageStackDatasourceFactory();
        ~ImageStackDatasourceFactory();

        ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet);
        void addDataSourcePrototype(int priority, ImageStackDatasource* dataSource);
    };
}