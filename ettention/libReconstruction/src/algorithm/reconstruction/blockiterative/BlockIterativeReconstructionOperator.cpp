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
        : ReconstructionAlgorithmUsingMask(framework)
        , al(framework->getOpenCLStack())
        , longObjectCompensationKernel(0)
    {
        projectionAccess = new BlockIterativeProjectionAccess(framework);
        state = new BlockIterativeState(al, projectionAccess);
        config = new BlockIterativeConfiguration(framework, projectionAccess);
        this->initializeKernels();
    }

    BlockIterativeReconstructionOperator::BlockIterativeReconstructionOperator(ImageStackDatasource* projectionDataSource, Framework* framework)
        : ReconstructionAlgorithmUsingMask(framework)
        , al(framework->getOpenCLStack())
        , longObjectCompensationKernel(0)
    {
        projectionAccess = new BlockIterativeProjectionAccess(framework, projectionDataSource);
        state = new BlockIterativeState(al, projectionAccess);
        config = new BlockIterativeConfiguration(framework, projectionAccess);
        this->initializeKernels();
    }

    BlockIterativeReconstructionOperator::~BlockIterativeReconstructionOperator()
    {
        this->deleteKernels();
        delete config;
        delete state;
        delete projectionAccess;
    }

    void BlockIterativeReconstructionOperator::run()
    {
        runNiterations(framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations());
    }

    void BlockIterativeReconstructionOperator::runSingleIteration()
    {
        runNiterations(1u);
    }

    void BlockIterativeReconstructionOperator::runNiterations(unsigned int iterationsCount /*= 1u*/)
    {
        logger.OnReconstructionStart(framework, projectionAccess, state, config->getVolume(), framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations(), framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation());
        for( unsigned int currentIteration = 0; currentIteration < iterationsCount; ++currentIteration )
        {
            logger.OnIterationStart(currentIteration);
            handleOneIteration();
            logger.OnIterationEnd();
        }
        if( config->getVoxelizer() )
        {
            config->getVoxelizer()->voxelize();
        }
        logger.OnReconstructionEnd();
    }

    GPUMappedVolume* BlockIterativeReconstructionOperator::getReconstructedVolume() const
    {
        if(config->getVoxelizer())
        {
            return config->getVoxelizer()->getVoxelVolume();
        } else {
            return config->getVolume();
        }
    }

    void BlockIterativeReconstructionOperator::handleOneIteration()
    {
        for(unsigned int block = 0; block < projectionAccess->GetBlockCount(); ++block)
        {
            state->setCurrentBlockIndex(block);
            logger.OnBlockStart();
            this->doForwardProjectionForBlock(block);
            this->computeResidualsForBlock(block);
            this->doBackProjectionForBlock(block);
            logger.OnBlockEnd();
        }
    }

    void BlockIterativeReconstructionOperator::doForwardProjectionForBlock(unsigned int block)
    {
        auto subVolumeCount = config->getVolume()->getProperties().getSubVolumeCount();
        auto blockSize = projectionAccess->getBlockSize(block);
        for( unsigned int subVolume = 0; subVolume < subVolumeCount; ++subVolume )
        {
            for( unsigned int projection = 0; projection < blockSize; ++projection )
            {
                state->setCurrentProjectionIndex(projection);
                handleForwardProjection(subVolume);
            }
        }
    }

    void BlockIterativeReconstructionOperator::computeResidualsForBlock(unsigned int block)
    {
        auto blockSize = projectionAccess->getBlockSize(block);
        for( unsigned int projection = 0; projection < blockSize; ++projection )
        {
            state->setCurrentProjectionIndex(projection);
            compensateLongObject();
            computeResidual();
        }
    }

    void BlockIterativeReconstructionOperator::doBackProjectionForBlock(unsigned int block)
    {
        auto subVolumeCount = config->getVolume()->getProperties().getSubVolumeCount();
        auto blockSize = projectionAccess->getBlockSize(block);
        for( unsigned int subVolume = 0; subVolume < subVolumeCount; ++subVolume )
        {
            for(unsigned int projection = 0; projection < blockSize; ++projection)
            {
                state->setCurrentProjectionIndex(projection);
                handleBackProjection(subVolumeCount - subVolume - 1, projectionAccess->getBlockSize(block));
            }
        }
    }

    void BlockIterativeReconstructionOperator::handleForwardProjection(unsigned int subVolume)
    {
        logger.OnForwardProjectionStart(subVolume);
        config->getGeometricSetup()->setScannerGeometry(state->getCurrentScannerGeometry());
        config->getForwardProjection()->SetOutput(state->getVirtualProjection(), state->getTraversalLength());
        config->getForwardProjection()->Execute(subVolume);
        logger.OnForwardProjectionEnd();
    }

    void BlockIterativeReconstructionOperator::compensateLongObject()
    {
        if(framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
        {
            logger.OnLongObjectCompensationStart();
            auto volumeBoundingBox = config->getVolume()->getObjectOnCPU()->getProperties().getRealSpaceBoundingBox();
            auto traversalLength = config->getGeometricSetup()->getScannerGeometry()->getTraversalLengthThroughBoundingBox(volumeBoundingBox);
            longObjectCompensationKernel->setConstantLength(traversalLength);
            longObjectCompensationKernel->setProjection(state->getVirtualProjection());
            longObjectCompensationKernel->setTraversalLength(state->getTraversalLength());
            longObjectCompensationKernel->run();
            logger.OnLongObjectCompensationEnd();
        }
    }

    void BlockIterativeReconstructionOperator::computeResidual()
    {
        logger.OnResidualComputationStart();
        compareKernel->setInput(state->getCurrentRealProjection(), state->getVirtualProjection());
        compareKernel->setOutput(state->getResidual());
        compareKernel->run();
        logger.OnResidualComputationEnd();
    }

    void BlockIterativeReconstructionOperator::handleBackProjection(unsigned int subVolume, unsigned int projectionBlockSize)
    {
        logger.OnBackProjectionStart(subVolume);
        config->getGeometricSetup()->setScannerGeometry(state->getCurrentScannerGeometry());
        config->getBackProjection()->setInput(state->getResidual(), state->getTraversalLength(), projectionBlockSize);
        config->getBackProjection()->execute(subVolume);
        config->getBackProjection()->setInput(nullptr, nullptr, 1);
        logger.OnBackProjectionEnd();
    }

    void BlockIterativeReconstructionOperator::setMaskVolume(GPUMappedVolume* volume)
    {
        config->setPriorknowledgeMask(volume);
    }

    GPUMappedVolume* BlockIterativeReconstructionOperator::getMaskVolume() const
    {
        return config->getPriorknowledgeMask();
    }

    void BlockIterativeReconstructionOperator::initializeKernels()
    {
        compareKernel = new CompareKernel(framework, projectionAccess->getProjectionResolution(), nullptr, nullptr);
        if(framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
        {
            longObjectCompensationKernel = new VirtualProjectionKernel(framework, nullptr, nullptr);
        }
    }

    void BlockIterativeReconstructionOperator::deleteKernels()
    {
        delete longObjectCompensationKernel;
        delete compareKernel;
    }

    void BlockIterativeReconstructionOperator::exportGeometryTo(Visualizer* visualizer)
    {
        projectionAccess->getImageStack()->exportAllGeometriesTo(visualizer, framework->getParameterSet());
        visualizer->appendBoundingBox(getReconstructedVolume()->getObjectOnCPU()->getProperties().getRealSpaceBoundingBox(), Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    }
}