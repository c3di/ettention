#include "stdafx.h"
#include "BackprojectionTest.h"
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "framework/VectorAlgorithms.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "framework/test/ProjectionTestBase.h"
#include "unit/sart_components/backprojection/BackprojectionTest.h"
#include "setup/parametersource/ManualParameterSource.h"

namespace ettention
{
    BackProjectionTest::BackProjectionTest()
        : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
    {
    }

    void BackProjectionTest::InitOptimizationParameter()
    {
        manualParameterSource = new ManualParameterSource;
        manualParameterSource->setParameter("voxelRepresentation", "float");
        framework->parseAndAddParameterSource(manualParameterSource);
    }

    void BackProjectionTest::SetUp()
    {
        ProjectionTestBase::SetUp();
        InitOptimizationParameter();
        accumulatedVolume = new GPUMappedVolume(clal, volume);
        residual = new GPUMapped<Image>(clal, projectionResolution);
        rayLengthImage = new GPUMapped<Image>(clal, projectionResolution);
        computeRayLength = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);
        backProjectKernel = new ParallelBeamsBackProjectionKernel(framework,
                                                                  geometricSetup,
                                                                  accumulatedVolume,
                                                                  1.0f,
                                                                  1,
                                                                  false);
        backProjectKernel->SetInput(residual, computeRayLength->getOutput());
    }

    void BackProjectionTest::TearDown()
    {
        delete backProjectKernel;
        delete computeRayLength;
        delete rayLengthImage;
        delete residual;
        delete accumulatedVolume;
        delete manualParameterSource;
        ProjectionTestBase::TearDown();
    }

    void BackProjectionTest::allocateVolume()
    {
        Vec3ui volumeResolution(64, 64, 64);
        allocateEmptyVolume(volumeResolution);
    }

    void BackProjectionTest::performBackProjection()
    {
        residual->ensureIsUpToDateOnGPU();
        backProjectKernel->run();
    }
}

using namespace ettention;

// 
// Voxel by Voxel Back Projection Tests
//
TEST_F(BackProjectionTest, ZDirection)
{
    Image* image = ImageDeserializer::readImage(testdataDirectory + "back_projection/projection_z.tif");
    residual->setObjectOnCPU(image);    
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(0.0f));
    
    computeRayLength->run();
    performBackProjection();

    writeOutVolume(workDirectory + "back_projection/voxel_by_voxel_generated_z.mrc", accumulatedVolume);
    StackComparator::assertVolumesAreEqual(workDirectory + "back_projection/voxel_by_voxel_generated_z.mrc", testdataDirectory + "back_projection/voxel_by_voxel_reference_z.mrc");    

    delete image;
}      

TEST_F(BackProjectionTest, Sampling_Pattern_Generation_ZDirection)
{
    ParallelBeamsBackProjectionKernel* bpKernel = (ParallelBeamsBackProjectionKernel*) backProjectKernel;

    bpKernel->setSamples(1);
    auto samples = bpKernel->generateSamplingPatternForPerVoxelSampling();

    ASSERT_EQ(samples.size(), 1);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().x, 0.0);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().y, 0.0);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().z, 0.0);
    ASSERT_FLOAT_EQ(samples[0].RayDistanceInsideVoxel(), 1.0f);

    bpKernel->setSamples(4);
    samples = bpKernel->generateSamplingPatternForPerVoxelSampling();

    ASSERT_EQ(samples.size(), 4);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().x, -0.25f);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().y, -0.25f);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().z, 0.0f);
    ASSERT_FLOAT_EQ(samples[0].RayDistanceInsideVoxel(), 1.0f);

    ASSERT_FLOAT_EQ(samples[1].PositionOffset().x, 0.25f);
    ASSERT_FLOAT_EQ(samples[1].PositionOffset().y, -0.25f);
    ASSERT_FLOAT_EQ(samples[1].PositionOffset().z, 0.0f);
    ASSERT_FLOAT_EQ(samples[1].RayDistanceInsideVoxel(), 1.0f);

    ASSERT_FLOAT_EQ(samples[2].PositionOffset().x, -0.25f);
    ASSERT_FLOAT_EQ(samples[2].PositionOffset().y, 0.25f);
    ASSERT_FLOAT_EQ(samples[2].PositionOffset().z, 0.0f);
    ASSERT_FLOAT_EQ(samples[2].RayDistanceInsideVoxel(), 1.0f);

    ASSERT_FLOAT_EQ(samples[3].PositionOffset().x, 0.25f);
    ASSERT_FLOAT_EQ(samples[3].PositionOffset().y, 0.25f);
    ASSERT_FLOAT_EQ(samples[3].PositionOffset().z, 0.0f);
    ASSERT_FLOAT_EQ(samples[3].RayDistanceInsideVoxel(), 1.0f);

    ASSERT_THROW(bpKernel->setSamples(0), Exception);
    ASSERT_THROW(bpKernel->setSamples(3), Exception);
    ASSERT_THROW(bpKernel->setSamples(5), Exception);
}

TEST_F(BackProjectionTest, Sampling_Pattern_Generation_45Direction)
{
    ParallelBeamsBackProjectionKernel* bpKernel = (ParallelBeamsBackProjectionKernel*)backProjectKernel;
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(45.0f));
    bpKernel->setSamples(1);
    auto samples = bpKernel->generateSamplingPatternForPerVoxelSampling();

    ASSERT_EQ(samples.size(), 1);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().x, 0.0);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().y, 0.0);
    ASSERT_FLOAT_EQ(samples[0].PositionOffset().z, 0.0);
    ASSERT_FLOAT_EQ(samples[0].RayDistanceInsideVoxel(), sqrtf(2.0f));
}


TEST_F(BackProjectionTest, YDirection)
{
    auto image = ImageDeserializer::readImage(testdataDirectory + "back_projection/projection_y.tif");
    residual->setObjectOnCPU(image);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(90.0f));

    computeRayLength->run();
    performBackProjection();
    writeOutVolume(workDirectory + "back_projection/voxel_by_voxel_generated_y.mrc", accumulatedVolume);
    StackComparator::assertVolumesAreEqual(workDirectory + "back_projection/voxel_by_voxel_generated_y.mrc", testdataDirectory + "back_projection/voxel_by_voxel_reference_y.mrc");

    delete image;
}      

TEST_F(BackProjectionTest, 45Direction)
{
    auto image = ImageDeserializer::readImage(testdataDirectory + "back_projection/projection_45.tif");
    residual->setObjectOnCPU(image);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(45.0f));

    computeRayLength->run();
    performBackProjection();

    writeOutVolume(workDirectory + "back_projection/voxel_by_voxel_generated_45.mrc", accumulatedVolume);
    StackComparator::assertVolumesAreEqual(workDirectory + "back_projection/voxel_by_voxel_generated_45.mrc", testdataDirectory + "back_projection/voxel_by_voxel_reference_45.mrc");

    delete image;
}
