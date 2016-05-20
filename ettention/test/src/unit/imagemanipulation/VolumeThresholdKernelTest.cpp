#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "model/volume/Volume.h"
#include "model/volume/FloatVolume.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "model/volume/StackComparator.h"
#include "algorithm/volumemanipulation/VolumeThresholdOperator.h"

using namespace ettention;

class VolumeThresholdKernelTest : public TestBase
{
public:
    VolumeThresholdKernelTest()
        : TestBase(std::string(TESTDATA_DIR"/data/synthetic_3_parallel/"), std::string(TESTDATA_DIR"/data/synthetic_3_parallel/"), std::string(TESTDATA_DIR"/work/synthetic/"))
    {
    }

    virtual void applySingleLevelThresholding(std::string inFileVolume, std::string outFile, float floorLevel, unsigned int subVolumeCount = 1)
    {
        GPUMappedVolume inVolume(framework->getOpenCLStack(), VolumeDeserializer::load(inFileVolume, Voxel::DataType::FLOAT_32, subVolumeCount));
        inVolume.takeOwnershipOfObjectOnCPU();

        VolumeThresholdOperator gpuKernel(framework, &inVolume, floorLevel);
        gpuKernel.run();

        MRCWriter writer;
        const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ);
        gpuKernel.getOutput()->ensureIsUpToDateOnCPU();
        writer.write(gpuKernel.getOutput()->getObjectOnCPU(), outFile, &format);
    }

    virtual void applyMultiLevelThresholding( std::string inFileVolume
                                            , std::string outFile
                                            , std::vector<float> *floorLevels
                                            , std::vector<float> *floorValues
                                            , unsigned int subVolumeCount = 1
                                            )
    {
        GPUMappedVolume inVolume(framework->getOpenCLStack(), VolumeDeserializer::load(inFileVolume, Voxel::DataType::FLOAT_32, subVolumeCount));
        inVolume.takeOwnershipOfObjectOnCPU();

        VolumeThresholdOperator gpuKernel(framework, &inVolume, floorLevels, floorValues);
        gpuKernel.run();

        MRCWriter writer;
        const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ);
        gpuKernel.getOutput()->ensureIsUpToDateOnCPU();
        writer.write(gpuKernel.getOutput()->getObjectOnCPU(), outFile, &format);
    }
};

TEST_F(VolumeThresholdKernelTest, DISABLED_SuspiciousSubvolumeCountTest)
{
    float floorValue = 0.0626f;

    std::string sourceName = "synt_blurred.mrc";
    std::string resultName = "synt_blurred_threshed_00626.mrc";

    std::cout << "38: ";
    applySingleLevelThresholding(pathToData + sourceName, pathToOutput + resultName, floorValue, 38);
    ettention::StackComparator::assertVolumesAreEqual(pathToInput + resultName, pathToOutput + resultName);
    std::cout << " Good!" << std::endl;

    std::cout << "39: ";
    applySingleLevelThresholding(pathToData + sourceName, pathToOutput + resultName, floorValue, 39);
    ettention::StackComparator::assertVolumesAreEqual(pathToInput + resultName, pathToOutput + resultName);
    std::cout << " GREAT!" << std::endl;

    std::cout << "64: ";
    applySingleLevelThresholding(pathToData + sourceName, pathToOutput + resultName, floorValue, 64);
    ettention::StackComparator::assertVolumesAreEqual(pathToInput + resultName, pathToOutput + resultName);
    std::cout << " EXCELLENT!" << std::endl;
}

TEST_F(VolumeThresholdKernelTest, SingleLevelThreshold)
{
    float floorValue = 0.0626f;

    std::string sourceName = "synt_blurred.mrc";
    std::string resultName = "synt_blurred_threshed_00626.mrc";

    applySingleLevelThresholding(pathToData + sourceName, pathToOutput + resultName, floorValue);
    ettention::StackComparator::assertVolumesAreEqual(pathToInput + resultName, pathToOutput + resultName);
}

TEST_F(VolumeThresholdKernelTest, MultiLevelThreshold)
{
    std::vector<float> levels{0.0626f, 0.2f, 1.0f};
    std::vector<float> values{0.0f, 0.5f, 0.9f};

    std::string sourceName = "synt_blurred.mrc";
    std::string resultName = "synt_blurred_threshed_00626_02000_10000.mrc";

    applyMultiLevelThresholding(pathToData + sourceName, pathToOutput + resultName, &levels, &values);
    ettention::StackComparator::assertVolumesAreEqual(pathToInput + resultName, pathToOutput + resultName);
}

TEST_F(VolumeThresholdKernelTest, DISABLED_MultiLevelThreshold_Phantom)
{
    std::vector<float> levels{0.0f, 0.000002f, 0.01f, 0.25f, 0.65f, 1.00f};
    std::vector<float> values{0.0f, 0.0f, 0.1f, 0.2f, 0.3f, 1.0f};

    std::string sourceName = "shepp_logan_512_4.mrc";
    std::string resultName = "shepp_logan_512_threshed_00_00_01_02_03_10.mrc";

    applyMultiLevelThresholding(std::string(TESTDATA_DIR) + "/data/discrete_reconstruction/" + sourceName, std::string(TESTDATA_DIR) + "/work/discrete_reconstruction/" + resultName, &levels, &values);
    //ettention::StackComparator::assertVolumesAreEqual(std::string(TESTDATA_DIR) + "/data/discrete_reconstruction/" + resultName, std::string(TESTDATA_DIR) + "/work/discrete_reconstruction/" + resultName);
}