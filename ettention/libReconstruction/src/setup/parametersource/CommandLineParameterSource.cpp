#include "stdafx.h"
#include <boost/program_options.hpp>
#include "CommandLineParameterSource.h"
#include "framework/error/ParameterNotFoundException.h"

namespace ettention
{
    CommandLineParameterSource::CommandLineParameterSource(const std::vector<std::string>& argv)
        : arguments(argv)
        , options("program options")
    {
    }

    CommandLineParameterSource::CommandLineParameterSource(int argc, char* argv[])
        : options("program options")
    {
        for(int i = 0; i < argc; ++i)
        {
            arguments.push_back(argv[i]);
        }
    }

    CommandLineParameterSource::~CommandLineParameterSource()
    {
    }

    namespace options = boost::program_options;

    void CommandLineParameterSource::parse()
    {
		if (vm.size() > 0)
			return;
        declareAcceptedParameters(options);
        store(boost::program_options::command_line_parser(arguments).options(options).style(options::command_line_style::default_style | options::command_line_style::allow_sticky | options::command_line_style::allow_slash_for_short).run(), vm);
        notify(vm);
    }

    bool CommandLineParameterSource::parameterExists(std::string aName) const
    {
        return vm.count(aName) != 0;
    };

    boost::filesystem::path CommandLineParameterSource::getPathParameter(std::string aName) const
    {
        return boost::filesystem::path(getStringParameter(aName));
    };

    std::string CommandLineParameterSource::getStringParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<std::string>();
    };

    float CommandLineParameterSource::getFloatParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<float>();
    }

    bool CommandLineParameterSource::getBoolParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<bool>();
    }

    int CommandLineParameterSource::getIntParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<int>();
    }

    unsigned int CommandLineParameterSource::getUIntParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<unsigned int>();
    }

    Vec3ui CommandLineParameterSource::getVec3uiParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<Vec3ui>();
    }

    Vec3f CommandLineParameterSource::getVec3fParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return vm[aName].as<Vec3f>();
    }

    Vec2f CommandLineParameterSource::getVec2fParameter(std::string aName) const
    {
        if(!parameterExists(aName))
            throw ParameterNotFoundException(aName);
        return Vec2f(vm[aName].as<std::vector<float>>()[0], vm[aName].as<std::vector<float>>()[1]);
    }

    boost::program_options::options_description& CommandLineParameterSource::getDescription()
    {
        return options;
    }

    void CommandLineParameterSource::declareAcceptedParameters(boost::program_options::options_description& options)
    {
    }

}