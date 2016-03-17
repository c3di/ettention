#include "stdafx.h"
#include <algorithm>
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "framework/Logger.h"
#include "framework/plugins/PluginManager.h"
#include "framework/time/Timer.h"
#include "framework/error/ParameterNotFoundException.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/CommandLineToolParameterSource.h"

#if defined(WIN32) && defined(max)
    #undef max
#endif

using namespace ettention;

CommandLineToolParameterSource* commandLineParameterSource = nullptr;
XMLParameterSource* xmlParameterSource = nullptr;

void initializeLogger()
{
    Logger::getInstance().activateConsoleLog();
    Logger::getInstance().setLogFilename("ettention.log");
    Logger::getInstance().activateFileLog();
    LOGGER("Ettention, version 2016, (c) DFKI GmbH 2015-16");
    Logger::getInstance().forceLogFlush();
}

void addParameterSources(Framework* framework)
{
    auto sourcesFromPlugins = framework->getPluginManager()->instantiateParameterSource();
    for (auto it = sourcesFromPlugins.begin(); it != sourcesFromPlugins.end(); ++it)
    {
        framework->parseAndAddParameterSource(*it);
    }

    if( commandLineParameterSource->parameterExists("config") )
    {
        LOGGER("Loading configuration from XML file...");
        xmlParameterSource = new XMLParameterSource(commandLineParameterSource->getStringParameter("config"));
        framework->parseAndAddParameterSource(xmlParameterSource);
    }
    framework->getParameterSource()->addSource(commandLineParameterSource);
}

void doReconstruction(Framework* framework)
{
    LOGGER("Launching reconstruction algorithm...");
    ReconstructionAlgorithm* app = framework->instantiateReconstructionAlgorithm();
    app->run();
    framework->writeFinalVolume(app->getReconstructedVolume());
    delete app;
    LOGGER("Reconstruction finished.");
}

void printCLDeviceList(bool printExtensions)
{
    CLContextCreator creator;
    auto devices = creator.GatherDeviceInfos();
    std::cout << std::endl;
    std::cout << " OpenCL devices on this machine" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << " ID | Type | Detail" << std::endl;
    std::cout << "----|------|";
    std::stringstream table;
    unsigned int maxDetailLength = 0;
    for( unsigned int i = 0; i < devices.size(); ++i )
    {
        std::string detail = devices[i].name + ", " + devices[i].version;
        table << " " << std::setfill(' ') << std::setw(2) << i << " | " << devices[i].type << "  | " << detail << std::endl;
        if( printExtensions )
        {
            for( auto it = devices[i].extensions.begin(); it != devices[i].extensions.end(); ++it )
            {
                if( !(*it).empty() )
                {
                    table << "    |      |  " << *it << std::endl;
                    maxDetailLength = std::max(maxDetailLength, (unsigned int)(*it).length());
                }
            }
        }
        table << "    |      |" << std::endl;
        maxDetailLength = std::max(maxDetailLength, (unsigned int)detail.length());
    }
    for( unsigned int i = 0; i < maxDetailLength + 2; ++i )
    {
        std::cout << "-";
    }
    std::cout << std::endl << "    |      |" << std::endl << table.str();
    std::cout << "----|------|";
    for( unsigned int i = 0; i < maxDetailLength + 2; ++i )
    {
        std::cout << "-";
    }
    std::cout << std::endl;
}

void printError(std::string message)
{
    LOGGER("Error: " + message);
}

int main(int argc, char* argv[])
{
    try
    {
        initializeLogger();

        commandLineParameterSource = new CommandLineToolParameterSource(argc, argv);
        commandLineParameterSource->parse();
        auto mode = commandLineParameterSource->getCurrentState();
        if( (mode == CLI_APP_STATE::HELP) || (mode == CLI_APP_STATE::HELPANDPARAMETERS) )
        {
            if( mode == CLI_APP_STATE::HELPANDPARAMETERS)
                std::cout << commandLineParameterSource->getConsoleParametersFullList() << std::endl;
            else
                std::cout << commandLineParameterSource->getConsoleParametersShortList() << std::endl;
            delete commandLineParameterSource;
            return 0;
        }

        auto framework = new Framework(Logger::getInstance());
        addParameterSources(framework);
        framework->initOpenCLStack();

        switch( mode )
        {
        case CLI_APP_STATE::RECONSTRUCT:
            doReconstruction(framework);
            break;

        case CLI_APP_STATE::DEVICES:
            printCLDeviceList(false);
            break;

        case CLI_APP_STATE::DEVICESANDEXTENSIONS:
            printCLDeviceList(true);
            break;

        case CLI_APP_STATE::DEFAULT:
        default:
            // Check the existence of all required parameters and run reconstruction
            try
            {
                doReconstruction(framework);
            } catch( ParameterNotFoundException& e )
            {
                printError("Missing required parameter " + e.getParameter() +". Set it by using --" + e.getParameter() + " <value> cmd argument or load from XML config file by --config <path> argument");
            }
            break;
        }

        delete framework;
    }
    catch(const std::exception &e)
    {
        printError(e.what());
        return 1;
    }
    catch(const std::exception *e)
    {
        printError(e->what());
        return 1;
    }
    catch(...)
    {
        printError("Unknown error");
        return 2;
    }
    delete xmlParameterSource;
    delete commandLineParameterSource;
    return 0;
}
