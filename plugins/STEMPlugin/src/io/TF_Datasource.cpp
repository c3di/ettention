#include "stdafx.h"
#include "io/TF_Datasource.h"
#include "io/deserializer/ImageDeserializer.h"  
#include "framework/error/ProjectionTypeNotSupportedException.h"

using boost::property_tree::ptree;

namespace ettention
{
    namespace stem
    {
        TF_Datasource::TF_Datasource()
        {
        }

        TF_Datasource::~TF_Datasource()
        {
        }

        ImageStackDatasource* TF_Datasource::instantiate(AlgebraicParameterSet* paramSet)
        {
            TF_Datasource* dataSource = new TF_Datasource();
            dataSource->openResource(paramSet);
            return dataSource;
        }

        void TF_Datasource::openResource(AlgebraicParameterSet* paramSet)
        {
            ImageStackDirectoryDatasource::openResource(paramSet);
            auto indices = this->CollectAllValidIndices();
            for(auto it = indices.begin(); it != indices.end(); ++it)
            {
                auto node = this->GetMetaData(*it);
                projectionProperties[*it] = ParseProjectionProperties(node);
            }
        }

        TFProjectionProperties TF_Datasource::ParseProjectionProperties(const boost::property_tree::ptree& node) const
        {
            if(!node.get_child_optional("convergent").is_initialized())
                throw Exception("TF_Datasource can only read projection metadata of type TF_ProjectionMetaData");
            float tiltAngle = node.get_child("tiltAngle").get<float>("<xmlattr>.value");
            float focalDistance = node.get_child("focalDistance").get<float>("<xmlattr>.value");
            float focalDistanceBetweenImages = node.get_child("focalDistanceBetweenImages").get<float>("<xmlattr>.value");
            float beamOpeningAngle = node.get_child("beamOpeningAngle").get<float>("<xmlattr>.value");
            return TFProjectionProperties(tiltAngle, focalDistance, focalDistanceBetweenImages, beamOpeningAngle);
        }

        std::vector<TFProjectionProperties> TF_Datasource::ParseProjectionStackProperties(const boost::property_tree::ptree& node, unsigned int imageCount) const
        {
            throw Exception("Not yet implemented!");
            std::vector<TFProjectionProperties> result(imageCount);
            TFProjectionProperties baseProperties = this->ParseProjectionProperties(node);
            float focalDistanceBase = node.get_child("focalDistanceBase").get<float>("<xmlattr>.value");
            unsigned int focusAtImage = node.get_child("focusAtImage").get<unsigned int>("<xmlattr>.value");
            for(unsigned int i = 0; i < imageCount; i++)
            {
                int numberOfImagesToFocus = (int)i - (int)focusAtImage;
                float focalDistance = focalDistanceBase + baseProperties.GetFocalDifferenceBetweenImages() * (float)numberOfImagesToFocus;
                result[i] = TFProjectionProperties(baseProperties.GetTiltAngle(),
                                                   focalDistance,
                                                   baseProperties.GetFocalDifferenceBetweenImages(),
                                                   baseProperties.GetConfocalOpeningHalfAngle());
            }
            return result;
        }

        void TF_Datasource::writeMetaDataFile(std::string metaDataFileName, const std::vector<TFProjectionProperties>& projectionProperties, const std::vector<std::string>& filenames, const ScannerGeometry& baseScannerGeometry, const VolumeProperties& volumeProperties)
        {
            if(projectionProperties.size() != filenames.size())
            {
                throw Exception("Number of filenames does not match number of properties!");
            }
            ptree propertyTree;

            writeMicroscopeMetaData(propertyTree, metaDataFileName, baseScannerGeometry, volumeProperties);
            writeProjectionMetaData(propertyTree, metaDataFileName, projectionProperties, filenames);

            boost::property_tree::xml_writer_settings<char> w(' ', 4);

            if(boost::filesystem::exists(metaDataFileName))
                boost::filesystem::remove(metaDataFileName);

            write_xml(metaDataFileName, propertyTree, std::locale(), w);
        }

        void TF_Datasource::writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const std::vector<TFProjectionProperties>& projectionProperties, const std::vector<std::string>& filenames)
        {
            for(unsigned int i = 0; i < projectionProperties.size(); i++)
            {
                ptree& projectionNode = propertyTree.add_child("projection", ptree());
                ptree& imageNode = projectionNode.add_child("image", ptree());
                imageNode.add("<xmlattr>.filename", filenames[i]);

                ptree& tiltNode = projectionNode.add_child("tiltAngle", ptree());
                tiltNode.add("<xmlattr>.value", projectionProperties[i].GetTiltAngle());

                projectionNode.add_child("convergent", ptree());

                ptree& openingNode = projectionNode.add_child("beamOpeningAngle", ptree());
                openingNode.add("<xmlattr>.value", projectionProperties[i].GetConfocalOpeningHalfAngle());

                ptree& focalNode = projectionNode.add_child("focalDistance", ptree());
                focalNode.add("<xmlattr>.value", projectionProperties[i].GetFocalDepth());

                ptree& focalDifferenceNode = projectionNode.add_child("focalDistanceBetweenImages", ptree());
                focalDifferenceNode.add("<xmlattr>.value", projectionProperties[i].GetFocalDifferenceBetweenImages());
            }
        }

        void TF_Datasource::writeToLog()
        {
            LOGGER("Datasource type: TF_Datasource");
        }

        std::string TF_Datasource::GetMetaDataFilename() const
        {
            return "stem_metadata.xml";
        }

        TFProjectionProperties TF_Datasource::GetProjectionProperties(const HyperStackIndex& index) const
        {
            auto findIt = projectionProperties.find(index);
            if(findIt == projectionProperties.end())
            {
                ETTENTION_THROW_EXCEPTION("There are no projection properties for index " << index << "!");
            }
            return findIt->second;
        }
    }
}

