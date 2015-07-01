#include "stdafx.h"
#include "algorithm/imagemanipulation/RMSKernel.h"
#include "framework/math/Vec2.h"
#include "framework/test/CLTestBase.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "model/image/ImageComparator.h"
#include "setup/parametersource/ManualParameterSource.h"

namespace ettention
{
    class RMSKernelTest : public CLTestBase
    {
    public:
        virtual Image* generateSyntheticImage(Vec2ui resolution)
        {
            Image* image = new Image(resolution);
            // white line on the diagonal
            for(unsigned int i = 0; i < resolution.x; i++)
                image->setPixel(i, i, 1.0f);
            return image;
        }
    };
}

using namespace ettention;

//
// Back Projection Tests
//
TEST_F(RMSKernelTest, RMSKernel_Constructor)
{
    Vec2ui resolution = Vec2ui(256, 256);
    GPUMapped<Image>* image = new GPUMapped<Image>(clal, resolution);
    RMSKernel* kernel = new RMSKernel(framework, image);
    delete kernel;
    delete image;
}

TEST_F(RMSKernelTest, RMSKernel_Execute)
{
    Image* image = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/rms/testdata.tif");
    GPUMapped<Image>* mappedImage = new GPUMapped<Image>(clal, image);
    RMSKernel* kernel = new RMSKernel(framework, mappedImage);
    kernel->run();
    const float knownRMSForTestData = 0.49641284f;
    float rms = kernel->getRootMeanSquare();
    ASSERT_NEAR(rms, knownRMSForTestData, 0.0001f);
    delete kernel;
    delete mappedImage;
    delete image;
}

TEST_F(RMSKernelTest, RMSKernel_BigFile)
{
    Vec2ui resolution = Vec2ui(2048, 2048);
    Image* image = generateSyntheticImage(resolution);
    GPUMapped<Image>* mappedImage = new GPUMapped<Image>(clal, image);

    RMSKernel* kernel = new RMSKernel(framework, mappedImage);
    kernel->run();

    const unsigned int numberOfWhitePixel = resolution.x;
    const unsigned int numberOfPixel = resolution.x * resolution.y;
    const float knownRMS = sqrtf((float) numberOfWhitePixel / (float) numberOfPixel);

    float rms = kernel->getRootMeanSquare();
    ASSERT_NEAR(rms, knownRMS, 0.0001f);

    delete kernel;
    delete mappedImage;
    delete image;
}