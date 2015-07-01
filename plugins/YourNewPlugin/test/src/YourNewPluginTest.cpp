#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "framework/test/CLTestBase.h"
#include "framework/plugins/PluginManager.h"
#include "YourNewPlugin.h"

namespace ettention
{
    namespace yournew
    {
        class YourNewPluginTest : public CLTestBase
        {
        public:
            YourNewPluginTest()
                : CLTestBase(HardwareParameterSet::nvidia, HardwareParameterSet::GPU_only)
            {
                testdataDirectory = std::string(TESTDATA_DIR) + "/data/";
                workDirectory = std::string(TESTDATA_DIR) + "/work/";
            }
            virtual ~YourNewPluginTest()
            {

            }
            virtual void SetUp() override
            {
                CLTestBase::SetUp();
            }

            virtual void TearDown() override
            {
                CLTestBase::TearDown();
            }

            std::string testdataDirectory;
            std::string workDirectory;
        };
    }
}

using namespace ettention;
using namespace ettention::yournew;

TEST_F(YourNewPluginTest, Hello)
{
    std::cout << "In the Hello test" << std::endl;
}

TEST_F(YourNewPluginTest, Hello2)
{
    std::cout << "In the Hello2" << std::endl;
}
/*
    some test functions independent of YourNewPluginTest
    
    // an example function for getting a test setup
    virtual void OwnSetUp()
    {
        testdataDirectory = std::string(TESTDATA_DIR) +"/data/";
        workDirectory = std::string(TESTDATA_DIR) + "/work/";

        projectionResolution = Vec2ui(256, 256);

        volume = new Volume(VolumeProperties(Vec3ui(128, 128, 128), VoxelType::FLOAT_32, 1), 0.0f);
        mappedVolume = new GPUMappedVolume(clal, volume);

        microscopeGeometry = getDefaultMicroscopeGeometry();

        rayLengthImage = new GPUMapped<Image>(clal, projectionResolution);
        computeRayLength = new ComputeRayLengthKernel(clal, projectionResolution, microscopeGeometry, rayLengthImage, true, false);
        
        plugin = new YourNewPlugin();
        framework->getPluginManager()->registerPluginManually(plugin);
    }

    // and deleting the generated objects
    virtual void OwnTearDown()
    {
        delete computeRayLength;
        delete rayLengthImage;
        delete microscopeGeometry;
        delete mappedVolume;
        delete volume;
        delete plugin;
    }

    // a test for checking the class VolumeStatistics
    void TestComputeMinMaxValues(Volume* volume)
    {
        volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
        volume->setVoxelToValue(Vec3ui(15, 15, 15), 121.0f);
        volume->setVoxelToValue(Vec3ui(15, 15, 15), 2.0f);

        size_t test = volume->Properties().GetVolumeVoxelCount();

        auto volumeStats = VolumeStatistics::compute(volume);
        ASSERT_FLOAT_EQ(volumeStats.getMin(), 0.0f)
        ASSERT_FLOAT_EQ(volumeStats.getMax(), 2.0f);
        ASSERT_FLOAT_EQ(volumeStats.getMean(), 0.000732421875f);
    }

    // a test for checking voxel values
    void Test_Voxel_Access(Volume* volume) 
    {
        ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(0, 0, 0)), 0.0f);
        ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(15, 15, 15)), 0.0f);

        ASSERT_THROW(volume->getVoxelValue(Vec3ui(16, 0, 0)), Exception);
        ASSERT_THROW(volume->getVoxelValue(Vec3ui(0, 16, 0)), Exception);
        ASSERT_THROW(volume->getVoxelValue(Vec3ui(0, 0, 16)), Exception);

        volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
        volume->setVoxelToValue(Vec3ui(15, 15, 15), 1.0f);

        ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(0, 1, 0)), 0.0f);
        ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(15, 14, 15)), 0.0f);

        ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(0, 0, 0)), 1.0f);
        ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(15, 15, 15)), 1.0f);
    }

*/
/*
    // the former test adapted to float_32
    TEST_F(VolumeTest, Test_Voxel_Access_Float32)
    {
        VolumeProperties properties(Vec3ui(16, 16, 16), VoxelType::FLOAT_32, boost::none);
        Volume* volume = new Volume(properties, 0.0f);

        Test_Voxel_Access(volume);

        delete volume;
    }
*/