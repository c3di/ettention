#pragma once
#include <boost/property_tree/ptree.hpp>
#include "setup/parametersource/ParameterSource.h"

using boost::property_tree::ptree;

namespace ettention
{
    class XMLParameterSource : public ParameterSource
    {
    public:
        XMLParameterSource(const std::string filename);
        virtual ~XMLParameterSource();

        virtual bool parameterExists(std::string nodeName) const override;
        virtual std::string getStringParameter(std::string nodeName) const override;
        virtual boost::filesystem::path getPathParameter(std::string nodeName) const override;
        virtual float getFloatParameter(std::string nodeName) const override;
        virtual bool getBoolParameter(std::string nodeName) const override;
        virtual int getIntParameter(std::string nodeName) const override;
        virtual unsigned int getUIntParameter(std::string nodeName) const override;
        virtual Vec3ui getVec3uiParameter(std::string nodeName) const override;
        virtual Vec3f getVec3fParameter(std::string nodeName) const override;

        virtual ParameterSource::LogLevel getLogLevel() const override;
        virtual void setLogLevel(const ParameterSource::LogLevel level) override;

        float getFloatParameter(std::string parameterName, std::string attributeName) const;

    protected:
        virtual void parse() override;
        const ptree& retrieveNode(std::string nodeName) const;
        boost::filesystem::path extendRelativeToXMLPath(boost::filesystem::path path) const;
        boost::filesystem::path normalize(const boost::filesystem::path &path);

        boost::property_tree::ptree propertyTree;
        std::string xmlFilename;
    };
}
