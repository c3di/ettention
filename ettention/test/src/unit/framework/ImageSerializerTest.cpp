#include "stdafx.h"
#include "framework/error/Exception.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class ImageSerializerTest : public TestBase {};
}

using namespace ettention;

TEST_F(ImageSerializerTest, Test_4x4_TIF)
{
    Image* image = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/imageio/test4x4.tif");
    ASSERT_EQ(image->getResolution().x, 4);
    ASSERT_EQ(image->getResolution().y, 4);

    ASSERT_NEAR(0.25f, image->getPixel(0, 0), 0.01f);
    ASSERT_NEAR(0.5f, image->getPixel(1, 1), 0.01f);
    ASSERT_NEAR(0.75f, image->getPixel(2, 2), 0.01f);
    ASSERT_NEAR(1.0f, image->getPixel(3, 3), 0.01f);
    delete image;
}

TEST_F(ImageSerializerTest, Test_1x512_TIF)
{
    Image* image = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/imageio/test1x512.tif");
    ASSERT_EQ(image->getResolution().x, 1);
    ASSERT_EQ(image->getResolution().y, 512);

    ASSERT_NEAR(0.0f, image->getPixel(0, 0), 0.01f);
    for(unsigned int y = 0; y < 510; y++)
        ASSERT_NEAR(0.0f, image->getPixel(0, y), 0.01f);

    ASSERT_NEAR(1.0f, image->getPixel(0, 511), 0.01f);
    ASSERT_NEAR(0.5f, image->getPixel(0, 510), 0.01f);

    delete image;
}

TEST_F(ImageSerializerTest, Test_512x1_TIF)
{
    Image* image = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/imageio/test512x1.tif");
    ASSERT_EQ(image->getResolution().x, 512);
    ASSERT_EQ(image->getResolution().y, 1);

    ASSERT_NEAR(0.0f, image->getPixel(0, 0), 0.01f);
    for(unsigned int x = 0; x < 510; x++)
        ASSERT_NEAR(0.0f, image->getPixel(x, 0), 0.01f);

    ASSERT_NEAR(1.0f, image->getPixel(511, 0), 0.01f);
    ASSERT_NEAR(0.5f, image->getPixel(510, 0), 0.01f);

    delete image;
}