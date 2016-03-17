#include "stdafx.h"
#include "CommandLineToolParameterSource.h"
#include "framework/Logger.h"
#include "gpu/opencl/CLContextCreator.h"

#define LOG_ABOVE_PARAMETER_DETAILS(logMessage)                             \
    if(getLogLevel() >= ParameterSource::LogLevelValues::PARAMETER_DETAILS)   \
    {                                                                       \
        LOGGER(logMessage);                                                 \
    }                                                                       \

namespace ettention
{

    CommandLineToolParameterSource::CommandLineToolParameterSource(const std::vector<std::string>& argv)
        : CommandLineParameterSource(argv)
        , logLevel(ParameterSource::LogLevel::OVERALL_PROGRESS)
    {
        setAcceptedParameters();
    }

    CommandLineToolParameterSource::CommandLineToolParameterSource(int argc, char* argv[])
        : CommandLineParameterSource(argc, argv)
        , logLevel(ParameterSource::LogLevel::OVERALL_PROGRESS)
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

        parseConsoleParameters();
        parseLibReconstructionParameters();
    }

    void CommandLineToolParameterSource::parseConsoleParameters()
    {
        if( vm.count("config") )
        {
            currentState = CLI_APP_STATE::RECONSTRUCT;
            storage.insertOrRewriteScalarParameter("config", "string", vm["config"].as<std::string>());
        }
        if( vm.count("explicitDeviceId") )
        {
            storage.insertOrRewriteScalarParameter("hardware.openclDeviceId", "int", vm["explicitDeviceId"].as<int>());
        }
        if( vm.count("devices") ) {                 currentState = CLI_APP_STATE::DEVICES; }
        if( vm.count("devicesAndExtensions") ) {    currentState = CLI_APP_STATE::DEVICESANDEXTENSIONS; }
        if( vm.count("help") ) {                    currentState = CLI_APP_STATE::HELP; }
        if( vm.count("parameters") ) {              currentState = CLI_APP_STATE::HELPANDPARAMETERS; }
    }

    void CommandLineToolParameterSource::parseLibReconstructionParameters()
    {
        // Options Group Hardware
        parseStringParameter("hardware.openclVendor");
        parseStringParameter("hardware.openclDeviceType");
        parseIntParameter("hardware.openclDeviceId");
        parseBoolParameter("hardware.disableImageSupport");
        parseUIntParameter("hardware.subVolumeCount");
    }

    void CommandLineToolParameterSource::setAcceptedParameters()
    {
        consoleOptions.add_options()
            ("config,c", boost::program_options::value<std::string>(), "Config filename for reconstruction.")
            ("explicitDeviceId,e", boost::program_options::value<int>(), "Use OpenCL device by id [same as hardware.openclDeviceId]")
            ("devices,d", "List OpenCL platform this machine offers")
            ("devicesAndExtensions,D", "Same as above plus OpenCL extensions")
            ("help,h", "Show this message.")
            ("parameters,H", "Show full LibReconstruction parameters list.")
            ;

        boost::program_options::options_description inputGroup("Input");
        inputGroup.add_options()
            ("input.projections", boost::program_options::value<std::string>(), "Required. Absolute path to the input stack containing the measured projection images.")
            ("input.tiltAngles", boost::program_options::value<std::string>(), "Required. This parameter specifies the path to an separate tilt angles file (.tlt). The tilt file must be an ASCII file with one line per projection that contains the tilt angle in degrees. Tilt files can only be used with for single tilt geometry. The specification of a 'tiltAngles' file is required if an MRC stack is used as input projections.")
            ("input.xAxisTilt ", boost::program_options::value<float>()->default_value(0.0), "This parameter specifies an angle between the tilt axis and the y-axis, i.e. it allows to tilt the geometry by a fixed angle around the x-axis.")
            ;

        boost::program_options::options_description outputGroup("Output");
        outputGroup.add_options()
            ("output.filename", boost::program_options::value<std::string>(), "Required. Path to the output volume that will be written after the completion of the reconstruction. The filename must be in an already existing directory.")
            ("output.format", boost::program_options::value<std::string>()->default_value("mrc"), "This parameter specifies in which file format the output volume is written. For now only 'mrc' (MRC stack) is allowed. If the parameter is not specified, the system uses the extension of the filename in the output. Additional file formats can be provided via plugins.")
            ("output.options.voxelType", boost::program_options::value<std::string>()->default_value("float32"), "Encoding of an individual voxel in the output stack. For the floating point voxel type float32, values are stored 'as they are'. Legal values are unsigned8|unsigned16|float32, default: float32.")
            ("output.options.orientation", boost::program_options::value<std::string>()->default_value("xzy_order"), "Specifies, in which orientation the output volume is written to disc, i.e. what coordinate layout order is used. Allowed: xzy_order|xyz_order")
            ("output.options.invert", boost::program_options::value<bool>()->default_value(false), "If set to true, the output volume will be inverted in the sense, that the absolute range of the values are preserved by the role high- and low contrast are exchanged, i.e. bright voxels become dark and vice versa.")
            ;

        boost::program_options::options_description reconstructionVolumeGroup("Reconstruction Volume");
        reconstructionVolumeGroup.add_options()
            ("volume.dimension", boost::program_options::value<std::string>(), "Required. Size of the reconstruction volume in world space units. Together with volume.origin this can be used to specify the bounding box of the reconstruction, i.e. perform an area of interest reconstruction. Format is Vec3f, ie. 0/0/0.")
            ("volume.origin", boost::program_options::value<std::string>()->default_value("0/0/0"), "Specifies the origin of the reconstruction volume in world space units. Together with volume.dimensions this can be used to specify the bounding box of the reconstruction, i.e. perform an area of interest reconstruction. Format is Vec3f.")
            ("volume.resolution", boost::program_options::value<std::string>(), "Specifies the resolution of the reconstruction volume. This always corresponds to the resolution of the output volume. The voxel size is given implicitly by dividing volume.dimension by volume.resolution. If the parameter is left unspecified, the system assumes a voxel size of 1/1/1 and computes the resolution from volume.dimensions. Format is Vec3ui.")
            ("volume.initValue", boost::program_options::value<float>()->default_value(0.0f), "This parameter specifies an initial, constant value for the reconstruction volume. The parameter is exclusive with 'volume.initFile', i.e. one can either specify an 'initValue' or an 'initFile', not both.")
            ("volume.initFile", boost::program_options::value<std::string>(), "This parameter specified an volume file to load as an initialization of the reconstruction process. The parameter is exclusive with 'volume.initValue', i.e. one can either specify an 'initValue' or an 'initFile', not both.")
            ;

        boost::program_options::options_description algorithmGroup("Algorithm");
        algorithmGroup.add_options()
            ("algorithm", boost::program_options::value<std::string>(), "Required. Specifies the identifier of the used algorithm. Legal values are 'sart', 'sirt' and 'blockIterative'. Additional legal values can be specified via plugins, i.e. plugins can add additional reconstruction algorithms, in which case the corresponding identifiers are also valid.")
            ("algorithm.numerOfIterations", boost::program_options::value<unsigned int>(), "Required. Specifies how often the algorithm should iterate. One iteration hereby corresponds to processing all input projections once. Typical values are 1-5 for SART and 5-15 for SIRT.")
            ("algorithm.lambda", boost::program_options::value<float>(), "Required. The relaxation parameter for the reconstruction. Legal values are in the interval -2.0 to 2.0, exclusively.")
            ("algorithm.useLongObjectCompensation", boost::program_options::value<bool>()->default_value(false), "If set to true, the system will perform long-object compensation in the sense of (Xu et. al. 2010).")
            ("algorithm.basisFunctions", boost::program_options::value<std::string>()->default_value("voxels"), "Alternative to 'voxels', the 'blobs' basis function could be used.")
            ;

        boost::program_options::options_description optimizationGroup("Optimization");
        optimizationGroup.add_options()
            ("optimization.internalVoxelRepresentationType", boost::program_options::value<std::string>()->default_value("float"), "Specifies data type for intermediate volume that goes to the GPU. Tradeoff between performance/quality.")
            ("optimization.projectionIteration", boost::program_options::value<std::string>()->default_value("random"), "Internal parameter, specifying order of projection traversal. Legal values are: random|identity|maxangle|buckets.")
            ;

        boost::program_options::options_description forwardProjectionGroup("Forward Projection");
        forwardProjectionGroup.add_options()
            ("forwardProjection.samples", boost::program_options::value<unsigned int>()->default_value(1), "Specifies the number of samples used in the forward projection. The default value '1' means that for each pixel, a single ray is used. Legal values are natural numbers that have integer square roots, i.e. 4, 9, 16 and so on.")
            ;

        boost::program_options::options_description backwardProjectionGroup("Back Projection");
        backwardProjectionGroup.add_options()
            ("backProjection.samples", boost::program_options::value<unsigned int>()->default_value(1), "Specifies the number of samples used in the back projection. The default value '1' means that for each voxel, the center point of the voxel is used as a representative sample. Legal values are natural numbers that have integer square roots, i.e. 4, 9, 16 and so on.")
            ;

        boost::program_options::options_description priorKnowledgeGroup("Prior Knowledge");
        priorKnowledgeGroup.add_options()
            ("priorknowledge.maskVolumeFilename", boost::program_options::value<std::string>(), "Specifies a path to the mask volume, which restricts space of processed voxels. Voxels of object that should not be taken into account have value of 0 in mask volume. All other voxels contribute to the final value proportional to their value. Mask volume voxels internally are represented by byte-long (8 bit) data type and assumed to have values from 0 to 255. If your mask is stored in float volume on disk (32 bit) denormalization from [0.0 – 1.0] to [0.0 – 255.0] range is required.")
            ("priorknowledge.volumeMinimumIntensity", boost::program_options::value<float>(), "Specifies a previously known minimal intensity of the tomogram gray values. A typical value is zero, which specifies that tomogram gray values must be positive.")
            ("priorknowledge.volumeMaximumIntensity", boost::program_options::value<float>(), "Specifies a previously known maximum intensity of the tomogram gray values.")
            ;

        boost::program_options::options_description hardwareGroup("Hardware");
        hardwareGroup.add_options()
            ("hardware.openclVendor", boost::program_options::value<std::string>()->default_value("any"), "Restrics OpenCL devices to a certain vendor. The default value is 'any', other legal values are 'intel', 'nvidia' and 'amd'.")
            ("hardware.openclDeviceType", boost::program_options::value<std::string>()->default_value("gpu_cpu"), "Specifies, which kind of OpenCL devices should be used for the reconstruction and in which order. The default value is 'gpu_cpu', other legal values are 'cpu_gpu', 'gpu' and 'cpu'.")
            ("hardware.openclDeviceId", boost::program_options::value<int>(), "If this parameter is set, an OpenCL device specified by id will be used explicitly. The parameter should not be used together with openclVendor and openclDeviceType. In other words: you can either give hints using specify openclVendor and openclDeviceType and let the system decide which device to use, or explicitly specify the device using openclDeviceId, not both.")
            ("hardware.disableImageSupport", boost::program_options::value<bool>()->default_value(false), "By default, the system detects if an OpenCL device has support for 3D textures (images) and uses this type of memory for the forward projections. On platforms without support for 3D textures, the behavior is emulated. By setting the parameter to true, the system will use the emulation on any platform, disregarding native texture support.")
            ("hardware.subVolumeCount", boost::program_options::value<unsigned int>(), "By default, the system automatically determines how much memory is available on the OpenCL device. If the available memory is insufficient to store the entire volume (plus all additional buffers), a number of subvolumes is used, i.e. the system incrementally transfers parts of the volume for processing. By setting the parameter, the automatic choice for the number of subvolumes can be set manually. However, setting a number lower than the system suggestion is likely to result in an out-of-memory error.")
            ;

        configOptions.add(inputGroup).add(outputGroup).add(reconstructionVolumeGroup).add(algorithmGroup).add(optimizationGroup).add(forwardProjectionGroup).add(backwardProjectionGroup).add(priorKnowledgeGroup).add(hardwareGroup); //.add(debug);

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
        // TODO change declareAcceptedParameters here or everywhere else ? Needs refactoring
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

    void CommandLineToolParameterSource::parseIntParameter(std::string paramName)
    {
        if( vm.count(paramName) )
        {
            storage.insertOrRewriteScalarParameter(paramName, "int", vm[paramName].as<int>());
        }
    }

    void CommandLineToolParameterSource::parseUIntParameter(std::string paramName)
    {
        if( vm.count(paramName) )
        {
            storage.insertOrRewriteScalarParameter(paramName, "unsigned int", vm[paramName].as<unsigned int>());
        }
    }

    void CommandLineToolParameterSource::parseBoolParameter(std::string paramName)
    {
        if( vm.count(paramName) )
        {
            storage.insertOrRewriteScalarParameter(paramName, "bool", vm[paramName].as<bool>());
        }
    }

    void CommandLineToolParameterSource::parseStringParameter(std::string paramName)
    {
        if( vm.count(paramName) )
        {
            storage.insertOrRewriteScalarParameter(paramName, "string", vm[paramName].as<std::string>());
        }
    }

}