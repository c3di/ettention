#pragma once
#include "gtest/gtest.h"
#include "framework/Framework.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include <memory/MemoryLeakDetection.h>

namespace ettention
{
    class MemoryListener;

    class TestBase : public ::testing::Test
    {
    public:
        TestBase();
        TestBase(std::string dataPath, std::string inputPath, std::string outputPath);
        virtual ~TestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;

        virtual void runTestReconstruction(std::string xmlFile);

    protected:
        Framework* framework;

        std::string pathToData;
        std::string pathToInput;
        std::string pathToOutput;

    private:
        unsigned int beginState;
        MemoryListener* listener;
        bool setUpCalled;
        bool tearDownCalled;
    };
}