#include "stdafx.h"
#include "BlockIterativeReconstructionOperator.h"
#include <boost/format.hpp>
#include "algorithm/projections/back/BackProjectionOperator.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionKernel.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"
#include "framework/geometry/Visualizer.h"
#include "framework/Logger.h"
#include "framework/VectorAlgorithms.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/ImageStackDataSourceFactory.h"
#include "io/datasource/MRCStack.h"
#include "io/rangetransformation/LinearRangeTransformation.h"
#include "io/rangetransformation/IdentityRangeTransformation.h"
#include "io/serializer/VolumeSerializer.h"
#include "model/geometry/GeometricSetup.h"
#include "model/projectionset/factory/ProjectionIteratorFactory.h"
#include "setup/parameterset/DebugParameterSet.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/DebugParameterSet.h"

namespace ettention
{
    BlockIterativeReconstructionOperator::BlockIterativeReconstructionOperator(Framework* framework)
        : ReconstructionAlgorithm(framework)
        , al(framework->getOpenCLStack())
        , longObjectCompensationKernel(0)
    {
        projectionAccess = new BlockIterativeProjectionAccess(framework);
        state = new BlockIterativeState(al, projectionAccess);
        config = new BlockIterativeConfiguration(framework, projectionAccess);
        this->InitializeKernels();
    }

    BlockIterativeReconstructionOperator::BlockIterativeReconstructionOperator(ImageStackDatasource* projectionDataSource, Framework* framework)
        : ReconstructionAlgorithm(framework)
        , al(framework->getOpenCLStack())
        , longObjectCompensationKernel(0)
    {
        projectionAccess = new BlockIterativeProjectionAccess(framework, projectionDataSource);
        state = new BlockIterativeState(al, projectionAccess);
        config = new BlockIterativeConfiguration(framework, projectionAccess);
        this->InitializeKernels();
    }

    BlockIterativeReconstructionOperator::~BlockIterativeReconstructionOperator()
    {
        this->DeleteKernels();
        delete config;
        delete state;
        delete projectionAccess;
    }

    void BlockIterativeReconstructionOperator::run()
    {
        logger.OnReconstructionStart(framework, projectionAccess, state, config->GetVolume(), framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations(), framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation());
        for(unsigned int currentIteration = 0; currentIteration < framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations(); ++currentIteration)
        {
            logger.OnIterationStart(currentIteration);
            HandleOneIteration();
            logger.OnIterationEnd();
        }
        if(config->GetVoxelizer())
        {
            config->GetVoxelizer()->voxelize();
        }
        logger.OnReconstructionEnd();
    }

    GPUMappedVolume* BlockIterativeReconstructionOperator::getReconstructedVolume() const
    {
        if(config->GetVoxelizer())
        {
            return config->GetVoxelizer()->getVoxelVolume();
        }
        else
        {
            return config->GetVolume();
        }
    }

    void BlockIterativeReconstructionOperator::HandleOneIteration()
    {
        for(unsigned int block = 0; block < projectionAccess->GetBlockCount(); ++block)
        {
            state->setCurrentBlockIndex(block);
            logger.OnBlockStart();
            this->DoForwardProjectionForBlock(block);
            this->ComputeResidualsForBlock(block);
            this->DoBackProjectionForBlock(block);
            logger.OnBlockEnd();
        }
    }

    void BlockIterativeReconstructionOperator::DoForwardProjectionForBlock(unsigned int block)
    {
        for(unsigned int subVolume = 0; subVolume < config->GetVolume()->getProperties().getSubVolumeCount(); ++subVolume)
        {
            for(unsigned int projection = 0; projection < projectionAccess->getBlockSize(block); ++projection)
            {
                state->setCurrentProjectionIndex(projection);
                HandleForwardProjection(subVolume);
            }
        }
    }

    void BlockIterativeReconstructionOperator::ComputeResidualsForBlock(unsigned int block)
    {
        for(unsigned int projection = 0; projection < projectionAccess->getBlockSize(block); ++projection)
        {
            state->setCurrentProjectionIndex(projection);
            CompensateLongObject();
            ComputeResidual();
        }
    }

    void BlockIterativeReconstructionOperator::DoBackProjectionForBlock(unsigned int block)
    {
        auto subVolumeCount = config->GetVolume()->getProperties().getSubVolumeCount();
        for( unsigned int subVolume = 0; subVolume < subVolumeCount; ++subVolume )
        {
            for(unsigned int projection = 0; projection < projectionAccess->getBlockSize(block); ++projection)
            {
                state->setCurrentProjectionIndex(projection);
                HandleBackProjection(subVolumeCount - subVolume - 1, projectionAccess->getBlockSize(block));
            }
        }
    }

    void BlockIterativeReconstructionOperator::HandleForwardProjection(unsigned int subVolume)
    {
        logger.OnForwardProjectionStart(subVolume);
        config->GetGeometricSetup()->setScannerGeometry(state->getCurrentScannerGeometry());
        config->GetForwardProjection()->SetOutput(state->getVirtualProjection(), state->getTraversalLength());
        config->GetForwardProjection()->Execute(subVolume);
        logger.OnForwardProjectionEnd();
    }

    void BlockIterativeReconstructionOperator::CompensateLongObject()
    {
        if(framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
        {
            logger.OnLongObjectCompensationStart();
            auto volumeBoundingBox = config->GetVolume()->getObjectOnCPU()->getProperties().getVolumeBoundingBox();
            auto traversalLength = config->GetGeometricSetup()->getScannerGeometry()->getTraversalLengthThroughBoundingBox(volumeBoundingBox);
            longObjectCompensationKernel->setConstantLength(traversalLength);
            longObjectCompensationKernel->setProjection(state->getVirtualProjection());
            longObjectCompensationKernel->setTraversalLength(state->getTraversalLength());
            longObjectCompensationKernel->run();
            logger.OnLongObjectCompensationEnd();
        }
    }

    void BlockIterativeReconstructionOperator::ComputeResidual()
    {
        logger.OnResidualComputationStart();
        compareKernel->setInput(state->getCurrentRealProjection(), state->getVirtualProjection());
        compareKernel->setOutput(state->getResidual());
        compareKernel->run();
        logger.OnResidualComputationEnd();
    }

    void BlockIterativeReconstructionOperator::HandleBackProjection(unsigned int subVolume, unsigned int projectionBlockSize)
    {
        logger.OnBackProjectionStart(subVolume);
        config->GetGeometricSetup()->setScannerGeometry(state->getCurrentScannerGeometry());
        config->GetBackProjection()->setInput(state->getResidual(), state->getTraversalLength(), projectionBlockSize);
        config->GetBackProjection()->execute(subVolume);
        config->GetBackProjection()->setInput(0, 0, 1);
        logger.OnBackProjectionEnd();
    }

    void BlockIterativeReconstructionOperator::InitializeKernels()
    {
        compareKernel = new CompareKernel(framework, projectionAccess->getProjectionResolution(), nullptr, nullptr);
        if(framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
        {
            longObjectCompensationKernel = new VirtualProjectionKernel(framework, nullptr, nullptr);
        }
    }

    void BlockIterativeReconstructionOperator::DeleteKernels()
    {
        delete longObjectCompensationKernel;
        delete compareKernel;
    }

    void BlockIterativeReconstructionOperator::exportGeometryTo(Visualizer* visualizer)
    {
        projectionAccess->getImageStack()->exportAllGeometriesTo(visualizer, framework->getParameterSet());
        visualizer->appendBoundingBox(getReconstructedVolume()->getObjectOnCPU()->getProperties().getVolumeBoundingBox(), Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    }
}