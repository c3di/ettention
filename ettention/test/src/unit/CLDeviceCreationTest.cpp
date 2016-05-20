#include "stdafx.h"
#include <gtest/gtest.h>
#include <gpu/opencl/CLContextCreator.h>

namespace ettention
{
    class CLDeviceCreationTest : public ::testing::Test { };
}

using namespace ettention;

TEST_F(CLDeviceCreationTest, ListDevices)
{
    CLContextCreator creator;
    auto infos = creator.GatherDeviceInfos();
    for(auto it = infos.begin(); it != infos.end(); ++it)
    {
        std::cout << (*it).type << " " << (*it).name << ", version: " << (*it).version << "." << std::endl;
    }
}