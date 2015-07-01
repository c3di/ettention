#include "stdafx.h"
#include <gtest/gtest.h>
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "unit/sart_components/forwardprojection/ForwardProjectionTest.h"

namespace ettention
{
    class ForwardProjection_HalfFloat16Test : public ForwardProjectionTest
    {
    public:
        void allocateEmptyVolume(Vec3ui volumeResolution) override
        {
            volume = new Volume(VolumeProperties(VoxelType::HALF_FLOAT_16, VolumeParameterSet(volumeResolution), 1), 1.0f);
        }
    };
}

using namespace ettention;

TEST_F(ForwardProjection_HalfFloat16Test, Unit_ForwardProjection)
{
    auto manualSource = new ManualParameterSource;
    framework->parseAndAddParameterSource(manualSource);
    manualSource->setParameter("voxelRepresentation", "half");
    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, 1);

    testUnitForwardProjection(kernel);

    delete kernel;
    delete manualSource;
}