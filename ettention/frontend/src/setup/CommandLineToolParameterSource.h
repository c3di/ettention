#pragma once
#include "setup/parametersource/ParameterSource.h"
#include "setup/ParameterStorage.h"
#include "setup/parametersource/CommandLineParameterSource.h"

namespace ettention
{
    enum class CLI_APP_STATE {
        DEFAULT = 0, RECONSTRUCT, DEVICES, DEVICESANDEXTENSIONS, HELP, HELPANDPARAMETERS
    };

    class CommandLineToolParameterSource : public CommandLineParameterSource
    {
    public:
        CommandLineToolParameterSource(const std::vector<std::string>& argv);
        CommandLineToolParameterSource(int argc, char* argv[]);
        virtual ~CommandLineToolParameterSource();

        virtual void parse() override;

        virtual bool parameterExists(std::string aName) const override;
        virtual std::string getStringParameter(std::string aName) const override;
        virtual float getFloatParameter(std::string aName) const override;
        virtual bool getBoolParameter(std::string aName) const override;
        virtual int getIntParameter(std::string aName) const override;
        virtual unsigned int getUIntParameter(std::string aName) const override;
        virtual Vec3ui getVec3uiParameter(std::string aName) const override;
        virtual Vec3f getVec3fParameter(std::string aName) const override;

        boost::program_options::options_description& getConsoleParametersShortList();
        boost::program_options::options_description& getConsoleParametersFullList();
        CLI_APP_STATE getCurrentState() const;

    protected:
        virtual void declareAcceptedParameters(boost::program_options::options_description& options) override;
        void parseLibReconstructionParameters();

        void parseIntParameter(std::string paramName);
        void parseUIntParameter(std::string paramName);
        void parseBoolParameter(std::string paramName);
        void parseStringParameter(std::string paramName);

        void parseConsoleParameters();
        void setAcceptedParameters();

        ParameterStorage storage;
        ParameterSource::LogLevel logLevel;
        std::string tilt_file;

    private:
        boost::program_options::options_description consoleOptions{"Ettention command line parameters"};
        boost::program_options::options_description configOptions{"LibReconstruction parameters"};
        boost::program_options::options_description publicOptions;
        CLI_APP_STATE currentState = CLI_APP_STATE::DEFAULT;
    };
}
