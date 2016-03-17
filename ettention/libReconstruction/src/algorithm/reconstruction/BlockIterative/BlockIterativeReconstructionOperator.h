#pragma once
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/volume/Volume.h"
#include "model/projectionset/ProjectionSetMaxAngle.h"
#include "algorithm/imagemanipulation/CompareKernel.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "algorithm/volumemanipulation/AccumulateVolumeKernel.h"
#include "algorithm/projections/forward/ForwardProjectionKernel.h"
#include "algorithm/projections/forward/ForwardProjectionOperator.h"
#include "algorithm/projections/back/BackProjectionKernel.h"
#include "algorithm/projections/back/BackProjectionOperator.h"
#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include "framework/time/Timer.h"
#include "framework/time/TimeUnits.h"
#include "framework/time/PerformanceReport.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionAlgorithmLogger.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeProjectionAccess.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeConfiguration.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeState.h"

namespace ettention
{
    class AlgebraicParameterSet;

    class BlockIterativeReconstructionOperator : public ReconstructionAlgorithm
    {
    public:
        BlockIterativeReconstructionOperator(Framework* framework);
        BlockIterativeReconstructionOperator(ImageStackDatasource* projectionDataSource, Framework* framework);
        ~BlockIterativeReconstructionOperator();

        void run() override;
        GPUMappedVolume* getReconstructedVolume() const override;
        void exportGeometryTo(Visualizer* visualizer) override;

    protected:
        virtual void InitializeKernels();
        virtual void DeleteKernels();
        virtual void HandleOneIteration();
        virtual void DoForwardProjectionForBlock(unsigned int block);
        virtual void ComputeResidualsForBlock(unsigned int block);
        virtual void DoBackProjectionForBlock(unsigned int block);
        virtual void HandleForwardProjection(unsigned int subVolume);
        virtual void CompensateLongObject();
        virtual void ComputeResidual();
        virtual void HandleBackProjection(unsigned int subVolume, unsigned int projectionBlockSize);

        BlockIterativeReconstructionAlgorithmLogger logger;

        BlockIterativeConfiguration* config;
        BlockIterativeProjectionAccess* projectionAccess;
        BlockIterativeState* state;

        CLAbstractionLayer* al;
        VirtualProjectionKernel* longObjectCompensationKernel;
        CompareKernel* compareKernel;

    };
}
