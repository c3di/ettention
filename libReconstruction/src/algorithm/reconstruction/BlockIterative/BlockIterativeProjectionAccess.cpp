#include "stdafx.h"
#include "BlockIterativeProjectionAccess.h"
#include "framework/error/Exception.h"
#include "model/projectionset/factory/ProjectionIteratorFactory.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"

namespace ettention
{
    BlockIterativeProjectionAccess::BlockIterativeProjectionAccess(Framework* framework, AlgebraicParameterSet* parameterSet, ImageStackDatasource* projections)
        : projections(projections)
        , generalBlockSize(parameterSet->get<AlgorithmParameterSet>()->ProjectionBlockSize())
        , parameterSet(parameterSet)
    {
        if(generalBlockSize == 0)
        {
            throw Exception("Illegal ProjectionBlockSize!");
        }

        if(!this->projections)
        {
            this->projections = framework->getImageStackDataSourceFactory()->instantiate(parameterSet);
        }
        if(!this->projections || this->projections->getNumberOfProjections() == 0)
        {
            throw Exception("No projections given!");
        }
        this->projections->writeToLog();

        projectionSet = ProjectionIteratorFactory::instantiate(framework->getPluginManager(), parameterSet->ProjectionSetType(), this->projections);
        if(!projectionSet)
        {
            throw Exception("Illegal ProjectionSetType!");
        }
        projectionSet->SetProjectionCount(this->projections->getNumberOfProjections(), parameterSet->SkipProjectionList());
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

    ScannerGeometry BlockIterativeProjectionAccess::getScannerGeometry(unsigned int blockIndex, unsigned int projectionIndex) const
    {
        return this->projections->getScannerGeometry(this->getProjectionIndex(blockIndex, projectionIndex), parameterSet);
    }

    ImageStackDatasource* BlockIterativeProjectionAccess::getImageStack() const
    {
        return projections;
    }
}