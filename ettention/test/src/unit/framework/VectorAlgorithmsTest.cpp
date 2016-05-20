#include "stdafx.h"
#include <framework/test/TestBase.h>
#include <framework/VectorAlgorithms.h>

namespace ettention
{
    class VectorAlgorithmsTest : public TestBase {};
}

using namespace ettention;

TEST_F(VectorAlgorithmsTest, TestSplitString_Default)
{
    auto result = VectorAlgorithms::splitString("Hallo World", " ");
    ASSERT_STREQ(result[0].c_str(), "Hallo");
    ASSERT_STREQ(result[1].c_str(), "World");
}

TEST_F(VectorAlgorithmsTest, TestSplitString_MiddleEmpty)
{
    auto result = VectorAlgorithms::splitString("Hallo  World", " ");
    ASSERT_STREQ(result[0].c_str(), "Hallo");
    ASSERT_STREQ(result[1].c_str(), "World");
}

TEST_F(VectorAlgorithmsTest, TestSplitString_TrailingEmptyString)
{
    auto result = VectorAlgorithms::splitString(" Hallo World", " ");
    ASSERT_STREQ(result[0].c_str(), "Hallo");
    ASSERT_STREQ(result[1].c_str(), "World");
}

TEST_F(VectorAlgorithmsTest, TestSplitString_EndingEmptyString)
{
    auto result = VectorAlgorithms::splitString("Hallo World ", " ");
    ASSERT_STREQ(result[0].c_str(), "Hallo");
    ASSERT_STREQ(result[1].c_str(), "World");
}

