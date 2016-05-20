#include "stdafx.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeReconstructionOperator.h"
#include "model/volume/StackComparator.h"
#include "setup/parameterset/OutputParameterSet.h"

using namespace ettention;

class VolumeParametersTest : public TestBase
{
public:
    void OwnSetUp()
    {
        Logger::getInstance().setLogFilename(std::string(TESTDATA_DIR) + "/work/volume/VolumeParametersTest.log");
        Logger::getInstance().activateFileLog(Logger::FORMAT_SIMPLE);
    }

    void OwnTearDown()
    {
        Logger::getInstance().deactivateFileLog();
    }

    std::string computeReconstruction(XMLParameterSource* xmlConfig)
    {
        framework->parseAndAddParameterSource(xmlConfig);
        BlockIterativeReconstructionOperator app(framework);
        app.run();

        framework->writeFinalVolume(app.getReconstructedVolume());

        auto filename = framework->getParameterSet()->get<OutputParameterSet>()->getFilename().string();
        return filename;
    }
};

// 
// VolumeParametersTest
//
TEST_F(VolumeParametersTest, DifferentVoxelSize)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/volume/empiar_10009_64_voxel.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    ettention::StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/volume/empiar_10009_64_voxel.mrc");
}

TEST_F(VolumeParametersTest, DifferentDimensions)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/volume/empiar_10009_64_dimension.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    auto rms = ettention::StackComparator::getRMSBetweenVolumes(computedStack, std::string(TESTDATA_DIR) + "/data/volume/empiar_10009_64_dimension.mrc");
    ASSERT_LT(rms, 0.2f);   // This test case started causing problems. Further research required if assertion fails one more time.
}

TEST_F(VolumeParametersTest, DifferentOrigin)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/volume/empiar_10009_64_origin.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    auto rms = ettention::StackComparator::getRMSBetweenVolumes(computedStack, std::string(TESTDATA_DIR) + "/data/volume/empiar_10009_64_origin.mrc");
    ASSERT_LT(rms, 0.2f);   // This test case started causing problems. Further research required if assertion fails one more time.
}
