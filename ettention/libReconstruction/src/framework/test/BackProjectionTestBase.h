#pragma once
#include "framework/test/ProjectionTestBase.h"

namespace ettention
{
    class BackProjectionKernel;
    class ComputeRayLengthKernel;
    class ManualParameterSource;

    class BackProjectionTestBase : public ProjectionTestBase
    {
    public:
        BackProjectionTestBase(std::string testdataDirectory, std::string workDirectory);
        virtual ~BackProjectionTestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;
        virtual void performBackProjection();
        virtual void allocateVolume() override;
        virtual void InitOptimizationParameter();

        ComputeRayLengthKernel* computeRayLength;
        BackProjectionKernel* backProjectKernel;

        GPUMappedVolume* accumulatedVolume;
        GPUMapped<Image>* residual;
        GPUMapped<Image>* rayLengthImage;
        unsigned int subVolumeCount;
        ManualParameterSource* manualParameterSource;
    };
}