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
            void reorder(std::vector<HyperStackIndex>& indices) override;

        public:
            ProjectionSetRandomDirectionSequentialFocus();
            ~ProjectionSetRandomDirectionSequentialFocus();

            unsigned int getDirectionCount() const;
            unsigned int getFocusCount(unsigned int directionIndex) const;
            const HyperStackIndex& getProjectionIndex(unsigned int directionIndex, unsigned int focusIndex) const;
            const HyperStackIndex& getOriginalProjectionIndex(unsigned int directionIndex, unsigned int focusIndex) const;
            unsigned int findMaximumFocusCountPerDirection() const;
        };
    }
}