#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/deserializer/ImageDeserializer.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "framework/math/Float16Compressor.h"
#include "model/volume/Volume.h"
#include "model/volume/FloatVolume.h"
#include "model/volume/HalfFloatVolume.h"
#include "model/volume/ByteVolume.h"
#include "model/volume/VolumeProperties.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"
#include "model/volume/Voxel.h"
#include "framework/test/TestBase.h"

namespace ettention
{

    class VolumeTest : public ettention::TestBase
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

            size_t test = volume->getProperties().getVolumeVoxelCount();

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

            for(unsigned int y = 1; y < volume->getProperties().getVolumeResolution().y; y += 2)
            {
                float expectedValueFromImage = (float)((y + 1) / 2);
                ASSERT_FLOAT_EQ(expectedValueFromImage, volume->getVoxelValue(Vec3ui(y, y, 2)));
            }
        }

        size_t distanceInBytesBetweenPointers(void* a, void* b) 
        {
            return (unsigned char*)b - (unsigned char*)a;
        }
    };

    TEST_F(VolumeTest, Test_Instance_FloatVolume)
    {
        FloatVolume *volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);
        Test_Voxel_Access(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_Instance_HalfFloatVolume)
    {
        HalfFloatVolume *volume = new HalfFloatVolume(Vec3ui(16, 16, 16), 0.0f);
        Test_Voxel_Access(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_Instance_ByteVolume)
    {
        ByteVolume *volume = new ByteVolume(Vec3ui(16, 16, 16), 0.0f);
        Test_Voxel_Access(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_Voxel_Access_Float32)
    {
        auto* volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);
        Test_Voxel_Access(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_Voxel_Access_HalfFloat16)
    {
        auto* volume = new HalfFloatVolume(Vec3ui(16, 16, 16), 0.0f);
        Test_Voxel_Access(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_Voxel_Access_UChar8)
    {
        auto* volume = new ByteVolume(Vec3ui(16, 16, 16), 0.0f);
        Test_Voxel_Access(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_MinMaxValues_Float32)
    {
        Volume* volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);
        TestComputeMinMaxValues(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_MinMaxValues_HalfFloat16)
    {
        Volume* volume = new HalfFloatVolume(Vec3ui(16, 16, 16), 0.0f);
        TestComputeMinMaxValues(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_MinMaxValues_UChar8)
    {
        Volume* volume = new ByteVolume(Vec3ui(16, 16, 16), 0.0f);
        TestComputeMinMaxValues(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_SetZSlice_Float32)
    {
        auto* volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);
        TestSetZSlide(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_SetZSlice_HalfFloat16)
    {
        auto* volume = new HalfFloatVolume(Vec3ui(16, 16, 16), 0.0f);
        TestSetZSlide(volume);
        delete volume;
    }

    TEST_F(VolumeTest, Test_SetZSlice_UChar8)
    {
        auto* volume = new ByteVolume(Vec3ui(16, 16, 16), 0.0f);
        TestSetZSlide(volume);
        delete volume;
    }

    TEST_F(VolumeTest, UpdateRegion)
    {
        Volume *volumeOnCPU = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);
        auto volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCPU);
        Vec3ui subVolumeBase(4, 5, 6);
        Vec3ui subVolumeRes(8, 6, 4);
        Vec3ui updateRegionBase(3, 3, 3);
        Vec3ui updateRegionRes(6, 6, 6);

        volume->setArbitrarySubVolume(subVolumeBase, subVolumeRes);
        volume->ensureIsUpToDateOnGPU();

        // set voxels in region to some values
        for(unsigned int z = 0; z < updateRegionRes.z; ++z)
        {
            for(unsigned int y = 0; y < updateRegionRes.y; ++y)
            {
                for(unsigned int x = 0; x < updateRegionRes.x; ++x)
                {
                    Vec3ui coords = updateRegionBase + Vec3ui(x, y, z);
                    volumeOnCPU->setVoxelToValue(coords, -(float)(z * updateRegionRes.y * updateRegionRes.x + y * updateRegionRes.x + x));
                }
            }
        }

        volume->updateRegionOnGPU(updateRegionBase, updateRegionRes);
        for(unsigned int z = 0; z < updateRegionRes.z; ++z)
        {
            for(unsigned int y = 0; y < updateRegionRes.y; ++y)
            {
                for(unsigned int x = 0; x < updateRegionRes.x; ++x)
                {
                    Vec3ui coords = updateRegionBase + Vec3ui(x, y, z);
                    volume->getObjectOnCPU()->setVoxelToValue(coords, 0.0f);
                }
            }
        }
        volume->markAsChangedOnGPU();
        volume->ensureIsUpToDateOnCPU();

        // now every voxel should be 0 except for the voxels in the updateregion intersected with the subvolume
        for(unsigned int z = 0; z < volumeOnCPU->getProperties().getVolumeResolution().z; ++z)
        {
            for(unsigned int y = 0; y < volumeOnCPU->getProperties().getVolumeResolution().y; ++y)
            {
                for(unsigned int x = 0; x < volumeOnCPU->getProperties().getVolumeResolution().x; ++x)
                {
                    Vec3ui coords(x, y, z);
                    float value = volumeOnCPU->getVoxelValue(coords);
                    if(coords >= updateRegionBase && coords < updateRegionBase + updateRegionRes && coords >= subVolumeBase && coords < subVolumeBase + subVolumeRes)
                    {
                        Vec3ui coordsInRegion = Vec3ui(x, y, z) - updateRegionBase;
                        float expectedValue = -(float)(coordsInRegion.z * updateRegionRes.y * updateRegionRes.x + coordsInRegion.y * updateRegionRes.x + coordsInRegion.x);
                        ASSERT_EQ(value, expectedValue);
                    }
                    else
                    {
                        ASSERT_EQ(value, 0.0f);
                    }
                }
            }
        }

        /*TEST_F(VolumeTest, DISABLED_Test_Voxel_Raw_Representation_Float32)
        {
        Volume* volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);

        ASSERT_EQ(volume->computeDistanceBetweenVoxelsInMemory(0, 1), 4);

        volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
        volume->setVoxelToValue(Vec3ui(15, 15, 15), 1.0f);

        float* ptr = NULL;

        ptr = (float*)volume->getRawPointerOfVoxel(Vec3ui(0, 0, 0));
        ASSERT_FLOAT_EQ(*ptr, 1.0f);

        ptr = (float*)volume->getRawPointerOfVoxel(Vec3ui(15, 15, 15));
        ASSERT_FLOAT_EQ(*ptr, 1.0f);

        ptr = (float*)volume->getRawPointerOfVoxel(Vec3ui(1, 0, 0));
        ASSERT_FLOAT_EQ(*ptr, 0.0f);
        ptr = (float*)volume->getRawPointerOfVoxel(Vec3ui(0, 1, 0));
        ASSERT_FLOAT_EQ(*ptr, 0.0f);
        ptr = (float*)volume->getRawPointerOfVoxel(Vec3ui(0, 0, 1));
        ASSERT_FLOAT_EQ(*ptr, 0.0f);

        ASSERT_THROW(volume->getRawPointerOfVoxel(Vec3ui(16, 0, 0)), Exception);
        ASSERT_THROW(volume->getRawPointerOfVoxel(Vec3ui(0, 16, 0)), Exception);
        ASSERT_THROW(volume->getRawPointerOfVoxel(Vec3ui(0, 0, 16)), Exception);

        delete volume;
        }

        TEST_F(VolumeTest, DISABLED_Test_Voxel_Raw_Representation_HalfFloat16)
        {
        Volume* volume = new FloatVolume(Vec3ui(16, 16, 16), 0.0f);

        ASSERT_EQ(volume->computeDistanceBetweenVoxelsInMemory(0, 1), 2);

        volume->setVoxelToValue(Vec3ui(0, 0, 0), 1.0f);
        volume->setVoxelToValue(Vec3ui(15, 15, 15), 2.0f);

        unsigned short* ptr = NULL;

        ptr = (unsigned short*)volume->getRawPointerOfVoxel(Vec3ui(0, 0, 0));
        ASSERT_FLOAT_EQ(Float16Compressor::decompress(*ptr), 1.0f);

        ptr = (unsigned short*)volume->getRawPointerOfVoxel(Vec3ui(15, 15, 15));
        ASSERT_FLOAT_EQ(Float16Compressor::decompress(*ptr), 2.0f);

        ptr = (unsigned short*)volume->getRawPointerOfVoxel(Vec3ui(1, 0, 0));
        ASSERT_FLOAT_EQ(Float16Compressor::decompress(*ptr), 0.0f);

        ASSERT_THROW(volume->getRawPointerOfVoxel(Vec3ui(16, 0, 0)), Exception);
        ASSERT_THROW(volume->getRawPointerOfVoxel(Vec3ui(0, 16, 0)), Exception);
        ASSERT_THROW(volume->getRawPointerOfVoxel(Vec3ui(0, 0, 16)), Exception);

        delete volume;
        }*/

        delete volumeOnCPU;
        delete volume;
    }
}