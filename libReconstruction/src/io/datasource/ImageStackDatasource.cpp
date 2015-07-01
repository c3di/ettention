#include "stdafx.h"
#include "io/datasource/ImageStackDatasource.h"
#include "framework/Logger.h"

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
        auto image = findIt == projectionImages.end() ? 0 : findIt->second;
        if(!image)
        {
            projectionImages[index] = this->LoadProjectionImage(index);
            image = projectionImages[index];
            lruProjectionImages.push_front(std::make_pair(index, image));
            this->ReorganizeCache();
        }
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

    void ImageStackDatasource::ReadTiltAngles(const boost::filesystem::path& tiltFilePath)
    {
        FILE* angles;
        angles = fopen(tiltFilePath.string().c_str(), "rt");

        if(!angles)
        {
            LOGGER_FILE_ERROR(tiltFilePath);
            throw Exception((boost::format("Could not open file: %1%") % tiltFilePath).str().c_str());
        }

        tiltAnglesInDegree.resize(getNumberOfProjections());

        for(unsigned int i = 0; i < getNumberOfProjections(); i++)
        {
            float angle;
            fscanf(angles, "%f", &angle);
            tiltAnglesInDegree[i] = angle;
        }

        fclose(angles);
    }

    std::vector<float> ImageStackDatasource::getTiltAnglesInDegree()
    {
        return tiltAnglesInDegree;
    }


    void ImageStackDatasource::ReorganizeCache()
    {
        while(lruProjectionImages.size() > maxCachedImages)
        {
            auto entry = lruProjectionImages.back();
            lruProjectionImages.pop_back();
            delete entry.second;
            projectionImages.erase(projectionImages.find(entry.first));
        }
    }
}