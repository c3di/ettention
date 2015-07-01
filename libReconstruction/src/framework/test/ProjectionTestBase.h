#pragma once
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/geometry/SingleAxisTiltRotator.h"
#include "model/volume/GPUMappedVolume.h"
#include "framework/test/CLTestBase.h"

namespace ettention
{
    class ProjectionTestBase : public CLTestBase
    {
    public:
        ProjectionTestBase(std::string testdataDirectory, std::string workDirectory);
        virtual ~ProjectionTestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;

        virtual void allocateEmptyVolume(Vec3ui volumeResolution);
        virtual void allocateVolume();
        virtual void makeBlobInVolume(Vec3f position, float radius, float intensity, Volume* volume);

        void AllocateGeometricSetup();

        void writeOutDeviceBuffer(const std::string filename, CLMemBuffer* deviceBuffer);
        void writeOutVolume(std::string filename, GPUMappedVolume* volume);

        std::string testdataDirectory;
        std::string workDirectory;
        Vec2ui projectionResolution;
        GeometricSetup* geometricSetup;
        SingleAxisTiltRotator satRotator;
        Volume* volume;
        Vec3ui volumeResolution;
        VolumeParameterSet* volumeOptions;
    };
}