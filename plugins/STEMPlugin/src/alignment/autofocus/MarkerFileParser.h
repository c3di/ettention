#pragma once
#include "alignment/autofocus/Marker.h"
#include "alignment/autofocus/LandmarkChain.h"
#include "io/TF_Datasource.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "model/ProjectionSetRandomDirectionSequentialFocus.h"

namespace ettention
{
    namespace stem
    {
        typedef std::vector<std::shared_ptr<LandmarkChain>>::iterator LandmarkChainIterator;

        class MarkerFileParser
        {
        public:
            MarkerFileParser(std::string markerFileName, ProjectionSetRandomDirectionSequentialFocus* projectionSet, TF_Datasource* dataSource);
            MarkerFileParser(std::istream& iss);
            virtual ~MarkerFileParser();

            LandmarkChainIterator begin();
            LandmarkChainIterator end();

            unsigned int numberOfLandmarkChains();

        protected:
            void parseMarkerFile(std::istream& iss);

        protected:
            std::vector<std::shared_ptr<LandmarkChain>> allMarkerSets;
            ProjectionSetRandomDirectionSequentialFocus* projectionSet;
            TF_Datasource* dataSource;
        };

    }
}