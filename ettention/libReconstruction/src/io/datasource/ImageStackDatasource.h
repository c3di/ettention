#pragma once
#include "io/HyperStackIndex.h"
#include "model/geometry/ScannerGeometry.h"

namespace ettention
{
    class AlgebraicParameterSet;
    class Image;
    class Visualizer;

    class ImageStackDatasource
    {
    public:
        static const unsigned int INFINITE_CACHED_IMAGES = (unsigned int)-1;

        ImageStackDatasource(unsigned int maxCachedImages = 16);
        virtual ~ImageStackDatasource();

        virtual Image* getProjectionImage(const HyperStackIndex& index);
        void clearCachedProjectionImages();
        void readTiltAngles(const boost::filesystem::path& tiltFilePath);
        std::vector<float> getTiltAnglesInDegree();
        void exportAllGeometriesTo(Visualizer* visualizer, AlgebraicParameterSet* paramSet);

        virtual Vec2ui getResolution() const = 0;
        virtual ScannerGeometry* createScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet) = 0;
        virtual bool canHandleResource(AlgebraicParameterSet* paramSet) = 0;
        virtual void openResource(AlgebraicParameterSet* paramSet) = 0;
        virtual ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) = 0;
        virtual void writeToLog() = 0;
        virtual unsigned int getNumberOfProjections() const = 0;
        virtual Image* loadProjectionImage(const HyperStackIndex& index) = 0;
        virtual const char* getName() const = 0;
        virtual HyperStackIndex firstIndex() const = 0;
        virtual HyperStackIndex lastIndex() const = 0;

    protected:
        std::vector<float> tiltAnglesInDegree;

    protected:
        std::unordered_map<HyperStackIndex, Image*, HyperStackIndexHash> projectionImages;
        unsigned int maxCachedImages;
        typedef std::pair<HyperStackIndex, Image*> LruEntry;
        std::list<LruEntry> lruProjectionImages;

        void reorganizeCache();
    };
}