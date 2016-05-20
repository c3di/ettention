#include "stdafx.h"
#include "BlockIterativeProjectionAccess.h"
#include "framework/error/Exception.h"
#include "model/projectionset/factory/ProjectionIteratorFactory.h"
#include "setup/parameterset/InputParameterSet.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/OptimizationParameterSet.h"

namespace ettention
{
    BlockIterativeProjectionAccess::BlockIterativeProjectionAccess(Framework* framework, ImageStackDatasource* projections)
        : projections(projections)
        , generalBlockSize(framework->getParameterSet()->get<AlgorithmParameterSet>()->getProjectionBlockSize())
        , parameterSet(framework->getParameterSet())
    {
        if(generalBlockSize == 0)
        {
            throw Exception("Illegal ProjectionBlockSize!");
        }

        if(!this->projections)
        {
            this->projections = framework->openInputStack();
        }
        if(!this->projections || this->projections->getNumberOfProjections() == 0)
        {
            throw Exception("No projections given!");
        }
        this->projections->writeToLog();

        projectionSet = ProjectionIteratorFactory::instantiate(framework->getPluginManager(), parameterSet->get<OptimizationParameterSet>()->getProjectionSetType(), this->projections);
        if(!projectionSet)
        {
            throw Exception("Illegal ProjectionSetType!");
        }
        projectionSet->setProjectionCount(this->projections->getNumberOfProjections(), parameterSet->get<InputParameterSet>()->getSkipProjectionList());
        if(projectionSet->getNumberOfProjections() == 0)
        {
            throw Exception("After applying SkipProjectionList no projections are left!");
        }
        if(projectionSet->getProjectionIndex(0).getNumberOfDimensions() != 1)
        {
            throw Exception("Blockiterative algorithm does only work with a hyperstack dimension of 1!");
        }

        blockCount = (unsigned int)std::ceil((float)this->projectionSet->getNumberOfProjections() / (float)generalBlockSize);
    }

    BlockIterativeProjectionAccess::~BlockIterativeProjectionAccess()
    {
        delete projectionSet;
        delete projections;
    }

    unsigned int BlockIterativeProjectionAccess::getBlockSize(unsigned int blockIndex) const
    {
        if(blockIndex < blockCount - 1)
        {
            return generalBlockSize;
        }
        else if(blockIndex == blockCount - 1)
        {
            // last block may contain less projections
            return projectionSet->getNumberOfProjections() - (blockCount - 1) * generalBlockSize;
        }
        else
        {
            return 0;
        }
    }

    HyperStackIndex BlockIterativeProjectionAccess::getProjectionIndex(unsigned int blockIndex, unsigned int projectionIndex) const
    {
        if(blockIndex >= blockCount || projectionIndex >= this->getBlockSize(blockIndex))
        {
            throw Exception("Invalid projection identifier given!");
        }
        unsigned int index = generalBlockSize * blockIndex + projectionIndex;
        return projectionSet->getProjectionIndex(index);
    }

    Image* BlockIterativeProjectionAccess::getProjectionImage(unsigned int blockIndex, unsigned int projectionIndex) const
    {
        return projections->getProjectionImage(this->getProjectionIndex(blockIndex, projectionIndex));
    }

    Vec2ui BlockIterativeProjectionAccess::getProjectionResolution() const
    {
        return projections->getResolution();
    }

    unsigned int BlockIterativeProjectionAccess::GetNumberOfProjections() const
    {
        return projectionSet->getNumberOfProjections();
    }

    unsigned int BlockIterativeProjectionAccess::GetBlockCount() const
    {
        return blockCount;
    }

    unsigned int BlockIterativeProjectionAccess::getPhysicalProjectionIndex(unsigned int blockIndex, unsigned int projectionIndex) const
    {
        return this->getProjectionIndex(blockIndex, projectionIndex)[0];
    }

    ScannerGeometry* BlockIterativeProjectionAccess::getScannerGeometry(unsigned int blockIndex, unsigned int projectionIndex) const
    {
        return this->projections->createScannerGeometry(this->getProjectionIndex(blockIndex, projectionIndex), parameterSet);
    }

    ImageStackDatasource* BlockIterativeProjectionAccess::getImageStack() const
    {
        return projections;
    }
}