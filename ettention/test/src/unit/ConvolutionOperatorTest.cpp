#include "stdafx.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionImplementation.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionFourierSpaceImplementation.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionRealSpaceImplementation.h"
#include "algorithm/imagemanipulation/MultiplyKernel.h"
#include "algorithm/volumemanipulation/VolumeConvolutionKernel.h"
#include "algorithm/volumemanipulation/VolumeConvolutionOperator.h"
#include "framework/test/ImageManipulationTestBase.h"
#include "gpu/GPUMapped.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "model/image/Image.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "model/volume/VolumeProperties.h"
#include "model/volume/FloatVolume.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "io/deserializer/VolumeDeserializer.h"

using namespace ettention;

class ConvolutionOperatorTest : public ImageManipulationTestBase
{
public:
    ConvolutionOperatorTest()
        : ImageManipulationTestBase(std::string("Conv"), std::string(TESTDATA_DIR"/data/convolution"), std::string(TESTDATA_DIR"/work/convolution"))
    {
    }

    virtual void convolutionCase(std::string inputAName, std::string inputBName, std::string outputName)
    {
        auto* mappedImageA = this->loadInputImageFileToGPU(inputAName);
        auto* mappedImageB = this->loadInputImageFileToGPU(inputBName);

        convolutionImplementation = new ConvolutionFourierSpaceImplementation(framework, mappedImageA, mappedImageB, false);
        convolutionImplementation->execute();

        auto *result = convolutionImplementation->getOutput();
        this->saveImageAsFile(outputName, result);

        delete convolutionImplementation;

        delete mappedImageB;
        delete mappedImageA;
    }

    ConvolutionRealSpaceImplementation *convolutionRealImp;
    ConvolutionFourierSpaceImplementation *convolutionFourierImp;
    ConvolutionImplementation *convolutionImplementation;
};

// 
// Back Projection Tests
//                                               
TEST_F(ConvolutionOperatorTest, MultiplyKernelTest)
{
    std::string input_a("testdata");
    std::string input_b("point_psf");

    auto* mappedImageA = this->loadInputImageFileToGPU(input_a);
    auto* mappedImageB = this->loadInputImageFileToGPU(input_b);
    
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
    std::string input_filter("point_psf_shifted");
    std::string input_source("testdata");
    std::string result_name("convoluted_in_realspace");

    auto *filterKernel = this->loadInputImageFileToGPU(input_filter);
    auto *sourceImage = this->loadInputImageFileToGPU(input_source);

    convolutionRealImp = new ConvolutionRealSpaceImplementation(framework, filterKernel, sourceImage);
    convolutionRealImp->execute();

    auto *result = convolutionRealImp->getOutput();
    this->saveImageAsFile(result_name, result);

    delete convolutionRealImp;

    delete sourceImage;
    delete filterKernel;

    ImageComparator::assertImagesAreEqual(this->makeInputFilePath(result_name), this->makeOutputFilePath(result_name));
}

GPUMappedVolume* createGaussKernel3D(Framework* framework)
{
    static const float gaussKernel3D[] =
    {
        1.0f,  4.0f,  6.0f,  4.0f, 1.0f,
        4.0f, 16.0f, 24.0f, 16.0f, 4.0f,
        6.0f, 24.0f, 36.0f, 24.0f, 6.0f,
        4.0f, 16.0f, 24.0f, 16.0f, 4.0f,
        1.0f,  4.0f,  6.0f,  4.0f, 1.0f,

         4.0f, 16.0f,  24.0f, 16.0f,  4.0f,
        16.0f, 64.0f,  96.0f, 64.0f, 16.0f,
        24.0f, 96.0f, 144.0f, 96.0f, 24.0f,
        16.0f, 64.0f,  96.0f, 64.0f, 16.0f,
         4.0f, 16.0f,  24.0f, 16.0f,  4.0f,

         6.0f,  24.0f,  36.0f,  24.0f,  6.0f,
        24.0f,  96.0f, 144.0f,  96.0f, 24.0f,
        36.0f, 144.0f, 216.0f, 144.0f, 36.0f,
        24.0f,  96.0f, 144.0f,  96.0f, 24.0f,
         6.0f,  24.0f,  36.0f,  24.0f,  6.0f,

         4.0f, 16.0f,  24.0f, 16.0f,  4.0f,
        16.0f, 64.0f,  96.0f, 64.0f, 16.0f,
        24.0f, 96.0f, 144.0f, 96.0f, 24.0f,
        16.0f, 64.0f,  96.0f, 64.0f, 16.0f,
         4.0f, 16.0f,  24.0f, 16.0f,  4.0f,

        1.0f,  4.0f,  6.0f,  4.0f, 1.0f,
        4.0f, 16.0f, 24.0f, 16.0f, 4.0f,
        6.0f, 24.0f, 36.0f, 24.0f, 6.0f,
        4.0f, 16.0f, 24.0f, 16.0f, 4.0f,
        1.0f,  4.0f,  6.0f,  4.0f, 1.0f,
    };
    return new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(Vec3ui(5, 5, 5), gaussKernel3D));
}

TEST_F(ConvolutionOperatorTest, VolumeConvolutionKernel)
{
    auto convolutionKernel = createGaussKernel3D(framework);
    auto output = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(Vec3ui(5, 5, 5), boost::none));
    auto input = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(Vec3ui(9, 9, 9), 0.0f));
    input->getObjectOnCPU()->setVoxelToValue(Vec3ui(4, 4, 4), 1.0f);
    input->markAsChangedOnCPU();

    VolumeConvolutionKernel* kernel = new VolumeConvolutionKernel(framework, input, output, convolutionKernel);
    kernel->run();
    delete kernel;

    output->ensureIsUpToDateOnCPU();
    StackComparator::assertVolumesAreEqual(convolutionKernel->getObjectOnCPU(), output->getObjectOnCPU());

    delete output->getObjectOnCPU();
    delete output;
    delete input->getObjectOnCPU();
    delete input;
    delete convolutionKernel->getObjectOnCPU();
    delete convolutionKernel;
}

TEST_F(ConvolutionOperatorTest, VolumeConvolutionOperator)
{
    auto convolutionKernel = createGaussKernel3D(framework);
    
    auto output = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(Vec3ui(5, 5, 5), 0.0f, 3));
    auto input = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(Vec3ui(5, 5, 5), 0.0f));
    input->getObjectOnCPU()->setVoxelToValue(Vec3ui(2, 2, 2), 1.0f);
    input->markAsChangedOnCPU();

    VolumeConvolutionOperator* convOperator = new VolumeConvolutionOperator(framework, input, output, convolutionKernel);
    convOperator->run();
    delete convOperator;

    output->ensureIsUpToDateOnCPU();
    StackComparator::assertVolumesAreEqual(convolutionKernel->getObjectOnCPU(), output->getObjectOnCPU());

    delete output->getObjectOnCPU();
    delete output;
    delete input->getObjectOnCPU();
    delete input;
    delete convolutionKernel->getObjectOnCPU();
    delete convolutionKernel;
}

TEST_F(ConvolutionOperatorTest, test111)
{
    static const float testKernel2D[] =
    {
        1.0f,  1.0f, 1.0f,
        1.0f, -8.0f, 1.0f,
        1.0f,  1.0f, 1.0f,
    };
    /*auto *filter = new GPUMapped<Image>(framework->getOpenCLStack(), new Image(Vec2ui(3, 3), testKernel2D));
    auto *volume = VolumeDeserializer::load(std::string(TESTDATA_DIR"/data/convolution/mask_1-40.mrc"), Voxel::DataType::FLOAT_32);

    auto *convolutionRealImp = new ConvolutionRealSpaceImplementation(framework, filter, volume->);
    convolutionRealImp->execute();

    auto *result = convolutionRealImp->getOutput();
    this->saveImageAsFile(std::string(TESTDATA_DIR"/work/convolution/mask_1-40"), result);

    delete convolutionRealImp;

    delete volume;

    delete filter->getObjectOnCPU();
    delete filter;*/
}