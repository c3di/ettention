#include "stdafx.h"
#include <framework/test/TestBase.h>
#include <framework/RandomAlgorithms.h>
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "model/image/ImageComparator.h"

namespace ettention
{
    class RandomAlgorithmsTest : public TestBase
    {
    public:
        RandomAlgorithmsTest() {}
        ~RandomAlgorithmsTest() {}

        virtual void generateTestImage(unsigned int width, unsigned int height, unsigned int seed)
        {
            Image* testImage = RandomAlgorithms::generateRandomImage(width, height, seed);
            ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/random_image/randomTest", testImage, ImageSerializer::TIFF_GRAY_32);
            delete testImage;
        }
    };
}

using namespace ettention;

TEST_F(RandomAlgorithmsTest, TestRandomImageGeneration)
{
    generateTestImage(256, 256, 42);
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/random_image/randomTest.tif", std::string(TESTDATA_DIR) + "/data/random_image/randomTest.tif");
}