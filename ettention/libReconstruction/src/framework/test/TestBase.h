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
        virtual ~TestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;

    protected:
        Framework* framework;

    private:
        unsigned int beginState;
        MemoryListener* listener;
        bool setUpCalled;
        bool tearDownCalled;
    };
}