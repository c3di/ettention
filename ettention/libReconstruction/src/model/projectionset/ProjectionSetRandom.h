#pragma once
#include "ProjectionSet.h"

namespace ettention
{
    /*
    */
    class ProjectionSetRandom : public ProjectionSet
    {
    public:
        ProjectionSetRandom();
        ~ProjectionSetRandom();

    protected:
        void reorder(std::vector<HyperStackIndex>& indices) override;
    };
}