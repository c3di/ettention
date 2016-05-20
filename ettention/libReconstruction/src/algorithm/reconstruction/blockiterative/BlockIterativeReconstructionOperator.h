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
#include "algorithm/reconstruction/ReconstructionAlgorithmUsingMask.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeReconstructionAlgorithmLogger.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeProjectionAccess.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeConfiguration.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeState.h"
#include "framework/time/Timer.h"
#include "framework/time/TimeUnits.h"
#include "framework/time/PerformanceReport.h"

namespace ettention
{
    class AlgebraicParameterSet;

    class BlockIterativeReconstructionOperator : public ReconstructionAlgorithmUsingMask
    {
    public:
        BlockIterativeReconstructionOperator(Framework* framework);
        BlockIterativeReconstructionOperator(ImageStackDatasource* projectionDataSource, Framework* framework);
        ~BlockIterativeReconstructionOperator();

        void run() override;
        void runSingleIteration() override;
        GPUMappedVolume* getReconstructedVolume() const override;
        void exportGeometryTo(Visualizer* visualizer) override;

    protected:
        virtual void runNiterations(unsigned int iterationsCount = 1u);
        virtual void initializeKernels();
        virtual void deleteKernels();
        virtual void handleOneIteration();
        virtual void doForwardProjectionForBlock(unsigned int block);
        virtual void computeResidualsForBlock(unsigned int block);
        virtual void doBackProjectionForBlock(unsigned int block);
        virtual void handleForwardProjection(unsigned int subVolume);
        virtual void compensateLongObject();
        virtual void computeResidual();
        virtual void handleBackProjection(unsigned int subVolume, unsigned int projectionBlockSize);

        virtual void setMaskVolume(GPUMappedVolume* volume);
        virtual GPUMappedVolume* getMaskVolume() const;

        BlockIterativeReconstructionAlgorithmLogger logger;

        BlockIterativeConfiguration* config;
        BlockIterativeProjectionAccess* projectionAccess;
        BlockIterativeState* state;

        CLAbstractionLayer* al;
        VirtualProjectionKernel* longObjectCompensationKernel;
        CompareKernel* compareKernel;

    };
}
