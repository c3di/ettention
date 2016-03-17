#include "stdafx.h"
#include "ImageStackDatasource.h"
#include "framework/geometry/Visualizer.h"
#include "framework/Logger.h"
#include "model/image/Image.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/DebugParameterSet.h"
#include <cstdio>

namespace ettention
{
    ImageStackDatasource::ImageStackDatasource(unsigned int maxCachedImages)
        : maxCachedImages(maxCachedImages)
    {
        if(maxCachedImages == 0)
        {
            throw Exception("Maximum cached images for ImageStackDatasource must be at least 1!");
        }
    }

    ImageStackDatasource::~ImageStackDatasource()
    {
        clearCachedProjectionImages();
    }

    Image* ImageStackDatasource::getProjectionImage(const HyperStackIndex& index)
    {
        auto findIt = projectionImages.find(index);
        if (findIt != projectionImages.end())
            return findIt->second;

        Image* image = this->loadProjectionImage(index);
        projectionImages[index] = image;
        lruProjectionImages.push_front(std::make_pair(index, image));
        this->reorganizeCache();
        return image;
    }

    void ImageStackDatasource::clearCachedProjectionImages()
    {
        for(auto it = projectionImages.begin(); it != projectionImages.end(); ++it)
        {
            delete it->second;
        }
        projectionImages.clear();
        lruProjectionImages.clear();
    }

    void ImageStackDatasource::readTiltAngles(const boost::filesystem::path& tiltFilePath)
    {
        FILE* angles;
        angles = fopen(tiltFilePath.string().c_str(), "rt");

        if(!angles)
        {
            LOGGER_FILE_ERROR(tiltFilePath);
            throw Exception((boost::format("Could not open file: %1%") % tiltFilePath).str().c_str());
        }

        tiltAnglesInDegree.resize(getNumberOfProjections());

        float angle;
        unsigned int number = 0U;
        while( (fscanf(angles, "%f", &angle) != EOF) )
        {
            if( number >= tiltAnglesInDegree.size() )
            {
                tiltAnglesInDegree.push_back(angle);
            } else {
                tiltAnglesInDegree[number] = angle;
            }            
            ++number;
        }

        if( number != getNumberOfProjections() )
        {
            throw Exception((boost::format("Number of projections DIFFERS from number of Tilt Angles given in %1%") % tiltFilePath).str().c_str());
        }
        /*for(unsigned int i = 0; i < getNumberOfProjections(); i++)
        {
            fscanf(angles, "%f", &angle);
            tiltAnglesInDegree[i] = angle;
        }*/

        fclose(angles);
    }

    std::vector<float> ImageStackDatasource::getTiltAnglesInDegree()
    {
        return tiltAnglesInDegree;
    }


    void ImageStackDatasource::reorganizeCache()
    {
        while(lruProjectionImages.size() > maxCachedImages)
        {
            auto entry = lruProjectionImages.back();
            lruProjectionImages.pop_back();
            delete entry.second;
            projectionImages.erase(projectionImages.find(entry.first));
        }
    }

    void ImageStackDatasource::exportAllGeometriesTo(Visualizer* visualizer, AlgebraicParameterSet* paramSet)
    {
        for(unsigned int i = 0; i < getNumberOfProjections(); i += paramSet->get<DebugParameterSet>()->getMeshExportProjectionStepRate())
        {
            auto geometry = getScannerGeometry(i, paramSet);
            geometry->exportGeometryToVisualizer(visualizer, getProjectionImage(i));
        }
    }
}