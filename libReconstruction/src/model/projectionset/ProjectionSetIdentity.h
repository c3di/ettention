#pragma once
#include "ProjectionSet.h"

namespace ettention
{
    class ProjectionSetIdentity : public ProjectionSet
    {
    public:
        ProjectionSetIdentity();
        ~ProjectionSetIdentity();

    protected:
        void Reorder(std::vector<HyperStackIndex>& indices) override;
    };
}