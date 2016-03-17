#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "framework/test/TestBase.h"
#include "model/volume/Voxel.h"
#include "model/volume/Volume.h"
#include "io/datasource/MRCHeader.h"
#include "io/serializer/MRCWriter.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"
#include "setup/parameterset/OutputParameterSet.h"

using namespace ettention;
using namespace boost::filesystem;

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

    void allocateVolume(Voxel::DataType voxelType)
    {
        volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);
        volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
        volume->setVoxelToValue(Vec3ui(15, 0, 0), 2.0f);
        volume->setVoxelToValue(Vec3ui(0, 15, 0), 3.0f);
        volume->setVoxelToValue(Vec3ui(0, 0, 15), 4.0f);
    }
};

TEST_F(MRCWriterTest, Test_Float32)
{
    this->allocateVolume(Voxel::DataType::FLOAT_32);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/float_32.mrc") , &format);
}

TEST_F(MRCWriterTest, Test_Unsigned_16)
{
    this->allocateVolume(Voxel::DataType::FLOAT_32);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_UNSIGNED_16, ORDER_XYZ);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/uint_16.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Signed_16)
{
    this->allocateVolume(Voxel::DataType::FLOAT_32);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_SIGNED_16, ORDER_XYZ);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/int_16.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Unsigned_8)
{
    this->allocateVolume(Voxel::DataType::FLOAT_32);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_UNSIGNED_8, ORDER_XYZ);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/uint_8.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Memory_Float16_Disk_Float32)
{
    this->allocateVolume(Voxel::DataType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/float_32_internal_halffloat_16.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Unsigned_16_Inverted)
{
    this->allocateVolume(Voxel::DataType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_UNSIGNED_16, ORDER_XYZ, true);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/uint16_inverted.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Float_32_Inverted)
{
    this->allocateVolume(Voxel::DataType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ, true);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/float_32_inverted.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_Float32_XZY)
{
    this->allocateVolume(Voxel::DataType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XZY, false);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/uint16_order_xzy.mrc"), &format);
}

TEST_F(MRCWriterTest, Test_UInt16_XZY)
{
    this->allocateVolume(Voxel::DataType::HALF_FLOAT_16);
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_UNSIGNED_16, ORDER_XZY, false);
    writer.write(volume, path(std::string(TESTDATA_DIR) + "/work/volume/float32_order_xzy.mrc"), &format);
}