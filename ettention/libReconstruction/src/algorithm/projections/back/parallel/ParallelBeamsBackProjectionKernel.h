#pragma once
#include "algorithm/projections/back/BackProjectionKernel.h"

namespace ettention
{
    class ParallelBeamsBackProjectionKernel : public BackProjectionKernel
    {
    public:
        ParallelBeamsBackProjectionKernel(Framework* framework,
                                          const GeometricSetup* geometricSetup,
                                          GPUMappedVolume* volume,
                                          GPUMappedVolume* priorKnowledgeMask,
                                          float lambda,
                                          unsigned int samples,
                                          bool useLongObjectCompensation);
        ~ParallelBeamsBackProjectionKernel();

        void setSamples(unsigned int samples) override;
        std::vector<VoxelSample> generateSamplingPatternForPerVoxelSampling();

    protected:
        void prepareKernelArguments() override;
        
    private:
        unsigned int getSamplesPerDirection();

        GPUMapped<Matrix4x4>* transform;
        GPUMappedSampleVector* voxelSamples;
    };
}
