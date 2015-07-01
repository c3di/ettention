#pragma once
#include "setup/parametersource/ParameterSource.h"
#include "setup/ParameterStorage.h"
#include "setup/parametersource/CommandLineParameterSource.h"

namespace ettention
{
    class CommandLineToolParameterSource : public CommandLineParameterSource
    {
    public:
        CommandLineToolParameterSource(const std::vector<std::string>& argv);
        CommandLineToolParameterSource(int argc, char* argv[]);
        virtual ~CommandLineToolParameterSource();

        virtual bool parameterExists(std::string aName) const override;
        virtual std::string getStringParameter(std::string aName) const override;
        virtual float getFloatParameter(std::string aName) const override;
        virtual bool getBoolParameter(std::string aName) const override;
        virtual int getIntParameter(std::string aName) const override;
        virtual unsigned int getUIntParameter(std::string aName) const override;
        virtual Vec3ui getVec3uiParameter(std::string aName) const override;
        virtual Vec3f getVec3fParameter(std::string aName) const override;
        virtual void parse() override;

    protected:
        virtual void declareAcceptedParameters(boost::program_options::options_description& options);
        void makeBpGridDivisorOfSubvolumeDimensions(unsigned int& bpGrid, unsigned int& subVolumeDim);

        ParameterStorage storage;

        ParameterSource::LogLevelValues logLevel;
        std::string tilt_file;
    };
}
