#include "stdafx.h"
#include "framework/test/ProjectionTestBase.h"

namespace ettention
{
    class ParallelBeamsForwardProjectionKernel;

    class ForwardProjectionTest : public ProjectionTestBase
    {
    public:
        ForwardProjectionTest();

        void SetUp() override;
        void TearDown() override;
        void allocateEmptyVolume(Vec3ui volumeResolution);
        void testUnitForwardProjection(ParallelBeamsForwardProjectionKernel* kernel);

        GPUMappedVolume* mappedVolume;
    };
}