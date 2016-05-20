#include "stdafx.h"
#include <gtest/gtest.h>
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "unit/sart_components/forwardprojection/ForwardProjectionTest.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    class ForwardProjection_HalfFloat16Test : public ForwardProjectionTest
    {
    public:
        void allocateEmptyVolume(Vec3ui volumeResolution) override
        {
            volume = new FloatVolume(volumeResolution, 1.0f, 1);
        }
    };
}

using namespace ettention;

TEST_F(ForwardProjection_HalfFloat16Test, Unit_ForwardProjection)
{
    auto manualSource = new ManualParameterSource;
    framework->parseAndAddParameterSource(manualSource);
    manualSource->setParameter("optimization.internalVoxelRepresentationType", "half");
    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, nullptr, 1);

    testUnitForwardProjection(kernel);

    delete kernel;
    delete manualSource;
}