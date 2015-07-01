#pragma once
#include "ProjectionSet.h"

namespace ettention
{
    /*
    */
    class ProjectionSetMaxAngle : public ProjectionSet
    {
    public:
        ProjectionSetMaxAngle();
        ~ProjectionSetMaxAngle();

    protected:
        void Reorder(std::vector<HyperStackIndex>& indices) override;
    };
}