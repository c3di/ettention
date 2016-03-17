#pragma once
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include "framework/Logger.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/ImageStackDatasource.h"

namespace ettention
{
    namespace stem
    {
		class STEMScannerGeometry;

        class PLUGIN_API TF_Datasource : public ImageStackDirectoryDatasource
        {
        private:
			std::unordered_map<HyperStackIndex, STEMScannerGeometry*, HyperStackIndexHash> projectionProperties;
			// SingleAxisTiltRotator geoCreator;

			STEMScannerGeometry* ParseProjectionProperties(const boost::property_tree::ptree& node);
			std::vector<STEMScannerGeometry*> ParseProjectionStackProperties(const boost::property_tree::ptree& node, unsigned int imageCount);

        public:
            TF_Datasource();
            ~TF_Datasource();

            ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) override;
			static void writeMetaDataFile(std::string metaDataFileName, std::vector<ScannerGeometry*> projectionProperties, const std::vector<std::string>& filenames, ScannerGeometry* baseScannerGeometry, const VolumeProperties& volumeProperties);

            void writeToLog() override;

            std::string GetMetaDataFilename() const override;
			STEMScannerGeometry* GetScannerGeometry(const HyperStackIndex& index) const;
			STEMScannerGeometry* GetBaseScannerGeometry() const;;
			void openResource(AlgebraicParameterSet* paramSet) override;

        protected:
			static void writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, std::vector<ScannerGeometry*> projectionProperties, const std::vector<std::string>& filenames);
			virtual ScannerGeometry* createScannerGeometry( boost::property_tree::ptree& geometryNode ) override;
        };

    }
}

