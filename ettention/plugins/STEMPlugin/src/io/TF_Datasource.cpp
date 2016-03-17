#include "stdafx.h"
#include "io/TF_Datasource.h"
#include "io/deserializer/ImageDeserializer.h"  
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "model/STEMScannerGeometry.h"

using boost::property_tree::ptree;

namespace ettention
{
    namespace stem
    {
        TF_Datasource::TF_Datasource()
        {            
            maxCachedImages = 4096;
        }

        TF_Datasource::~TF_Datasource()
        {
            for (auto it : projectionProperties)
                delete( it.second );
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
                auto node = this->GetMetaDataNode(*it);
                if (node.nodename == "projection" )
                    projectionProperties[*it] = ParseProjectionProperties( node.xmlNode );
                else if (node.nodename == "projectionstack")
                {
                    auto allProjectionPropertiesOfThisStack = ParseProjectionStackProperties(node.xmlNode, 0);
                    for ( auto projectionProperty : allProjectionPropertiesOfThisStack )
                        projectionProperties[*it] = projectionProperty;
                }
            }
        }

        STEMScannerGeometry* TF_Datasource::ParseProjectionProperties( const boost::property_tree::ptree& node ) 
        {
            if(!node.get_child_optional("convergent").is_initialized())
                throw Exception("TF_Datasource can only read projection metadata of type TF_ProjectionMetaData");

            float tiltAngle = node.get_child("tiltAngle").get<float>("<xmlattr>.value");
            float focalDistance = node.get_child("focalDistance").get<float>("<xmlattr>.value");
            float focalDistanceBetweenImages = node.get_child("focalDistanceBetweenImages").get<float>("<xmlattr>.value");
            float beamOpeningAngle = node.get_child("beamOpeningAngle").get<float>("<xmlattr>.value");

            ScannerGeometry* rotatedGeometry = satRotator.getRotatedScannerGeometry( tiltAngle, 0.0f );
            STEMScannerGeometry* stemGeometry = dynamic_cast<STEMScannerGeometry*>( rotatedGeometry );
            stemGeometry->setTiltAngle(tiltAngle);

            return stemGeometry;
        }

        std::vector<STEMScannerGeometry*> TF_Datasource::ParseProjectionStackProperties(const boost::property_tree::ptree& node, unsigned int imageCount) 
        {
            if (!node.get_child_optional("convergent").is_initialized())
                throw Exception("TF_Datasource can only read projection metadata of type TF_ProjectionMetaData");

            std::string filename = node.get_child("image").get<std::string>("<xmlattr>.filename");
            float tiltAngle = node.get_child("tiltAngle").get<float>("<xmlattr>.value");
            float focalDistanceBase = node.get_child("focalDistanceBase").get<float>("<xmlattr>.value");
            float focusAtImage = node.get_child("focusAtImage").get<float>("<xmlattr>.value");
            float focalDistanceBetweenImages = node.get_child("focalDistanceBetweenImages").get<float>("<xmlattr>.value");
            float beamOpeningAngle = node.get_child("beamOpeningAngle").get<float>("<xmlattr>.value");

            boost::filesystem::path pathToImage( filename );            
            unsigned int imagesInStack = ImageDeserializer::getNumberOfImagesInStack( getAbsoluteImageLocation(pathToImage).string() );

            std::vector<STEMScannerGeometry*> result;

            for (unsigned int i = 0; i < imagesInStack; i++)
            {
                ScannerGeometry* rotatedGeometry = satRotator.getRotatedScannerGeometry(tiltAngle, 0.0f);
                STEMScannerGeometry* stemGeometry = dynamic_cast<STEMScannerGeometry*>(rotatedGeometry);
                stemGeometry->setFocalDifferenceBetweenImages(focalDistanceBetweenImages);
                stemGeometry->setConfocalOpeningHalfAngle(beamOpeningAngle);
                float focusPosition = focalDistanceBase + (focalDistanceBetweenImages * (float) (imagesInStack-i) );
                stemGeometry->setFocalDepth(focusPosition);
                stemGeometry->setTiltAngle(tiltAngle);
                result.push_back(stemGeometry);
            }

            return result;
        }

        void TF_Datasource::writeMetaDataFile(std::string metaDataFileName, std::vector<ScannerGeometry*> projectionProperties, const std::vector<std::string>& filenames, ScannerGeometry* baseScannerGeometry, const VolumeProperties& volumeProperties)
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

        void TF_Datasource::writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, std::vector<ScannerGeometry*> projectionProperties, const std::vector<std::string>& filenames)
        {
            for(unsigned int i = 0; i < projectionProperties.size(); i++)
            {
                STEMScannerGeometry* geometry = dynamic_cast<STEMScannerGeometry*>(projectionProperties[i]);
                if (geometry == nullptr)
                    throw Exception("TF_Datasource requires STEMScannerGeometry data");

                ptree& projectionNode = propertyTree.add_child("projection", ptree());
                ptree& imageNode = projectionNode.add_child("image", ptree());
                imageNode.add("<xmlattr>.filename", filenames[i]);

                ptree& tiltNode = projectionNode.add_child("tiltAngle", ptree());
                tiltNode.add("<xmlattr>.value", geometry->getTiltAngle());

                projectionNode.add_child("convergent", ptree());

                ptree& openingNode = projectionNode.add_child("beamOpeningAngle", ptree());
                openingNode.add("<xmlattr>.value", geometry->getConfocalOpeningHalfAngle());

                ptree& focalNode = projectionNode.add_child("focalDistance", ptree());
                focalNode.add("<xmlattr>.value", geometry->getFocalDepth());

                ptree& focalDifferenceNode = projectionNode.add_child("focalDistanceBetweenImages", ptree());
                focalDifferenceNode.add("<xmlattr>.value", geometry->getFocalDifferenceBetweenImages());
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

        STEMScannerGeometry* TF_Datasource::GetScannerGeometry(const HyperStackIndex& index) const
        {
            auto findIt = projectionProperties.find(index);
            if(findIt == projectionProperties.end())
            {
                ETTENTION_THROW_EXCEPTION("There are no projection properties for index " << index << "!");
            }
            return findIt->second;
        }

        STEMScannerGeometry* TF_Datasource::GetBaseScannerGeometry() const
        {
            return (STEMScannerGeometry*) satRotator.getBaseScannerGeometry();
        }

        ScannerGeometry* TF_Datasource::createScannerGeometry( boost::property_tree::ptree& geometryNode )
        {
            STEMScannerGeometry* geometry = new STEMScannerGeometry(this->resolution);
            geometry->set(geometryNode.get_child("sourcePosition").get<Vec3f>("<xmlattr>.value"),
                geometryNode.get_child("detectorPosition").get<Vec3f>("<xmlattr>.value"),
                geometryNode.get_child("horizontalPitch").get<Vec3f>("<xmlattr>.value"),
                geometryNode.get_child("verticalPitch").get<Vec3f>("<xmlattr>.value"));
            return geometry;
        }
    
    }

}

