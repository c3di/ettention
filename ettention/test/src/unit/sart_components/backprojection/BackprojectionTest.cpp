#include "stdafx.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionWithBlobsKernel.h"
#include "framework/test/BackProjectionTestBase.h"
#include "io/deserializer/ImageDeserializer.h"
#include "model/geometry/GeometricSetup.h"
#include "model/volume/StackComparator.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    class BackProjectionTest : public BackProjectionTestBase
    {
    public:
        BackProjectionTest()
            : BackProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
        {
        }

        ~BackProjectionTest()
        {
        }
    };
}

using namespace ettention;

// 
// Voxel by Voxel Back Projection Tests
//
TEST_F(BackProjectionTest, ZDirection)
{
    Image* image = ImageDeserializer::readImage(testdataDirectory + "back_projection/projection_z.tif");
    residual->setObjectOnCPU(image);    
    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(0.0f));
    
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
    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(45.0f));
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
    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(90.0f));

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
    geometricSetup->setScannerGeometry(satRotator->getRotatedScannerGeometry(45.0f));

    computeRayLength->run();
    performBackProjection();

    writeOutVolume(workDirectory + "back_projection/voxel_by_voxel_generated_45.mrc", accumulatedVolume);
    StackComparator::assertVolumesAreEqual(workDirectory + "back_projection/voxel_by_voxel_generated_45.mrc", testdataDirectory + "back_projection/voxel_by_voxel_reference_45.mrc");

    delete image;
}

TEST_F(BackProjectionTest, Perspective)
{
    Volume* volumeOnCpu = new FloatVolume(Vec3ui(128, 128, 128), boost::none);
    geometricSetup->setVolume(volumeOnCpu);
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(testdataDirectory + "perspective/projection_reference.tif"));
    projection->takeOwnershipOfObjectOnCPU();
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(testdataDirectory + "perspective/intersection_lengths_reference.tif"));
    intersectionLengths->takeOwnershipOfObjectOnCPU();

    auto kernel = new PerspectiveBeamsBackProjectionKernel(framework, geometricSetup, volume, nullptr, 1.0f, 8, false);
    kernel->SetInput(projection, intersectionLengths);
    kernel->run();
    writeOutVolume(workDirectory + "perspective/volume_generated.mrc", volume, ORDER_XYZ);
    delete kernel;

    delete intersectionLengths;
    delete projection;
    delete volume;
    delete volumeOnCpu;

    //float rms = StackComparator::getRMSBetweenVolumes(workDirectory + "perspective/volume_generated.mrc", testdataDirectory + "perspective/volume_reference.mrc");
    StackComparator::assertVolumesAreEqual(workDirectory + "perspective/volume_generated.mrc", testdataDirectory + "perspective/volume_reference.mrc");
}

TEST_F(BackProjectionTest, PerspectiveBlobs)
{
    Volume* volumeOnCpu = new FloatVolume(Vec3ui(128, 128, 128), boost::none);
    geometricSetup->setVolume(volumeOnCpu);
    geometricSetup->setScannerGeometry(getPerspectiveTestGeometry());

    GPUMappedVolume* volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
    auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(testdataDirectory + "perspective/projection_blobs_reference.tif"));
    projection->takeOwnershipOfObjectOnCPU();
    auto intersectionLengths = new GPUMapped<Image>(framework->getOpenCLStack(), ImageDeserializer::readImage(testdataDirectory + "perspective/intersection_lengths_blobs_reference.tif"));
    intersectionLengths->takeOwnershipOfObjectOnCPU();

    auto kernel = new PerspectiveBeamsBackProjectionWithBlobsKernel(framework, geometricSetup, volume, nullptr, BlobParameters::createDefault(), 1.0f, false);
    kernel->SetInput(projection, intersectionLengths);
    kernel->run();
    writeOutVolume(workDirectory + "perspective/volume_generated.mrc", volume, ORDER_XYZ);
    delete kernel;

    delete intersectionLengths;
    delete projection;
    delete volume;
    delete volumeOnCpu;

    //float rms = StackComparator::getRMSBetweenVolumes(workDirectory + "perspective/volume_generated.mrc", testdataDirectory + "perspective/volume_reference.mrc");
    StackComparator::assertVolumesAreEqual(workDirectory + "perspective/volume_generated.mrc", testdataDirectory + "perspective/volume_blobs_reference.mrc");
}
