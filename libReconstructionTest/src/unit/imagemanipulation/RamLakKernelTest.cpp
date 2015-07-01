#include "stdafx.h"
#include "framework/test/CLTestBase.h"
#include "algorithm/imagemanipulation/GenerateRamLakFilterKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"

namespace ettention
{
    class RamLakKernelTest : public CLTestBase {};
}

using namespace ettention;

TEST_F(RamLakKernelTest, TestKernelGeneration)
{
    GenerateRamLakKernel* kernel = new GenerateRamLakKernel(framework, 128);
    kernel->run();
    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/filter/ramlak/kernel_128", kernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/filter/ramlak_128.tif", std::string(TESTDATA_DIR) + "/work/filter/ramlak/kernel_128.tif");
    delete kernel;
}