#include "stdafx.h"
#include "framework/test/ProjectionTestBase.h"

namespace ettention
{
    class ParallelBeamsForwardProjectionKernel;

    class ForwardProjectionTest : public ProjectionTestBase
    {
    public:
        ForwardProjectionTest();

        virtual void SetUp() override;
        virtual void TearDown() override;
        virtual void allocateEmptyVolume(Vec3ui volumeResolution);
        virtual void testUnitForwardProjection(ParallelBeamsForwardProjectionKernel* kernel);
        GPUMappedVolume* mappedVolume;
    };
}