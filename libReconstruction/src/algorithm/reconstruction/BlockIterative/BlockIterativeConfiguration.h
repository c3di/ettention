#pragma once
#include "algorithm/projections/back/BackProjectionOperator.h"
#include "algorithm/projections/forward/ForwardProjectionOperator.h"
#include "algorithm/projections/back/Voxelizer.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeProjectionAccess.h"
#include "framework/geometry/Visualizer.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"

namespace ettention
{
    class BlockIterativeConfiguration
    {
    public:
        BlockIterativeConfiguration(Framework* framework, BlockIterativeProjectionAccess* projectionAccess);
        ~BlockIterativeConfiguration();

        GeometricSetup* GetGeometricSetup() const;
        ForwardProjectionOperator* GetForwardProjection() const;
        BackProjectionOperator* GetBackProjection() const;
        Voxelizer* GetVoxelizer() const;
        GPUMappedVolume* GetVolume() const;

    private:
        AlgebraicParameterSet* paramSet;
        BlockIterativeProjectionAccess* projectionAccess;
        GPUMappedVolume* volume;
        GPUMappedVolume* priorknowledgeMask;
        GeometricSetup* geometricSetup;
        ForwardProjectionOperator* forwardProjection;
        BackProjectionOperator* backProjection;
        Voxelizer* voxelizer;

        void OptimizeMemoryUsage();
        void ExportGeometryToVisualizer(Visualizer& visualizer, float f);

    };
}