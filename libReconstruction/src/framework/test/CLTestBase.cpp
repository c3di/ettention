#include "stdafx.h"
#include "CLTestBase.h"
#include "io/deserializer/ImageDeserializer.h"
#include "setup/parametersource/XMLParameterSource.h"
#include <memory/MemoryListener.h>

namespace ettention
{
    CLTestBase::CLTestBase(HardwareParameterSet::OpenCLPlatformVendor vendor, HardwareParameterSet::OpenCLDevice device)
        : TestBase()
    {
        hwParamSet = new HardwareParameterSet(vendor, device);
    }

    CLTestBase::CLTestBase(const std::string& xmlHardwareParameterSourceFile)
        : TestBase()
    {
        auto xmlSource = new XMLParameterSource(xmlHardwareParameterSourceFile);
        hwParamSet = new HardwareParameterSet(xmlSource);
        delete xmlSource;
    }

    CLTestBase::~CLTestBase()
    {
        delete hwParamSet;
    }

    void CLTestBase::SetUp()
    {
        TestBase::SetUp();
        framework->initOpenCLStack(hwParamSet);
        clal = framework->getOpenCLStack();
    }

    void CLTestBase::TearDown()
    {
        framework->destroyOpenCLStack();
        TestBase::TearDown();
    }

    bool CLTestBase::On64bitArchitecture()
    {
        return sizeof(void*) == 8;
    }
}