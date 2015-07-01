#include "stdafx.h"
#include "BlockIterativeReconstructionOperator.h"
#include <boost/format.hpp>
#include "algorithm/projections/back/BackProjectionOperator.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionKernel.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"
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
#include "setup/ParameterSet/DebugParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"

namespace ettention
{
    BlockIterativeReconstructionOperator::BlockIterativeReconstructionOperator(Framework* framework)
        : ReconstructionAlgorithm(framework)
        , al(framework->getOpenCLStack())
        , parameterSet(framework->getParameterSet())
        , longObjectCompensationKernel(0)
    {
        projectionAccess = new BlockIterativeProjectionAccess(framework, parameterSet);
        state = new BlockIterativeState(al, projectionAccess);
        config = new BlockIterativeConfiguration(framework, projectionAccess);
        this->InitializeKernels();
    }

    BlockIterativeReconstructionOperator::BlockIterativeReconstructionOperator(ImageStackDatasource* projectionDataSource, Framework* framework)
        : ReconstructionAlgorithm(framework)
        , al(framework->getOpenCLStack())
        , parameterSet(framework->getParameterSet())
        , longObjectCompensationKernel(0)
    {
        projectionAccess = new BlockIterativeProjectionAccess(framework, parameterSet, projectionDataSource);
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
        logger.OnReconstructionStart(framework, projectionAccess, state, config->GetVolume(), parameterSet->get<AlgorithmParameterSet>()->NumberOfIterations(), parameterSet->get<AlgorithmParameterSet>()->UseLongObjectCompensation());
        for(unsigned int currentIteration = 0; currentIteration < parameterSet->get<AlgorithmParameterSet>()->NumberOfIterations(); ++currentIteration)
        {
            logger.OnIterationStart(currentIteration);
            HandleOneIteration();
            logger.OnIterationEnd();
        }
        if(config->GetVoxelizer())
        {
            config->GetVoxelizer()->Voxelize();
        }
        logger.OnReconstructionEnd();
    }

    GPUMappedVolume* BlockIterativeReconstructionOperator::getReconstructedVolume() const
    {
        if(config->GetVoxelizer())
        {
            return config->GetVoxelizer()->GetVoxelVolume();
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
        for(unsigned int subVolume = 0; subVolume < config->GetVolume()->Properties().GetSubVolumeCount(); ++subVolume)
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
        for(unsigned int subVolume = 0; subVolume < config->GetVolume()->Properties().GetSubVolumeCount(); ++subVolume)
        {
            for(unsigned int projection = 0; projection < projectionAccess->getBlockSize(block); ++projection)
            {
                state->setCurrentProjectionIndex(projection);
                HandleBackProjection(subVolume, projectionAccess->getBlockSize(block));
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
        if(parameterSet->get<AlgorithmParameterSet>()->UseLongObjectCompensation())
        {
            logger.OnLongObjectCompensationStart();
            longObjectCompensationKernel->setConstantLength(config->GetGeometricSetup()->getScannerGeometry().getTraversalLengthThroughBoundingBox(config->GetVolume()->getObjectOnCPU()->Properties().GetVolumeBoundingBox()));
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
        compareKernel = new CompareKernel(framework, projectionAccess->getProjectionResolution(), 0, 0);
        if(parameterSet->get<AlgorithmParameterSet>()->UseLongObjectCompensation())
        {
            longObjectCompensationKernel = new VirtualProjectionKernel(framework, 0);
        }

    }

    void BlockIterativeReconstructionOperator::DeleteKernels()
    {
        delete longObjectCompensationKernel;
        delete compareKernel;
    }
}