#include "stdafx.h"
#include "CommandLineToolParameterSource.h"
#include "framework/Logger.h"
#include "gpu/opencl/CLContextCreator.h"

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
        setAcceptedParameters();
    }

    CommandLineToolParameterSource::CommandLineToolParameterSource(int argc, char* argv[])
        : CommandLineParameterSource(argc, argv)
        , logLevel(ParameterSource::LogLevelValues::OVERALL_PROGRESS)
    {
        setAcceptedParameters();
    }

    CommandLineToolParameterSource::~CommandLineToolParameterSource()
    {
    }

    void CommandLineToolParameterSource::parse()
    {
        //CommandLineParameterSource::parse();
        auto parsed = boost::program_options::command_line_parser(arguments)
            .options(options)
            .style(boost::program_options::command_line_style::default_style | boost::program_options::command_line_style::allow_sticky)
            .allow_unregistered()
            .run();
        store(parsed, vm);
        notify(vm);

        if( vm.count("config") )
        {
            currentState = CLI_APP_STATE::RECONSTRUCT;
            storage.insertOrRewriteScalarParameter("config", "string", vm["config"].as<std::string>());
        }
        if( vm.count("explicitDeviceId") )
        {
            storage.insertOrRewriteScalarParameter("explicitDeviceId", "int", vm["explicitDeviceId"].as<int>());
        }
        if( vm.count("devices") )
        {
            currentState = CLI_APP_STATE::DEVICES;
        }
        if( vm.count("devicesAndExtensions") )
        {
            currentState = CLI_APP_STATE::DEVICESANDEXTENSIONS;
        }
        if( vm.count("help") )
        {
            currentState = CLI_APP_STATE::HELP;
        }
        if( vm.count("parameters") )
        {
            currentState = CLI_APP_STATE::PARAMETERS;
        }
        //if(vm.count("lambda"))
        //{
        //    storage.insertOrRewriteScalarParameter("lambda", "float", vm["lambda"].as<float>());
        //}
        //if(vm.count("outputMode"))
        //{
        //    storage.insertOrRewriteScalarParameter("mode", "int", vm["outputMode"].as<int>());
        //}
        //if(vm.count("numberOfIterations"))
        //{
        //    storage.insertOrRewriteScalarParameter("numberOfIterations", "unsigned int", vm["numberOfIterations"].as<unsigned int>());
        //}
        //if(vm.count("algorithm"))
        //{
        //    storage.insertOrRewriteScalarParameter("algorithm", "string", vm["algorithm"].as<std::string>());
        //}
    }

    void CommandLineToolParameterSource::setAcceptedParameters()
    {
        consoleOptions.add_options()
            ("config,c", boost::program_options::value<std::string>(), "Config filename for reconstruction.")
            ("explicitDeviceId,e", boost::program_options::value<int>(), "Explicit OpenCL Device ID to use for reconstruction")
            ("devices,d", "List OpenCL platform this machine offers")
            ("devicesAndExtensions,D", "Same as above plus OpenCL extensions")
            ("help,h", "Show this message.")
            ("parameters,H", "Show full LibReconstruction parameters list.")
            ;

        //configOptions.add_options()
            //("algorithm,a", boost::program_options::value<std::string>()->default_value("sart"), "Reconstruction algorithm to use [sart|sirt|blockIterative]")
            //("lambda,l", boost::program_options::value<float>(), "Relaxation parameter lambda. Float in range 0.0 - 2.0 (more noise in input data -> lower lambda).")
            //("numberOfIterations", boost::program_options::value<unsigned int>(), "Number of iterations. Unsigned integer.")
            //("outputMode", boost::program_options::value<int>(), "Mode of the output reconstruction. Integer: 1 ... .")
         //   ;

        boost::program_options::options_description inputGroup("Input");
        inputGroup.add_options()
            ("input.projections", boost::program_options::value<std::string>(), "Absolute path to the input stack containing the measured projection images.")
            ("input.tiltAngles", boost::program_options::value<std::string>(), "Absolute path to an separate tilt angles file. The specification of a tiltAngles file is required if an mrc stack is used as input projections.")
            ("input.tiltAngles", boost::program_options::value<float>()->default_value(0.0), "Angle between the tilt axis and the y-axis.")
            ;

        boost::program_options::options_description outputGroup("Output");
        outputGroup.add_options()
            ("output.filename", boost::program_options::value<std::string>(), "Path to the output volume that will be written after the completion of the reconstruction. The filename must be in an already existing directory.")
            ("output.format", boost::program_options::value<std::string>(), "Allowed 'mrc' (MRC stack) and 'rek' (Fraunhofer REK format). If the parameter is not specified, the system uses the extension of the filename in the output.")
            ("output.options.voxelType", boost::program_options::value<std::string>()->default_value("float32"), "encoding of an individual voxel in the output stack. For the floating point voxel type float32, values are stored 'as they are'.")
            ("output.options.orientation", boost::program_options::value<std::string>()->default_value("xzy_order"), "Specifies, in which orientation the output volume is written to disc, i.e. what coordinate layout order is used. Allowed: xzy_order|xyz_order")
            ("output.options.invert", boost::program_options::value<bool>()->default_value(false), "If set to true, the output volume will be inverted in the sense, that the absolute range of the values are preserved by the role high- and low contrast are exchanged, i.e. bright voxels become dark and vice versa.")
            ;

        boost::program_options::options_description reconstructionVolumeGroup("Reconstruction Volume");
        reconstructionVolumeGroup.add_options()
            ("volume.dimensions", boost::program_options::value<std::string>(), "Size of the reconstruction volume in world space units.")
            ;

        configOptions.add(inputGroup).add(outputGroup).add(reconstructionVolumeGroup);//.add(algorithmGroup).add(forwardProjectionGroup).add(backwardProjectionGroup).add(priorKnowledgeGroup).add(hardwareGroup).add(debug);

        publicOptions.add(consoleOptions).add(configOptions);
        declareAcceptedParameters(publicOptions);
    }

    boost::program_options::options_description& CommandLineToolParameterSource::getConsoleParametersShortList()
    {
        return consoleOptions;
    }

    boost::program_options::options_description& CommandLineToolParameterSource::getConsoleParametersFullList()
    {
        return publicOptions;
    }

    void CommandLineToolParameterSource::declareAcceptedParameters(boost::program_options::options_description& options)
    {
        // TODO change declareAcceptedParameters here or everywhere else ?
        this->options.add(options);
    }

    CLI_APP_STATE CommandLineToolParameterSource::getCurrentState() const
    {
        return currentState;
    };

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