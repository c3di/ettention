#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "framework/math/Float16Compressor.h"
#include "model/volume/Volume.h"
#include "model/volume/VolumeProperties.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"
#include "model/volume/VoxelType.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class VolumeTest : public TestBase
    {
    public:

        VolumeTest()
        {
        }

        virtual ~VolumeTest()
        {
        }

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

        void TestComputeMinMaxValues(Volume* volume)
        {
            volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
            volume->setVoxelToValue(Vec3ui(15, 15, 15), 121.0f);
            volume->setVoxelToValue(Vec3ui(15, 15, 15), 2.0f);

            size_t test = volume->Properties().GetVolumeVoxelCount();

            auto volumeStats = VolumeStatistics::compute(volume);
            ASSERT_FLOAT_EQ(volumeStats.getMin(), 0.0f);
            ASSERT_FLOAT_EQ(volumeStats.getMax(), 2.0f);
            ASSERT_FLOAT_EQ(volumeStats.getMean(), 0.000732421875f);
        }

        void TestSetZSlide(Volume* volume)
        {
            Image* image = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/volume/striped_slice.tif");
            volume->setZSlice(2, image);
            delete image;

            for(unsigned int y = 1; y < 16; y += 2)
            {
                float expectedValueFromImage = (float)((y + 1) / 2);
                ASSERT_FLOAT_EQ(volume->getVoxelValue(Vec3ui(y, y, 2)), expectedValueFromImage);
            }
        }

        size_t distanceInBytesBetweenPointers(void* a, void* b) 
        {
            return (unsigned char*)b - (unsigned char*)a;
        }
    };
}

using namespace ettention;

TEST_F(VolumeTest, Test_Voxel_Access_Float32)
{
    VolumeProperties properties(VoxelType::FLOAT_32, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    Test_Voxel_Access(volume);

    delete volume;
}

TEST_F(VolumeTest, Test_Voxel_Access_HalfFloat16)
{
    VolumeProperties properties(VoxelType::HALF_FLOAT_16, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    Test_Voxel_Access(volume);

    delete volume;
}

TEST_F(VolumeTest, Test_Voxel_Raw_Representation_Float32)
{
    VolumeProperties properties(VoxelType::FLOAT_32, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    ASSERT_EQ(distanceInBytesBetweenPointers(volume->getRawDataOfOfVoxel(0), volume->getRawDataOfOfVoxel(1)), 4);

    volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
    volume->setVoxelToValue(Vec3ui(15, 15, 15), 1.0f);

    float* ptr = NULL;

    ptr = (float*)volume->getRawDataOfOfVoxel(Vec3ui(0, 0, 0));
    ASSERT_FLOAT_EQ(*ptr, 1.0f);

    ptr = (float*)volume->getRawDataOfOfVoxel(Vec3ui(15, 15, 15));
    ASSERT_FLOAT_EQ(*ptr, 1.0f);

    ptr = (float*)volume->getRawDataOfOfVoxel(Vec3ui(1, 0, 0));
    ASSERT_FLOAT_EQ(*ptr, 0.0f);
    ptr = (float*)volume->getRawDataOfOfVoxel(Vec3ui(0, 1, 0));
    ASSERT_FLOAT_EQ(*ptr, 0.0f);
    ptr = (float*)volume->getRawDataOfOfVoxel(Vec3ui(0, 0, 1));
    ASSERT_FLOAT_EQ(*ptr, 0.0f);

    ASSERT_THROW(volume->getRawDataOfOfVoxel(Vec3ui(16, 0, 0)), Exception);
    ASSERT_THROW(volume->getRawDataOfOfVoxel(Vec3ui(0, 16, 0)), Exception);
    ASSERT_THROW(volume->getRawDataOfOfVoxel(Vec3ui(0, 0, 16)), Exception);

    delete volume;
}

TEST_F(VolumeTest, Test_Voxel_Raw_Representation_HalfFloat16)
{
    VolumeProperties properties(VoxelType::HALF_FLOAT_16, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    ASSERT_EQ(distanceInBytesBetweenPointers(volume->getRawDataOfOfVoxel(0), volume->getRawDataOfOfVoxel(1)), 2);

    volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
    volume->setVoxelToValue(Vec3ui(15, 15, 15), 2.0f);

    unsigned short* ptr = NULL;

    ptr = (unsigned short*)volume->getRawDataOfOfVoxel(Vec3ui(0, 0, 0));
    ASSERT_FLOAT_EQ(Float16Compressor::decompress(*ptr), 1.0f);

    ptr = (unsigned short*)volume->getRawDataOfOfVoxel(Vec3ui(15, 15, 15));
    ASSERT_FLOAT_EQ(Float16Compressor::decompress(*ptr), 2.0f);

    ptr = (unsigned short*)volume->getRawDataOfOfVoxel(Vec3ui(1, 0, 0));
    ASSERT_FLOAT_EQ(Float16Compressor::decompress(*ptr), 0.0f);

    ASSERT_THROW(volume->getRawDataOfOfVoxel(Vec3ui(16, 0, 0)), Exception);
    ASSERT_THROW(volume->getRawDataOfOfVoxel(Vec3ui(0, 16, 0)), Exception);
    ASSERT_THROW(volume->getRawDataOfOfVoxel(Vec3ui(0, 0, 16)), Exception);

    delete volume;
}

TEST_F(VolumeTest, Test_MinMaxValues_Float32)
{
    VolumeProperties properties(VoxelType::FLOAT_32, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    TestComputeMinMaxValues(volume);

    delete volume;
}

TEST_F(VolumeTest, Test_MinMaxValues_HalfFloat16)
{
    VolumeProperties properties(VoxelType::HALF_FLOAT_16, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    TestComputeMinMaxValues(volume);

    delete volume;
}

TEST_F(VolumeTest, Test_SetZSlice_Float32)
{
    VolumeProperties properties(VoxelType::FLOAT_32, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    TestSetZSlide(volume);

    delete volume;
}

TEST_F(VolumeTest, Test_SetZSlice_HalfFloat16)
{
    VolumeProperties properties(VoxelType::HALF_FLOAT_16, VolumeParameterSet(Vec3ui(16, 16, 16)), boost::none);
    Volume* volume = new Volume(properties, 0.0f);

    TestSetZSlide(volume);

    delete volume;
}