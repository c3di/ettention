#include "stdafx.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeReconstructionOperator.h"
#include "model/volume/StackComparator.h"
#include "setup/parameterset/OutputParameterSet.h"

using namespace ettention;

class SingleTiltElectronTomographyTest : public TestBase
{
public:
    void OwnSetUp()
    {
        Logger::getInstance().setLogFilename(std::string(TESTDATA_DIR) + "/work/jason64/SingleTiltElectronTomographyTest.log");
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
// SingleTiltElectronTomographyTest
//
TEST_F(SingleTiltElectronTomographyTest, SART_Real_Data_Smoketest)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sart_no_oversampling.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    ettention::StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sart_no_oversampling.mrc");
}

TEST_F(SingleTiltElectronTomographyTest, SART_Real_Data)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sart.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    ettention::StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sart.mrc");
}

TEST_F(SingleTiltElectronTomographyTest, SIRT_Real_Data)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sirt.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    auto rms = ettention::StackComparator::getRMSBetweenVolumes(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sirt.mrc");
    ASSERT_LT(rms, 0.2f);   // This test case started causing problems. Further research required if assertion fails one more time.
}

TEST_F(SingleTiltElectronTomographyTest, BLOCKITERATIVE_Real_Data)
{
    XMLParameterSource xmlConfig(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_blockIterative.xml");
    std::string computedStack = computeReconstruction(&xmlConfig);
    ettention::StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_blockIterative.mrc");
}
