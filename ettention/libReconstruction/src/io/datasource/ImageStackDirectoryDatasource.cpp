#include "stdafx.h"
#include "ImageStackDirectoryDatasource.h"
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "framework/Logger.h"
#include "io/deserializer/ImageDeserializer.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/InputParameterSet.h"
#include "model/geometry/ParallelScannerGeometry.h"

namespace ettention
{
    ImageStackDirectoryDatasource::ImageStackDirectoryDatasource()
    {
    }

    ImageStackDirectoryDatasource::~ImageStackDirectoryDatasource()
    {
    }

    Image* ImageStackDirectoryDatasource::loadProjectionImage(const HyperStackIndex& index)
    {
        auto findIt = imageLocations.find(index);
        if(findIt == imageLocations.end())
        {
            ETTENTION_THROW_EXCEPTION("There is no projection for index " << index << "!");
        }
        const ImageLocation& location = findIt->second;
        boost::filesystem::path fpath = getAbsoluteImageLocation( location.GetPath() );
        Image* image = 0;
        if(location.IsInsideImageStack())
        {
            image = ImageDeserializer::readImageFromStack(fpath.string(), location.GetIndexInImageStack());
        }
        else
        {
            image = ImageDeserializer::readImage(fpath.string());
        }
        if(metaDataNodes[index].onLogarithmicScale)
        {
            image->convertToLinearScale();
        }
        return image;
    }

    boost::filesystem::path ImageStackDirectoryDatasource::getAbsoluteImageLocation( const boost::filesystem::path& location )
    {
        return directory / location.string();
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

    const ImageStackDirectoryDatasource::MetaDataNode ImageStackDirectoryDatasource::GetMetaDataNode(const HyperStackIndex& index) const
    {
        auto findIt = metaDataNodes.find(index);
        if(findIt == metaDataNodes.end())
        {
            ETTENTION_THROW_EXCEPTION("There is no meta data for index " << index << "!");
        }
        return findIt->second;
    }

    bool ImageStackDirectoryDatasource::canHandleResource(AlgebraicParameterSet* paramSet)
    {
        auto location = paramSet->get<InputParameterSet>()->getProjectionsPath();

        if(!boost::filesystem::is_directory(location))
            return false;

        if(!boost::filesystem::exists(location / this->GetMetaDataFilename()))
            return false;

        return true;
    }

    const char* ImageStackDirectoryDatasource::getName() const
    {
        return "ImageStackDirectoryDatasource";
    }

    ettention::HyperStackIndex ImageStackDirectoryDatasource::firstIndex() const
    {
        return metaDataNodes.begin()->first;
    }

    ettention::HyperStackIndex ImageStackDirectoryDatasource::lastIndex() const
    {
        return metaDataNodes.rbegin()->first;
    }

    void ImageStackDirectoryDatasource::openResource(AlgebraicParameterSet* paramSet)
    {
        this->directory = paramSet->get<InputParameterSet>()->getProjectionsPath();
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

    unsigned int ImageStackDirectoryDatasource::getNumberOfProjections() const
    {
        return static_cast<unsigned int>(imageLocations.size());
    }

    void ImageStackDirectoryDatasource::writeMetaDataFile(std::string metaDataFileName, const std::vector<float>& tiltAngles, const std::vector<std::string>& filenames, const ScannerGeometry* baseScannerGeometry, const VolumeProperties& volumeProperties)
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

    void ImageStackDirectoryDatasource::writeMicroscopeMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const ScannerGeometry* baseScannerGeometry, const VolumeProperties& volumeProperties)
    {
        boost::property_tree::ptree& geometryNode = propertyTree.add_child("geometry", boost::property_tree::ptree());

        boost::property_tree::ptree& detectorNode = geometryNode.add_child("detectorPosition", boost::property_tree::ptree());
        detectorNode.add("<xmlattr>.value", baseScannerGeometry->getDetectorBase());

        boost::property_tree::ptree& sourceNode = geometryNode.add_child("sourcePosition", boost::property_tree::ptree());
        sourceNode.add("<xmlattr>.value", baseScannerGeometry->getSourceBase());

        boost::property_tree::ptree& hPitchNode = geometryNode.add_child("horizontalPitch", boost::property_tree::ptree());
        hPitchNode.add("<xmlattr>.value", baseScannerGeometry->getHorizontalPitch());

        boost::property_tree::ptree& vPitchNode = geometryNode.add_child("verticalPitch", boost::property_tree::ptree());
        vPitchNode.add("<xmlattr>.value", baseScannerGeometry->getVerticalPitch());

        boost::property_tree::ptree& volumeDimNode = geometryNode.add_child("volumeDimensions", boost::property_tree::ptree());
        volumeDimNode.add("<xmlattr>.value", volumeProperties.getVolumeResolution());
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
        std::vector<MetaDataNode> tempProjectionProperties;
        for(auto it = xmlTree.begin(); it != xmlTree.end(); ++it)
        {
            if(it->first == "projection")
            {
                std::string filename = it->second.get_child("image").get<std::string>("<xmlattr>.filename");
                tempImageLocations.push_back(ImageLocation(filename));
                auto props = this->ParseProjectionProperties(it->second);
                props.nodename = it->first;
                tempProjectionProperties.push_back(props);
            }
            if(it->first == "projectionstack")
            {
                auto props = this->ParseProjectionProperties(it->second);
                props.nodename = it->first;
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
                // Without the temporary variable, there seems to be a memory corruption on linux,
                // but I did not find the real cause
                unsigned int currentIndex = (unsigned int)tiltAngleToIndex.size();
                tiltAngleToIndex[tiltAngle] = currentIndex;
                tiltAngleProjectionCount[tiltAngle] = 0;
            }
            HyperStackIndex index(tiltAngleToIndex[tiltAngle], tiltAngleProjectionCount[tiltAngle]++);
            imageLocations[index] = tempImageLocations[i];
            metaDataNodes[index] = tempProjectionProperties[i];
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

        auto baseScannerGeometry = createScannerGeometry( xmlTree.get_child("geometry") );
        this->satRotator.setBaseScannerGeometry(baseScannerGeometry);
    }

    ScannerGeometry* ImageStackDirectoryDatasource::createScannerGeometry( boost::property_tree::ptree& geometryNode )
    {
        ParallelScannerGeometry* geometry = new ParallelScannerGeometry(this->resolution);
        geometry->set( geometryNode.get_child("sourcePosition").get<Vec3f>("<xmlattr>.value"),
                       geometryNode.get_child("detectorPosition").get<Vec3f>("<xmlattr>.value"),
                       geometryNode.get_child("horizontalPitch").get<Vec3f>("<xmlattr>.value"),
                       geometryNode.get_child("verticalPitch").get<Vec3f>("<xmlattr>.value"));
        return geometry;
    }

    ImageStackDirectoryDatasource::MetaDataNode ImageStackDirectoryDatasource::ParseProjectionProperties(const boost::property_tree::ptree& node) const
    {
        MetaDataNode props;
        props.onLogarithmicScale = node.get_child_optional("logscale").is_initialized();
        props.tiltAngle = node.get_child("tiltAngle").get<float>("<xmlattr>.value");
        props.xmlNode = node;
        return props;
    }

    std::string ImageStackDirectoryDatasource::GetMetaDataFilename() const
    {
        return "metadata.xml";
    }

    ScannerGeometry* ImageStackDirectoryDatasource::getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet)
    {
        auto findIt = metaDataNodes.find(index);
        if ( findIt == metaDataNodes.end() )
        {
            ETTENTION_THROW_EXCEPTION("There is no scanner geometry for index " << index << "!");
        }
        return satRotator.getRotatedScannerGeometry(findIt->second.tiltAngle, paramSet->get<InputParameterSet>()->getXAxisTilt());
    }

    float ImageStackDirectoryDatasource::GetTiltAngle(const HyperStackIndex& index) const
    {
        auto findIt = metaDataNodes.find(index);
        if ( findIt == metaDataNodes.end() )
        {
            ETTENTION_THROW_EXCEPTION("There is no tilt angle for index " << index << "!");
        }
        return findIt->second.tiltAngle;
    }

    bool ImageStackDirectoryDatasource::IsEvereyTiltAngleUnique() const
    {
        std::unordered_set<float> tiltAngleSet;
        for ( auto it = metaDataNodes.begin(); it != metaDataNodes.end(); ++it )
        {
            tiltAngleSet.insert(it->second.tiltAngle);
        }
        return tiltAngleSet.size() == metaDataNodes.size();
    }

    void ImageStackDirectoryDatasource::ConvertToOneDimension()
    {
        auto oldProjectionProperties = metaDataNodes;
        metaDataNodes.clear();
        auto oldImageLocations = imageLocations;
        imageLocations.clear();
        for ( auto it = oldProjectionProperties.begin(); it != oldProjectionProperties.end(); ++it )
        {
            HyperStackIndex oldIndex = it->first;
            HyperStackIndex newIndex(oldIndex[0]);
            metaDataNodes[newIndex] = oldProjectionProperties[oldIndex];
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