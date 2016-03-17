#include "stdafx.h"
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "XMLParameterSource.h"
#include "framework/error/Exception.h"
#include "framework/error/InputFormatException.h"
#include "framework/FileAlgorithms.h"
#include "framework/Logger.h"
#include "framework/VectorAlgorithms.h"

namespace ettention
{

    XMLParameterSource::XMLParameterSource(const std::string filename)
        : xmlFilename(filename)
    {
        parse();
    }

    XMLParameterSource::~XMLParameterSource()
    {
    }

    boost::filesystem::path XMLParameterSource::getPathParameter(std::string aName) const
    {
        std::string xmlValue = getStringParameter(aName);
        boost::replace_all(xmlValue, "\\", "/");
        boost::filesystem::path path = boost::filesystem::path(xmlValue);
        if(path.is_relative())
        {
            path = extendRelativeToXMLPath(path);
        }
        return path;
    }

    bool XMLParameterSource::parameterExists(std::string propertyPath) const
    {
        ptree node = propertyTree;
        auto propertyPathSteps = VectorAlgorithms::splitString(propertyPath, "/");

        for(size_t i = 0; i < propertyPathSteps.size()-1; ++i)
        {
            boost::optional<ptree&> potentialNode = node.get_child_optional(propertyPathSteps[i]);
            if(!potentialNode)
            {
                return false;
            }
            node = potentialNode.get();
        }
        return node.get_optional<std::string>(propertyPathSteps[propertyPathSteps.size()-1]) != boost::none;
    };

    std::string XMLParameterSource::getStringParameter(std::string parameterName)  const
    {
        try {
            return retrieveNode(parameterName).get<std::string>("<xmlattr>.value");
        }
        catch ( std::exception const& )
        {
            throw InputFormatException( parameterName );
        }
    };

    float XMLParameterSource::getFloatParameter(std::string parameterName) const
    {
        return retrieveNode(parameterName).get<float>("<xmlattr>.value");
    }

    bool XMLParameterSource::getBoolParameter(std::string parameterName) const
    {
        return retrieveNode(parameterName).get<bool>("<xmlattr>.value");
    }

    int XMLParameterSource::getIntParameter(std::string parameterName) const
    {
        return retrieveNode(parameterName).get<int>("<xmlattr>.value");
    }

    unsigned int XMLParameterSource::getUIntParameter(std::string parameterName) const
    {
        return retrieveNode(parameterName).get<unsigned int>("<xmlattr>.value");
    }

    Vec3ui XMLParameterSource::getVec3uiParameter(std::string parameterName) const
    {
        return retrieveNode(parameterName).get<Vec3ui>("<xmlattr>.value");
    }

    Vec3f XMLParameterSource::getVec3fParameter(std::string parameterName) const
    {
        return retrieveNode(parameterName).get<Vec3f>("<xmlattr>.value");
    }

    void XMLParameterSource::parse()
    {
        read_xml(xmlFilename, propertyTree);
    }

    const ptree& XMLParameterSource::retrieveNode(std::string nodeName) const
    {
        try 
        {
            return propertyTree.get_child(nodeName);
        }
        catch (boost::property_tree::ptree_bad_path e)
        {
            throw Exception("xml config file entry does not exist: " + nodeName);
        }
    }

    ParameterSource::LogLevel XMLParameterSource::getLogLevel() const 
    {
        return ParameterSource::LogLevel::OVERALL_PROGRESS;
    }

    void XMLParameterSource::setLogLevel(const ParameterSource::LogLevel level)
    {
        //
    }

    boost::filesystem::path XMLParameterSource::extendRelativeToXMLPath(boost::filesystem::path path) const
    {
        boost::filesystem::path pathToXMLFile = boost::filesystem::path(xmlFilename).parent_path();
        boost::filesystem::path totalPath = pathToXMLFile / path;
        return FileAlgorithms::normalizePath(totalPath);
    }

}