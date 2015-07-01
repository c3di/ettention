#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "framework/test/TestBase.h"
#include "model/volume/VoxelType.h"
#include "model/volume/Volume.h"
#include "io/datasource/MRCHeader.h"
#include "io/serializer/MRCWriter.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"

namespace ettention
{
    class MRCWriterTest : public TestBase
    {
    public:
        Volume* volume;

        MRCWriterTest()
            : volume(0)
        {
        }

        ~MRCWriterTest()
        {
        }

        void TearDown() override
        {
            delete volume;
            TestBase::TearDown();
        }

        void allocateVolume(VoxelType voxelType)
        {
            VolumeProperties properties(voxelType, VolumeParameterSet(Vec3ui(16, 16, 16)), 1);
            volume = new Volume(properties, 0.0f);
            volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
            volume->setVoxelToValue(Vec3ui(15, 0, 0), 2.0f);
            volume->setVoxelToValue(Vec3ui(0, 15, 0), 3.0f);
            volume->setVoxelToValue(Vec3ui(0, 0, 15), 4.0f);
        }
    };
}

using namespace ettention;

TEST_F(MRCWriterTest, Test_Float32)
{
    this->allocateVolume(VoxelType::FLOAT_32);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32, OutputFormatParameterSet::ORDER_XYZ);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/float_32.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Unsigned_16)
{
    this->allocateVolume(VoxelType::FLOAT_32);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::UNSIGNED_16, OutputFormatParameterSet::ORDER_XYZ);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/uint_16.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Signed_16)
{
    this->allocateVolume(VoxelType::FLOAT_32);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::SIGNED_16, OutputFormatParameterSet::ORDER_XYZ);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/int_16.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Unsigned_8)
{
    this->allocateVolume(VoxelType::FLOAT_32);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::UNSIGNED_8, OutputFormatParameterSet::ORDER_XYZ);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/uint_8.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Memory_Float16_Disk_Float32)
{
    this->allocateVolume(VoxelType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32, OutputFormatParameterSet::ORDER_XYZ);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/float_32_internal_halffloat_16.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Unsigned_16_Inverted)
{
    this->allocateVolume(VoxelType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::UNSIGNED_16, OutputFormatParameterSet::ORDER_XYZ, true);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/uint16_inverted.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Float_32_Inverted)
{
    this->allocateVolume(VoxelType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32, OutputFormatParameterSet::ORDER_XYZ, true);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/float_32_inverted.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Float32_XZY)
{
    this->allocateVolume(VoxelType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32, OutputFormatParameterSet::ORDER_XZY, false);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/uint16_order_xzy.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_UInt16_XZY)
{
    this->allocateVolume(VoxelType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputFormatParameterSet format(OutputFormatParameterSet::UNSIGNED_16, OutputFormatParameterSet::ORDER_XZY, false);
    writer.write(volume, boost::filesystem::path(std::string(TESTDATA_DIR) + "/work/volume/float32_order_xzy.mrc"), &format);
}