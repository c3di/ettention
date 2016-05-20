#include "stdafx.h"
#include "ProjectionSetBuckets.h"

namespace ettention
{
    ProjectionSetBuckets::ProjectionSetBuckets()
    { 

    }

    ProjectionSetBuckets::~ProjectionSetBuckets()
    {

    }

    void ProjectionSetBuckets::reorder(std::vector<HyperStackIndex>& indices)
    {
        std::vector<HyperStackIndex> temp;
        temp.resize(indices.size());

        // subdivide all angles into several groups and periodically switch between them
        // for 360 degrees a subdivision in 4 groups means perpendicular angles (around 90deg.)
        const int kAngleSubdivisionCount = 4;
        std::vector<int> current_idx;
        for(int i = 0; i < kAngleSubdivisionCount; ++i)
        {
            current_idx.push_back(i * static_cast<int>(indices.size()) / kAngleSubdivisionCount);
        }

        for(int i = 0; i < (int)indices.size(); i++)
        {
            int segment_nr = i % kAngleSubdivisionCount;  // one of angle subdivision segments
            temp[i] = indices[current_idx[segment_nr]];
            current_idx[segment_nr]++;
        }

        // copy back into index list
        for(unsigned int i = 0; i < indices.size(); i++)
        {
            indices[i] = temp[i];
        }
    }
}