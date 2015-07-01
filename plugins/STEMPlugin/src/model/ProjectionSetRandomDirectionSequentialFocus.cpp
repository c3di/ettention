#include "stdafx.h"
#include "ProjectionSetRandomDirectionSequentialFocus.h"
#include <algorithm>
#include <unordered_map>
#include <framework/error/Exception.h>
#include <framework/RandomAlgorithms.h>

namespace ettention
{
    namespace stem
    {
        ProjectionSetRandomDirectionSequentialFocus::ProjectionSetRandomDirectionSequentialFocus()
        {
        }

        ProjectionSetRandomDirectionSequentialFocus::~ProjectionSetRandomDirectionSequentialFocus()
        {
        }

        void ProjectionSetRandomDirectionSequentialFocus::Reorder(std::vector<HyperStackIndex>& indices)
        {
            std::vector<unsigned int> directions;
            std::unordered_map<unsigned int, std::vector<unsigned int>> fociPerDirection;
            for(auto it = indices.begin(); it != indices.end(); ++it)
            {
                if(it->getNumberOfDimensions() != 2)
                {
                    throw Exception("All hyper stack indices must have a dimension of 2!");
                }
                directions.push_back((*it)[0]);
                fociPerDirection[(*it)[0]].push_back((*it)[1]);
            }
            indices.clear();
            indicesPerDirection.clear();
            indicesPerDirection.resize(directions.size());
            originalIndicesPerDirection.clear();
            originalIndicesPerDirection.resize(directions.size());
            auto dirRandomOrder = RandomAlgorithms::generateRandomPermutationSequence((unsigned int)directions.size());
            for(unsigned int dir = 0; dir < directions.size(); ++dir)
            {
                auto shuffledDir = dirRandomOrder[dir];
                auto directionIndex = directions[shuffledDir];
                const std::vector<unsigned int>& foci = fociPerDirection[directionIndex];
                for(auto focusIt = foci.begin(); focusIt != foci.end(); ++focusIt)
                {
                    HyperStackIndex index(directionIndex, *focusIt);
                    indices.push_back(index);
                    indicesPerDirection[dir].push_back(index);
                    originalIndicesPerDirection[shuffledDir].push_back(index);
                }
            }
        }

        unsigned int ProjectionSetRandomDirectionSequentialFocus::GetDirectionCount() const
        {
            return (unsigned int)indicesPerDirection.size();
        }

        unsigned int ProjectionSetRandomDirectionSequentialFocus::GetFocusCount(unsigned int directionIndex) const
        {
            return (unsigned int)indicesPerDirection[directionIndex].size();
        }

        const HyperStackIndex& ProjectionSetRandomDirectionSequentialFocus::getProjectionIndex(unsigned int directionIndex, unsigned int focusIndex) const
        {
            return indicesPerDirection[directionIndex][focusIndex];
        }

        unsigned int ProjectionSetRandomDirectionSequentialFocus::FindMaximumFocusCountPerDirection() const
        {
            unsigned int max = 0;
            for(auto it = indicesPerDirection.begin(); it != indicesPerDirection.end(); ++it)
            {
                max = std::max(max, (unsigned int)it->size());
            }
            return max;
        }

        const HyperStackIndex& ProjectionSetRandomDirectionSequentialFocus::GetOriginalProjectionIndex(unsigned int directionIndex, unsigned int focusIndex) const
        {
            return originalIndicesPerDirection[directionIndex][focusIndex];
        }
    }
}