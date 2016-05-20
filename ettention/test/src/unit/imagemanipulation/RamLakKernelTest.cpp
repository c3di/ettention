#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "algorithm/imagemanipulation/GenerateRamLakFilterKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"

using namespace ettention;

class RamLakKernelTest : public TestBase 
{
};

TEST_F(RamLakKernelTest, TestKernelGeneration)
{
    GenerateRamLakKernel* kernel = new GenerateRamLakKernel(framework, 128);
    kernel->run();
    ettention::ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/filter/ramlak/kernel_128", kernel->getOutput(), ettention::ImageSerializer::TIFF_GRAY_32);
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/filter/ramlak_128.tif", std::string(TESTDATA_DIR) + "/work/filter/ramlak/kernel_128.tif");
    delete kernel;
}
