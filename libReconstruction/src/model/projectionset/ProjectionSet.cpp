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

    void ProjectionSet::SetProjectionCount(unsigned int projectionCount, const std::set<HyperStackIndex>& skippedProjections)
    {
        indices.clear();
        for(unsigned int i = 0; i < projectionCount; ++i)
        {
            if(skippedProjections.find(i) == skippedProjections.end())
            {
                indices.push_back(i);
            }
        }
        this->Reorder(indices);
    }

    void ProjectionSet::SetProjectionIndices(const std::vector<HyperStackIndex> indices, const std::set<HyperStackIndex>& skippedIndices)
    {
        this->indices.clear();
        for(auto it = indices.begin(); it != indices.end(); ++it)
        {
            if(skippedIndices.find(*it) == skippedIndices.end())
            {
                this->indices.push_back(*it);
            }
        }
        this->Reorder(this->indices);
    }

    unsigned int ProjectionSet::getNumberOfProjections() const
    {
        return (unsigned int)indices.size();
    }

    unsigned int ProjectionSet::GetDimensionCount() const
    {
        return projectionCountPerDimension.getNumberOfDimensions();
    }

    unsigned int ProjectionSet::GetProjectionCountOfDimension(unsigned int dimension) const
    {
        return projectionCountPerDimension[dimension];
    }

    HyperStackIndex ProjectionSet::getProjectionIndex(unsigned int projectionNumber) const
    {
        return indices[projectionNumber];
    }
}