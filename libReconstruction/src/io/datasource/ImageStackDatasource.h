#pragma once
#include <boost/filesystem.hpp>
#include "io/HyperStackIndex.h"
#include "model/geometry/ScannerGeometry.h"
#include "model/image/Image.h"
#include "model/volume/VolumeProperties.h"

namespace ettention
{
    class AlgebraicParameterSet;
    class IOParameterSet;

    class ImageStackDatasource
    {
    private:
        std::unordered_map<HyperStackIndex, Image*, HyperStackIndexHash> projectionImages;
        unsigned int maxCachedImages;
        typedef std::pair<HyperStackIndex, Image*> LruEntry;
        std::list<LruEntry> lruProjectionImages;

        void ReorganizeCache();

    protected:
        std::vector<float> tiltAnglesInDegree;

    public:
        static const unsigned int INFINITE_CACHED_IMAGES = (unsigned int)-1;

        ImageStackDatasource(unsigned int maxCachedImages = 16);
        virtual ~ImageStackDatasource();

        virtual Image* getProjectionImage(const HyperStackIndex& index);
        void clearCachedProjectionImages();
        void ReadTiltAngles(const boost::filesystem::path& tiltFilePath);
        std::vector<float> getTiltAnglesInDegree();

        virtual ScannerGeometry::ProjectionType getProjectionType() const = 0;
        virtual Vec2ui getResolution() const = 0;
        virtual ScannerGeometry getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet) = 0;
        virtual bool canHandleResource(AlgebraicParameterSet* paramSet) = 0;
        virtual void openResource(AlgebraicParameterSet* paramSet) = 0;
        virtual ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) = 0;
        virtual void writeToLog() = 0;
        virtual unsigned int getNumberOfProjections() = 0;
        virtual Image* LoadProjectionImage(const HyperStackIndex& index) = 0;
    };
}