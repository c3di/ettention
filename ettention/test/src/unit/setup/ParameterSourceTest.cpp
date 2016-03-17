#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "setup/CommandLineToolParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "framework/FileAlgorithms.h"
#include "framework/Logger.h"

namespace ettention
{
    class ParameterSourceTest : public TestBase
    {
    public:
        void SetUp()
        {
            TestBase::SetUp();

            std::vector<std::string> parameters;
            parameters.push_back("test.exe");
            /*parameters.push_back("-lambda");
            parameters.push_back("1.2");
            parameters.push_back("-numberOfIterations");
            parameters.push_back("4");*/
            parameters.push_back("--config");
            parameters.push_back("pathToConfig");

            commandLineParameterSource = new CommandLineToolParameterSource(parameters);
            cascadingParameterSource = new CascadingParameterSource;
            cascadingParameterSource->parseAndAddSource(commandLineParameterSource);
        }

        void TearDown()
        {
            delete cascadingParameterSource;
            delete commandLineParameterSource;
            TestBase::TearDown();
        }

        CommandLineToolParameterSource* commandLineParameterSource;
        CascadingParameterSource* cascadingParameterSource;
    };
}

using namespace ettention;

TEST_F(ParameterSourceTest, Unit_CommandLine_ParametersExistPositive) 
{
    ASSERT_TRUE(commandLineParameterSource->parameterExists("config"));
}

TEST_F(ParameterSourceTest, Unit_CommandLine_ParametersExistNegative) 
{
    ASSERT_FALSE(commandLineParameterSource->parameterExists("any"));
    ASSERT_FALSE(commandLineParameterSource->parameterExists("test.exe"));
    ASSERT_FALSE(commandLineParameterSource->parameterExists("subareaSize"));
}

TEST_F(ParameterSourceTest, Unit_CommandLine_ParametersValueString)
{
    EXPECT_STREQ(commandLineParameterSource->getStringParameter("config").c_str(), "pathToConfig");
}

TEST_F(ParameterSourceTest, Unit_CommandLine_ParametersValueFloat) 
{
    //ASSERT_FLOAT_EQ(commandLineParameterSource->getFloatParameter("lambda"), 1.2f);
}

TEST_F(ParameterSourceTest, Unit_CommandLine_ParametersValueUInt) 
{
    //ASSERT_EQ(commandLineParameterSource->getUIntParameter("numberOfIterations"), 4);
}

// 
// Cascading Parameter Source
//
TEST_F(ParameterSourceTest, Unit_Cascading_ParametersExistPositive) 
{
    //ASSERT_TRUE(cascadingParameterSource->parameterExists("lambda"));
}

TEST_F(ParameterSourceTest, Unit_Cascading_ParametersExistNegative) 
{
    ASSERT_FALSE(cascadingParameterSource->parameterExists("any"));
    ASSERT_FALSE(cascadingParameterSource->parameterExists("test.exe"));
    ASSERT_FALSE(cascadingParameterSource->parameterExists("subareaSize"));
}

TEST_F(ParameterSourceTest, Unit_Cascading_ParametersValueFloat) 
{
    //ASSERT_FLOAT_EQ(cascadingParameterSource->getFloatParameter("lambda"), 1.2f);
}

TEST_F(ParameterSourceTest, Unit_Cascading_ParametersValueUInt) 
{
    //ASSERT_EQ(cascadingParameterSource->getUIntParameter("numberOfIterations"), 4);
}

TEST_F(ParameterSourceTest, XML_Relative_Path)
{
    XMLParameterSource* source = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/setup/config.xml");

    auto inputStackFilename = source->getPathParameter("input.projections");
    auto expectedFilename = boost::filesystem::path(std::string(TESTDATA_DIR) + "/data/jason_64_SART/jason64.ali");
    ASSERT_TRUE(boost::filesystem::equivalent(inputStackFilename, expectedFilename));

    delete source;
}

TEST_F(ParameterSourceTest, XML_Relative_Path_Existing_File)
{
    XMLParameterSource* source = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/setup/config.xml");

    auto inputStackFilename = source->getPathParameter("input.projections");
    auto expectedFilename = boost::filesystem::path(std::string(TESTDATA_DIR) + "/data/jason_64_SART/jason64.ali");
    ASSERT_TRUE(boost::filesystem::equivalent(inputStackFilename, expectedFilename));

    delete source;
}

TEST_F(ParameterSourceTest, XML_Relative_Path_Nonexisting_File)
{
    XMLParameterSource* source = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/setup/config.xml");

    auto inputStackFilename = source->getPathParameter("output.filename");
    auto expectedFilename = FileAlgorithms::normalizePath(boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/jason64/nonexistent.mrc"));
    
    ASSERT_EQ(inputStackFilename, expectedFilename);

    delete source;
}

TEST_F(ParameterSourceTest, XML_Rootlevel_Node_Access)
{
    XMLParameterSource* source = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/setup/config.xml");

    auto inputStackFilename = source->getPathParameter("output.filename");

    delete source;
}

TEST_F(ParameterSourceTest, XML_Level1_Node_Access)
{
    XMLParameterSource* source = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/setup/config.xml");

    bool writeResiduals = source->getBoolParameter("debug.writeResiduals");
    ASSERT_TRUE(writeResiduals);

    bool writeProjections = source->getBoolParameter("debug.writeProjections");
    ASSERT_FALSE(writeProjections);

    delete source;
}

TEST_F(ParameterSourceTest, XML_Level1_Node_Access_NonExistent)
{
    XMLParameterSource* source = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/setup/config.xml");

    ASSERT_THROW(source->getBoolParameter("nonExistentParameter"), Exception);
    ASSERT_THROW(source->getBoolParameter("debug.nonExistentParameter"), Exception);
    ASSERT_THROW(source->getBoolParameter("writeProjections"), Exception);
    ASSERT_THROW(source->getBoolParameter("writeResiduals"), Exception);

    delete source;
}
