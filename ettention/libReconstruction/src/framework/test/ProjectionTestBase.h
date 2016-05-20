#pragma once
#include "framework/test/TestBase.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/geometry/SingleAxisTiltRotator.h"
#include "model/volume/GPUMappedVolume.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "setup/parameterset/IOEnumerations.h"

namespace ettention
{
    class ProjectionTestBase : public TestBase
    {
    public:
        ProjectionTestBase(std::string testdataDirectory, std::string workDirectory);
        virtual ~ProjectionTestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;

        virtual void allocateEmptyVolume(Vec3ui volumeResolution);
        virtual void allocateVolume();
        virtual void makeBlobInVolume(Vec3f position, float radius, float intensity, Volume* volume);

        void allocateGeometricSetup();

        void writeOutDeviceBuffer(const std::string filename, CLMemBuffer* deviceBuffer);
        void writeOutVolume(std::string filename, GPUMappedVolume* volume, CoordinateOrder order = ORDER_XZY);
        ScannerGeometry* getPerspectiveTestGeometry() const;

        std::string testdataDirectory;
        std::string workDirectory;
        Vec2ui projectionResolution;
        GeometricSetup* geometricSetup;
        SingleAxisTiltRotator *satRotator;
        Volume* volume;
        Vec3ui volumeResolution;
        VolumeParameterSet* volumeOptions;
    };
}