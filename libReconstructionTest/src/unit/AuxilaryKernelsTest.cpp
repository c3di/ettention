#include "stdafx.h"
#include "framework/test/ImageManipulationTestBase.h"
#include "algorithm/imagemanipulation/SquareKernel.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"
#include "algorithm/imagestatistics/MeanOperator.h"
#include "algorithm/imagestatistics/GlobalMaximumKernel.h"
#include "algorithm/imagestatistics/OtsuThresholdDecider.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSpiralKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "model/image/ImageComparator.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/parametersource/ManualParameterSource.h"

namespace ettention
{
    class AuxilaryKernelsTest : public ImageManipulationTestBase
    {
    public:
        AuxilaryKernelsTest()
            : ImageManipulationTestBase(std::string("AUX"), std::string(TESTDATA_DIR"/data/alignment/aux_kernels"), std::string(TESTDATA_DIR"/work/alignment/aux_kernels"))
        {
        }
    };
}

using namespace ettention;

TEST_F(AuxilaryKernelsTest, AuxKernels_Debug_Run_Mean)
{
    std::string input_src("meanTest_halfBlack_size12_mean05");

    auto *buffer = this->loadImageFileToGPU(input_src);
    auto *kernel = new MeanOperator(framework, buffer);

    kernel->computeMean();
    auto result = kernel->getMean();

    delete kernel;
    delete buffer;

    ASSERT_NEAR(result, 0.5f, 0.0001f);
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Square)
{
    std::string input_src("squareTest_pyramid_size13_source");
    std::string input_res("squareTest_pyramid_size13_answer");
    std::string output_src("squareTest_pyramid_size13_source");
    std::string output_res("squareTest_pyramid_size13_result");

    auto *buffer = this->loadImageFileToGPU(input_src);
    auto *kernel = new SquareKernel(framework, buffer);
    kernel->run();

    auto input = kernel->getInput();
    this->saveImageAsFile(output_src, input);

    auto result = kernel->getResult();
    this->saveImageAsFile(output_res, result);
    
    delete kernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_src), this->makeOutputFilePath(output_src));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res), this->makeOutputFilePath(output_res));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Max)
{
    std::string input_src("maxTest_pyramid_size13_maxVal10_maxLocCenter");

    auto *buffer = this->loadImageFileToGPU(input_src);
    auto *kernel = new GlobalMaximumKernel(framework, buffer);

    kernel->run();
    float resultValue = kernel->getMaximum();
    Vec2ui resultPosition = kernel->getMaximumPosition();

    delete kernel;
    delete buffer;

    EXPECT_EQ(6, resultPosition.x);                    // Center of 13x13 square starting from [0, 0] is [6, 6]
    EXPECT_EQ(6, resultPosition.y);
    ASSERT_NEAR(resultValue, 1.0f, 0.0001f);
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Threshold)
{
    std::string input_src("thresholdTest_pyramid_size13_source");
    std::string output_res10("thresholdTest_pyramid_size13_threshVal10_result");
    std::string output_res05("thresholdTest_pyramid_size13_threshVal05_result");

    auto *buffer = this->loadImageFileToGPU(input_src);
    auto *kernel = new ThresholdKernel(framework, buffer);

    kernel->setThreshold(1.0f);
    kernel->run();
    auto result = kernel->getResult();
    this->saveImageAsFile(output_res10, result);

    kernel->setThreshold(0.5f);
    kernel->run();
    result = kernel->getResult();
    this->saveImageAsFile(output_res05, result);

    delete kernel;
    delete buffer;
}

TEST_F(AuxilaryKernelsTest, AuxKernels_LocalMax)
{
    std::string input_src("localMaxTest_4pyramids");
    std::string input_res("localMaxTest_4pyramids_answer");
    std::string output_src("localMaxTest_4pyramids_source");
    std::string output_res("localMaxTest_4pyramids_result");

    auto *buffer = this->loadImageFileToGPU(input_src);
    LocalMaximumSimpleKernel *kernel = new LocalMaximumSimpleKernel(framework, buffer, Vec2ui(12, 12));
    kernel->run();

    auto input = kernel->getSource();
    this->saveImageAsFile(output_src, input);
    
    auto result = kernel->getResult();
    this->saveImageAsFile(output_res, result);

    delete kernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_src), this->makeOutputFilePath(output_src));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res), this->makeOutputFilePath(output_res));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_LocalMax_CompareImplementations)
{
    std::string input_src_real("localMaxTest_real");
    std::string output_simpl_real("localMaxTest_real_simple");
    std::string output_spirl_real("localMaxTest_real_spiral");

    auto *bufferR = this->loadImageFileToGPU(input_src_real);
    
    LocalMaximumSimpleKernel *kernel1 = new LocalMaximumSimpleKernel(framework, bufferR, Vec2ui(12, 12));
    kernel1->run();
    auto result = kernel1->getResult();
    this->saveImageAsFile(output_simpl_real, result);
    delete kernel1;
    
    LocalMaximumSpiralKernel *kernel2 = new LocalMaximumSpiralKernel(framework, bufferR, Vec2ui(12, 12));
    kernel2->run();
    result = kernel2->getResult();
    this->saveImageAsFile(output_spirl_real, result);
    delete kernel2;

    delete bufferR;

    ImageComparator::assertImagesAreEqual(this->makeOutputFilePath(output_simpl_real), this->makeOutputFilePath(output_spirl_real));
}