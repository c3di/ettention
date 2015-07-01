#pragma once
#include "model/TFProjectionProperties.h"

namespace ettention
{
    namespace stem
    {
        enum PatternType
        {
            STACK, SPIRAL
        };

        class ProjectionPattern
        {
        public:
            virtual std::list<TFProjectionProperties> getProjectionMetaData() = 0;

            static PatternType parsePatternType(std::string ptype);

        protected:
            ProjectionPattern();
            ~ProjectionPattern();
        };
    }
}