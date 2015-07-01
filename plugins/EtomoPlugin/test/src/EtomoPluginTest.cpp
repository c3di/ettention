#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "framework/test/CLTestBase.h"
#include "framework/RandomAlgorithms.h"
#include "framework/plugins/PluginManager.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "model/volume/VoxelType.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"
#include "setup/ParameterSet/BackprojectionParameterSet.h"
#include "setup/ParameterSet/ForwardProjectionParameterSet.h"
#include "setup/ParameterSet/VolumeParameterSet.h"
#include "setup/ParameterSet/PriorKnowledgeParameterSet.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "setup/EtomoParameterSource.h"
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

    ASSERT_EQ(parameters->get<IOParameterSet>()->OutputVolumeRotation(), IOParameterSet::ALONG_XZ);
    //ASSERT_EQ(parameters->get<IOParameterSet>()->FormatOptions().InvertData(), true);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->PreferedOpenCLDevice(), HardwareParameterSet::CPU_only);
    ASSERT_EQ(parameters->ProjectionSetType(), "maxangle");
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->InitValue(), 0.0f);
}

TEST_F(EtomoPluginTest, EtomoBinningTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setup3/");
    auto parameters = framework->getParameterSet();
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->Dimension(), Vec3f(256.0f, 256.0f, 25.0f));
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupVolumeParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<VolumeParameterSet>()->Dimension(), Vec3f(200.0f, 300.0f, 40.0f));
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->Origin(), Vec3f(-50.0f, 70.0f, 0.0f));
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->Resolution(), Vec3f(100,150,20));
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->InitFile(), "initialVolume.mrc");
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupAlgorithmParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->NumberOfIterations(), 3);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->Lambda(), 0.3f);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->Algorithm(), "blockIterative");
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->UseLongObjectCompensation(), true);
    ASSERT_EQ(parameters->get<AlgorithmParameterSet>()->ProjectionBlockSize(), 7);
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupDebugParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    ASSERT_EQ(parameters->get<DebugParameterSet>()->DisplayKernelParameters(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->DisplayProjectionRMS(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteProjections(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteProjectionsInFourierSpace(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteVirtualProjections(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteVirtualProjectionsInFourierSpace(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteResiduals(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteRayLengthImages(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteIntermediateVolumes(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->WriteIntermediateVolumesAfterIterations(), true);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->DebugInfoPath(), "folderWithDebugFiles");
    ASSERT_EQ(parameters->get<DebugParameterSet>()->GetProfilingLevel(), DebugParameterSet::ProfilingLevel::PROFILING_DETAIL);
    ASSERT_EQ(parameters->get<DebugParameterSet>()->PerformanceReportFile(), "performaceFile.csv");
}

TEST_F(EtomoPluginTest, EtomoEttentionsetupHardwareParametersTest)
{
    addEtomoParameterSourceToFramework(std::string(TESTDATA_DIR) + "/data/setupGroupParameters/");
    auto parameters = framework->getParameterSet();

    boost::optional<unsigned int> subvolumes = 5;

    ASSERT_EQ(parameters->get<HardwareParameterSet>()->PreferedOpenCLPlatform(), HardwareParameterSet::intel);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->PreferedOpenCLDevice(), HardwareParameterSet::CPU_first);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->DisableImageSupport(), true);
    ASSERT_EQ(parameters->get<HardwareParameterSet>()->SubVolumeCount(), subvolumes);
}

/*
TEST_F(EtomoPluginTest, EtomoEttentionsetupOutputParametersTest)
{
    std::string testdataDirectory = std::string(TESTDATA_DIR) + "/data/setupGroupParameters/";

    EtomoPlugin* plugin = new EtomoPlugin(testdataDirectory);
    AlgebraicParameterSet* parameters = new AlgebraicParameterSet(plugin->instantiateParameterSource()[0]);

    ASSERT_EQ(parameters->get<OutputFormatOptions>()->VoxelType(), OutputFormatOptions::UNSIGNED_16);
    ASSERT_EQ(parameters->get<OutputFormatOptions>()->OutputVolumeRotation(), OutputFormatOptions::ORDER_XYZ);
    ASSERT_EQ(parameters->get<OutputFormatOptions>()->InvertData(), true);

    delete parameters;
    delete plugin;
}*/

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

    ASSERT_EQ(parameters->get<BackProjectionParameterSet>()->Samples(), 9);
    ASSERT_EQ(parameters->get<ForwardProjectionParameterSet>()->Samples(), 16);
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

    ASSERT_EQ(parameters->get<IOParameterSet>()->OutputVolumeFileName(), "example_output.rec_et3");
    ASSERT_EQ(parameters->get<IOParameterSet>()->InputStackFileName(), "example_input.ali");
    ASSERT_EQ(parameters->get<IOParameterSet>()->TiltAnglesFileName(), "example_angles.tlt");
    ASSERT_EQ(parameters->get<VolumeParameterSet>()->Dimension(), Vec3f(512.0f, 512.0f, 25.0f));
    ASSERT_EQ(parameters->get<IOParameterSet>()->LogaritmizeData(), true);
    ASSERT_EQ(parameters->get<IOParameterSet>()->XAxisTilt(), 0.3f);
    ASSERT_EQ(parameters->SkipProjectionList(), excludeList);
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