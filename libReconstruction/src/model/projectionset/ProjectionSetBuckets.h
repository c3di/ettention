#pragma once
#include "ProjectionSet.h"

namespace ettention
{
    /*
    */
    class ProjectionSetBuckets : public ProjectionSet
    {
    public:
        ProjectionSetBuckets();
        ~ProjectionSetBuckets();

    protected:
        void Reorder(std::vector<HyperStackIndex>& indices) override;
    };
}

