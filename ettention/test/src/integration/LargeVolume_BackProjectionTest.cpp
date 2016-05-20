#include "stdafx.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/volumemanipulation/SetToValueKernel.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/datasource/MRCStack.h"
#include "framework/test/BackProjectionTestBase.h"
#include "framework/VectorAlgorithms.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "model/geometry/ParallelScannerGeometry.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    class VoxelwiseBackProjectionLarge : public BackProjectionTestBase
    {
    public:
        VoxelwiseBackProjectionLarge()
            : BackProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
        {}

        virtual void SetUp() override
        {
            TestBase::SetUp();
            Logger::getInstance();

            projectionResolution = Vec2ui(2048, 2048);
            Vec3ui volumeResolution;
            if(framework->on64bitArchitecture())
            {
                volumeResolution = Vec3ui(2000, 2000, 100);
            }
            else
            {
                volumeResolution = Vec3ui(2000, 2000, 100);
            }
            volumeOptions = new VolumeParameterSet(volumeResolution);
            subVolumeCount = 4;

            volume = new FloatVolume(volumeOptions->getResolution(), 1.0f, subVolumeCount);

            Vec3f sourcePosition(-1000.0f, -1000.0f, -100.0f);
            Vec3f detectorPosition(-1000.0f, -1000.0f, 100.0f);
            Vec3f horizontalPitch(1.0f, 0.0f, 0.0f);
            Vec3f verticalPitch(0.0f, 1.0f, 0.0f);
            ScannerGeometry* initialScannerGeometry = new ParallelScannerGeometry ( projectionResolution );
            initialScannerGeometry->set(sourcePosition,
                                       detectorPosition,
                                       horizontalPitch,
                                       verticalPitch);
            satRotator->setBaseScannerGeometry(initialScannerGeometry);
            geometricSetup = new GeometricSetup(initialScannerGeometry->clone(), volume);

            accumulatedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);
            residual = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
            rayLengthImage = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
            computeRayLength = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);
            backProjectKernel = new ParallelBeamsBackProjectionKernel(framework,
                                                                      geometricSetup,
                                                                      accumulatedVolume,
                                                                      nullptr,
                                                                      1.0f,
                                                                      1,
                                                                      false);
            backProjectKernel->SetInput(residual, computeRayLength->getOutput());
        }

        virtual void TearDown() override
        {
            delete backProjectKernel;
            delete computeRayLength;
            delete rayLengthImage;
            delete residual;
            delete accumulatedVolume;
            delete geometricSetup;
            delete satRotator;
            delete volume;
            delete volumeOptions;
            TestBase::TearDown();
        }

        virtual ~VoxelwiseBackProjectionLarge()
        {}
    };
}

using namespace ettention;

TEST_F(VoxelwiseBackProjectionLarge, ZDirection_LargeVolume)
{
    Image* image = new Image(projectionResolution);
    residual->setObjectOnCPU(image);
    geometricSetup->setScannerGeometry(satRotator->createRotatedScannerGeometry(0.0f));

    computeRayLength->run();
    performBackProjection();
    writeOutVolume(workDirectory + "back_projection/voxel_by_voxel_generated_y.mrc", accumulatedVolume);

    delete image;
}