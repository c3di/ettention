#include "stdafx.h"
#include "ProjectionTestBase.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/volumemanipulation/AccumulateVolumeKernel.h"
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
#include "setup/parameterset/HardwareParameterSet.h"
#include "setup/parameterset/IOEnumerations.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/geometry/PerspectiveScannerGeometry.h"
#include "model/geometry/ParallelScannerGeometry.h"

namespace ettention
{
    ProjectionTestBase::ProjectionTestBase(std::string testdataDirectory, std::string workDirectory)
        : projectionResolution(Vec2ui(256, 256))
        , testdataDirectory(testdataDirectory)
        , workDirectory(workDirectory)
        , volume(0)
        , geometricSetup(0)
        , volumeOptions(0)
        , satRotator(new SingleAxisTiltRotator())
    {

    }

    ProjectionTestBase::~ProjectionTestBase()
    {

    }

    void ProjectionTestBase::SetUp()
    {
        TestBase::SetUp();
        Logger::getInstance();

        volumeResolution = Vec3ui(64, 64, 64);
        volumeOptions = new VolumeParameterSet(volumeResolution);

        allocateVolume();
        allocateGeometricSetup();
    }

    void ProjectionTestBase::TearDown()
    {
        delete volume;
        delete geometricSetup;
        geometricSetup = nullptr;
        delete volumeOptions;
        delete satRotator;
        TestBase::TearDown();
    }

    void ProjectionTestBase::allocateEmptyVolume(Vec3ui volumeResolution)
    {
        delete volume;
        volume = new FloatVolume(volumeResolution, 0.0f);
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
                    if(distance.getLength() < radius)
                    {
                        volume->setVoxelToValue(Vec3ui(x, y, z), intensity);
                    }
                }
            }
        }

    }

    void ProjectionTestBase::allocateGeometricSetup()
    {
        Vec3f sourcePosition(-64.0f, -64.0f, -128.0f);
        Vec3f detectorPosition(-64.0f, -64.0f, 128.0f);
        Vec3f horizontalPitch(0.5f, 0.0f, 0.0f);
        Vec3f verticalPitch(0.0f, 0.5f, 0.0f);
        auto initialScannerGeometry = new ParallelScannerGeometry(projectionResolution);
        initialScannerGeometry->set(sourcePosition,
                                   detectorPosition,
                                   horizontalPitch,
                                   verticalPitch);
        satRotator->setBaseScannerGeometry(initialScannerGeometry);
        delete geometricSetup;
        geometricSetup = new GeometricSetup(initialScannerGeometry->clone(), volume);
    }

    void ProjectionTestBase::writeOutDeviceBuffer(const std::string filename, CLMemBuffer* deviceBuffer)
    {
        std::vector<float> imageBuffer(projectionResolution.x * projectionResolution.y);
        deviceBuffer->transferTo(&imageBuffer[0], imageBuffer.size() * sizeof(float));
        Image image(projectionResolution, &imageBuffer[0]);
        ImageSerializer::writeImage(filename.c_str(), &image, ImageSerializer::TIFF_GRAY_32);
    }

    void ProjectionTestBase::writeOutVolume(std::string filename, GPUMappedVolume* volume, CoordinateOrder order)
    {
        volume->ensureIsUpToDateOnCPU();
        MRCWriter writer;
        const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, order);
        writer.write(volume->getObjectOnCPU(), filename, &format);
    }

    ScannerGeometry* ProjectionTestBase::getPerspectiveTestGeometry() const
    {
        auto geometry = new PerspectiveScannerGeometry( Vec2ui(128, 128) );
        geometry->set( Vec3f(0.0f, 0.0f, -700.0f),
                      Vec3f(-0.10510424f, -0.10510424f, -699.0f),
                      Vec3f(0.0016422537f, 0.0f, 0.0f),
                      Vec3f(0.0f, 0.0016422537f, 0.0f) );
        return geometry;
    }
}