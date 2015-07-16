#include "stdafx.h"
#include <algorithm>
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "framework/Logger.h"
#include "framework/plugins/PluginManager.h"
#include "framework/time/Timer.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLContextCreator.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/CommandLineToolParameterSource.h"

#if defined(WIN32) && defined(max)
    #undef max
#endif

using namespace ettention;

CommandLineToolParameterSource* commandLineParameterSource = nullptr;
XMLParameterSource* xmlParameterSource = nullptr;

void addParameterSources(Framework* framework, int argc, char* argv[])
{
    commandLineParameterSource = new CommandLineToolParameterSource(argc, argv);
    framework->parseAndAddParameterSource(commandLineParameterSource);

    auto sourcesFromPlugins = framework->getPluginManager()->instantiateParameterSource();
    for (auto it = sourcesFromPlugins.begin(); it != sourcesFromPlugins.end(); ++it)
    {
        framework->parseAndAddParameterSource(*it);
    }

    if(commandLineParameterSource->parameterExists("configfile"))
    {
        xmlParameterSource = new XMLParameterSource(commandLineParameterSource->getStringParameter("configfile"));
        framework->parseAndAddParameterSource(xmlParameterSource);
    }
}

void initializeLogger()
{
    Logger::getInstance().activateConsoleLog();
    Logger::getInstance().LogFilename("ettention.log");
    Logger::getInstance().activateFileLog();
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
    for(unsigned int i = 0; i < devices.size(); ++i)
    {
        std::string detail = devices[i].name + ", " + devices[i].version;
        table << " " << std::setfill(' ') << std::setw(2) << i << " | " << devices[i].type << "  | " << detail << std::endl;
        if(printExtensions)
        {
            for(auto it = devices[i].extensions.begin(); it != devices[i].extensions.end(); ++it)
            {
                if(!(*it).empty())
                {
                    table << "    |      |  " << *it << std::endl;
                    maxDetailLength = std::max(maxDetailLength, (unsigned int)(*it).length());
                }
            }
        }
        table << "    |      |" << std::endl;
        maxDetailLength = std::max(maxDetailLength, (unsigned int)detail.length());
    }
    for(unsigned int i = 0; i < maxDetailLength + 2; ++i)
    {
        std::cout << "-";
    }
    std::cout << std::endl << "    |      |" << std::endl << table.str();
    std::cout << "----|------|";
    for(unsigned int i = 0; i < maxDetailLength + 2; ++i)
    {
        std::cout << "-";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        bool printDevices = false;
        bool printExtensions = false;
        for(int i = 0; i < argc; ++i)
        {
            std::string arg(argv[i]);
            if(arg == "--devices" || arg == "-d")
            {
                printDevices = true;
            }
            else if(arg == "--devicesAndExtensions" || arg == "-de")
            {
                printDevices = true;
                printExtensions = true;
            }
        }
        if(printDevices)
        {
            printCLDeviceList(printExtensions);
            return 0;
        }
        initializeLogger();
        LOGGER("Ettention, version 2015, (c) DFKI GmbH 2015");
        Logger::getInstance().forceLogFlush();
        auto framework = new Framework( Logger::getInstance() );
        addParameterSources(framework, argc, argv);
        framework->initOpenCLStack();
        ReconstructionAlgorithm* app = framework->instantiateReconstructionAlgorithm();
        app->run();
        framework->writeFinalVolume(app->getReconstructedVolume());
        LOGGER("Reconstruction finished.");
        delete app;
        delete framework;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch(const std::exception* e)
    {
        std::cerr << "Error: " << e->what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "Error: Unknown error" << std::endl;
        return 2;
    }
    delete xmlParameterSource;
    delete commandLineParameterSource;
    return 0;
}
