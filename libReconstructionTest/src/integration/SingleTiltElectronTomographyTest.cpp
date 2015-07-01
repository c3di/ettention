#include "stdafx.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "io/datasource/ImageStackDatasource.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/MRCStack.h"
#include "io/serializer/VolumeSerializer.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/volume/StackComparator.h"

namespace ettention
{
    class SingleTiltElectronTomographyTest : public TestBase
    {
    public:
        void OwnSetUp()
        {
            Logger::getInstance().LogFilename(std::string(TESTDATA_DIR) + "/work/jason64/SingleTiltElectronTomographyTest.log");
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
            auto filename = framework->getParameterSet()->get<IOParameterSet>()->OutputVolumeFileName().string();
            return filename;
        }

        CLMemBuffer* volumeBuffer;
        CLMemBuffer* traversalLengthBuffer;
        CLMemBuffer* residualBuffer;
    };
}

using namespace ettention;

TEST_F(SingleTiltElectronTomographyTest, SART_Real_Data_Smoketest)
{
    auto xmlConfig = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sart_no_oversampling.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sart_no_oversampling.mrc");
    delete xmlConfig;
}

TEST_F(SingleTiltElectronTomographyTest, SART_Real_Data)
{
    auto xmlConfig = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sart.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sart.mrc");
    delete xmlConfig;
}

TEST_F(SingleTiltElectronTomographyTest, SIRT_Real_Data)
{
    auto xmlConfig = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sirt.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sirt.mrc");
    delete xmlConfig;
}

TEST_F(SingleTiltElectronTomographyTest, BLOCKITERATIVE_Real_Data)
{
    auto xmlConfig = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_blockIterative.xml");
    std::string computedStack = computeReconstruction(xmlConfig);
    StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_blockIterative.mrc");
    delete xmlConfig;
}