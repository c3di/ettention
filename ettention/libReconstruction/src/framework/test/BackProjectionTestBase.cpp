#include "stdafx.h"
#include "BackProjectionTestBase.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "setup/parametersource/ManualParameterSource.h"

namespace ettention
{
    BackProjectionTestBase::BackProjectionTestBase(std::string testdataDirectory, std::string workDirectory)
        : ProjectionTestBase(testdataDirectory, workDirectory)
    {
    }

    BackProjectionTestBase::~BackProjectionTestBase()
    {
    }

    void BackProjectionTestBase::InitOptimizationParameter()
    {
        manualParameterSource = new ManualParameterSource;
        manualParameterSource->setParameter("optimization.internalVoxelRepresentationType", "float");
        framework->parseAndAddParameterSource(manualParameterSource);
    }

    void BackProjectionTestBase::SetUp()
    {
        ProjectionTestBase::SetUp();
        InitOptimizationParameter();
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

    void BackProjectionTestBase::TearDown()
    {
        delete backProjectKernel;
        delete computeRayLength;
        delete rayLengthImage;
        delete residual;
        delete accumulatedVolume;
        delete manualParameterSource;
        ProjectionTestBase::TearDown();
    }

    void BackProjectionTestBase::allocateVolume()
    {
        Vec3ui volumeResolution(64, 64, 64);
        allocateEmptyVolume(volumeResolution);
    }

    void BackProjectionTestBase::performBackProjection()
    {
        residual->ensureIsUpToDateOnGPU();
        backProjectKernel->run();
    }
}