#pragma once
#include "gtest/gtest.h"
#include "framework/Framework.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
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

    protected:
        unsigned int beginState;
        MemoryListener* listener;
    };
}