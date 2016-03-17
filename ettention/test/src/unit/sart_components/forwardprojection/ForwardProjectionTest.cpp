#include "stdafx.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionKernel.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionWithBlobsKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionWithBlobsKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "framework/test/ProjectionTestBase.h"
#include "framework/VectorAlgorithms.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/datasource/MRCStack.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/GPUMappedVolume.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "setup/parameterset/PriorKnowledgeParameterSet.h"
#include "unit/sart_components/forwardprojection/ForwardProjectionTest.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    ForwardProjectionTest::ForwardProjectionTest()
        : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
    {
    }

    void ForwardProjectionTest::SetUp()
    {
        ProjectionTestBase::SetUp();
        mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);
        mappedVolume->ensureIsUpToDateOnGPU();
    }

    void ForwardProjectionTest::TearDown()
    {
        delete mappedVolume;
        ProjectionTestBase::TearDown();
    }

    void ForwardProjectionTest::allocateEmptyVolume(Vec3ui volumeResolution)
    {
        volume = new FloatVolume(volumeResolution, 1.0f, 1);
    }

    void ForwardProjectionTest::testUnitForwardProjection(ParallelBeamsForwardProjectionKernel* kernel)
    {
        GPUMapped<Image>* projection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        GPUMapped<Image>* traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        kernel->setOutput(projection, traversalLength);

        geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(45.0f));
        kernel->run();

        ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/forward_projection/generated", projection, ImageSerializer::ImageFormat::TIFF_GRAY_32);
        ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/forward_projection/generated.tif", std::string(TESTDATA_DIR) + "/data/forward_projection/projection_45.tif");

        delete traversalLength;
        delete projection;
    }
}

using namespace ettention;

TEST_F(ForwardProjectionTest, Unit_ForwardProjection)
{
    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, nullptr, 1);

    testUnitForwardProjection(kernel);

    delete kernel;
}

TEST_F(ForwardProjectionTest, Unit_ForwardProjection_Raysign)
{
    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, nullptr, 1);
    GPUMapped<Image>* projection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
    GPUMapped<Image>* traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
    kernel->setOutput(projection, traversalLength);

    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(225.0f));
    kernel->run();

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/forward_projection/generated_opposite_direction", projection, ImageSerializer::ImageFormat::TIFF_GRAY_32);
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/forward_projection/generated_opposite_direction.tif", std::string(TESTDATA_DIR) + "/data/forward_projection/projection_45.tif");

    delete traversalLength;
    delete projection;
    delete kernel;
}

TEST_F(ForwardProjectionTest, Unit_ForwardProjectionMatchesBackProjection)
{
    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(45.0f));

    Image* projectionImage = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/forward_projection/projection_45.tif");
    Image* traversalLengthImage  = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/forward_projection/raylength_reference.tif");
    GPUMapped<Image>* projection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionImage);
    GPUMapped<Image>* traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), traversalLengthImage);
    auto generatedProjection = new GPUMapped<Image>(framework->getOpenCLStack(), projection->getResolution());
    auto generatedTraversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), traversalLength->getResolution());
    Volume* cpuVolume = new FloatVolume(volumeResolution, boost::none);
    GPUMappedVolume* outputVolume = new GPUMappedVolume(framework->getOpenCLStack(), cpuVolume);

    // back projection
    ParallelBeamsBackProjectionKernel* backKernel = new ParallelBeamsBackProjectionKernel(framework, geometricSetup, outputVolume, nullptr, 1.0f, 1, false);
    backKernel->SetInput(projection, traversalLength);
    backKernel->run();
    delete backKernel;

    // forward projection
    ParallelBeamsForwardProjectionKernel* forwardKernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, nullptr, 1);
    forwardKernel->setOutput(generatedProjection, generatedTraversalLength);
    forwardKernel->run();
    delete forwardKernel;

    // comparison
    generatedProjection->ensureIsUpToDateOnCPU();
    generatedTraversalLength->ensureIsUpToDateOnCPU();
    ImageComparator::assertImagesAreEqual(projectionImage, generatedProjection->getObjectOnCPU());
    ImageComparator::assertImagesAreEqual(traversalLengthImage, generatedTraversalLength->getObjectOnCPU());

    // cleanup
    delete outputVolume;
    delete cpuVolume;
    delete generatedTraversalLength;
    delete generatedProjection;
    delete traversalLength;
    delete projection;
    delete traversalLengthImage;
    delete projectionImage;
} 

TEST_F(ForwardProjectionTest, Unit_ComputeRayLength) 
{
    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(45.0f));
    GPUMapped<Image>* rayLengthImage = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);

    ComputeRayLengthKernel* kernel = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);

    kernel->run();
    kernel->getOutput()->ensureIsUpToDateOnCPU();

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/forward_projection/raylength", kernel->getOutput(), ImageSerializer::ImageFormat::TIFF_GRAY_32);    

    Image* generatedImage = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/work/forward_projection/raylength.tif");
    Image* referenceImage = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/forward_projection/raylength_reference.tif");
    float error = ImageComparator::getRMS(generatedImage, referenceImage);
    ASSERT_TRUE(error < 0.1f);

    delete referenceImage;
    delete generatedImage;

    delete kernel;
    delete rayLengthImage;
}

TEST_F(ForwardProjectionTest, PerspectiveIntersectionLengthAccumulation)
{
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    Volume* volumeOnCpu = new FloatVolume(Vec3ui(128, 128, 128), 1.0f);
    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));

    auto kernel = new PerspectiveBeamsForwardProjectionKernel(framework, geometricSetup, volume, nullptr, 1);
    kernel->setOutput(projection, intersectionLengths);
    kernel->run();    
    delete kernel;

    ImageComparator::assertImagesAreEqual(projection->getObjectOnCPU(), intersectionLengths->getObjectOnCPU());

    delete intersectionLengths;
    delete projection;
    delete volume;
    delete volumeOnCpu;
}

TEST_F(ForwardProjectionTest, PerspectiveBlobsIntersectionLengthAccumulation)
{
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    Volume* volumeOnCpu = new FloatVolume(Vec3ui(128, 128, 128), 1.0f);
    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));

    auto sampler = std::make_shared<BlobRayIntegrationSampler>(BlobParameters::createDefault(), 128);

    auto kernel = new PerspectiveBeamsForwardProjectionWithBlobsKernel(framework, sampler, geometricSetup, volume, nullptr);
    kernel->setOutput(projection, intersectionLengths);
    kernel->run();
    delete kernel;

    ImageComparator::assertImagesAreEqual(projection->getObjectOnCPU(), intersectionLengths->getObjectOnCPU());

    delete intersectionLengths;
    delete projection;
    delete volume;
    delete volumeOnCpu;
}

TEST_F(ForwardProjectionTest, Perspective)
{
    Volume* volumeOnCpu = VolumeDeserializer::load(std::string(TESTDATA_DIR) + "/data/perspective/volume.mrc", Voxel::DataType::FLOAT_32);
    geometricSetup->setVolume(volumeOnCpu);
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));

    auto kernel = new PerspectiveBeamsForwardProjectionKernel(framework, geometricSetup, volume, nullptr, 8);
    kernel->setOutput(projection, intersectionLengths);
    kernel->run();
    delete kernel;

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/perspective/projection", projection, ImageSerializer::TIFF_GRAY_32);
    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/perspective/intersection_lengths", intersectionLengths, ImageSerializer::TIFF_GRAY_32);

    delete intersectionLengths;
    delete projection;
    delete volume;
    delete volumeOnCpu;

    //float rms = ImageComparator::getRMS(std::string(TESTDATA_DIR) + "/work/perspective/projection.tif", std::string(TESTDATA_DIR) + "/data/perspective/projection_reference.tif");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/perspective/projection.tif", std::string(TESTDATA_DIR) + "/data/perspective/projection_reference.tif");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/perspective/intersection_lengths.tif", std::string(TESTDATA_DIR) + "/data/perspective/intersection_lengths_reference.tif");
}

TEST_F(ForwardProjectionTest, PerspectiveBlobs)
{
    Volume* volumeOnCpu = VolumeDeserializer::load(std::string(TESTDATA_DIR) + "/data/perspective/volume.mrc", Voxel::DataType::FLOAT_32);
    geometricSetup->setVolume(volumeOnCpu);
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), Vec2ui(128, 128));

    auto sampler = std::make_shared<BlobRayIntegrationSampler>(BlobParameters::createDefault(), 128);

    auto kernel = new PerspectiveBeamsForwardProjectionWithBlobsKernel(framework, sampler, geometricSetup, volume, nullptr);
    kernel->setOutput(projection, intersectionLengths);
    kernel->run();
    delete kernel;

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/perspective/projection", projection, ImageSerializer::TIFF_GRAY_32);
    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/perspective/intersection_lengths", intersectionLengths, ImageSerializer::TIFF_GRAY_32);

    delete intersectionLengths;
    delete projection;
    delete volume;
    delete volumeOnCpu;

    //float rms = ImageComparator::getRMS(std::string(TESTDATA_DIR) + "/work/perspective/projection.tif", std::string(TESTDATA_DIR) + "/data/perspective/projection_reference.tif");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/perspective/projection.tif", std::string(TESTDATA_DIR) + "/data/perspective/projection_blobs_reference.tif");
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/perspective/intersection_lengths.tif", std::string(TESTDATA_DIR) + "/data/perspective/intersection_lengths_blobs_reference.tif");
}

TEST_F(ForwardProjectionTest, PerspectiveForwardMatchesBack)
{
    Volume* volumeOnCpu = new FloatVolume(Vec3ui(128, 128, 128), 0.0f);
    geometricSetup->setVolume(volumeOnCpu);
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto initialProjection = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/perspective/projection_reference.tif"));
    auto initialIntersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/perspective/intersection_lengths_reference.tif"));
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), initialProjection->getResolution());
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), initialIntersectionLengths->getResolution());
    initialProjection->takeOwnershipOfObjectOnCPU();
    initialIntersectionLengths->takeOwnershipOfObjectOnCPU();

    auto backKernel = new PerspectiveBeamsBackProjectionKernel(framework, geometricSetup, volume, nullptr, 1.0f, 8, false);
    backKernel->SetInput(initialProjection, initialIntersectionLengths);
    backKernel->run();
    delete backKernel;

    auto forwardKernel = new PerspectiveBeamsForwardProjectionKernel(framework, geometricSetup, volume, nullptr, 8);
    forwardKernel->setOutput(projection, intersectionLengths);
    forwardKernel->run();
    delete forwardKernel;

    projection->ensureIsUpToDateOnCPU();
    intersectionLengths->ensureIsUpToDateOnCPU();

    ImageComparator::assertImagesAreEqual(intersectionLengths->getObjectOnCPU(), initialIntersectionLengths->getObjectOnCPU());
    float rms = ImageComparator::getRMS(projection->getObjectOnCPU(), initialProjection->getObjectOnCPU());
    ASSERT_NEAR(0.0f, rms, 2e-2f);

    delete intersectionLengths;
    delete projection;
    delete initialIntersectionLengths;
    delete initialProjection;
    delete volume;
    delete volumeOnCpu;
}

TEST_F(ForwardProjectionTest, PerspectiveForwardBlobsMatchesBack)
{
    Volume* volumeOnCpu = new FloatVolume(Vec3ui(128, 128, 128), 0.0f);
    geometricSetup->setVolume(volumeOnCpu);
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto initialProjection = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/perspective/projection_reference.tif"));
    auto initialIntersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/perspective/intersection_lengths_blobs_reference.tif"));
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), initialProjection->getResolution());
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), initialIntersectionLengths->getResolution());
    initialProjection->takeOwnershipOfObjectOnCPU();
    initialIntersectionLengths->takeOwnershipOfObjectOnCPU();

    BlobParameters blobParams = BlobParameters::createDefault();
    auto blobSampler = std::make_shared<BlobRayIntegrationSampler>(blobParams, 128);

    auto backKernel = new PerspectiveBeamsBackProjectionWithBlobsKernel(framework, geometricSetup, volume, nullptr, blobParams, 1.0f, false);
    backKernel->SetInput(initialProjection, initialIntersectionLengths);
    backKernel->run();
    delete backKernel;

    auto forwardKernel = new PerspectiveBeamsForwardProjectionWithBlobsKernel(framework, blobSampler, geometricSetup, volume, nullptr);
    forwardKernel->setOutput(projection, intersectionLengths);
    forwardKernel->run();
    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/perspective/reprojection", forwardKernel->getVirtualProjection(), ImageSerializer::TIFF_GRAY_32);
    delete forwardKernel;

    projection->ensureIsUpToDateOnCPU();
    intersectionLengths->ensureIsUpToDateOnCPU();

    float rms = ImageComparator::getRMS(projection->getObjectOnCPU(), initialProjection->getObjectOnCPU());
    ASSERT_NEAR(0.0f, rms, 0.4f);

    delete intersectionLengths;
    delete projection;
    delete initialIntersectionLengths;
    delete initialProjection;
    delete volume;
    delete volumeOnCpu;
}