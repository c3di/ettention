#include "stdafx.h"
#include "GenerateProjectionsParameterSource.h"
  
namespace ettention
{
    GenerateProjectionsParameterSource::GenerateProjectionsParameterSource(const std::vector<std::string>& args)
        : CommandLineParameterSource(args)
    {
    }

    GenerateProjectionsParameterSource::GenerateProjectionsParameterSource(int argc, char* argv[])
        : CommandLineParameterSource(argc, argv)
    {
    }

    GenerateProjectionsParameterSource::~GenerateProjectionsParameterSource()
    {
    }

    void GenerateProjectionsParameterSource::declareAcceptedParameters(boost::program_options::options_description& options)
    {
        options.add_options()
            ("help,h", "Output command line tool help to stdout")
            ("inputFile,i", boost::program_options::value<std::string>(), "Input file in mrc format")
            ("outputDirectory,o", boost::program_options::value<std::string>(), "Output directory for projections")
            ("halfAngle,a", boost::program_options::value<float>()->default_value(0.041f), "Beam opening half-angle in radians")
            ("resolution,r", boost::program_options::value<unsigned int>()->default_value(256), "Resolution of projections in pixel squared (i.e. specify -r 128 for resolution of 128x128)")
            ("samples,s", boost::program_options::value<unsigned int>()->default_value(128), "samples per Beam")
            ("focalPositions,f", boost::program_options::value<unsigned int>()->default_value(1), "focal positions per tilt position")
            ("pattern,p", boost::program_options::value<std::string>()->default_value("stack"), "sampling pattern: either of [stack|spiral]")
            ("tiltAngleDistance,t", boost::program_options::value<float>()->default_value(1.0f), "angular distance between tilt positions")
            ("minTiltAngle", boost::program_options::value<float>()->default_value(-90.0f), "minimal tilt angle")
            ("maxTiltAngle", boost::program_options::value<float>()->default_value(90.0f), "maximal tilt angle")
            ("metaDataOnly", boost::program_options::value<bool>()->default_value(false), "generate only metadata");
    }
}