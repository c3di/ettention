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
    private:
        class ImageLocation
        {
        private:
            boost::filesystem::path path;
            unsigned int indexInImageStack;

        public:
            static const unsigned int NOT_INSIDE_IMAGE_STACK = (unsigned int)-1;

            ImageLocation();
            ImageLocation(const boost::filesystem::path& path, unsigned int indexInImageStack = NOT_INSIDE_IMAGE_STACK);

            const boost::filesystem::path& GetPath() const;
            bool IsInsideImageStack() const;
            unsigned int GetIndexInImageStack() const;
        };

        struct ProjectionProperties
        {
            bool onLogarithmicScale;
            float tiltAngle;
            boost::property_tree::ptree xmlNode;
        };

        Vec2ui resolution;
        std::unordered_map<HyperStackIndex, ProjectionProperties, HyperStackIndexHash> projectionProperties;
        std::unordered_map<HyperStackIndex, ImageLocation, HyperStackIndexHash> imageLocations;
        SingleAxisTiltRotator satRotator;

        ProjectionProperties ParseProjectionProperties(const boost::property_tree::ptree& node) const;
        bool IsEvereyTiltAngleUnique() const;
        void ConvertToOneDimension();

    protected:
        Image* LoadProjectionImage(const HyperStackIndex& index) override;
        const boost::property_tree::ptree& GetMetaData(const HyperStackIndex& index) const;

    public:
        ImageStackDirectoryDatasource();
        ~ImageStackDirectoryDatasource();

        float GetTiltAngle(const HyperStackIndex& index) const;
        std::vector<HyperStackIndex> CollectAllValidIndices() const;

        ScannerGeometry::ProjectionType getProjectionType() const override;
        Vec2ui getResolution() const override;
        ScannerGeometry getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet) override;
        
        bool canHandleResource(AlgebraicParameterSet* paramSet) override;
        virtual void openResource(AlgebraicParameterSet* paramSet) override;
        ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) override;
        void writeToLog() override;
        unsigned int getNumberOfProjections() override;

        virtual std::string GetMetaDataFilename() const;
        static void writeMetaDataFile(std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames, const ScannerGeometry& baseScannerGeometry, const VolumeProperties& volumeProperties);

    protected:
        static void writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames);
        static void writeMicroscopeMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const ScannerGeometry& baseScannerGeometry, const VolumeProperties& volumeProperties);
        virtual void readMetaDataFile(boost::filesystem::path metaDataFileName);

    protected:
        boost::filesystem::path directory;
    };
}