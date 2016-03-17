#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "algorithm/imagemanipulation/fft/FFTMagnitudeKernel.h"
#include "algorithm/imagemanipulation/fft/FFTPhaseKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"

using namespace ettention;

class FTPhaseAndMagnitudeTest : public TestBase
{
public:
    virtual void applyFFTPhase(std::string inFileRealPart, std::string inFileImageinaryPart, std::string outFile)
    {
        Image* realImage = ImageDeserializer::readImage(inFileRealPart);
        Image* imaginaryImage = ImageDeserializer::readImage(inFileImageinaryPart);

        Vec2ui imageResolution = realImage->getResolution();
        auto* bufferReal = new GPUMapped<Image>(framework->getOpenCLStack(), realImage);
        auto* bufferImaginary = new GPUMapped<Image>(framework->getOpenCLStack(), imaginaryImage);

        auto* kernel = new FFTPhaseKernel(framework, bufferReal, bufferImaginary);
        kernel->run();

        ImageSerializer::writeImage(outFile, kernel->getOutput(), ettention::ImageSerializer::TIFF_GRAY_32);

        delete realImage;
        delete imaginaryImage;
        delete kernel;
        delete bufferImaginary;
        delete bufferReal;
    }

    virtual void applyFFTMagnitude(std::string inFileRealPart, std::string inFileImageinaryPart, std::string outFile)
    {
        Image* realImage = ImageDeserializer::readImage(inFileRealPart);
        Image* imaginaryImage = ImageDeserializer::readImage(inFileImageinaryPart);

        Vec2ui imageResolution = realImage->getResolution();
        auto* bufferReal = new GPUMapped<Image>(framework->getOpenCLStack(), realImage);
        auto* bufferImaginary = new GPUMapped<Image>(framework->getOpenCLStack(), imaginaryImage);

        auto* kernel = new FFTMagnitudeKernel(framework, bufferReal, bufferImaginary);
        kernel->run();

        ImageSerializer::writeImage(outFile, kernel->getOutput(), ettention::ImageSerializer::TIFF_GRAY_32);

        delete realImage;
        delete imaginaryImage;
        delete kernel;
        delete bufferImaginary;
        delete bufferReal;
    }

    ettention::FFTPhaseKernel* pKernel;
    ettention::FFTMagnitudeKernel* mKernel;
};

// 
// Phase and Magnitude Tests
//                                               
TEST_F(FTPhaseAndMagnitudeTest, DISABLED_FTPhaseAndMagnitude_PhaseConstructor)
{
    Vec2ui imageResolution(128, 128);
    auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), imageResolution);
    pKernel = new ettention::FFTPhaseKernel(framework, buffer, buffer);
    delete pKernel;
    delete buffer;
}

TEST_F(FTPhaseAndMagnitudeTest, DISABLED_FTPhaseAndMagnitude_MagnitudeConstructor)
{
    Vec2ui imageResolution(128, 128);
    auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), imageResolution);
    mKernel = new ettention::FFTMagnitudeKernel(framework, buffer, buffer);
    delete mKernel;
    delete buffer;
}

TEST_F(FTPhaseAndMagnitudeTest, DISABLED_FTPhaseAndMagnitude_Phase)
{
    applyFFTPhase(std::string(TESTDATA_DIR) + "/data/fft/testdata_real.tif", std::string(TESTDATA_DIR) + "/data/fft/testdata_imaginary.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_phase");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/testdata_phase.tif", std::string(TESTDATA_DIR) + "/data/fft/testdata_phase.tif");
}                                                           

TEST_F(FTPhaseAndMagnitudeTest, DISABLED_FTPhaseAndMagnitude_Magnitude)
{
    applyFFTMagnitude(std::string(TESTDATA_DIR) + "/data/fft/testdata_real.tif", std::string(TESTDATA_DIR) + "/data/fft/testdata_imaginary.tif", std::string(TESTDATA_DIR) + "/work/fft/testdata_magnitude");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/fft/testdata_magnitude.tif", std::string(TESTDATA_DIR) + "/data/fft/testdata_magnitude.tif");
}