#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"

using namespace ettention;

class FTKernelTest : public TestBase
{
public:
    virtual void applyFFTForward(std::string inFile, std::string realOutFile, std::string imageinaryOutFile)
    {
        Image* image = ettention::ImageDeserializer::readImage(inFile);

        Vec2ui imageResolution = image->getResolution();
        auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), image);

        kernel = new FFTForwardKernel(framework->getOpenCLStack(), buffer);
        kernel->run();

        auto realImage = kernel->getRealOutputAsImage();
        auto imaginaryImage = kernel->getImaginaryOutputAsImage();

        ettention::ImageSerializer::writeImage(realOutFile, realImage, ettention::ImageSerializer::TIFF_GRAY_32);
        ettention::ImageSerializer::writeImage(imageinaryOutFile, imaginaryImage, ettention::ImageSerializer::TIFF_GRAY_32);

        delete image;
        delete kernel;
        delete buffer;
    }

    virtual void applyFFTBackward(std::string inFileRealPart, std::string inFileImageinaryPart, std::string outFile)
    {
        Image* realImage = ImageDeserializer::readImage(inFileRealPart);
        Image* imaginaryImage = ImageDeserializer::readImage(inFileImageinaryPart);

        Vec2ui imageResolution = realImage->getResolution();
        auto* bufferReal = new GPUMapped<Image>(framework->getOpenCLStack(), realImage);
        auto* bufferImaginary = new GPUMapped<Image>(framework->getOpenCLStack(), imaginaryImage);

        auto* kernel = new FFTBackKernel(framework->getOpenCLStack(), bufferReal, bufferImaginary);
        kernel->run();

        ImageSerializer::writeImage(outFile, kernel->getOutput(), ettention::ImageSerializer::TIFF_GRAY_32);

        delete realImage;
        delete imaginaryImage;
        delete kernel;
        delete bufferImaginary;
        delete bufferReal;
    }

    ettention::FFTForwardKernel* kernel;
};

// 
// Back Projection Tests
//                                               
TEST_F(FTKernelTest, FTKernel_Constructor) 
{
    Vec2ui imageResolution(128, 128);
    auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), imageResolution);
    kernel = new ettention::FFTForwardKernel(framework->getOpenCLStack(), buffer);
    delete kernel;
    delete buffer;
}                                                           

TEST_F(FTKernelTest, FTKernel_Launch) 
{
    applyFFTForward(std::string(TESTDATA_DIR) + "/data/fft/square30.tif", std::string(TESTDATA_DIR) + "/work/fft/square30_real", std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/square30_real.tif", std::string(TESTDATA_DIR) + "/data/fft/square30_real.tif");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary.tif", std::string(TESTDATA_DIR) + "/data/fft/square30_imaginary.tif");
}                                                           

TEST_F(FTKernelTest, FTKernel_Roundtrip_Square) 
{
    applyFFTForward(std::string(TESTDATA_DIR) + "/data/fft/square30.tif", std::string(TESTDATA_DIR) + "/work/fft/square30_real", std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary");
    applyFFTBackward(std::string(TESTDATA_DIR) + "/work/fft/square30_real.tif",  std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary.tif",  std::string(TESTDATA_DIR) + "/work/fft/square30_roundtrip");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/square30_roundtrip.tif", std::string(TESTDATA_DIR) + "/data/fft/square30.tif");
}                                                           

TEST_F(FTKernelTest, FTKernel_Roundtrip_Image) 
{
    applyFFTForward(std::string(TESTDATA_DIR) + "/data/fft/testdata.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_real", std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_imaginary");
    applyFFTBackward(std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_real.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_imaginary.tif",  std::string(TESTDATA_DIR) + "/work/fft/testdata_roundtrip");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/fft/testdata.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_roundtrip.tif");
}                                                           
