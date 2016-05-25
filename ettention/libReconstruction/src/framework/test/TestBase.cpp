#include "stdafx.h"
#include "TestBase.h"
#include "io/deserializer/ImageDeserializer.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include <memory/MemoryListener.h>

namespace ettention
{
    TestBase::TestBase()
        : setUpCalled(false)
        , tearDownCalled(false)
    {
    }

    TestBase::TestBase(std::string dataPath, std::string inputPath, std::string outputPath)
        : TestBase()
    {
        pathToData = dataPath;
        pathToInput = inputPath;
        pathToOutput = outputPath;
    }

    TestBase::~TestBase()
    {
        if(!setUpCalled)
        {
            throw Exception("SetUp() of class TestBase was not called properly which might hide memory leaks!");
        }
        if(!tearDownCalled)
        {
            throw Exception("TearDown() of class TestBase was not called properly which might hide memory leaks!");
        }
    }

    void TestBase::SetUp()
    {
        setUpCalled = true;
        const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        listener = new MemoryListener(test_info->test_case_name() + std::string(".") + test_info->name());
        Logger::getInstance().setPrintPluginList(false);
        framework = new Framework(Logger::getInstance());
        Logger::getInstance().activateConsoleLog(Logger::FORMAT_SIMPLE);
        Logger::getInstance().setConsoleLogLevel(Logger::important);
    }

    void TestBase::TearDown()
    {
        tearDownCalled = true;
        delete framework;
        if(listener->isMemoryLeaked())
        {
            std::stringstream stream;
            listener->printSummary(stream);
            delete listener;
            FAIL() << "Memory leak detected.\n" << stream.str();
        }
        else
        {
            delete listener;
        }
    }

    void TestBase::runTestReconstruction(std::string xmlFile)
    {
        auto *config = new XMLParameterSource(xmlFile);
        framework->resetParameterSource(config);

        ReconstructionAlgorithm* app = framework->instantiateReconstructionAlgorithm();
        app->run();
        framework->writeFinalVolume(app->getReconstructedVolume());
        delete app;

        delete config;
    }

}