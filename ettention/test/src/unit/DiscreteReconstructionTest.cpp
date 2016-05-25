#include "stdafx.h"
#include "algorithm/volumemanipulation/VolumeThresholdOperator.h"
#include "framework/test/TestBase.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "model/volume/Volume.h"
#include "model/volume/FloatVolume.h"
#include "model/volume/StackComparator.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parametersource/XMLParameterSource.h"

using namespace ettention;

class DiscreteReconstructionTest : public TestBase
{
public:
    DiscreteReconstructionTest()
        : TestBase(std::string(TESTDATA_DIR"/data/discrete_reconstruction"), std::string(TESTDATA_DIR"/data/discrete_reconstruction"), std::string(TESTDATA_DIR"/work/discrete_reconstruction"))
    {
    }
};

TEST_F(DiscreteReconstructionTest, DISABLED_DT_Phantom_SheppLogan_IdentityTest)
{
    //runTestReconstruction(pathToInput + "/shepp_logan_mask_dart.xml");
    //runTestReconstruction(pathToInput + "/shepp_logan_mask_sart.xml");
    //TODO: Take (input from SART to DART [initial reconstruction] of first) and (result [reconstruction] of last) and compare. Must be the same
}

TEST_F(DiscreteReconstructionTest, DISABLED_DT_Phantom_SheppLogan_ThresholdTest)
{
    auto *volume = VolumeDeserializer::load(pathToData + "/shepp_logan_256.mrc", Voxel::DataType::FLOAT_32);
    GPUMappedVolume source(framework->getOpenCLStack(), volume);

    std::vector<float> levels{0.0f, 0.000002f, 0.01f, 0.25f, 0.65f, 1.00f};
    std::vector<float> values{0.0f, 0.0f, 0.1f, 0.2f, 0.3f, 1.0f};

    VolumeThresholdOperator thresholdKernel(framework, &source, &levels, &values);
    thresholdKernel.run();
    thresholdKernel.getOutput()->ensureIsUpToDateOnCPU();

    MRCWriter writer;
    OutputParameterSet format;
    writer.write(thresholdKernel.getOutput()->getObjectOnCPU(), pathToOutput + "/shepp_logan_threshed.mrc", &format);

    delete volume;
}

TEST_F(DiscreteReconstructionTest, DISABLED_DT_Phantom_SheppLogan)
{
    runTestReconstruction(pathToInput + "/shepp_logan_mask_dart.xml");
    runTestReconstruction(pathToInput + "/shepp_logan_mask_sart.xml");
}

TEST_F(DiscreteReconstructionTest, DISABLED_DT_Syntetic_Binary)
{
    runTestReconstruction(pathToInput + "/syntetic_binary_dart.xml");
    runTestReconstruction(pathToInput + "/syntetic_binary_sart.xml");
}

TEST_F(DiscreteReconstructionTest, DISABLED_DT_Real_Empiar10009)
{
    runTestReconstruction(pathToInput + "/discrete_test.xml");
    runTestReconstruction(pathToInput + "/sart_test.xml");
}