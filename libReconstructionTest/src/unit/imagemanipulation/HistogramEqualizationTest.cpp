#include "stdafx.h"
#include "framework/test/CLTestBase.h"
#include "algorithm/imagestatistics/histogram/HistogramEqualization.h"
#include "algorithm/imagemanipulation/MissingWedgeImageGenerator.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"

namespace ettention
{
    class HistogramEqualizationTest : public CLTestBase
    {
    public:
        virtual void applyHistogramEqualization(std::string inFileImageForHistogram, std::string inFileImageToBeAdapted, std::string outFile)
        {
            Image* imageForHistogram = ImageDeserializer::readImage(inFileImageForHistogram);
            Image* imageToBeAdapted = ImageDeserializer::readImage(inFileImageToBeAdapted);
            Image* outImage = HistogramEqualization::equalizeHistograms(imageForHistogram, imageToBeAdapted);
            ImageSerializer::writeImage(outFile, outImage, ImageSerializer::TIFF_GRAY_32);

            delete imageForHistogram;
            delete imageToBeAdapted;
            delete outImage;
        }

        virtual void applyPhaseRandomizationAndHistogramEqualization(std::string inFile, std::string outFile)
        {
            Image* inputImage = ImageDeserializer::readImage(inFile);
            auto* buffer = new GPUMapped<Image>(clal, inputImage);

            MissingWedgeImageGenerator* randomImageGenerator = new MissingWedgeImageGenerator(framework, buffer);
            randomImageGenerator->run();
            ImageSerializer::writeImage(outFile, randomImageGenerator->getOutput(), ImageSerializer::TIFF_GRAY_32);

            delete randomImageGenerator;
            delete inputImage;
            delete buffer;
        }
    };
}

using namespace ettention;

// 
// Histogram Equalization Test
//                                                                                                        

TEST_F(HistogramEqualizationTest, HistogramEqualizationTest_Launch)
{
    applyHistogramEqualization(std::string(TESTDATA_DIR) + "/data/random_image/phantom.tif", std::string(TESTDATA_DIR) + "/data/random_image/phantom_seed_42.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_histogramEqualized");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/random_image/phantom_histogramEqualized.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_histogramEqualized.tif");
}

TEST_F(HistogramEqualizationTest, DISABLED_HistogramEqualizationTest_MissingWedgeDataGeneration)
{
    applyPhaseRandomizationAndHistogramEqualization(std::string(TESTDATA_DIR) + "/data/random_image/phantom.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_phaseRandomizedAndHistogramEqualized");
}