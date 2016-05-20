#include "stdafx.h"
#include "ProjectionSet.h"

namespace ettention
{
    ProjectionSet::ProjectionSet()
    {
    }

    ProjectionSet::~ProjectionSet()
    {
    }

    void ProjectionSet::setProjectionCount(unsigned int projectionCount, const std::set<HyperStackIndex>& skippedProjections)
    {
        indices.clear();
        for(unsigned int i = 0; i < projectionCount; ++i)
        {
            if(skippedProjections.find(i) == skippedProjections.end())
            {
                indices.push_back(i);
            }
        }
        this->reorder(indices);
    }

    void ProjectionSet::setProjectionIndices(const std::vector<HyperStackIndex> indices, const std::set<HyperStackIndex>& skippedIndices)
    {
        this->indices.clear();
        for(auto it = indices.begin(); it != indices.end(); ++it)
        {
            if(skippedIndices.find(*it) == skippedIndices.end())
            {
                this->indices.push_back(*it);
            }
        }
        this->reorder(this->indices);
    }

    unsigned int ProjectionSet::getNumberOfProjections() const
    {
        return (unsigned int)indices.size();
    }

    unsigned int ProjectionSet::getDimensionCount() const
    {
        return projectionCountPerDimension.getNumberOfDimensions();
    }

    unsigned int ProjectionSet::getProjectionCountOfDimension(unsigned int dimension) const
    {
        return projectionCountPerDimension[dimension];
    }

    HyperStackIndex ProjectionSet::getProjectionIndex(unsigned int projectionNumber) const
    {
        return indices[projectionNumber];
    }
}