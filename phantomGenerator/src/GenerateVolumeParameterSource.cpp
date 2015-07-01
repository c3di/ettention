#include "stdafx.h"
#include "GenerateVolumeParameterSource.h"
  
namespace ettention
{
    GenerateVolumeParameterSource::GenerateVolumeParameterSource(const std::vector<std::string>& args)
        : CommandLineParameterSource(args)
    {
    }

    GenerateVolumeParameterSource::GenerateVolumeParameterSource(int argc, char* argv[])
        : CommandLineParameterSource(argc, argv)
    {
    }

    GenerateVolumeParameterSource::~GenerateVolumeParameterSource()
    {
    }

    void GenerateVolumeParameterSource::declareAcceptedParameters(boost::program_options::options_description& options)
    {
        options.add_options()
            ("help,h", "Output command line tool help to stdout")
            ("outputFile,o", boost::program_options::value<std::string>(), "Output filename for the volume")
            ("fileMode", boost::program_options::value<std::string>()->default_value("float32"), "File mode (for MRC Stacks only), can be either of [float32,unsigned8,signed16,unsigned16]")
            ("resolution,r", boost::program_options::value<unsigned int>()->default_value(64), "Volume resolution in voxel^3, i.e. specify -r 32 for a volume of dimensions 32 x 32 x 32")
            ("samples,s", boost::program_options::value<unsigned int>()->default_value(1), "Oversampling factor per dimension. specify -s 2 to use 2x2x2 = 8 samples per voxel")
            ("background,b", boost::program_options::value<float>()->default_value(0.0f), "background intensity (volume value before shapes are added")
            ("shape", boost::program_options::value<std::string>()->default_value("diagonal_3"), "Shape of the volume, can be either of [diagonal_3|shepp_logan|particles]")
            ("mask", "if to true, a mask (outer contour) will be generated")
            ("useIModFormat,i", "Use IMOD format for mrc file");
    }
}