#include "stdafx.h"
#include "BlockIterativeState.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeProjectionAccess.h"
#include "gpu/opencl/CLMemBuffer.h"

namespace ettention
{
    BlockIterativeState::BlockIterativeState(CLAbstractionLayer* al, BlockIterativeProjectionAccess* projectionAccess)
        : al(al)
        , projectionAccess(projectionAccess)
    {
        currentRealProjection = new GPUMapped<Image>(al, projectionAccess->getProjectionResolution());
    }

    BlockIterativeState::~BlockIterativeState()
    {
        delete currentRealProjection;
        while(!virtualProjections.empty())
        {
            delete virtualProjections.back();
            virtualProjections.pop_back();
            delete residuals.back();
            residuals.pop_back();
            delete traversalLengths.back();
            traversalLengths.pop_back();
        }
    }

    void BlockIterativeState::setCurrentBlockIndex(unsigned int blockIndex)
    {
        unsigned int blockSize = projectionAccess->getBlockSize(blockIndex);
        for(unsigned int i = (unsigned int)virtualProjections.size(); i < blockSize; ++i)
        {
            virtualProjections.push_back(new GPUMapped<Image>(al, projectionAccess->getProjectionResolution()));
            traversalLengths.push_back(new GPUMapped<Image>(al, projectionAccess->getProjectionResolution()));
            residuals.push_back(new GPUMapped<Image>(al, projectionAccess->getProjectionResolution()));
        }
        for(unsigned int i = 0; i < blockSize; ++i)
        {
            virtualProjections[i]->getBufferOnGPU()->clear();
            virtualProjections[i]->markAsChangedOnGPU();
            traversalLengths[i]->getBufferOnGPU()->clear();
            traversalLengths[i]->markAsChangedOnGPU();
            residuals[i]->getBufferOnGPU()->clear();
            residuals[i]->markAsChangedOnGPU();
        }
        currentBlockIndex = blockIndex;        
    }

    void BlockIterativeState::setCurrentProjectionIndex(unsigned int projectionIndex)
    {
        if(projectionIndex >= getCurrentBlockSize())
        {
            throw Exception("Illegal projection index given!");
        }
        currentProjectionIndex = projectionIndex;
    }

    GPUMapped<Image>* BlockIterativeState::getVirtualProjection() const
    {
        return virtualProjections[currentProjectionIndex];
    }

    GPUMapped<Image>* BlockIterativeState::getTraversalLength() const
    {
        return traversalLengths[currentProjectionIndex];
    }

    GPUMapped<Image>* BlockIterativeState::getResidual() const
    {
        return residuals[currentProjectionIndex];
    }

    unsigned int BlockIterativeState::getCurrentBlockIndex() const
    {
        return currentBlockIndex;
    }

    unsigned int BlockIterativeState::getCurrentProjectionIndex() const
    {
        return currentProjectionIndex;
    }

    unsigned int BlockIterativeState::getCurrentPhysicalProjectionIndex() const
    {
        return projectionAccess->getPhysicalProjectionIndex(currentBlockIndex, currentProjectionIndex);
    }

    unsigned int BlockIterativeState::getCurrentBlockSize() const
    {
        return projectionAccess->getBlockSize(currentBlockIndex);
    }

    Image* BlockIterativeState::getCurrentRealProjectionImage() const
    {
        return projectionAccess->getProjectionImage(currentBlockIndex, currentProjectionIndex);
    }

    ScannerGeometry* BlockIterativeState::getCurrentScannerGeometry() const
    {
        return projectionAccess->getScannerGeometry(currentBlockIndex, currentProjectionIndex);
    }

    GPUMapped<Image>* BlockIterativeState::getCurrentRealProjection() const
    {
        if(getCurrentRealProjectionImage() != currentRealProjection->getObjectOnCPU())
            currentRealProjection->setObjectOnCPU(getCurrentRealProjectionImage());

        return currentRealProjection;
    }
}