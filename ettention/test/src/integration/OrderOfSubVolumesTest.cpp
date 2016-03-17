#include "stdafx.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "framework/test/ProjectionTestBase.h"
#include "framework/Framework.h"

namespace ettention
{
    class OrderOfSubVolumesTest : public ProjectionTestBase
    {
    public:
        // Test gain in speed if we traverse subvolumes in normal or reversed order. Comment\Uncomment following lines of code:
        //HandleBackProjection(subVolume, projectionAccess->getBlockSize(block));
        //HandleBackProjection(subVolumeCount - subVolume - 1, projectionAccess->getBlockSize(block));

        OrderOfSubVolumesTest()
            : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/subvolumes/", std::string(TESTDATA_DIR) + "/work/subvolumes/")
        {
        }

        std::string computeReconstruction(XMLParameterSource* xmlConfig)
        {
            LOGGER("Number of subvolumes: " << xmlConfig->getIntParameter("hardware.subVolumeCount"));

            framework->parseAndAddParameterSource(xmlConfig);
            BlockIterativeReconstructionOperator app(framework);
            app.run();

            framework->writeFinalVolume(app.getReconstructedVolume());

            auto filename = framework->getParameterSet()->get<OutputParameterSet>()->getFilename().string();
            return filename;
        }

        virtual void SetUp() override
        {
            ProjectionTestBase::SetUp();
            Logger::getInstance().setLogFilename(workDirectory + "OrderOfSubVolumesTest.log");
            Logger::getInstance().activateFileLog(Logger::FORMAT_SIMPLE);
        }

        virtual void TearDown() override
        {
            Logger::getInstance().deactivateFileLog();
            ProjectionTestBase::TearDown();
        }

        virtual ~OrderOfSubVolumesTest()
        {}
    };
}

using namespace ettention;

TEST_F(OrderOfSubVolumesTest, SubVolumeCount1_TestCase)
{
    auto xmlConfig = new XMLParameterSource(testdataDirectory + "empiar_10009_512_sart_sub1.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    delete xmlConfig;
}

TEST_F(OrderOfSubVolumesTest, SubVolumeCount2_TestCase)
{
    auto xmlConfig = new XMLParameterSource(testdataDirectory + "empiar_10009_512_sart_sub2.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    delete xmlConfig;
}

TEST_F(OrderOfSubVolumesTest, SubVolumeCount4_TestCase)
{
    auto xmlConfig = new XMLParameterSource(testdataDirectory + "empiar_10009_512_sart_sub4.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    delete xmlConfig;
}

TEST_F(OrderOfSubVolumesTest, SubVolumeCount8_TestCase)
{
    auto xmlConfig = new XMLParameterSource(testdataDirectory + "empiar_10009_512_sart_sub8.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    delete xmlConfig;
}

TEST_F(OrderOfSubVolumesTest, SubVolumeCount16_TestCase)
{
    auto xmlConfig = new XMLParameterSource(testdataDirectory + "empiar_10009_512_sart_sub16.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    delete xmlConfig;
}