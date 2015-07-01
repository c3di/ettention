#pragma once
#include "setup/parametersource/CommandLineParameterSource.h"

namespace ettention
{
    class GenerateProjectionsParameterSource : public CommandLineParameterSource
    {
    public:
        GenerateProjectionsParameterSource(const std::vector<std::string>& args);
        GenerateProjectionsParameterSource(int argc, char* argv[]);
        virtual ~GenerateProjectionsParameterSource();

    protected:
        virtual void declareAcceptedParameters(boost::program_options::options_description& options);

        std::string inputFileName;
    };
}