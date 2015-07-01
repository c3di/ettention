#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include <framework/test/TestBase.h>
#include <model/volume/StackComparator.h>
#include <reconstruction/WeightedSIRT.h>
#include <setup/ParameterSet/IOParameterSet.h>
#include <setup/parametersource/XMLParameterSource.h>

namespace ettention
{
    namespace wsirt
    {
        class WeightedSIRTPluginTest : public TestBase
        {
        public:
            std::string computeWeightedSirt(ParameterSource* xmlParameter)
            {
                xmlParameter->LogLevel(ParameterSource::LogLevelValues::SILENT);
                framework->parseAndAddParameterSource(xmlParameter);

                WeightedSIRT reco(framework);
                reco.run();

                framework->writeFinalVolume(reco.getReconstructedVolume());
                std::string ret = framework->getParameterSet()->get<IOParameterSet>()->OutputVolumeFileName().string();

                return ret;
            }
        };
    }
}

using namespace ettention;
using namespace ettention::wsirt;

TEST_F(WeightedSIRTPluginTest, Integration)
{
    std::string testDataDirectory(std::string(TESTDATA_DIR) + "/data");
    ParameterSource* xmlParameter = new XMLParameterSource(testDataDirectory + "/wsirt_shepp_logan/weightedsirt.xml");
    std::string computedStack = computeWeightedSirt(xmlParameter);
    //StackComparator::assertVolumesAreEqual(computedStack, testDataDirectory + "/ground_truth.mrc");
    const float rms = StackComparator::getRMSBetweenVolumes(computedStack, testDataDirectory + "/wsirt_shepp_logan/ground_truth.mrc");
    std::cout << "RMS (WeightedSIRT) " << rms << std::endl;
    delete xmlParameter;
    ASSERT_NEAR(rms, 0.0f, 5e-2f);
}

