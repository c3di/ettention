#include "stdafx.h"
#include "setup/CommandLineToolParameterSource.h"
#include "framework/Logger.h"

#define LOG_ABOVE_PARAMETER_DETAILS(logMessage)                             \
    if(LogLevel() >= ParameterSource::LogLevelValues::PARAMETER_DETAILS)   \
    {                                                                       \
        LOGGER(logMessage);                                                 \
    }                                                                       \

namespace ettention
{

    CommandLineToolParameterSource::CommandLineToolParameterSource(const std::vector<std::string>& argv)
        : CommandLineParameterSource(argv)
        , logLevel(ParameterSource::OVERALL_PROGRESS)
    {
    }

    CommandLineToolParameterSource::CommandLineToolParameterSource(int argc, char* argv[])
        : CommandLineParameterSource(argc, argv)
        , logLevel(ParameterSource::OVERALL_PROGRESS)
    {
    }

    CommandLineToolParameterSource::~CommandLineToolParameterSource()
    {
    }

    void CommandLineToolParameterSource::parse()
    {
        CommandLineParameterSource::parse();

        //add command line parameters to property tree
        //TODO - implement all the parameters
        if(vm.count("help")) {
            std::cout << options << "\n";
        }
        if(vm.count("Lambda"))
        {
            storage.insertOrRewriteScalarParameter("lambda", "float", vm["Lambda"].as<float>());
        }
        if(vm.count("OutputMode"))
        {
            storage.insertOrRewriteScalarParameter("mode", "int", vm["OutputMode"].as<int>());
        }
        if(vm.count("NumberOfIterations"))
        {
            storage.insertOrRewriteScalarParameter("numberOfIterations", "unsigned int", vm["NumberOfIterations"].as<unsigned int>());
        }
        if(vm.count("configfile"))
        {
            storage.insertOrRewriteScalarParameter("configfile", "string", vm["configfile"].as<std::string>());
        }
        if(vm.count("Algorithm"))
        {
            storage.insertOrRewriteScalarParameter("algorithm", "string", vm["Algorithm"].as<std::string>());
        }
        if(vm.count("ExplicitDeviceId"))
        {
            storage.insertOrRewriteScalarParameter("explicitDeviceId", "int", vm["ExplicitDeviceId"].as<int>());
        }
    }

    void CommandLineToolParameterSource::declareAcceptedParameters(boost::program_options::options_description& options)
    {
        options.add_options()
            ("help,h", "Show help message.")
            ("Algorithm,a", boost::program_options::value<std::string>()->default_value("sart"), "Reconstruction algorithm to use [sart|sirt|blockIterative]")
            ("Lambda,l", boost::program_options::value<float>(), "Relaxation parameter lambda. Float in range 0.0 - 2.0 (more noise in input data -> lower lambda).")
            ("NumberOfIterations", boost::program_options::value<unsigned int>(), "Number of iterations. Unsigned integer.")
            ("OutputMode", boost::program_options::value<int>(), "Mode of the output reconstruction. Integer: 1 ... .")
            ("configfile,c", boost::program_options::value<std::string>(), "Config file for reconstruction.")
            ("devices,d", "List OpenCL platform this machine offers")
            ("ExplicitDeviceId", boost::program_options::value<int>(), "Explicit OpenCL Device ID to use for reconstruction");
    }

    bool CommandLineToolParameterSource::parameterExists(std::string aName) const
    {
        return storage.parameterExists(aName);
    };

    std::string CommandLineToolParameterSource::getStringParameter(std::string aName) const
    {
        return storage.getScalarParameter<std::string>(aName);
    };

    float CommandLineToolParameterSource::getFloatParameter(std::string aName) const
    {
        return storage.getScalarParameter<float>(aName);
    }

    bool CommandLineToolParameterSource::getBoolParameter(std::string aName) const
    {
        return storage.getScalarParameter<bool>(aName);
    }

    int CommandLineToolParameterSource::getIntParameter(std::string aName) const
    {
        return storage.getScalarParameter<int>(aName);
    }

    unsigned int CommandLineToolParameterSource::getUIntParameter(std::string aName) const
    {
        return storage.getScalarParameter<unsigned int>(aName);
    }

    Vec3ui CommandLineToolParameterSource::getVec3uiParameter(std::string aName) const
    {
        return storage.getVec3Parameter<Vec3ui, unsigned int>(aName);
    }

    Vec3f CommandLineToolParameterSource::getVec3fParameter(std::string aName) const
    {
        return storage.getVec3Parameter<Vec3f, float>(aName);
    }

}