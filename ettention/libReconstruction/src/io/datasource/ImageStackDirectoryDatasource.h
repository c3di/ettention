#pragma once
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include "io/datasource/ImageStackDatasource.h"
#include "model/geometry/GeometricSetup.h"
#include "model/geometry/SingleAxisTiltRotator.h"

namespace ettention
{
    class ImageStackDirectoryDatasource : public ImageStackDatasource
    {
    protected:
        class ImageLocation
        {
        protected:
            boost::filesystem::path path;
            unsigned int indexInImageStack;

        public:
            static const unsigned int NOT_INSIDE_IMAGE_STACK = (unsigned int)-1;

            ImageLocation();
            ImageLocation(const boost::filesystem::path& path, unsigned int indexInImageStack = NOT_INSIDE_IMAGE_STACK);

            const boost::filesystem::path& getPath() const;
            bool isInsideImageStack() const;
            unsigned int getIndexInImageStack() const;
        };

        struct MetaDataNode
        {
            std::string nodename;
            bool onLogarithmicScale;
            float tiltAngle;
            boost::property_tree::ptree xmlNode;
        };

        Vec2ui resolution;
        std::map<HyperStackIndex, MetaDataNode> metaDataNodes;
        std::map<HyperStackIndex, ImageLocation> imageLocations;
        SingleAxisTiltRotator satRotator;

        MetaDataNode parseProjectionProperties(const boost::property_tree::ptree& node) const;
        bool isEveryTiltAngleUnique() const;
        void convertToOneDimension();
        const char* getName() const override;
        virtual HyperStackIndex firstIndex() const override;
        virtual HyperStackIndex lastIndex() const override;

    protected:
        Image* loadProjectionImage(const HyperStackIndex& index) override;
        boost::filesystem::path getAbsoluteImageLocation(const boost::filesystem::path& location);
        const MetaDataNode getMetaDataNode(const HyperStackIndex& index) const;

    public:
        ImageStackDirectoryDatasource();
        ~ImageStackDirectoryDatasource();

        float getTiltAngle(const HyperStackIndex& index) const;
        std::vector<HyperStackIndex> collectAllValidIndices() const;

        Vec2ui getResolution() const override;
        ScannerGeometry* createScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet) override;
        
        bool canHandleResource(AlgebraicParameterSet* paramSet) override;
        virtual void openResource(AlgebraicParameterSet* paramSet) override;
        ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) override;
        void writeToLog() override;
        unsigned int getNumberOfProjections() const override;

        virtual std::string getMetaDataFilename() const;
        static void writeMetaDataFile(std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames, const ScannerGeometry* baseScannerGeometry, const VolumeProperties& volumeProperties);

    protected:
        static void writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames);
        static void writeMicroscopeMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const ScannerGeometry* baseScannerGeometry, const VolumeProperties& volumeProperties);
        virtual void readMetaDataFile(boost::filesystem::path metaDataFileName);
        virtual ScannerGeometry* createScannerGeometry(boost::property_tree::ptree& geometryNode);

    protected:
        boost::filesystem::path directory;
    };
}