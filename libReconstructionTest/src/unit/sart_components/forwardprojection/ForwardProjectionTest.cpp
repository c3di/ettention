#include "stdafx.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "framework/test/ProjectionTestBase.h"
#include "framework/VectorAlgorithms.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/datasource/MRCStack.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/GPUMappedVolume.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "setup/ParameterSet/PriorKnowledgeParameterSet.h"
#include "unit/sart_components/forwardprojection/ForwardProjectionTest.h"

namespace ettention
{
    ForwardProjectionTest::ForwardProjectionTest()
        : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
    {
    }

    void ForwardProjectionTest::SetUp()
    {
        ProjectionTestBase::SetUp();
        mappedVolume = new GPUMappedVolume(clal, volume);
        mappedVolume->ensureIsUpToDateOnGPU();
    }

    void ForwardProjectionTest::TearDown()
    {
        delete mappedVolume;
        ProjectionTestBase::TearDown();
    }

    void ForwardProjectionTest::allocateEmptyVolume(Vec3ui volumeResolution)
    {
        volume = new Volume(VolumeProperties(VoxelType::FLOAT_32, VolumeParameterSet(volumeResolution), 1), 1.0f);
    }

    void ForwardProjectionTest::testUnitForwardProjection(ParallelBeamsForwardProjectionKernel* kernel)
    {
        GPUMapped<Image>* projection = new GPUMapped<Image>(clal, projectionResolution);
        GPUMapped<Image>* traversalLength = new GPUMapped<Image>(clal, projectionResolution);
        kernel->SetOutput(projection, traversalLength);

        geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(45.0f));
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
    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, 1);

    testUnitForwardProjection(kernel);

    delete kernel;
}

TEST_F(ForwardProjectionTest, Unit_ForwardProjection_Raysign)
{
    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, 1);
    GPUMapped<Image>* projection = new GPUMapped<Image>(clal, projectionResolution);
    GPUMapped<Image>* traversalLength = new GPUMapped<Image>(clal, projectionResolution);
    kernel->SetOutput(projection, traversalLength);

    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(225.0f));
    kernel->run();

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/forward_projection/generated_opposite_direction", projection, ImageSerializer::ImageFormat::TIFF_GRAY_32);
    ImageComparator::assertImagesAreEqual(std::string(TESTDATA_DIR) + "/work/forward_projection/generated_opposite_direction.tif", std::string(TESTDATA_DIR) + "/data/forward_projection/projection_45.tif");

    delete traversalLength;
    delete projection;
    delete kernel;
}

TEST_F(ForwardProjectionTest, Unit_ForwardProjectionMatchesBackProjection)
{
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(45.0f));

    Image* projectionImage = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/forward_projection/projection_45.tif");
    Image* traversalLengthImage  = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/forward_projection/raylength_reference.tif");
    GPUMapped<Image>* projection = new GPUMapped<Image>(clal, projectionImage);
    GPUMapped<Image>* traversalLength = new GPUMapped<Image>(clal, traversalLengthImage);
    auto generatedProjection = new GPUMapped<Image>(clal, projection->getResolution());
    auto generatedTraversalLength = new GPUMapped<Image>(clal, traversalLength->getResolution());
    Volume* cpuVolume = new Volume(volumeResolution);
    GPUMappedVolume* outputVolume = new GPUMappedVolume(clal, cpuVolume);

    // back projection
    ParallelBeamsBackProjectionKernel* backKernel = new ParallelBeamsBackProjectionKernel(framework, geometricSetup, outputVolume, 1.0f, 1, false);
    backKernel->SetInput(projection, traversalLength);
    backKernel->run();
    delete backKernel;

    // forward projection
    ParallelBeamsForwardProjectionKernel* forwardKernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, 1);
    forwardKernel->SetOutput(generatedProjection, generatedTraversalLength);
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
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(45.0f));
    GPUMapped<Image>* rayLengthImage = new GPUMapped<Image>(clal, projectionResolution);

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

