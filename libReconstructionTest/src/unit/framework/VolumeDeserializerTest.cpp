#include "stdafx.h"
#include "framework/error/Exception.h"
#include "model/volume/Volume.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class VolumeDeserializerTest : public TestBase
    {
    public:
        void voidCheckPatternInVolume(Volume* volume)
        {
            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(0, 0, 0)));
            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(1, 0, 0)));
            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(2, 0, 0)));
            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(3, 0, 0)));

            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(0, 0, 0)));
            ASSERT_FLOAT_EQ(1.0f, volume->getVoxelValue(Vec3ui(0, 1, 0)));
            ASSERT_FLOAT_EQ(2.0f, volume->getVoxelValue(Vec3ui(0, 2, 0)));
            ASSERT_FLOAT_EQ(3.0f, volume->getVoxelValue(Vec3ui(0, 3, 0)));

            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(0, 0, 0)));
            ASSERT_FLOAT_EQ(1.0f, volume->getVoxelValue(Vec3ui(0, 0, 1)));
            ASSERT_FLOAT_EQ(2.0f, volume->getVoxelValue(Vec3ui(0, 0, 2)));
            ASSERT_FLOAT_EQ(3.0f, volume->getVoxelValue(Vec3ui(0, 0, 3)));

            ASSERT_FLOAT_EQ(0.0f, volume->getVoxelValue(Vec3ui(0, 0, 0)));
            ASSERT_FLOAT_EQ(2.0f, volume->getVoxelValue(Vec3ui(1, 1, 1)));
            ASSERT_FLOAT_EQ(4.0f, volume->getVoxelValue(Vec3ui(2, 2, 2)));
            ASSERT_FLOAT_EQ(6.0f, volume->getVoxelValue(Vec3ui(3, 3, 3)));
        }
    };
}

using namespace ettention;

TEST_F(VolumeDeserializerTest, Test_32BIT_MRC)
{
    auto volume = VolumeDeserializer::load(std::string(TESTDATA_DIR) + "/data/volume/volume_test_32bit.mrc", VoxelType::FLOAT_32);
    voidCheckPatternInVolume(volume);
    delete volume;
}

TEST_F(VolumeDeserializerTest, Test_32BIT_TIF)
{
    auto volume = VolumeDeserializer::load(std::string(TESTDATA_DIR) + "/data/volume/volume_test_16bit.mrc", VoxelType::FLOAT_32);
    voidCheckPatternInVolume(volume);
    delete volume;
}

TEST_F(VolumeDeserializerTest, Test_8BIT_MRC)
{
    auto volume = VolumeDeserializer::load(std::string(TESTDATA_DIR) + "/data/volume/volume_test_8bit.mrc", VoxelType::FLOAT_32);
    voidCheckPatternInVolume(volume);
    delete volume;
}
