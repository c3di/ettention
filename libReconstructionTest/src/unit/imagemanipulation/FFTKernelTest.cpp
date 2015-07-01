#include "stdafx.h"
#include "framework/test/CLTestBase.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"

namespace ettention
{
    class FTKernelTest : public CLTestBase
    {
    public:
        virtual void applyFFTForward(std::string inFile, std::string realOutFile, std::string imageinaryOutFile)
        {
            Image* image = ImageDeserializer::readImage(inFile);

            Vec2ui imageResolution = image->getResolution();
            auto* buffer = new GPUMapped<Image>(clal, image);

            kernel = new FFTForwardKernel(clal, buffer);
            kernel->run();

            auto realImage = kernel->getRealOutputAsImage();
            auto imaginaryImage = kernel->getImaginaryOutputAsImage();

            ImageSerializer::writeImage(realOutFile, realImage, ImageSerializer::TIFF_GRAY_32);
            ImageSerializer::writeImage(imageinaryOutFile, imaginaryImage, ImageSerializer::TIFF_GRAY_32);

            delete image;
            delete kernel;
            delete buffer;
        }

        virtual void applyFFTBackward(std::string inFileRealPart, std::string inFileImageinaryPart, std::string outFile)
        {
            Image* realImage = ImageDeserializer::readImage(inFileRealPart);
            Image* imaginaryImage = ImageDeserializer::readImage(inFileImageinaryPart);

            Vec2ui imageResolution = realImage->getResolution();
            auto* bufferReal = new GPUMapped<Image>(clal, realImage);
            auto* bufferImaginary = new GPUMapped<Image>(clal, imaginaryImage);

            auto* kernel = new FFTBackKernel(clal, bufferReal, bufferImaginary);
            kernel->run();

            ImageSerializer::writeImage(outFile, kernel->getOutput(), ImageSerializer::TIFF_GRAY_32);

            delete realImage;
            delete imaginaryImage;
            delete kernel;
            delete bufferImaginary;
            delete bufferReal;
        }

        FFTForwardKernel* kernel;
    };
}

using namespace ettention;

// 
// Back Projection Tests
//                                               
TEST_F(FTKernelTest, FTKernel_Constructor) 
{
    Vec2ui imageResolution(128, 128);
    auto* buffer = new GPUMapped<Image>(clal, imageResolution);
    kernel = new FFTForwardKernel(clal, buffer);
    delete kernel;
    delete buffer;
}                                                           

TEST_F(FTKernelTest, FTKernel_Launch) 
{
    applyFFTForward(std::string(TESTDATA_DIR) + "/data/fft/square30.tif", std::string(TESTDATA_DIR) + "/work/fft/square30_real", std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/square30_real.tif", std::string(TESTDATA_DIR) + "/data/fft/square30_real.tif");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary.tif", std::string(TESTDATA_DIR) + "/data/fft/square30_imaginary.tif");
}                                                           

TEST_F(FTKernelTest, FTKernel_Roundtrip_Square) 
{
    applyFFTForward(std::string(TESTDATA_DIR) + "/data/fft/square30.tif", std::string(TESTDATA_DIR) + "/work/fft/square30_real", std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary");
    applyFFTBackward(std::string(TESTDATA_DIR) + "/work/fft/square30_real.tif",  std::string(TESTDATA_DIR) + "/work/fft/square30_imaginary.tif",  std::string(TESTDATA_DIR) + "/work/fft/square30_roundtrip");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/square30_roundtrip.tif", std::string(TESTDATA_DIR) + "/data/fft/square30.tif");
}                                                           

TEST_F(FTKernelTest, FTKernel_Roundtrip_Image) 
{
    applyFFTForward(std::string(TESTDATA_DIR) + "/data/fft/testdata.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_real", std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_imaginary");
    applyFFTBackward(std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_real.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_fourier_imaginary.tif",  std::string(TESTDATA_DIR) + "/work/fft/testdata_roundtrip");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/fft/testdata.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_roundtrip.tif");
}                                                           
