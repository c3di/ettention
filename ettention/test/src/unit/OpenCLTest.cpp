#include "stdafx.h"

#include "algorithm/volumemanipulation/SetToValueKernel.h"
#include "io/serializer/VolumeSerializer.h"
#include "framework/test/TestBase.h"
#include "gpu/GPUMapped.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/volume/FloatVolume.h"
#include "setup/parameterset/HardwareParameterSet.h"

using namespace ettention;

class OpenCLTest : public TestBase
{

public:
    OpenCLTest()
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

TEST_F(OpenCLTest, TestMappedVolume_Subvolumes)
{
    Vec3ui volumeResolution;
    if(framework->on64bitArchitecture())
    {
        volumeResolution = Vec3ui(2000, 2000, 200);
    } else {
        volumeResolution = Vec3ui(2000, 2000, 100);
    }
    Volume* volume = new FloatVolume(volumeResolution, boost::none);
    volume->getProperties().setSubVolumeCount(10);
    GPUMappedVolume* mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);

    SetToValueKernel* kernel = new SetToValueKernel(framework, mappedVolume->getMappedSubVolume(), 1.0f);

    for(unsigned int i = 0; i < volume->getProperties().getSubVolumeCount(); i++)
    {
        mappedVolume->setCurrentSubvolumeIndex(i);
        mappedVolume->ensureIsUpToDateOnGPU();

        kernel->setOutputSubvolume(mappedVolume->getMappedSubVolume());
        kernel->run();

        mappedVolume->ensureIsUpToDateOnCPU();
    }

    Vec3i index;
    for(index.z = 0; index.z < (int)volume->getProperties().getVolumeResolution().z; index.z++)
    for(index.y = 0; index.y < (int)volume->getProperties().getVolumeResolution().y; index.y += 10)
    for(index.x = 0; index.x < (int)volume->getProperties().getVolumeResolution().x; index.x += 10)
        ASSERT_FLOAT_EQ(volume->getVoxelValue(index), 1.0f);

    delete kernel;
    delete mappedVolume;
    delete volume;
}

TEST_F(OpenCLTest, GPUMappedVolume)
{
    Vec3ui volumeResolution = Vec3ui(1000, 1000, 500);

    Volume* volume = new FloatVolume(volumeResolution, 0.0f, HardwareParameterSet::SUBVOLUME_COUNT_AUTO);
    GPUMappedVolume* mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);

    Image* imageA = new Image(Vec2ui(1000, 1000));
    GPUMapped<Image>* mappedImageA = new GPUMapped<Image>(framework->getOpenCLStack(), imageA);

    ASSERT_EQ(volume->getProperties().getSubVolumeCount(), 500);
    ASSERT_EQ(mappedVolume->getBufferOnGPU()->getByteWidth(), 4000000);

    mappedVolume->changeSubVolumeSizeToPartOfTotalMemory(1.0f);

    unsigned int subVolumeCount = volume->getProperties().getSubVolumeCount();

    ASSERT_TRUE(subVolumeCount < 1000);

    delete mappedImageA;
    delete imageA;

    delete volume;
    delete mappedVolume;
}

TEST_F(OpenCLTest, KernelArgumentParserTest)
{
    auto paramList = CLKernelParameterParser::KernelParameters();
    auto parser = CLKernelParameterParser( "__kernel void testKernel( TYPE_2D_READONLY(param_0_1), float * param_2 , TYPE_3D_READONLY(param_3_4), int param_5, TYPE_2D_MASK(param_6_7), FLOAT_TYPE(param_8), TYPE_3D_MASK(param_9_10), float last ) {}", "testKernel" );

    parser.storeParametersTo(paramList);

    ASSERT_EQ(paramList.getParameterCount(), 8);

    ASSERT_EQ(paramList.getParameterByName("param_0_1").number, 0);
    ASSERT_EQ(paramList.getParameterByName("param_2").number, 2);
    ASSERT_EQ(paramList.getParameterByName("param_3_4").number, 3);
    ASSERT_EQ(paramList.getParameterByName("param_5").number, 5);
    ASSERT_EQ(paramList.getParameterByName("param_6_7").number, 6);
    ASSERT_EQ(paramList.getParameterByName("param_8").number, 8);
    ASSERT_EQ(paramList.getParameterByName("param_9_10").number, 9);
    ASSERT_EQ(paramList.getParameterByName("last").number, 11);

    ASSERT_EQ(paramList.getParameterByNr(0).name, "param_0_1");
    ASSERT_EQ(paramList.getParameterByNr(2).name, "param_2");
    ASSERT_EQ(paramList.getParameterByNr(3).name, "param_3_4");
    ASSERT_EQ(paramList.getParameterByNr(5).name, "param_5");
    ASSERT_EQ(paramList.getParameterByNr(6).name, "param_6_7");
    ASSERT_EQ(paramList.getParameterByNr(8).name, "param_8");
    ASSERT_EQ(paramList.getParameterByNr(9).name, "param_9_10");
    ASSERT_EQ(paramList.getParameterByNr(11).name, "last");
}