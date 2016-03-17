#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "algorithm/imagemanipulation/fft/PhaseRandomizationOperator.h"
#include "algorithm/imagemanipulation/fft/PhaseRandomizationKernel.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"

using namespace ettention;

class PhaseRandomizationOperatorTest : public TestBase
{
public:
    
    virtual ~PhaseRandomizationOperatorTest()
    {
    }

    virtual void applyPhaseRandomization(std::string inFile, std::string outFile)
    {
        Image* image = ettention::ImageDeserializer::readImage(inFile);

        auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), image);

        phaseRandomizationOperator = new PhaseRandomizationOperator(framework, buffer);

        phaseRandomizationOperator->setRandomPhase(42);
        phaseRandomizationOperator->run();

        ettention::ImageSerializer::writeImage(outFile, phaseRandomizationOperator->getOutput(), ImageSerializer::TIFF_GRAY_32);

        delete image;
        delete phaseRandomizationOperator;
        delete buffer;
    }
    
    PhaseRandomizationKernel* kernel;
    PhaseRandomizationOperator* phaseRandomizationOperator;
};

TEST_F(PhaseRandomizationOperatorTest, PhaseRandomizationOperatorTestConstructor) 
{
    Vec2ui imageResolution(128, 128);
    auto* buffer = new GPUMapped<Image>(framework->getOpenCLStack(), imageResolution);
    kernel = new ettention::PhaseRandomizationKernel(framework, buffer, buffer);
    delete kernel;
    delete buffer;
}

TEST_F(PhaseRandomizationOperatorTest, PhaseRandomizationOperatorTestLaunch)
{
    applyPhaseRandomization(std::string(TESTDATA_DIR) + "/data/random_image/phantom.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_seed_42");
    applyPhaseRandomization(std::string(TESTDATA_DIR) + "/data/random_image/empiar_10009_512_11.tif", std::string(TESTDATA_DIR) + "/work/random_image/empiar_seed_42");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/random_image/phantom_seed_42.tif", std::string(TESTDATA_DIR) + "/work/random_image/phantom_seed_42.tif");
    ettention::ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/data/random_image/empiar_seed_42.tif", std::string(TESTDATA_DIR) + "/work/random_image/empiar_seed_42.tif");
}