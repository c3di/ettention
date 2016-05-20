#include "stdafx.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeReconstructionOperator.h"
#include "io/datasource/ImageStackDatasource.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/MRCStack.h"
#include "io/serializer/VolumeSerializer.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/volume/StackComparator.h"
#include "setup/parameterset/OutputParameterSet.h"

using namespace ettention;

class HardwareParametersTest : public TestBase
{
public:
    void OwnSetUp()
    {
        Logger::getInstance().setLogFilename(std::string(TESTDATA_DIR) + "/work/hardware/HardwareParametersTest.log");
        Logger::getInstance().activateFileLog(Logger::FORMAT_SIMPLE);
    }

    void OwnTearDown()
    {
        Logger::getInstance().deactivateFileLog();
    }

    std::string computeReconstruction(XMLParameterSource* xmlConfig, ManualParameterSource* manualConfig)
    {
        framework->parseAndAddParameterSource(xmlConfig);
        framework->parseAndAddParameterSource(manualConfig);

        BlockIterativeReconstructionOperator app(framework);
        app.run();

        framework->writeFinalVolume(app.getReconstructedVolume());

        auto filename = framework->getParameterSet()->get<OutputParameterSet>()->getFilename().string();
        return filename;
    }
};

TEST_F(HardwareParametersTest, Hardware_ImageSupport)
{
    auto *xml = new XMLParameterSource(std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_512_sirt.xml");
    auto *manual = new ManualParameterSource;
    manual->setParameter("output.filename", std::string(TESTDATA_DIR) + "/work/hardware/empiar_10009_sirt_imgdisable.mrc");
    manual->setParameter("hardware.disableImageSupport", true);
    manual->setParameter("hardware.openclDeviceType", "gpu");
    manual->setParameter("debug.profilingLevel", "detail");
    manual->setParameter("debug.performanceReportFile", std::string(TESTDATA_DIR) + "/work/hardware/kernel_timing.csv");

    std::string computedStack = computeReconstruction(xml, manual);
    //ettention::StackComparator::assertVolumesAreEqual(computedStack, std::string(TESTDATA_DIR) + "/data/empiar_10009/empiar_10009_sirt.mrc");
    delete manual;
    delete xml;
}