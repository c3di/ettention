#pragma once
#include "algorithm/projections/back/BackProjectionKernel.h"

namespace ettention
{
    class PerspectiveBeamsBackProjectionKernel : public BackProjectionKernel
    {
    public:
        PerspectiveBeamsBackProjectionKernel(Framework* framework,
                                             const GeometricSetup* geometricSetup,
                                             GPUMappedVolume* volume,
                                             GPUMappedVolume* priorKnowledgeMask,
                                             float lambda,
                                             unsigned int samplesPerVoxel,
                                             bool useLongObjectCompensation);
        ~PerspectiveBeamsBackProjectionKernel();

    protected:
        void prepareKernelArguments() override;
        void preRun() override;

    private:
        GPUMapped<Matrix4x4>* transform;
        unsigned int samplesPerVoxel;

        unsigned int computeSamplingFactor(const Vec2ui& projectionResolution, const Vec3ui& volumeResolution, unsigned int samplesPerVoxel) const;

        static std::string getKernelDefines(Framework* framework);
    };
}
