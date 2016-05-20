#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "framework/test/TestBase.h"
#include "framework/RandomAlgorithms.h"
#include "framework/plugins/PluginManager.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "model/volume/Voxel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "setup/parameterset/BackprojectionParameterSet.h"
#include "setup/parameterset/ForwardProjectionParameterSet.h"
#include "setup/parameterset/InputParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parameterset/OptimizationParameterSet.h"
#include "setup/parameterset/VolumeParameterSet.h"
#include "setup/parameterset/PriorKnowledgeParameterSet.h"
#include "setup/EtomoParameterSource.h"
#include "setup/parameterset/IOEnumerations.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeReconstructionOperator.h"
#include <fstream>

namespace ettention
{
    namespace etomo
    {
        class EtomoPluginTest : public TestBase
        {
        public:
            EtomoPluginTest()
                : parameterSource(0)
            {
            }            

            ~EtomoPluginTest()
            {
            }

            void TearDown() override
            {
                delete parameterSource;
                TestBase::TearDown();
            }

            void addEtomoParameterSourceToFramework(const std::string& pathToSetupFiles, bool addPathToFiles = false)
            {
                parameterSource = new EtomoParameterSource(pathToSetupFiles, addPathToFiles);
                framework->parseAndAddParameterSource(parameterSource);
            }

        private:
            EtomoParameterSource* parameterSource;
        };
    }
}

using namespace ettention;
using namespace ettention::etomo;

TEST_F(EtomoPluginTest, EtomoInternalParameterSetTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setup1/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<OutputParameterSet>()->getOrientation(), ORDER_XZY);
    //ASSERT_EQ(parameters->get<IOParameterSet>()->FormatOptions().InvertData(), true);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->getPreferedOpenCLDevice(), HardwareParameterSet::CPU_only);
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getInitValue(), 0.0f);
}

TEST_F(EtomoPluginTest, EtomoBinningTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setup3/");
    auto parameters = framework->getParameterSet();
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getDimension(), Vec3f(256.0f, 256.0f, 25.0f));
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupVolumeParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getDimension(), Vec3f(200.0f, 300.0f, 40.0f));
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getOrigin(), Vec3f(-50.0f, 70.0f, 0.0f));
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getResolution(), Vec3f(100, 150, 20));
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getInitFile(), "initialVolume.mrc");
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupAlgorithmParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->getNumberOfIterations(), 3);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->getLambda(), 0.3f);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->getAlgorithm(), "blockIterative");
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->getProjectionBlockSize(), 7);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation(), true);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->getBasisFunctions(), AlgorithmParameterSet::BasisFunctionType::BASIS_VOXELS);
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupDebugParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldDisplayKernelParameters(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldDisplayProjectionRMS(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteProjections(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteProjectionsInFourierSpace(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteVirtualProjections(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteVirtualProjectionsInFourierSpace(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteResiduals(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteRayLengthImages(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteIntermediateVolumes(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->shouldWriteIntermediateVolumesAfterIterations(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->getDebugInfoPath(), "folderWithDebugFiles");
    ASSERT_EQ(parameters->get<DebugParameterSet>()->getProfilingLevel(), DebugParameterSet::ProfilingLevel::DETAIL);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->getPerformanceReportFile(), "performanceFile.csv");
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupHardwareParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    unsigned int subvolumes = 5;

    ASSERT_EQ(parameters->get<HardwareParameterSet>()->getPreferedOpenCLPlatform(), HardwareParameterSet::intel);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->getPreferedOpenCLDevice(), HardwareParameterSet::CPU_first);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->isImageSupportDisabled(), true);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->getSubVolumeCount(), subvolumes);
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupOptimizationParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<OptimizationParameterSet>()->getVoxelRepresentation(), Voxel::DataType::FLOAT_32);
    ASSERT_EQ(parameters->get<OptimizationParameterSet>()->getProjectionSetType(), "maxangle");
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupOutputParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<OutputParameterSet>()->getVoxelType(), VoxelValueType::IO_VOXEL_TYPE_UNSIGNED_16);
    ASSERT_EQ(parameters->get<OutputParameterSet>()->getOrientation(), CoordinateOrder::ORDER_XYZ);
    ASSERT_EQ(parameters->get<OutputParameterSet>()->getInvert(), true);
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupPriorKnowledgeParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    boost::optional<float> min = 1.0f;
    boost::optional<float> max = 100.0f;

    ASSERT_EQ(parameters->get<PriorKnowledgeParameterSet>()->getVolumeMinimumIntensity(), min);
    ASSERT_EQ(parameters->get<PriorKnowledgeParameterSet>()->getVolumeMaximumIntensity(), max);
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupSamplingParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<BackProjectionParameterSet>()->getSamplesNumber(), 9);
    ASSERT_EQ(parameters->get<ForwardProjectionParameterSet>()->getSamplesNumber(), 16);
}

TEST_F(EtomoPluginTest, EtomoTiltTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setup2/");
    auto parameters = framework->getParameterSet();

    std::set<HyperStackIndex> excludeList;
    excludeList.insert(HyperStackIndex(0));
    excludeList.insert(HyperStackIndex(1));
    excludeList.insert(HyperStackIndex(4));
    excludeList.insert(HyperStackIndex(5));
    excludeList.insert(HyperStackIndex(6));
    excludeList.insert(HyperStackIndex(7));
    excludeList.insert(HyperStackIndex(32));
    excludeList.insert(HyperStackIndex(37));

    ASSERT_EQ(parameters->get<OutputParameterSet>()->getFilename(), "example_output.rec_et3");
    ASSERT_EQ(parameters->get<InputParameterSet>()->getProjectionsPath(), "example_input.ali");
    ASSERT_EQ(parameters->get<InputParameterSet>()->getTiltAnglesPath(), "example_angles.tlt");
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->getDimension(), Vec3f(512.0f, 512.0f, 25.0f));
    ASSERT_EQ(parameters->get<InputParameterSet>()->getLogarithmize(), true);
    ASSERT_EQ(parameters->get<InputParameterSet>()->getXAxisTilt(), 0.3f);
    ASSERT_EQ(parameters->get<InputParameterSet>()->getSkipProjectionList(), excludeList);
}

TEST_F(EtomoPluginTest, EtomoWholeVolumeIntegrationTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setup4/", true);
    auto reco = framework->instantiateReconstructionAlgorithm();
    reco->run();
    delete reco;
}

TEST_F(EtomoPluginTest, EtomoSubareaIntegrationTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setup5/", true);
    auto reco = framework->instantiateReconstructionAlgorithm();
    reco->run();
    delete reco;
}