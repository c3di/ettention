#include "stdafx.h"
#include "framework/test/ImageManipulationTestBase.h"
#include "algorithm/imagemanipulation/BorderSegmentKernel.h"
#include "algorithm/imagemanipulation/SquareKernel.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"
#include "algorithm/imagemanipulation/morphology/DilationKernel.h"
#include "algorithm/imagemanipulation/morphology/ErosionKernel.h"
#include "algorithm/imagemanipulation/morphology/StructuringElementGenerator.h"
#include "algorithm/imagestatistics/MeanOperator.h"
#include "algorithm/imagestatistics/globalmaximum/GlobalMaximumPositionImageKernel.h"
#include "algorithm/imagestatistics/globalminimum/GlobalMinimumPositionImageKernel.h"
#include "algorithm/imagestatistics/OtsuThresholdDecider.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSpiralKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "model/image/ImageComparator.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/parametersource/ManualParameterSource.h"

using namespace ettention;

class AuxilaryKernelsTest : public ImageManipulationTestBase
{
public:
    AuxilaryKernelsTest()
        : ImageManipulationTestBase(std::string("AUX"), std::string(TESTDATA_DIR"/data/alignment/aux_kernels"), std::string(TESTDATA_DIR"/work/alignment/aux_kernels"))
    {
    }
};

//
// Various Image Manipulation Kernels Test
//
TEST_F(AuxilaryKernelsTest, AuxKernels_Mean)
{
    std::string input_src("meanTest_halfBlack_size12_mean05");

    auto *buffer = loadInputImageFileToGPU(input_src);
    auto *auxkernel = new MeanOperator(framework, buffer);

    auxkernel->computeMean();
    auto result = auxkernel->getMean();

    delete auxkernel;
    delete buffer;

    ASSERT_NEAR(result, 0.5f, 0.0001f);
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Square)
{
    std::string input_src ("squareTest_pyramid_size13_source");
    std::string input_res ("squareTest_pyramid_size13_answer");
    std::string output_res("squareTest_pyramid_size13_result");

    auto *buffer = loadInputImageFileToGPU(input_src);
    auto *auxkernel = new SquareKernel(framework, buffer);
    auxkernel->run();

    auto result = auxkernel->getOutput();
    this->saveImageAsFile(output_res, result);

    auto input = auxkernel->getInput();
    this->saveImageAsFile(input_src, input);
    
    delete auxkernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(makeInputFilePath(input_src), makeOutputFilePath(input_src));
    ImageComparator::assertImagesAreEqual(makeInputFilePath(input_res), makeOutputFilePath(output_res));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Min)
{
    std::string input_src("minTest_pyramid_size13_minVal01_minLocCenter");

    auto *buffer = loadInputImageFileToGPU(input_src);
    auto *auxkernel = new GlobalMinimumPositionImageKernel(framework, buffer);

    auxkernel->run();
    float resultValue = auxkernel->getMinimumValue();
    Vec2ui resultPosition = auxkernel->getMinimumXY();

    delete auxkernel;
    delete buffer;

    EXPECT_EQ(6, resultPosition.x);                    // Center of 13x13 square starting from [0, 0] is [6, 6]
    EXPECT_EQ(6, resultPosition.y);
    ASSERT_NEAR(resultValue, 0.1f, 0.0001f);
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Max)
{
    std::string input_src("maxTest_pyramid_size13_maxVal10_maxLocCenter");

    auto *buffer = loadInputImageFileToGPU(input_src);
    auto *auxkernel = new GlobalMaximumPositionImageKernel(framework, buffer);

    auxkernel->run();
    float resultValue = auxkernel->getMaximumValue();
    Vec2ui resultPosition = auxkernel->getMaximumXY();

    delete auxkernel;
    delete buffer;

    EXPECT_EQ(6, resultPosition.x);                    // Center of 13x13 square starting from [0, 0] is [6, 6]
    EXPECT_EQ(6, resultPosition.y);
    ASSERT_NEAR(resultValue, 1.0f, 0.0001f);
}

TEST_F(AuxilaryKernelsTest, AuxKernels_Threshold)
{
    std::string input_src   ("thresholdTest_pyramid_size13_source");
    std::string input_res_10("thresholdTest_pyramid_size13_10_answer");
    std::string input_res_05("thresholdTest_pyramid_size13_05_answer");
    std::string output_res10("thresholdTest_pyramid_size13_10_result");
    std::string output_res05("thresholdTest_pyramid_size13_05_result");

    auto *buffer = loadInputImageFileToGPU(input_src);
    auto *auxkernel = new ThresholdKernel(framework, buffer);

    auxkernel->setThreshold(1.0f);
    auxkernel->run();
    auto result = auxkernel->getOutput();
    this->saveImageAsFile(output_res10, result);

    auxkernel->setThreshold(0.5f);
    auxkernel->run();
    result = auxkernel->getOutput();
    this->saveImageAsFile(output_res05, result);

    auto input = auxkernel->getInput();
    this->saveImageAsFile(input_src, input);

    delete auxkernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(makeInputFilePath(input_src),    makeOutputFilePath(input_src));
    ImageComparator::assertImagesAreEqual(makeInputFilePath(input_res_10), makeOutputFilePath(output_res10));
    ImageComparator::assertImagesAreEqual(makeInputFilePath(input_res_05), makeOutputFilePath(output_res05));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_LocalMax)
{
    std::string input_src ("localMaxTest_4pyramids_source");
    std::string input_res ("localMaxTest_4pyramids_answer");
    std::string output_res("localMaxTest_4pyramids_result");

    auto *buffer = this->loadInputImageFileToGPU(input_src);
    LocalMaximumSimpleKernel *auxkernel = new LocalMaximumSimpleKernel(framework, buffer, Vec2ui(12, 12));
    auxkernel->run();
    
    auto result = auxkernel->getOutput();
    this->saveImageAsFile(output_res, result);

    auto input = auxkernel->getSource();
    this->saveImageAsFile(input_src, input);

    delete auxkernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_src), this->makeOutputFilePath(input_src));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res), this->makeOutputFilePath(output_res));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_LocalMax_CompareImplementations)
{
    std::string input_src_real   ("localMaxTest_real");
    std::string output_simpl_real("localMaxTest_real_simple");
    std::string output_spirl_real("localMaxTest_real_spiral");

    auto *bufferR = this->loadInputImageFileToGPU(input_src_real);
    
    LocalMaximumSimpleKernel *kernel1 = new LocalMaximumSimpleKernel(framework, bufferR, Vec2ui(12, 12));
    kernel1->run();
    auto result = kernel1->getOutput();
    this->saveImageAsFile(output_simpl_real, result);
    delete kernel1;
    
    LocalMaximumSpiralKernel *kernel2 = new LocalMaximumSpiralKernel(framework, bufferR, Vec2ui(12, 12));
    kernel2->run();
    result = kernel2->getOutput();
    this->saveImageAsFile(output_spirl_real, result);
    delete kernel2;

    delete bufferR;

    ImageComparator::assertImagesAreEqual(this->makeOutputFilePath(output_simpl_real), this->makeOutputFilePath(output_spirl_real));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_BorderSegment)
{
    std::string input_src("borderTest_pyramid_size13_source");
    std::string input_res_4("borderTest_pyramid_size13_4_answer");
    std::string input_res_8("borderTest_pyramid_size13_8_answer");
    std::string output_res_4("borderTest_pyramid_size13_4_result");
    std::string output_res_8("borderTest_pyramid_size13_8_result");

    auto *buffer = this->loadInputImageFileToGPU(input_src);
    auto *auxkernel = new BorderSegmentKernel(framework, buffer);

    auxkernel->setUsingEightNeighbors(false);
    auxkernel->run();
    auto result = auxkernel->getOutput();
    this->saveImageAsFile(output_res_4, result);

    auxkernel->setUsingEightNeighbors(true);
    auxkernel->run();
    result = auxkernel->getOutput();
    this->saveImageAsFile(output_res_8, result);

    auto input = auxkernel->getInput();
    this->saveImageAsFile(input_src, input);

    delete auxkernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_src), this->makeOutputFilePath(input_src));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res_4), this->makeOutputFilePath(output_res_4));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res_8), this->makeOutputFilePath(output_res_8));
}

TEST_F(AuxilaryKernelsTest, AuxKernels_DilationAndErosion)
{
    std::string input_src("morphologyTest_pyramid_size13_source");
    std::string input_res_dilation("morphologyTest_pyramid_size13_dilation_answer");
    std::string input_res_dilation_erosion("morphologyTest_pyramid_size13_dilation_erosion_answer");
    std::string output_res_dilation("morphologyTest_pyramid_size13_dilation_result");
    std::string output_res_dilation_erosion("morphologyTest_pyramid_size13_dilation_erosion_result");

    auto *structureElement = StructuringElementGenerator::generateDiamondElement();
    auto *structureBuffer = new GPUMapped<Image>(framework->getOpenCLStack(), structureElement);

    auto *buffer = this->loadInputImageFileToGPU(input_src);
    auto *auxkernel = new DilationKernel(framework, buffer, structureBuffer);

    auxkernel->run();
    auto result = auxkernel->getOutput();
    this->saveImageAsFile(output_res_dilation, result);

    auto *auxkernel2 = new ErosionKernel(framework, result, structureBuffer);
    auxkernel2->run();
    auto result2 = auxkernel2->getOutput();
    this->saveImageAsFile(output_res_dilation_erosion, result2);

    auto input = auxkernel->getInput();
    this->saveImageAsFile(input_src, input);

    delete structureBuffer;
    delete structureElement;

    delete auxkernel2;
    delete auxkernel;
    delete buffer;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_src), this->makeOutputFilePath(input_src));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res_dilation), this->makeOutputFilePath(output_res_dilation));
    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(input_res_dilation_erosion), this->makeOutputFilePath(output_res_dilation_erosion));
}
