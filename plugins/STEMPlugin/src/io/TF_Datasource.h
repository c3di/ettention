#pragma once
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include "framework/Logger.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/ImageStackDatasource.h"
#include "model/TFProjectionProperties.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API TF_Datasource : public ImageStackDirectoryDatasource
        {
        private:
            std::unordered_map<HyperStackIndex, TFProjectionProperties, HyperStackIndexHash> projectionProperties;

            TFProjectionProperties ParseProjectionProperties(const boost::property_tree::ptree& node) const;
            std::vector<TFProjectionProperties> ParseProjectionStackProperties(const boost::property_tree::ptree& node, unsigned int imageCount) const;

        public:
            TF_Datasource();
            ~TF_Datasource();

            ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) override;
            static void writeMetaDataFile(std::string metaDataFileName, const std::vector<TFProjectionProperties>& projectionProperties, const std::vector<std::string>& filenames, const ScannerGeometry& baseScannerGeometry, const VolumeProperties& volumeProperties);

            void writeToLog() override;

            std::string GetMetaDataFilename() const override;
            TFProjectionProperties GetProjectionProperties(const HyperStackIndex& index) const;
            void openResource(AlgebraicParameterSet* paramSet) override;

        protected:
            static void writeProjectionMetaData(boost::property_tree::ptree& propertyTree, std::string metaDataFileName, const std::vector<TFProjectionProperties>& stack, const std::vector<std::string>& filenames);
        };

    }
}

