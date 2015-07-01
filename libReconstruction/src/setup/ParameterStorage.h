#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <string>

namespace ettention
{
    class ParameterStorage
    {
    public:
        ParameterStorage();
        ~ParameterStorage();

        template <typename parameterType>
        void insertOrRewriteScalarParameter(std::string name, std::string parameterTypeString, parameterType value)
        {
            //if the node exists rewrite the value in it (order of importance: command line parameters / tilt.com / internal_config)
            if(propertyTree.find(name.c_str()) != propertyTree.not_found())
            {
                propertyTree.get_child(name.c_str()).erase("<xmlattr>.value");
                propertyTree.get_child(name.c_str()).put("<xmlattr>.value", value);
                return;
            }

            boost::property_tree::ptree &node = propertyTree.put(name.c_str(), "");
            node.put("<xmlattr>.type", parameterTypeString);
            node.put("<xmlattr>.value", value);
        }

        template<typename parameterType>
        void insertOrRewriteVec3Parameter(std::string name, std::string parameterTypeString, parameterType valueX, parameterType valueY, parameterType valueZ)
        {
            //if the node exists rewrite the value in it (order of importance: command line parameters / tilt.com / internal_config)
            if(propertyTree.find(name.c_str()) != propertyTree.not_found())
            {
                boost::property_tree::ptree &node = propertyTree.get_child(name.c_str());
                node.erase("<xmlattr>.x");
                node.erase("<xmlattr>.y");
                node.erase("<xmlattr>.z");
                node.put("<xmlattr>.x", valueX);
                node.put("<xmlattr>.y", valueY);
                node.put("<xmlattr>.z", valueZ);
                return;
            }

            boost::property_tree::ptree &node = propertyTree.put(name.c_str(), "");
            node.put("<xmlattr>.type", parameterTypeString.c_str());
            node.put("<xmlattr>.x", valueX);
            node.put("<xmlattr>.y", valueY);
            node.put("<xmlattr>.z", valueZ);
        }

        template<typename parameterType, typename componentType>
        parameterType getVec3Parameter(std::string name) const
        {

            std::string vectorTypeString = propertyTree.get_child(name).get<std::string>("<xmlattr>.type");
            if(vectorTypeString != "Vec3f" && vectorTypeString != "Vec3ui")
            {
                throw std::runtime_error("Unsupported parameter of vector 3 type. Only Vec3f and Vec3ui are supported");
            }

            parameterType resultVector;
            resultVector.x = propertyTree.get_child(name).get<componentType>("<xmlattr>.x");
            resultVector.y = propertyTree.get_child(name).get<componentType>("<xmlattr>.y");
            resultVector.z = propertyTree.get_child(name).get<componentType>("<xmlattr>.z");

            return resultVector;
        }

        template<typename parameterType>
        parameterType getScalarParameter(std::string name) const
        {
            return propertyTree.get_child(name).get<parameterType>("<xmlattr>.value");
        }

        bool parameterExists(std::string name) const;

    private:
        boost::property_tree::ptree propertyTree;

    };
}
