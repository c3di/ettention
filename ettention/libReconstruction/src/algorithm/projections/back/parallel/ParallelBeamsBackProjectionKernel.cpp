#include "stdafx.h"
#include "ParallelBeamsBackProjectionKernel.h"
#include "ParallelBeamsBackProjection_bin2c.h"
#include "framework/math/Matrix4x4.h"
#include "framework/geometry/BoundingBox.h"
#include "framework/geometry/GeometricAlgorithms.h"
#include "framework/geometry/Ray.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    ParallelBeamsBackProjectionKernel::ParallelBeamsBackProjectionKernel(Framework* framework,
                                                                         const GeometricSetup* geometricSetup,
                                                                         GPUMappedVolume* volume,
                                                                         GPUMappedVolume* priorKnowledgeMask,
                                                                         float lambda,
                                                                         unsigned int samples,
                                                                         bool useLongObjectCompensation)
        : BackProjectionKernel(framework,
                               ParallelBeamsBackProjection_kernel_SourceCode,
                               "correct",
                               "Backprojection (parallel,voxelwise,basis:voxel)",
                               geometricSetup,
                               volume,
                               priorKnowledgeMask,
                               lambda,
                               samples,
                               useLongObjectCompensation,
                               true)
    {
        voxelSamples = new GPUMappedSampleVector(abstractionLayer, 1);
        setSamples(samples);
        transform = new GPUMapped<Matrix4x4>(abstractionLayer, Vec2ui(4, 4));
    }

    ParallelBeamsBackProjectionKernel::~ParallelBeamsBackProjectionKernel()
    {
        delete voxelSamples;
        delete transform;
    }

    void ParallelBeamsBackProjectionKernel::prepareKernelArguments()
    {
        BackProjectionKernel::prepareKernelArguments();
        transform->setObjectOnCPU( new Matrix4x4( geometricSetup->getScannerGeometry()->getProjectionMatrix() ) );
        transform->takeOwnershipOfObjectOnCPU();
        implementation->setArgument("transform", transform);

        generateSamplingPatternForPerVoxelSampling();
        voxelSamples->ensureIsUpToDateOnGPU();
        implementation->setArgument("samples", voxelSamples);
    }

    void ParallelBeamsBackProjectionKernel::setSamples(unsigned int samples)
    {
        if(samples == 0)
            throw Exception("zero samples not supported for BackProjection");

        this->samples = samples;
        if(getSamplesPerDirection() * getSamplesPerDirection() != samples)
            throw Exception("non-square root number of samples not supported for BackProjection");

        voxelSamples->getObjectOnCPU()->resize(samples);
        voxelSamples->reallocateGPUBuffer();
        //generateSamplingPatternForPerVoxelSampling();
    };

    std::vector<VoxelSample> ParallelBeamsBackProjectionKernel::generateSamplingPatternForPerVoxelSampling()
    {
        Vec3f voxelCenter = Vec3f(0.0f, 0.0f, 0.0f);

        auto voxelSize = volume->getObjectOnCPU()->getProperties().getVoxelSize();
        float maxVoxelSize = fmax(fmax(voxelSize.x, voxelSize.y), voxelSize.z);
        Vec3f h = doNormalize(geometricSetup->getScannerGeometry()->getHorizontalPitch()) * maxVoxelSize;
		Vec3f v = doNormalize(geometricSetup->getScannerGeometry()->getVerticalPitch()) * maxVoxelSize;

        unsigned int sampleIndex = 0;
        for(float y = 0.0f; y < (float)getSamplesPerDirection(); y += 1.0f)
        {
            float yPos = ((y + 0.5f) / (float)(getSamplesPerDirection())) - 0.5f;
            for(float x = 0.0f; x < (float)getSamplesPerDirection(); x += 1.0f)
            {
                float xPos = ((x + 0.5f) / (float)(getSamplesPerDirection())) - 0.5f;
                Vec3f samplePos = voxelCenter + xPos * h + yPos * v;

                BoundingBox3f aabb(voxelSize * -0.5f, voxelSize * 0.5f);
				auto tLimits = GeometricAlgorithms::getClipRayAgainstBoundingBox(Ray::createFromPointAndDirection(samplePos, geometricSetup->getScannerGeometry()->getRayDirection()), aabb);
                float distance = tLimits.second - tLimits.first;

                voxelSamples->getObjectOnCPU()->at(sampleIndex) = VoxelSample(samplePos, distance);
                sampleIndex++;
            }
        }

        voxelSamples->markAsChangedOnCPU();
        return *voxelSamples->getObjectOnCPU();
    }

    unsigned int ParallelBeamsBackProjectionKernel::getSamplesPerDirection()
    {
        return (unsigned int)sqrtf((float)samples);
    }
}
