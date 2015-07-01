#pragma once
#include "setup/parametersource/CommandLineParameterSource.h"

namespace ettention
{
    class GenerateVolumeParameterSource : public CommandLineParameterSource
    {
    public:
        GenerateVolumeParameterSource(const std::vector<std::string>& args);
        GenerateVolumeParameterSource(int argc, char* argv[]);
        virtual ~GenerateVolumeParameterSource();

    protected:
        virtual void declareAcceptedParameters(boost::program_options::options_description& options);
    };
}