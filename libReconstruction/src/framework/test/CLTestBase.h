#pragma once

#include "gtest/gtest.h"
#include "framework/Framework.h"
#include "framework/test/TestBase.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include <memory/MemoryLeakDetection.h>

namespace ettention
{
    class XMLParameterSource;

    class CLTestBase : public TestBase
    {
    public:
        CLTestBase(HardwareParameterSet::OpenCLPlatformVendor vendor = HardwareParameterSet::nvidia, HardwareParameterSet::OpenCLDevice device = HardwareParameterSet::GPU_first);
        CLTestBase(const std::string& xmlHardwareParameterSourceFile);
        virtual ~CLTestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;

        bool On64bitArchitecture();

    protected:
        CLAbstractionLayer* clal;

    private:
        HardwareParameterSet* hwParamSet;
    };
}
