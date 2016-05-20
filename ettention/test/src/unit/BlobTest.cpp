#include "stdafx.h"
#include <algorithm/blobs/BlobRayIntegrationSampler.h>
#include <algorithm/blobs/BlobVoxelizerOperator.h>
#include <algorithm/volumemanipulation/VolumeConvolutionKernel.h>
#include <framework/NumericalAlgorithms.h>
#include <framework/test/TestBase.h>
#include <io/serializer/ImageSerializer.h>
#include <io/serializer/MRCWriter.h>
#include <model/image/ImageComparator.h>
#include <model/volume/StackComparator.h>
#include <setup/parameterset/OutputParameterSet.h>

using namespace ettention;

#define SAMPLE_COUNT 256

class BlobTest : public TestBase
{
protected:
    BlobRayIntegrationSampler* blobSampler;
    BlobParameters blobParameters;

public:
    BlobTest() : blobParameters(blobParameters.createDefault()) {}
    ~BlobTest() {}

    void SetUp() override
    {
        TestBase::SetUp();
        blobSampler = new BlobRayIntegrationSampler(blobParameters, SAMPLE_COUNT);
    }

    void TearDown() override
    {
        delete blobSampler;
        TestBase::TearDown();
    }
};

void SavePlot(float const* data, unsigned int numberOfValues, const std::string& filename)
{
    if(numberOfValues > 0)
    {
        float minVal = data[0];
        float maxVal = data[0];
        for(unsigned int i = 0; i < numberOfValues; ++i)
        {
            minVal = std::min(minVal, data[i]);
            maxVal = std::max(maxVal, data[i]);
        }
        float* pPlotImageData = new float[numberOfValues * numberOfValues];
        memset(pPlotImageData, 0, numberOfValues * numberOfValues * sizeof(float));
        for(unsigned int i = 0; i < numberOfValues; ++i)
        {
            unsigned int yPos = (unsigned int)std::floor((float)(numberOfValues - 1) * (data[i] - minVal) / (maxVal - minVal) + 0.5f);
            pPlotImageData[(numberOfValues - 1 - yPos) * numberOfValues + i] = 1.0f;
        }
        Image* pPlotImage = new Image(Vec2ui(numberOfValues, numberOfValues), pPlotImageData);
        ImageSerializer::writeImage(filename, pPlotImage, ImageSerializer::PNG_GRAY_8);
        delete pPlotImage;
        delete[] pPlotImageData;
    }
}

TEST_F(BlobTest, TestKaiserBessel)
{
    float* pPlotData = new float[SAMPLE_COUNT + 1];
    for(unsigned int i = 0; i <= SAMPLE_COUNT; ++i)
    {
        double r = (double)i / (double)SAMPLE_COUNT * blobSampler->getParameters().getSupportRadius();
        pPlotData[i] = (float)blobSampler->getParameters().evaluate(r);
    }
    SavePlot(pPlotData, SAMPLE_COUNT + 1, std::string(TESTDATA_DIR) + "/work/blobs/blob_plot");
    delete[] pPlotData;
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/blobs/blob_plot.png", std::string(TESTDATA_DIR) + "/data/blobs/blob_plot_ref.png");
}

TEST_F(BlobTest, CheckSpecificValues)
{
    double a = blobSampler->getParameters().getSupportRadius();
    int m = blobSampler->getParameters().getDegree();
    double alpha = blobSampler->getParameters().getDensityFallOff();
    unsigned int maxIterations = 200;

    double sum1 = 0.0;
    double sum2 = 0.0;
    double summand1 = 1.0 / (double)NumericalAlgorithms::factorial(m);
    double summand2 = (double)NumericalAlgorithms::factorial(m) / (double)NumericalAlgorithms::factorial(2 * m + 1);
    for(unsigned int n = 0; n < maxIterations; ++n)
    {
        sum1 += summand1;
        sum2 += summand2;
        summand1 *= 0.25 * alpha * alpha / (double)((n + 1) * (n + m + 1));
        summand2 *= alpha * alpha / (double)(2 * (n + 1) * (2 * n + 2 * m + 3));
    }
    double refVal = 2.0 * NumericalAlgorithms::uintPow(2.0, 2 * m) * a * sum2 / sum1;

    ASSERT_NEAR(blobSampler->getParameters().evaluate(0.0), 1.0, 1e-5);
    ASSERT_NEAR(blobSampler->getParameters().evaluate(blobSampler->getParameters().getSupportRadius()), 0.0, 1e-5);
    ASSERT_NEAR(blobSampler->getSamples()[0], refVal, 1e-5);
    ASSERT_NEAR(blobSampler->getSamples()[blobSampler->getSampleCount() - 1], 0.0, 1e-5);
}

TEST_F(BlobTest, Voxelizer)
{
    GPUMappedVolume* blobVolume = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(Vec3ui(16, 16, 16), boost::none));
    blobVolume->getObjectOnCPU()->setVoxelToValue(Vec3ui(3, 4, 5), 1.0f);
    blobVolume->markAsChangedOnCPU();    

    auto voxelizer = new BlobVoxelizerOperator(framework, blobVolume, BlobParameters::createDefault());
    voxelizer->voxelize();

    voxelizer->getVoxelVolume()->ensureIsUpToDateOnCPU();
    OutputParameterSet linuxFriendlyNonTempVar;
    MRCWriter().write(voxelizer->getVoxelVolume()->getObjectOnCPU(), std::string(TESTDATA_DIR) + "/work/blobs/voxelized.mrc", &linuxFriendlyNonTempVar);
    StackComparator::assertVolumesAreEqual(std::string(TESTDATA_DIR) + "/work/blobs/voxelized.mrc", std::string(TESTDATA_DIR) + "/data/blobs/voxelized_reference.mrc");

    delete voxelizer;
    delete blobVolume->getObjectOnCPU();
    delete blobVolume;
}

//////////////////////////////////////////////////////////////////////////
//           Although this test is disabled do NOT delete it!           //
//////////////////////////////////////////////////////////////////////////
TEST_F(BlobTest, DISABLED_MemoryTest)
{
    int* leak = 0;
    for(int i = 0; i < 10; ++i)
    {
        leak = new int[20];
        if(i == 4)
        {
            delete leak;
        }
    }
    delete[] leak;
}