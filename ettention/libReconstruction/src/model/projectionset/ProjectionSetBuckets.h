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
        void reorder(std::vector<HyperStackIndex>& indices) override;
    };
}

