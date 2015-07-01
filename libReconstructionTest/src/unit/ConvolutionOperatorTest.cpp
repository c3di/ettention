#include "stdafx.h"
#include "framework/test/ImageManipulationTestBase.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionFourierSpaceImplementation.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionRealSpaceImplementation.h"
#include "algorithm/imagemanipulation/MultiplyKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class ConvolutionOperatorTest : public ImageManipulationTestBase
    {
    public:
        ConvolutionOperatorTest()
            : ImageManipulationTestBase(std::string("Conv"), std::string(TESTDATA_DIR"/data/convolution"), std::string(TESTDATA_DIR"/work/convolution"))
        {
        }

        virtual void convolutionCase(std::string inputAName, std::string inputBName, std::string outputName)
        {
            auto* mappedImageA = this->loadImageFileToGPU(inputAName);
            auto* mappedImageB = this->loadImageFileToGPU(inputBName);

            convolutionOperator = new ConvolutionFourierSpaceImplementation(framework, mappedImageA, mappedImageB, false);
            convolutionOperator->execute();

            auto *result = convolutionOperator->getOutput();
            this->saveImageAsFile(outputName, result);

            delete convolutionOperator;

            delete mappedImageB;
            delete mappedImageA;
        }

        ConvolutionRealSpaceImplementation* convolutionRealImp;
        ConvolutionFourierSpaceImplementation* convolutionFourierImp;
        ConvolutionOperatorImplementation* convolutionOperator;
    };
}

using namespace ettention;

TEST_F(ConvolutionOperatorTest, MultiplyKernelTest)
{
    std::string input_a("testdata");
    std::string input_b("point_psf");

    auto* mappedImageA = this->loadImageFileToGPU(input_a);
    auto* mappedImageB = this->loadImageFileToGPU(input_b);
    
    ASSERT_TRUE(mappedImageA->getResolution() == mappedImageB->getResolution());

    MultiplyKernel* multiplyKernel = new MultiplyKernel(framework, mappedImageA, mappedImageB);
    multiplyKernel->run();

    auto *result = multiplyKernel->getOutput();
    this->saveImageAsFile("multiply_result", result);

    delete multiplyKernel;

    delete mappedImageB;
    delete mappedImageA;
}

TEST_F(ConvolutionOperatorTest, ConvolutionOperator_Point_PSF)
{
    std::string input_a("testdata");
    std::string input_b("point_psf");
    std::string result_name("convoluted_point");

    this->convolutionCase(input_a, input_b, result_name);

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(result_name), this->makeOutputFilePath(result_name));
}

TEST_F(ConvolutionOperatorTest, ConvolutionOperator_Blurr_PSF)
{
    std::string input_a("testdata");
    std::string input_b("blurr_psf");
    std::string result_name("convoluted_blurr");

    this->convolutionCase(input_a, input_b, result_name);

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(result_name), this->makeOutputFilePath(result_name));
}

TEST_F(ConvolutionOperatorTest, ConvolutionOperator_Padding_Cropping)
{
    std::string input_filter("point_psf_84x84_norm");
    std::string input_source("testdata_192x236_cut");
    std::string result_name("convoluted_point_withpadding");

    this->convolutionCase(input_filter, input_source, result_name);

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(result_name), this->makeOutputFilePath(result_name));
}

TEST_F(ConvolutionOperatorTest, ConvolutionOperator_RealSpaceImp)
{
    std::string input_filter("testdata");
    std::string input_source("blurr_psf");
    std::string result_name("convoluted_in_realspace");

    auto *filterKernel = this->loadImageFileToGPU(input_filter);
    auto *sourceImage = this->loadImageFileToGPU(input_source);

    convolutionRealImp = new ConvolutionRealSpaceImplementation(framework, filterKernel, sourceImage);
    convolutionRealImp->execute();

    auto *result = convolutionRealImp->getOutput();
    this->saveImageAsFile(result_name, result);

    delete convolutionRealImp;

    delete sourceImage;
    delete filterKernel;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(result_name), this->makeOutputFilePath(result_name));
}