#include "stdafx.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/volumemanipulation/AccumulateVolumeKernel.h"
#include "framework/test/ProjectionTestBase.h"
#include "framework/VectorAlgorithms.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "io/datasource/MRCStack.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/serializer/VolumeSerializer.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "setup/parametersource/ManualParameterSource.h"

namespace ettention
{
    ProjectionTestBase::ProjectionTestBase(std::string testdataDirectory, std::string workDirectory)
        : CLTestBase(HardwareParameterSet::nvidia, HardwareParameterSet::GPU_only)
        , projectionResolution(Vec2ui(256, 256))
        , testdataDirectory(testdataDirectory)
        , workDirectory(workDirectory)
    {

    }

    ProjectionTestBase::~ProjectionTestBase()
    {

    }

    void ProjectionTestBase::SetUp()
    {
        CLTestBase::SetUp();
        Logger::getInstance();

        volumeResolution = Vec3ui(64, 64, 64);
        volumeOptions = new VolumeParameterSet(volumeResolution);

        allocateVolume();
        AllocateGeometricSetup();
    }

    void ProjectionTestBase::TearDown()
    {
        delete geometricSetup;
        delete volumeOptions;
        delete volume;
        CLTestBase::TearDown();
    }

    void ProjectionTestBase::allocateEmptyVolume(Vec3ui volumeResolution)
    {
        volume = new Volume(VolumeProperties(VoxelType::FLOAT_32, *volumeOptions, 1), 0.0f);
    }

    void ProjectionTestBase::allocateVolume()
    {
        allocateEmptyVolume(volumeResolution);

        for(float x = 16.0f; x <= 48.0f; x += 16.0f)
        for(float y = 16.0f; y <= 48.0f; y += 16.0f)
        for(float z = 16.0f; z <= 48.0f; z += 16.0f)
            makeBlobInVolume(Vec3f(x, y, z), 1, 16.0f, volume);

        makeBlobInVolume(Vec3f(16.0, 16.0, 16.0), 3, 16.0f, volume);
        makeBlobInVolume(Vec3f(32.0, 32.0, 32.0), 3, 16.0f, volume);
        makeBlobInVolume(Vec3f(48.0, 48.0, 48.0), 3, 16.0f, volume);
    }

    void ProjectionTestBase::makeBlobInVolume(Vec3f position, float radius, float intensity, Volume* volume)
    {
        Vec3i rangeMin = Vec3i((int)position.x, (int)position.y, (int)position.z) - Vec3i((int)radius, (int)radius, (int)radius);
        Vec3i rangeMax = Vec3i((int)position.x, (int)position.y, (int)position.z) + Vec3i((int)radius, (int)radius, (int)radius);
        for(int z = rangeMin.z; z <= rangeMax.z; z++)
        {
            for(int y = rangeMin.y; y <= rangeMax.y; y++)
            {
                for(int x = rangeMin.x; x <= rangeMax.x; x++)
                {
                    Vec3f distance = Vec3f((float)x, (float)y, (float)z) - position;
                    if(distance.Length() < radius)
                    {
                        volume->setVoxelToValue(Vec3ui(x, y, z), intensity);
                    }
                }
            }
        }

    }

    void ProjectionTestBase::AllocateGeometricSetup()
    {
        Vec3f sourcePosition(-64.0f, -64.0f, -128.0f);
        Vec3f detectorPosition(-64.0f, -64.0f, 128.0f);
        Vec3f horizontalPitch(0.5f, 0.0f, 0.0f);
        Vec3f verticalPitch(0.0f, 0.5f, 0.0f);
        ScannerGeometry initialScannerGeometry(ScannerGeometry::PROJECTION_PARALLEL, projectionResolution);
        initialScannerGeometry.set(sourcePosition,
                                   detectorPosition,
                                   horizontalPitch,
                                   verticalPitch);
        satRotator.SetBaseScannerGeometry(initialScannerGeometry);
        geometricSetup = new GeometricSetup(initialScannerGeometry, volume);
    }

    void ProjectionTestBase::writeOutDeviceBuffer(const std::string filename, CLMemBuffer* deviceBuffer)
    {
        std::vector<float> imageBuffer(projectionResolution.x * projectionResolution.y);
        deviceBuffer->transferTo(&imageBuffer[0]);
        Image image(projectionResolution, &imageBuffer[0]);
        ImageSerializer::writeImage(filename.c_str(), &image, ImageSerializer::TIFF_GRAY_32);
    }

    void ProjectionTestBase::writeOutVolume(std::string filename, GPUMappedVolume* volume)
    {
        volume->ensureIsUpToDateOnCPU();
        MRCWriter writer;
        const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32, OutputFormatParameterSet::ORDER_XZY);
        writer.write(volume->getObjectOnCPU(), filename, &format);
    }
}