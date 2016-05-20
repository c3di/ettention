#include "stdafx.h"
#include "ProjectionSetMaxAngle.h"

namespace ettention
{
    ProjectionSetMaxAngle::ProjectionSetMaxAngle()
    {

    }

    ProjectionSetMaxAngle::~ProjectionSetMaxAngle()
    {

    }

    void ProjectionSetMaxAngle::reorder(std::vector<HyperStackIndex>& indices)
    {
        std::vector<HyperStackIndex> temp(indices.size());
        unsigned int j = (int)(indices.size() / 2);
        unsigned int k = 0;
        for(unsigned int i = 0; i < indices.size(); i++)
        {
            if((i % 2) == 0)
            {
                temp[i] = indices[j];
                j++;
            }
            else
            {
                temp[i] = indices[k];
                k++;
            }
        }
        for(unsigned int i = 0; i < indices.size(); i++)
        {
            indices[i] = temp[i];
        }
    }
}