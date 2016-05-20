#pragma once
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "model/projectionset/ProjectionSet.h"
#include "io/datasource/ImageStackDatasource.h"
#include "framework/Framework.h"

namespace ettention
{
    class BlockIterativeProjectionAccess
    {
        HyperStackIndex getProjectionIndex(unsigned int blockIndex, unsigned int projectionIndex) const;

    public:
        BlockIterativeProjectionAccess(Framework* framework, ImageStackDatasource* projections = 0);
        ~BlockIterativeProjectionAccess();

        unsigned int GetNumberOfProjections() const;
        unsigned int getBlockSize(unsigned int blockIndex) const;
        unsigned int GetBlockCount() const;
        Image* getProjectionImage(unsigned int blockIndex, unsigned int projectionIndex) const;
        Vec2ui getProjectionResolution() const;
        unsigned int getPhysicalProjectionIndex(unsigned int blockIndex, unsigned int projectionIndex) const;
        ScannerGeometry* getScannerGeometry(unsigned int blockIndex, unsigned int projectionIndex) const;
        ImageStackDatasource* getImageStack() const;

    private:
        AlgebraicParameterSet* parameterSet;
        unsigned int blockCount;
        unsigned int generalBlockSize;
        ProjectionSet* projectionSet;
        ImageStackDatasource* projections;

        HyperStackIndex GetProjectionIndex(unsigned int blockIndex, unsigned int projectionIndex) const;
    };
}