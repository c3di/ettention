#pragma once
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include "setup/parametersource/ParameterSource.h"
#include "setup/ParameterStorage.h"

namespace ettention
{
    class CommandLineParameterSource : public ParameterSource
    {
    public:
        CommandLineParameterSource(const std::vector<std::string>& argv);
        CommandLineParameterSource(int argc, char* argv[]);
        virtual ~CommandLineParameterSource();

        virtual void parse() override;
        virtual bool parameterExists(std::string aName) const override;
        boost::filesystem::path getPathParameter(std::string aName) const override;
        virtual std::string getStringParameter(std::string aName) const override;
        virtual float getFloatParameter(std::string aName) const override;
        virtual bool getBoolParameter(std::string aName) const override;
        virtual int getIntParameter(std::string aName) const override;
        virtual unsigned int getUIntParameter(std::string aName) const override;
        virtual Vec3ui getVec3uiParameter(std::string aName) const override;
        virtual Vec3f getVec3fParameter(std::string aName) const override;
        virtual Vec2f getVec2fParameter(std::string aName) const override;

        boost::program_options::options_description& getDescription();

        virtual ParameterSource::LogLevelValues LogLevel() const override;
        virtual void LogLevel(const ParameterSource::LogLevelValues level) override;

    protected:
        virtual void declareAcceptedParameters(boost::program_options::options_description& options);

    protected:
        boost::program_options::options_description options;
        boost::program_options::variables_map vm;
        ParameterSource::LogLevelValues logLevel;
        std::vector<std::string> arguments;
    };
}
