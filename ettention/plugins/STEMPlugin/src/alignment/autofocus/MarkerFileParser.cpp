#include "stdafx.h"

#include "MarkerFileParser.h"
#include "alignment/autofocus/Marker.h"
#include "io/serializer/ImageSerializer.h"

#include "framework/Logger.h"

namespace ettention
{
    namespace stem
    {
        MarkerFileParser::MarkerFileParser(std::string markerFileName,
                                           ProjectionSetRandomDirectionSequentialFocus* projectionSet,
                                           TF_Datasource* dataSource)
           : projectionSet(projectionSet)
           , dataSource(dataSource)
        {
            if((! boost::filesystem::exists(markerFileName)) || (! boost::filesystem::is_regular_file(markerFileName)))
                throw Exception("unable to open marker file " + markerFileName);

            std::ifstream is(markerFileName);
            parseMarkerFile(is);
            is.close();
        }

        MarkerFileParser::MarkerFileParser(std::istream& is)
        {
            parseMarkerFile(is);
        }

        MarkerFileParser::~MarkerFileParser()
        {
        }

        LandmarkChainIterator MarkerFileParser::begin()
        {
            return allMarkerSets.begin();
        }

        LandmarkChainIterator MarkerFileParser::end()
        {
            return allMarkerSets.end();
        }
 
        unsigned int MarkerFileParser::numberOfLandmarkChains()
        {
            return (unsigned int) allMarkerSets.size();
        }

        void MarkerFileParser::parseMarkerFile(std::istream& iss)
        {
            LOGGER("parsing marker stream ...");
            std::string line;   
            int currentMarkerId = -1;
            std::shared_ptr<LandmarkChain> currentMarkerSet;
            while(iss.good())
            {
                getline(iss, line);
                std::stringstream linestream(line);
                int markerId, directionIndex;
                Vec2f positionInImage;
                linestream >> markerId;

                if(markerId != currentMarkerId) 
                {
                    currentMarkerSet = std::make_shared<LandmarkChain>();
                    currentMarkerId = markerId;
                    allMarkerSets.push_back(currentMarkerSet);
                 }

                linestream >> directionIndex;
                directionIndex--; // in the file, counting starts with 1, not 0

                linestream >> positionInImage.x;
                linestream >> positionInImage.y;

                // auto originalIndex = projectionSet->GetOriginalProjectionIndex(directionIndex, 0);
                float tiltAngle = dataSource->GetTiltAngle( HyperStackIndex(directionIndex, 0) );
                std::shared_ptr<Marker> marker = std::make_shared<Marker>(currentMarkerId, tiltAngle, positionInImage);
                currentMarkerSet->addEntryForTiltAngle(tiltAngle, marker);

                int numberOfProjections = projectionSet->getFocusCount(directionIndex);
                marker->getIntensityZProjection().resize(numberOfProjections);
                for(int zPosition = 0; zPosition < numberOfProjections; zPosition++) 
                {
                    auto projectionIndex = projectionSet->getOriginalProjectionIndex(directionIndex, zPosition);
                    auto projection = dataSource->getProjectionImage(projectionIndex);
                    // float intensity = projection->ImageData()->Pixel((int) positionInImage.x, (int) positionInImage.y);
                    // float intensity = projection->ImageData()->getLocalVarianceAtPixel(Vec2i((int) positionInImage.x, (int) positionInImage.y), 3);
                    float intensity = projection->computeGaussFilteredPixel((Vec2i)positionInImage, 3);
 
                    marker->getIntensityZProjection()[ zPosition ] = intensity;
                }
            }
            LOGGER("done parsing marker stream");
        }
    }
}