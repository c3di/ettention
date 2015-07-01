#include "stdafx.h"
#include "ImageStackDirectoryDatasource.h"
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "framework/Logger.h"
#include "io/deserializer/ImageDeserializer.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"

namespace ettention
{
    ImageStackDirectoryDatasource::ImageStackDirectoryDatasource()
    {
    }

    ImageStackDirectoryDatasource::~ImageStackDirectoryDatasource()
    {
    }

    Image* ImageStackDirectoryDatasource::LoadProjectionImage(const HyperStackIndex& index)
    {
        auto findIt = imageLocations.find(index);
        if(findIt == imageLocations.end())
        {
            ETTENTION_THROW_EXCEPTION("There is no projection for index " << index << "!");
        }
        const ImageLocation& location = findIt->second;
        boost::filesystem::path fpath = directory / location.GetPath().string();
        Image* image = 0;
        if(location.IsInsideImageStack())
        {
            image = ImageDeserializer::readImageFromStack(fpath.string(), location.GetIndexInImageStack());
        }
        else
        {
            image = ImageDeserializer::readImage(fpath.string());
        }
        if(projectionProperties[index[0]].onLogarithmicScale)
        {
            image->convertToLinearScale();
        }
        return image;
    }

    std::vector<HyperStackIndex> ImageStackDirectoryDatasource::CollectAllValidIndices() const
    {
        std::vector<HyperStackIndex> indices;
        for(auto it = imageLocations.begin(); it != imageLocations.end(); ++it)
        {
            indices.push_back(it->first);
        }
        return indices;
    }

    const boost::property_tree::ptree& ImageStackDirectoryDatasource::GetMetaData(const HyperStackIndex& index) const
    {
        auto findIt = projectionProperties.find(index);
        if(findIt == projectionProperties.end())
        {
            ETTENTION_THROW_EXCEPTION("There is no meta data for index " << index << "!");
        }
        return findIt->second.xmlNode;
    }

    bool ImageStackDirectoryDatasource::canHandleResource(AlgebraicParameterSet* paramSet)
    {
        boost::filesystem::path location = paramSet->get<IOParameterSet>()->InputStackFileName();

        if(!boost::filesystem::is_directory(location))
            return false;

        if(!boost::filesystem::exists(location / this->GetMetaDataFilename()))
            return false;

        return true;
    }

    void ImageStackDirectoryDatasource::openResource(AlgebraicParameterSet* paramSet)
    {
        this->directory = paramSet->get<IOParameterSet>()->InputStackFileName();
        this->readMetaDataFile(directory / this->GetMetaDataFilename());
    }

    ImageStackDatasource* ImageStackDirectoryDatasource::instantiate(AlgebraicParameterSet* paramSet)
    {
        ImageStackDirectoryDatasource* dataSource = new ImageStackDirectoryDatasource();
        dataSource->openResource(paramSet);
        return dataSource;
    }

    Vec2ui ImageStackDirectoryDatasource::getResolution() const
    {
        return resolution;
    }

    void ImageStackDirectoryDatasource::writeToLog()
    {
        LOGGER("Datasource type: ImageStackDirectoryDatasource");
    }

    unsigned int ImageStackDirectoryDatasource::getNumberOfProjections()
    {
        return static_cast<unsigned int>(imageLocations.size());
    }

    void ImageStackDirectoryDatasource::writeMetaDataFile(std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames, const ScannerGeometry& baseScannerGeometry, const VolumeProperties& volumeProperties)
    {
        if(tiltAngles.size() != filenames.size())
        {
            throw Exception("Filename count does not match tilt angle count!");
        }
        boost::property_tree::ptree propertyTree;

        writeMicroscopeMetaData(propertyTree, metaDataFileName, baseScannerGeometry, volumeProperties);
        writeProjectionMetaData(propertyTree, metaDataFileName, tiltAngles, filenames);

        boost::property_tree::xml_writer_settings<char> w(' ', 4);

        if(boost::filesystem::exists(metaDataFileName))
            boost::filesystem::remove(metaDataFileName);

        write_xml(metaDataFileName, propertyTree, std::locale(), w);
    }

    void ImageStackDirectoryDatasource::writeMicroscopeMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const ScannerGeometry& baseScannerGeometry, const VolumeProperties& volumeProperties)
    {
        boost::property_tree::ptree& geometryNode = propertyTree.add_child("geometry", boost::property_tree::ptree());

        boost::property_tree::ptree& detectorNode = geometryNode.add_child("detectorPosition", boost::property_tree::ptree());
        detectorNode.add("<xmlattr>.value", baseScannerGeometry.getDetectorBase());

        boost::property_tree::ptree& sourceNode = geometryNode.add_child("sourcePosition", boost::property_tree::ptree());
        sourceNode.add("<xmlattr>.value", baseScannerGeometry.getSourceBase());

        boost::property_tree::ptree& hPitchNode = geometryNode.add_child("horizontalPitch", boost::property_tree::ptree());
        hPitchNode.add("<xmlattr>.value", baseScannerGeometry.getHorizontalPitch());

        boost::property_tree::ptree& vPitchNode = geometryNode.add_child("verticalPitch", boost::property_tree::ptree());
        vPitchNode.add("<xmlattr>.value", baseScannerGeometry.getVerticalPitch());

        boost::property_tree::ptree& volumeDimNode = geometryNode.add_child("volumeDimensions", boost::property_tree::ptree());
        volumeDimNode.add("<xmlattr>.value", volumeProperties.GetVolumeResolution());
    }

    void ImageStackDirectoryDatasource::writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames)
    {
        for(unsigned int i = 0; i < tiltAngles.size(); i++)
        {
            boost::property_tree::ptree& projectionNode = propertyTree.add_child("projection", boost::property_tree::ptree());
            boost::property_tree::ptree& imageNode = projectionNode.add_child("image", boost::property_tree::ptree());
            imageNode.add("<xmlattr>.filename", filenames[i]);

            boost::property_tree::ptree& tiltNode = projectionNode.add_child("tiltAngle", boost::property_tree::ptree());
            tiltNode.add("<xmlattr>.value", tiltAngles[i]);
        }
    }

    void ImageStackDirectoryDatasource::readMetaDataFile(boost::filesystem::path metaDataFileName)
    {
        boost::property_tree::ptree xmlTree;
        read_xml(metaDataFileName.string(), xmlTree);
        std::vector<ImageLocation> tempImageLocations;
        std::vector<ProjectionProperties> tempProjectionProperties;
        for(auto it = xmlTree.begin(); it != xmlTree.end(); ++it)
        {
            if(it->first == "projection")
            {
                std::string filename = it->second.get_child("image").get<std::string>("<xmlattr>.filename");
                tempImageLocations.push_back(ImageLocation(filename));
                tempProjectionProperties.push_back(this->ParseProjectionProperties(it->second));
            }
            if(it->first == "projectionstack")
            {
                auto props = this->ParseProjectionProperties(it->second);
                std::string filename = it->second.get_child("image").get<std::string>("<xmlattr>.filename");
                unsigned int numberOfImages = ImageDeserializer::getNumberOfImagesInStack((directory / filename).string());
                for(unsigned int i = 0; i < numberOfImages; i++)
                {
                    tempImageLocations.push_back(ImageLocation(filename, i));
                    tempProjectionProperties.push_back(props);
                }
            }
        }
        std::unordered_map<float, unsigned int> tiltAngleToIndex;
        std::unordered_map<float, unsigned int> tiltAngleProjectionCount;
        for(unsigned int i = 0; i < tempProjectionProperties.size(); ++i)
        {
            
            float tiltAngle = tempProjectionProperties[i].tiltAngle;
            auto findIt = tiltAngleToIndex.find(tiltAngle);
            if(findIt == tiltAngleToIndex.end())
            {
                //Without the temporary variable, there seems to be a memory corruption on linux,
                //but I did not find the real cause
                unsigned int currentIndex = (unsigned int)tiltAngleToIndex.size();
                tiltAngleToIndex[tiltAngle] = currentIndex;
                tiltAngleProjectionCount[tiltAngle] = 0;
            }
            HyperStackIndex index(tiltAngleToIndex[tiltAngle], tiltAngleProjectionCount[tiltAngle]++);
            imageLocations[index] = tempImageLocations[i];
            projectionProperties[index] = tempProjectionProperties[i];
        }

        if(this->IsEvereyTiltAngleUnique())
        {
            this->ConvertToOneDimension();
            this->resolution = this->getProjectionImage(HyperStackIndex(0))->getResolution();
        }
        else
        {
            this->resolution = this->getProjectionImage(HyperStackIndex(0, 0))->getResolution();
        }

        ScannerGeometry baseScannerGeometry(ScannerGeometry::PROJECTION_PARALLEL, this->resolution);
        baseScannerGeometry.set(xmlTree.get_child("geometry").get_child("sourcePosition").get<Vec3f>("<xmlattr>.value"),
                                xmlTree.get_child("geometry").get_child("detectorPosition").get<Vec3f>("<xmlattr>.value"),
                                xmlTree.get_child("geometry").get_child("horizontalPitch").get<Vec3f>("<xmlattr>.value"),
                                xmlTree.get_child("geometry").get_child("verticalPitch").get<Vec3f>("<xmlattr>.value"));
        this->satRotator.SetBaseScannerGeometry(baseScannerGeometry);
    }

    ImageStackDirectoryDatasource::ProjectionProperties ImageStackDirectoryDatasource::ParseProjectionProperties(const boost::property_tree::ptree& node) const
    {
        ProjectionProperties props;
        props.onLogarithmicScale = node.get_child_optional("logscale").is_initialized();
        props.tiltAngle = node.get_child("tiltAngle").get<float>("<xmlattr>.value");
        props.xmlNode = node;
        return props;
    }

    std::string ImageStackDirectoryDatasource::GetMetaDataFilename() const
    {
        return "metadata.xml";
    }

    ScannerGeometry ImageStackDirectoryDatasource::getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet)
    {
        auto findIt = projectionProperties.find(index);
        if(findIt == projectionProperties.end())
        {
            ETTENTION_THROW_EXCEPTION("There is no scanner geometry for index " << index << "!");
        }
        return satRotator.GetRotatedScannerGeometry(findIt->second.tiltAngle, paramSet->get<IOParameterSet>()->XAxisTilt());
    }

    ScannerGeometry::ProjectionType ImageStackDirectoryDatasource::getProjectionType() const
    {
        return satRotator.GetBaseScannerGeometry().getProjectionType();
    }

    float ImageStackDirectoryDatasource::GetTiltAngle(const HyperStackIndex& index) const
    {
        auto findIt = projectionProperties.find(index);
        if(findIt == projectionProperties.end())
        {
            ETTENTION_THROW_EXCEPTION("There is no tilt angle for index " << index << "!");
        }
        return findIt->second.tiltAngle;
    }

    bool ImageStackDirectoryDatasource::IsEvereyTiltAngleUnique() const
    {
        std::unordered_set<float> tiltAngleSet;
        for(auto it = projectionProperties.begin(); it != projectionProperties.end(); ++it)
        {
            tiltAngleSet.insert(it->second.tiltAngle);
        }
        return tiltAngleSet.size() == projectionProperties.size();
    }

    void ImageStackDirectoryDatasource::ConvertToOneDimension()
    {
        auto oldProjectionProperties = projectionProperties;
        projectionProperties.clear();
        auto oldImageLocations = imageLocations;
        imageLocations.clear();
        for(auto it = oldProjectionProperties.begin(); it != oldProjectionProperties.end(); ++it)
        {
            HyperStackIndex oldIndex = it->first;
            HyperStackIndex newIndex(oldIndex[0]);
            projectionProperties[newIndex] = oldProjectionProperties[oldIndex];
            imageLocations[newIndex] = oldImageLocations[oldIndex];
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ImageLocation
    //////////////////////////////////////////////////////////////////////////

    ImageStackDirectoryDatasource::ImageLocation::ImageLocation()
        : indexInImageStack(NOT_INSIDE_IMAGE_STACK)
    {
    }

    ImageStackDirectoryDatasource::ImageLocation::ImageLocation(const boost::filesystem::path& path, unsigned int indexInImageStack)
        : path(path)
        , indexInImageStack(indexInImageStack)
    {
    }

    const boost::filesystem::path& ImageStackDirectoryDatasource::ImageLocation::GetPath() const
    {
        return path;
    }

    bool ImageStackDirectoryDatasource::ImageLocation::IsInsideImageStack() const
    {
        return indexInImageStack != NOT_INSIDE_IMAGE_STACK;
    }

    unsigned int ImageStackDirectoryDatasource::ImageLocation::GetIndexInImageStack() const
    {
        if(!this->IsInsideImageStack())
        {
            throw Exception("Path " + path.string() + " points directly to an image and not a stack of images!");
        }
        return indexInImageStack;
    }
}