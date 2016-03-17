#include "stdafx.h"
#include "TestBase.h"
#include "io/deserializer/ImageDeserializer.h"
#include "setup/parametersource/XMLParameterSource.h"
#include <memory/MemoryListener.h>

namespace ettention
{
    TestBase::TestBase()
        : setUpCalled(false)
        , tearDownCalled(false)
    {
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
        framework = new Framework(Logger::getInstance());
        Logger::getInstance().activateConsoleLog(Logger::FORMAT_SIMPLE);
        Logger::getInstance().setConsoleLogLevel(Logger::important);
    }

    void TestBase::TearDown()
    {
        tearDownCalled = true;
        delete framework;
        if(listener->IsMemoryLeaked())
        {
            std::stringstream stream;
            listener->PrintSummary(stream);
            delete listener;
            FAIL() << stream.str();
        }
        else
        {
            delete listener;
        }
    }

}