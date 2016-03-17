#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include <framework/test/TestBase.h>
#include <model/volume/StackComparator.h>
#include <reconstruction/WBP.h>
#include <setup/parametersource/XMLParameterSource.h>
#include <setup/parameterset/OutputParameterSet.h>

namespace ettention
{
    namespace wbp
    {
        class WBPPluginTest : public TestBase
        {
        public:
            std::string computeWBP(ParameterSource* xmlParameter)
            {
                xmlParameter->setLogLevel(ParameterSource::LogLevel::SILENT);
                framework->parseAndAddParameterSource(xmlParameter);

                WBP application(framework);
                application.run();

                framework->writeFinalVolume(application.getReconstructedVolume());
                return framework->getParameterSet()->get<OutputParameterSet>()->getFilename().string();
            }
        };
    }
}

using namespace ettention;
using namespace ettention::wbp;

TEST_F(WBPPluginTest, Integration)
{
    std::string testDataDirectory(std::string(TESTDATA_DIR) + "/data");
    ParameterSource* xmlParameter = new XMLParameterSource(testDataDirectory + "/wbp_shepp_logan/wbp.xml");
    std::string computedStack = computeWBP(xmlParameter);
    ASSERT_TRUE(StackComparator::getRMSBetweenVolumes(computedStack, testDataDirectory + "/wbp_shepp_logan/ground_truth.mrc") < 0.05f);
    delete xmlParameter;
}

