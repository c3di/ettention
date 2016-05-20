#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "algorithm/imagestatistics/histogram/HistogramEqualization.h"
#include "algorithm/imagemanipulation/MissingWedgeImageGenerator.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"

using namespace ettention;

class HistogramEqualizationTest : public TestBase
{
public:
    virtual void applyHistogramEqualization(std::string inFileImageForHistogram, std::string inFileImageToBeAdapted, std::string outFile)
    {
        Image* imageForHistogram = ettention::ImageDeserializer::readImage(inFileImageForHistogram);
        Image* imageToBeAdapted = ettention::ImageDeserializer::readImage(inFileImageToBeAdapted);
        Image* outImage = HistogramEqualization::equalizeHistograms(imageForHistogram, imageToBeAdapted);
        ettention::ImageSerializer::writeImage(outFile, outImage, ettention::ImageSerializer::TIFF_GRAY_32);

        delete imageForHistogram;
        delete imageToBeAdapted;
        delete outImage;
    }

    virtual void applyPhaseRandomizationAndHistogramEqualization(std::string inFile, std::string outFile)
    {
        Image* inputImage = ettention::ImageDeserializer::readImage(inFile);
        auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), inputImage);

        MissingWedgeImageGenerator* randomImageGenerator = new MissingWedgeImageGenerator(framework, buffer);
        randomImageGenerator->run();
        ettention::ImageSerializer::writeImage(outFile, randomImageGenerator->getOutput(), ettention::ImageSerializer::TIFF_GRAY_32);

        delete randomImageGenerator;
        delete inputImage;
        delete buffer;
    }

};

// 
// Histogram Equalization Test
//                                                                                                        

TEST_F(HistogramEqualizationTest, HistogramEqualizationTest_Launch)
{
    applyHistogramEqualization(std::string(TESTDATA_DIR) + "/data/random_image/phantom.tif", std::string(TESTDATA_DIR) + "/data/random_image/phantom_seed_42.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_histogramEqualized");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/random_image/phantom_histogramEqualized.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_histogramEqualized.tif");
}

TEST_F(HistogramEqualizationTest, HistogramEqualizationTest_MissingWedgeDataGeneration)
{
    applyPhaseRandomizationAndHistogramEqualization(std::string(TESTDATA_DIR) + "/data/random_image/phantom.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_phaseRandomizedAndHistogramEqualized");
    //add Histogram Comparator and apply it here?
    //ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/random_image/phantom_histogramEqualized.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_histogramEqualized.tif");
}