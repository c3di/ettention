#include "stdafx.h"
#include "BackprojectionTest_HalfFloat16.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/parametersource/ManualParameterSource.h"

namespace ettention
{
    void BackProjectionHalfFloat16Test::InitOptimizationParameter()
    {
        manualParameterSource = new ManualParameterSource;
        manualParameterSource->setParameter("voxelRepresentation", "half");
        framework->parseAndAddParameterSource(manualParameterSource);
    }

    void BackProjectionHalfFloat16Test::allocateEmptyVolume(Vec3ui volumeResolution)
    {
        volume = new Volume(VolumeProperties(VoxelType::HALF_FLOAT_16, VolumeParameterSet(volumeResolution), 1), 0.0f);
    }
}

using namespace ettention;

TEST_F(BackProjectionHalfFloat16Test, ZDirection)
{
    auto image = ImageDeserializer::readImage(testdataDirectory + "back_projection/projection_z.tif");
    residual->setObjectOnCPU(image);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(0.0f));

    computeRayLength->run();
    performBackProjection();
    writeOutVolume(workDirectory + "back_projection/voxel_by_voxel_generated_z.mrc", accumulatedVolume);
    StackComparator::assertVolumesAreEqual(workDirectory + "back_projection/voxel_by_voxel_generated_z.mrc", testdataDirectory + "back_projection/voxel_by_voxel_reference_z.mrc");

    delete image;
}