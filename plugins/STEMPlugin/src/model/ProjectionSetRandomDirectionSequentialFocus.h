#pragma once
#include <model/projectionset/ProjectionSet.h>

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API ProjectionSetRandomDirectionSequentialFocus : public ProjectionSet
        {
        private:
            std::vector<std::vector<HyperStackIndex>> indicesPerDirection;
            std::vector<std::vector<HyperStackIndex>> originalIndicesPerDirection;

        protected:
            void Reorder(std::vector<HyperStackIndex>& indices) override;

        public:
            ProjectionSetRandomDirectionSequentialFocus();
            ~ProjectionSetRandomDirectionSequentialFocus();

            unsigned int GetDirectionCount() const;
            unsigned int GetFocusCount(unsigned int directionIndex) const;
            const HyperStackIndex& getProjectionIndex(unsigned int directionIndex, unsigned int focusIndex) const;
            const HyperStackIndex& GetOriginalProjectionIndex(unsigned int directionIndex, unsigned int focusIndex) const;
            unsigned int FindMaximumFocusCountPerDirection() const;
        };
    }
}