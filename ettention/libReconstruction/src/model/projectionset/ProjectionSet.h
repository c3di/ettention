#pragma once

#include "framework/Logger.h"
#include "io/HyperStackIndex.h"

namespace ettention
{
    class ProjectionSet
    {
    public:
        ProjectionSet();
        virtual ~ProjectionSet();

        void setProjectionCount(unsigned int projectionCount, const std::set<HyperStackIndex>& skippedProjections);
        void setProjectionIndices(const std::vector<HyperStackIndex> indices, const std::set<HyperStackIndex>& skippedIndices);
        HyperStackIndex getProjectionIndex(unsigned int projectionNumber) const;
        unsigned int getNumberOfProjections() const;
        unsigned int getDimensionCount() const;
        unsigned int getProjectionCountOfDimension(unsigned int dimension) const;

    protected:
        virtual void reorder(std::vector<HyperStackIndex>& indices) = 0;

    private:
        HyperStackIndex projectionCountPerDimension;
        std::vector<HyperStackIndex> indices;
    };
}