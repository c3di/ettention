#pragma once
#include "algorithm/projections/back/BackProjectionOperator.h"
#include "algorithm/projections/forward/ForwardProjectionOperator.h"
#include "algorithm/projections/back/Voxelizer.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeProjectionAccess.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    enum CoordinateOrder;

    class BlockIterativeConfiguration
    {
    public:
        BlockIterativeConfiguration(Framework* framework, BlockIterativeProjectionAccess* projectionAccess);
        ~BlockIterativeConfiguration();

        GeometricSetup* getGeometricSetup() const;
        ForwardProjectionOperator* getForwardProjection() const;
        BackProjectionOperator* getBackProjection() const;
        Voxelizer* getVoxelizer() const;
        GPUMappedVolume* getVolume() const;

        GPUMappedVolume* getPriorknowledgeMask() const;
        void setPriorknowledgeMask(GPUMappedVolume* volume);

    private:
        AlgebraicParameterSet* paramSet;
        BlockIterativeProjectionAccess* projectionAccess;
        GPUMappedVolume* volume;
        GPUMappedVolume* priorknowledgeMask;
        GeometricSetup* geometricSetup;
        ForwardProjectionOperator* forwardProjection;
        BackProjectionOperator* backProjection;
        Voxelizer* voxelizer;

        void optimizeMemoryUsage();
        void applyInitialVolumeToWorkingVolume(Volume* initialVolume, Volume* workingVolume, CoordinateOrder initialVolumeOrientation);
        Vec3ui translateCoordinates(const Vec3ui& from, CoordinateOrder order);
    };
}