#include "stdafx.h"
#include <framework/test/CLTestBase.h>
#include <io/serializer/VolumeSerializer.h>
#include <algorithm/volumemanipulation/SetToValueKernel.h>
#include <model/image/ImageComparator.h>
#include <model/volume/StackComparator.h>
#include <model/volume/GPUMappedVolume.h>
#include <gpu/GPUMapped.h>
#include <gpu/opencl/CLMemBuffer.h>

namespace ettention
{
    class OpenCLTest : public CLTestBase
    {
    public:
        OpenCLTest() : CLTestBase(HardwareParameterSet::nvidia, HardwareParameterSet::GPU_only)
        {
            testdataDirectory = std::string(TESTDATA_DIR) + "/data/";
            workDirectory = std::string(TESTDATA_DIR) + "/work/";
        }

        ~OpenCLTest()
        {
        }

        std::string testdataDirectory;
        std::string workDirectory;
    };
}

using namespace ettention;

TEST_F(OpenCLTest, TestMappedVolume_Subvolumes)
{
    Vec3ui volumeResolution;
    if(On64bitArchitecture())
    {
        volumeResolution = Vec3ui(2000, 2000, 200);
    }
    else
    {
        volumeResolution = Vec3ui(2000, 2000, 100);
    }
    Volume* volume = new Volume(volumeResolution);
    volume->Properties().SetSubVolumeCount(10);
    GPUMappedVolume* mappedVolume = new GPUMappedVolume(clal, volume);

    SetToValueKernel* kernel = new SetToValueKernel(framework, mappedVolume->getMappedSubVolume(), 1.0f);

    for(unsigned int i = 0; i < volume->Properties().GetSubVolumeCount(); i++)
    {
        mappedVolume->setCurrentSubvolumeIndex(i);
        mappedVolume->ensureIsUpToDateOnGPU();
        
        kernel->setOutputSubvolume(mappedVolume->getMappedSubVolume());
        kernel->run();

        mappedVolume->ensureIsUpToDateOnCPU();
    }

    Vec3i index;
    for(index.z = 0; index.z < (int) volume->Properties().GetVolumeResolution().z; index.z++)
        for(index.y = 0; index.y < (int) volume->Properties().GetVolumeResolution().y; index.y += 10)
            for(index.x = 0; index.x < (int) volume->Properties().GetVolumeResolution().x; index.x += 10)
                ASSERT_FLOAT_EQ(volume->getVoxelValue(index), 1.0f);

    delete kernel;
    delete mappedVolume;
    delete volume;
}

TEST_F(OpenCLTest, GPUMappedVolume)
{
    Vec3ui volumeResolution = Vec3ui(1000, 1000, 500);
    VolumeParameterSet options = VolumeParameterSet(volumeResolution);
    VolumeProperties properties = VolumeProperties(FLOAT_32, options, boost::none);
    Volume* volume = new Volume(properties, 0.0f);
    GPUMappedVolume* mappedVolume = new GPUMappedVolume(clal, volume);

    Image* imageA = new Image(Vec2ui(1000, 1000));
    GPUMapped<Image>* mappedImageA = new GPUMapped<Image>(clal, imageA);

    ASSERT_EQ(volume->Properties().GetSubVolumeCount(), 500);
    ASSERT_EQ(mappedVolume->getBufferOnGPU()->getByteWidth(), 4000000);

    mappedVolume->changeSubVolumeSizeToPartOfTotalMemory(1.0f);

    unsigned int subVolumeCount = volume->Properties().GetSubVolumeCount();
    
    ASSERT_TRUE(subVolumeCount < 1000);

    delete mappedImageA;
    delete imageA;

    delete volume;
    delete mappedVolume;
}